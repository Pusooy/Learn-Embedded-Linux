#include "linux/export.h"
#include "linux/stddef.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/miscdevice.h>

#define CCM_CCGR1_BASE 0x20C406C
#define IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 0x2290014
#define GPIO5_BASE 0x020AC000
#define GPIO5_GDIR_OFFSET 0x4
#define GPIO5_DR_OFFSET 0x0

// 描述LED相关寄存器结构体
struct led1_reg
{
    void __iomem *ccm_ccgr1;
    void __iomem *iomuxc_snvs_sw_mux_ctl_pad_snvs_tamper3;
    void __iomem *gpio5_gdir;
    void __iomem *gpio5_dr;
};

// 描述设备结构体
struct device_led
{
    dev_t dev_num; // 设备号
    int major;
    int minor;
    bool status;
    struct miscdevice misc_led; // MISC结构体
    struct led1_reg *led_reg;   // LED相关寄存器
};

struct led1_reg tem_reg;
struct device_led led1;

/*打开设备函数*/
static int cdev_led_open(struct inode *inode, struct file *file)
{
    unsigned int val = 0;
    file->private_data = &led1; // 设置私有数据

    /* 映射物理地址到虚拟地址 */
    printk("start ioremap \n");
    tem_reg.ccm_ccgr1 = ioremap(CCM_CCGR1_BASE, 4);
    tem_reg.iomuxc_snvs_sw_mux_ctl_pad_snvs_tamper3 = ioremap(IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3, 4);
    tem_reg.gpio5_gdir = ioremap(GPIO5_BASE + GPIO5_GDIR_OFFSET, 4);
    tem_reg.gpio5_dr = ioremap(GPIO5_BASE + GPIO5_DR_OFFSET, 4);

    led1.led_reg = &tem_reg;
    printk("finish ioremap \n");

    /* Enable GPIO5 */
    val = readl(led1.led_reg->ccm_ccgr1);
    val |= (3 << 30);
    writel(val, led1.led_reg->ccm_ccgr1);

    /* Configure GPIO5_IO03 as GPIO */
    val = readl(led1.led_reg->iomuxc_snvs_sw_mux_ctl_pad_snvs_tamper3);
    val &= ~(0xf);
    val |= (5);
    writel(val, led1.led_reg->iomuxc_snvs_sw_mux_ctl_pad_snvs_tamper3);

    /* Set GPIO5_IO03 as output */
    val = readl(led1.led_reg->gpio5_gdir);
    val |= (1 << 3);
    writel(val, led1.led_reg->gpio5_gdir);

    printk("This is cdev_test_open\r\n");
    return 0;
}

/*向设备写入数据函数*/
static ssize_t cdev_led_write(struct file *file, const char __user *buf, size_t size, loff_t *off)
{
    char kbuf[1];
    unsigned int val;

    val = readl(led1.led_reg->gpio5_gdir);
    if (copy_from_user(kbuf, buf, 1))
        return -EFAULT;
    if (kbuf[0] == '1')
    {
        /* Set GPIO5_IO03 to low level */
        val &= ~(1 << 3);
        led1.status = true;
    }
    else if (kbuf[0] == '0')
    {
        /* Set GPIO5_IO03 to high level */
        val |= (1 << 3);
        led1.status = false;
    }
    else
        return -EINVAL;
    writel(val, led1.led_reg->gpio5_dr);

    return size;
}

/**从设备读取数据*/
static ssize_t cdev_led_read(struct file *file, char __user *buf, size_t size, loff_t *off)
{
    char kbuf[1];
    if (led1.status)
        kbuf[0] = '1';
    else
        kbuf[0] = '0';
    if (copy_to_user(buf, kbuf, 1))
        return -EFAULT;

    return size;
}

/**释放设备*/
static int cdev_led_release(struct inode *inode, struct file *file)
{
    iounmap(led1.led_reg->ccm_ccgr1);
    iounmap(led1.led_reg->iomuxc_snvs_sw_mux_ctl_pad_snvs_tamper3);
    iounmap(led1.led_reg->gpio5_gdir);
    iounmap(led1.led_reg->gpio5_dr);
    printk("iounmap \n");

    return 0;
}

/*设备操作函数*/
struct file_operations misc_led_fops = {
    .owner = THIS_MODULE,        // 将owner字段指向本模块，可以避免在模块的操作正在被使用时卸载该模块
    .open = cdev_led_open,       // 将open字段指向chrdev_open(...)函数
    .read = cdev_led_read,       // 将open字段指向chrdev_read(...)函数
    .write = cdev_led_write,     // 将open字段指向chrdev_write(...)函数
    .release = cdev_led_release, // 将open字段指向chrdev_release(...)函数
};

static int __init led_module_init(void)
{
    int ret;
    led1.misc_led.minor = MISC_DYNAMIC_MINOR, // 动态申请的次设备号
        led1.misc_led.name = "led1_misc",     // 杂项设备名字是hello_misc
        led1.misc_led.fops = &misc_led_fops,  // 文件操作集

        ret = misc_register(&led1.misc_led); // 在初始化函数中注册杂项设备
    if (ret < 0)
    {
        printk("misc registe is error \n"); // 打印注册杂项设备失败
    }
    printk("misc registe is succeed \n"); // 打印注册杂项设备成功
    return 0;
}

static void __exit led_module_exit(void)
{
    misc_deregister(&led1.misc_led); // 在卸载函数中注销杂项设备
    printk(" misc goodbye! \n");
}

module_init(led_module_init);
module_exit(led_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pusooy");
MODULE_DESCRIPTION("A simple LED control module using char device");