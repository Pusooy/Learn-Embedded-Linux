#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>

// LED寄存器基地址和偏移
#define CCM_CCGR1_BASE 0x20C406C
#define IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 0x2290014
#define GPIO5_BASE 0x020AC000
#define GPIO5_GDIR_OFFSET 0x4
#define GPIO5_DR_OFFSET 0x0

// LED资源数组
static struct resource led_resources[] = {
    {
        .start = CCM_CCGR1_BASE,
        .end = CCM_CCGR1_BASE + 0x4 - 1,
        .flags = IORESOURCE_MEM,
    },
    {
        .start = IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3,
        .end = IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 + 0x4 - 1,
        .flags = IORESOURCE_MEM,
    },
    {
        .start = GPIO5_BASE + GPIO5_GDIR_OFFSET,
        .end = GPIO5_BASE + GPIO5_GDIR_OFFSET + 0x4 - 1,
        .flags = IORESOURCE_MEM,
    },
    {
        .start = GPIO5_BASE + GPIO5_DR_OFFSET,
        .end = GPIO5_BASE + GPIO5_DR_OFFSET + 0x4 - 1,
        .flags = IORESOURCE_MEM,
    }};

// 定义平台设备
static struct platform_device led_device = {
    .name = "led_platform_device",
    .id = -1,
    .num_resources = ARRAY_SIZE(led_resources),
    .resource = led_resources,
};

// 初始化平台设备
static int __init led_device_init(void)
{
    int ret = platform_device_register(&led_device);
    if (ret)
        printk("Failed to register platform device\n");
    else
        printk("Platform device registered successfully\n");

    return ret;
}

// 退出时注销平台设备
static void __exit led_device_exit(void)
{
    platform_device_unregister(&led_device);
    printk("Platform device unregistered\n");
}

module_init(led_device_init);
module_exit(led_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pusooy");
MODULE_DESCRIPTION("LED Platform Device");
