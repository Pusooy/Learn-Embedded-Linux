#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>

// 描述LED相关寄存器结构体
struct led1_reg
{
  volatile void __iomem *ccm_ccgr1;
  volatile void __iomem *iomuxc_mux_ctl;
  volatile void __iomem *iomuxc_pad_ctl;
  volatile void __iomem *gpio5_gdir;
  volatile void __iomem *gpio5_dr;
};

// 描述设备结构体
struct device_led
{
  bool status;
  struct led1_reg led_reg;    // LED相关寄存器
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
  struct device_node *np;
  struct resource res;

  led_dev = devm_kzalloc(&pdev->dev, sizeof(*led_dev), GFP_KERNEL);
  if (!led_dev)
    return -ENOMEM;

  // 获取设备树节点
  np = pdev->dev.of_node;
  if (!np)
  {
    dev_err(&pdev->dev, "No device node found!\n");
    return -ENODEV;
  }

  // 1. 映射 ccm_ccgr1
  if (of_address_to_resource(np, 0, &res))
  {
    dev_err(&pdev->dev, "Failed to get resource for ccm_ccgr1\n");
    return -ENODEV;
  }
  led_dev->led_reg.ccm_ccgr1 = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
  if (!led_dev->led_reg.ccm_ccgr1)
  {
    dev_err(&pdev->dev, "Failed to map ccm_ccgr1\n");
    return -ENOMEM;
  }
  dev_info(&pdev->dev, "ccm_ccgr1: start = 0x%lx, end = 0x%lx, size = 0x%lx\n",
           (unsigned long)res.start, (unsigned long)res.end, (unsigned long)resource_size(&res));

  // 2. 映射 iomuxc_mux_ctl
  if (of_address_to_resource(np, 1, &res))
  {
    dev_err(&pdev->dev, "Failed to get resource for iomuxc_mux_ctl\n");
    return -ENODEV;
  }
  led_dev->led_reg.iomuxc_mux_ctl = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
  if (!led_dev->led_reg.iomuxc_mux_ctl)
  {
    dev_err(&pdev->dev, "Failed to map iomuxc_mux_ctl\n");
    return -ENOMEM;
  }
  dev_info(&pdev->dev, "iomuxc_mux_ctl: start = 0x%lx, end = 0x%lx, size = 0x%lx\n",
           (unsigned long)res.start, (unsigned long)res.end, (unsigned long)resource_size(&res));

  // 3. 映射 iomuxc_pad_ctl
  if (of_address_to_resource(np, 2, &res))
  {
    dev_err(&pdev->dev, "Failed to get resource for iomuxc_pad_ctl\n");
    return -ENODEV;
  }
  led_dev->led_reg.iomuxc_pad_ctl = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
  if (!led_dev->led_reg.iomuxc_pad_ctl)
  {
    dev_err(&pdev->dev, "Failed to map iomuxc_pad_ctl\n");
    return -ENOMEM;
  }
  dev_info(&pdev->dev, "iomuxc_pad_ctl: start = 0x%lx, end = 0x%lx, size = 0x%lx\n",
           (unsigned long)res.start, (unsigned long)res.end, (unsigned long)resource_size(&res));

  // 4. 映射 gpio5_gdir
  if (of_address_to_resource(np, 3, &res))
  {
    dev_err(&pdev->dev, "Failed to get resource for gpio5_gdir\n");
    return -ENODEV;
  }
  led_dev->led_reg.gpio5_gdir = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
  if (!led_dev->led_reg.gpio5_gdir)
  {
    dev_err(&pdev->dev, "Failed to map gpio5_gdir\n");
    return -ENOMEM;
  }
  dev_info(&pdev->dev, "gpio5_gdir: start = 0x%lx, end = 0x%lx, size = 0x%lx\n",
           (unsigned long)res.start, (unsigned long)res.end, (unsigned long)resource_size(&res));

  // 5. 映射 gpio5_dr
  if (of_address_to_resource(np, 4, &res))
  {
    dev_err(&pdev->dev, "Failed to get resource for gpio5_dr\n");
    return -ENODEV;
  }
  led_dev->led_reg.gpio5_dr = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
  if (!led_dev->led_reg.gpio5_dr)
  {
    dev_err(&pdev->dev, "Failed to map gpio5_dr\n");
    return -ENOMEM;
  }
  dev_info(&pdev->dev, "gpio5_dr: start = 0x%lx, end = 0x%lx, size = 0x%lx\n",
           (unsigned long)res.start, (unsigned long)res.end, (unsigned long)resource_size(&res));

  // 配置GPIO
  writel(0xFFFFFFFF, led_dev->led_reg.ccm_ccgr1);
  writel(0x5, led_dev->led_reg.iomuxc_mux_ctl);
  writel(0x1F838, led_dev->led_reg.iomuxc_pad_ctl);
  writel(readl(led_dev->led_reg.gpio5_gdir) | (1 << 3), led_dev->led_reg.gpio5_gdir);
  writel(readl(led_dev->led_reg.gpio5_dr) & ~(1 << 0), led_dev->led_reg.gpio5_dr);

  // 注册MISC设备
  led_dev->misc_led.minor = MISC_DYNAMIC_MINOR;
  led_dev->misc_led.name = "led1_misc";
  led_dev->misc_led.fops = &led_fops;

  platform_set_drvdata(pdev, led_dev);

  return misc_register(&led_dev->misc_led);
}

// 卸载驱动时调用
static int led_remove(struct platform_device *pdev)
{
  struct device_led *led_dev = platform_get_drvdata(pdev);
  misc_deregister(&led_dev->misc_led);

  return 0;
}

// 设备树匹配表
static const struct of_device_id led_of_match[] = {
    {.compatible = "100ask,leds"},
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
MODULE_DESCRIPTION("LED Platform Driver with Device Tree and devm_ioremap");
