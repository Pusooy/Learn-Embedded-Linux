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

#define CCM_CCGR1_BASE                             0x20C406C
#define IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3    0x2290014
#define GPIO5_BASE                                 0x020AC000
#define GPIO5_GDIR_OFFSET                          0x4
#define GPIO5_DR_OFFSET                            0x0

// 描述LED相关寄存器结构体
struct led1_reg {
    void __iomem *ccm_ccgr1;
    void __iomem *iomuxc_snvs_sw_mux_ctl_pad_snvs_tamper3;
    void __iomem *gpio5_gdir;
    void __iomem *gpio5_dr;
};

// 描述设备结构体
struct device_led {
    dev_t dev_num; // 设备号
    int major;
    int minor;
    bool status;
    struct cdev cdev_led; // cdev,led的字符设备结构体
    struct class *class;   // /sys/class 下的类
    struct device *device; // /dev 下的设备
    struct led1_reg *led_reg; // LED相关寄存器
};

struct led1_reg tem_reg;
struct device_led led1;

/*打开设备函数*/
static int cdev_led_open(struct inode *inode, struct file *file)
{
    unsigned int val = 0;
    file->private_data=&led1;//设置私有数据

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
    if (kbuf[0] == '1'){
        /* Set GPIO5_IO03 to low level */
        val &= ~(1 << 3);
        led1.status = true;
    }
    else if (kbuf[0] == '0'){
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
    if(led1.status)
        kbuf[0] = '1';
    else
        kbuf[0] = '0';
    if(copy_to_user(buf, kbuf, 1))
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
struct file_operations cdev_led_fops = {
    .owner = THIS_MODULE, //将owner字段指向本模块，可以避免在模块的操作正在被使用时卸载该模块
    .open = cdev_led_open, //将open字段指向chrdev_open(...)函数
    .read = cdev_led_read, //将open字段指向chrdev_read(...)函数
    .write = cdev_led_write, //将open字段指向chrdev_write(...)函数
    .release = cdev_led_release, //将open字段指向chrdev_release(...)函数
};

static int __init led_module_init(void)
{
    int ret;
    ret = alloc_chrdev_region(&led1.dev_num,0,1,"led_on_borad"); // 申请设备号 lsmod
    if (ret < 0)
        printk("alloc_chrdev_region is %d\n", ret);
    else
        printk("alloc_chrdev_region is ok\n");
    
    led1.major = MAJOR(led1.dev_num); //获取主设备号
    led1.minor = MINOR(led1.dev_num); //获取次设备号
    printk("major is %d \r\n", led1.major); //打印主设备号
    printk("minor is %d \r\n", led1.minor); //打印次设备号

    cdev_init(&led1.cdev_led,&cdev_led_fops); // 初始化字符设备结构体
    ret = cdev_add(&led1.cdev_led, led1.dev_num, 1); // 添加字符设备到文件系统 cat /proc/devices
    if (ret < 0)
        printk("cdev_init is %d\n", ret);
    else
        printk("cdev_init is ok\n");

    led1.class = class_create(THIS_MODULE, "led_class"); // 添加类到文件系统 ls /sys/class/[class_name]
    if(IS_ERR(led1.class))
    {
        ret=PTR_ERR(led1.class);
        printk("class_create is %d\n", ret);
    }
    else
        printk("class_create is ok\n");

    led1.device = device_create(led1.class, NULL, led1.dev_num, NULL, "led1"); // 添加设备文件到文件系统 ls /sys/class/[class_name]/[device_name]   ls /dev
    if(IS_ERR(led1.device))
    {
        ret=PTR_ERR(led1.device);
        printk("device_create is %d\n", ret);
    }
    else
        printk("device_create is ok\n");

    return 0;
}

static void __exit led_module_exit(void)
{
    device_destroy(led1.class, led1.dev_num);
    printk("device_destroy \n");

    class_destroy(led1.class);
    printk("class_destroy \n");

    cdev_del(&led1.cdev_led);
    printk("cdev_del \n");

    unregister_chrdev_region(led1.dev_num, 1);
    printk("unregister_chrdev_region \n");

}

module_init(led_module_init);
module_exit(led_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pusooy");
MODULE_DESCRIPTION("A simple LED control module using char device");