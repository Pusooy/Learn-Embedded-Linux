#include "asm/io.h"
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/of.h>
#include <linux/of_address.h>

// 描述LED相关寄存器结构体
struct led_reg
{
  void __iomem *ccm_ccgr1;
  void __iomem *iomuxc_snvs_sw_mux_ctl_pad_snvs_tamper3;
  void __iomem *gpio5_gdir;
  void __iomem *gpio5_dr;
};

// 描述设备结构体
struct device_led
{
  bool status;
  struct led_reg led_reg;     // LED相关寄存器
  struct miscdevice misc_led; // MISC设备结构体
};

// 打开设备
static int led_open(struct inode *inode, struct file *file)
{
  struct device_led *led_dev = container_of(file->private_data, struct device_led, misc_led);
  file->private_data = led_dev;
  return 0;
}

// 向设备写入数据
static ssize_t led_write(struct file *file, const char __user *buf, size_t count, loff_t *off)
{
  struct device_led *led_dev = file->private_data;
  char kbuf[1];
  unsigned int val;

  if (copy_from_user(kbuf, buf, 1))
    return -EFAULT;

  val = readl(led_dev->led_reg.gpio5_dr);
  if (kbuf[0] == '1')
  {
    val &= ~(1 << 3); // 点亮LED
    led_dev->status = true;
  }
  else if (kbuf[0] == '0')
  {
    val |= (1 << 3); // 熄灭LED
    led_dev->status = false;
  }
  else
  {
    return -EINVAL;
  }

  writel(val, led_dev->led_reg.gpio5_dr);
  return count;
}

// 从设备读取数据
static ssize_t led_read(struct file *file, char __user *buf, size_t count, loff_t *off)
{
  struct device_led *led_dev = file->private_data;
  char kbuf[1] = {led_dev->status ? '1' : '0'};

  if (copy_to_user(buf, kbuf, 1))
    return -EFAULT;

  return count;
}

// 设备操作函数集
static const struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .write = led_write,
    .read = led_read,
};

// 设备初始化
static int led_probe(struct platform_device *pdev)
{
  struct device_led *led_dev;
  struct resource *res;

  led_dev = devm_kzalloc(&pdev->dev, sizeof(*led_dev), GFP_KERNEL);
  if (!led_dev)
    return -ENOMEM;

  // 使用设备树获取资源
  res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  led_dev->led_reg.ccm_ccgr1 = ioremap(res->start, res->end - res->start + 1);

  res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
  led_dev->led_reg.iomuxc_snvs_sw_mux_ctl_pad_snvs_tamper3 = ioremap(res->start, res->end - res->start + 1);

  res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
  led_dev->led_reg.gpio5_gdir = ioremap(res->start, res->end - res->start + 1);

  res = platform_get_resource(pdev, IORESOURCE_MEM, 3);
  led_dev->led_reg.gpio5_dr = ioremap(res->start, res->end - res->start + 1);

  if (IS_ERR(led_dev->led_reg.ccm_ccgr1) || IS_ERR(led_dev->led_reg.iomuxc_snvs_sw_mux_ctl_pad_snvs_tamper3) ||
      IS_ERR(led_dev->led_reg.gpio5_gdir) || IS_ERR(led_dev->led_reg.gpio5_dr))
    return -EINVAL;

  // 配置GPIO
  writel(readl(led_dev->led_reg.ccm_ccgr1) | (3 << 30), led_dev->led_reg.ccm_ccgr1);
  writel((readl(led_dev->led_reg.iomuxc_snvs_sw_mux_ctl_pad_snvs_tamper3) & ~0xF) | 0x5,
         led_dev->led_reg.iomuxc_snvs_sw_mux_ctl_pad_snvs_tamper3);
  writel(readl(led_dev->led_reg.gpio5_gdir) | (1 << 3), led_dev->led_reg.gpio5_gdir);

  // 注册MISC设备
  led_dev->misc_led.minor = MISC_DYNAMIC_MINOR;
  led_dev->misc_led.name = dev_name(&pdev->dev);
  led_dev->misc_led.fops = &led_fops;

  platform_set_drvdata(pdev, led_dev);

  return misc_register(&led_dev->misc_led);
}

// 卸载驱动时调用
static int led_remove(struct platform_device *pdev)
{
  struct device_led *led_dev = platform_get_drvdata(pdev);
  misc_deregister(&led_dev->misc_led);
  iounmap(led_dev->led_reg.ccm_ccgr1);
  iounmap(led_dev->led_reg.iomuxc_snvs_sw_mux_ctl_pad_snvs_tamper3);
  iounmap(led_dev->led_reg.gpio5_gdir);
  iounmap(led_dev->led_reg.gpio5_dr);

  return 0;
}

// 设备树匹配表
static const struct of_device_id led_of_match[] = {
    {.compatible = "myled,led1"},
    {},
};
MODULE_DEVICE_TABLE(of, led_of_match);

// 平台驱动结构体
static struct platform_driver led_driver = {
    .driver = {
        .name = "led_platform_device",
        .of_match_table = led_of_match,
    },
    .probe = led_probe,
    .remove = led_remove,
};

// 驱动初始化
static int __init led_driver_init(void)
{
  return platform_driver_register(&led_driver);
}

// 驱动卸载
static void __exit led_driver_exit(void)
{
  platform_driver_unregister(&led_driver);
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pusooy");
MODULE_DESCRIPTION("LED Platform Driver Supporting Multiple Devices");
