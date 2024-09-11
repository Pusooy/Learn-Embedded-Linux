#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/io.h> // for devm_ioremap

static volatile void __iomem *ccm_ccgr1;
static volatile void __iomem *iomuxc_mux_ctl;
static volatile void __iomem *iomuxc_pad_ctl;
static volatile void __iomem *gpio5_gdir;
static volatile void __iomem *gpio5_dr;

static int led_probe(struct platform_device *pdev)
{
    struct resource res;

    // 获取设备树节点
    struct device_node *np = pdev->dev.of_node;

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
    ccm_ccgr1 = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
    if (!ccm_ccgr1)
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
    iomuxc_mux_ctl = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
    if (!iomuxc_mux_ctl)
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
    iomuxc_pad_ctl = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
    if (!iomuxc_pad_ctl)
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
    gpio5_gdir = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
    if (!gpio5_gdir)
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
    gpio5_dr = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
    if (!gpio5_dr)
    {
        dev_err(&pdev->dev, "Failed to map gpio5_dr\n");
        return -ENOMEM;
    }
    dev_info(&pdev->dev, "gpio5_dr: start = 0x%lx, end = 0x%lx, size = 0x%lx\n",
             (unsigned long)res.start, (unsigned long)res.end, (unsigned long)resource_size(&res));

    return 0;
}

static const struct of_device_id led_of_match[] = {
    {
        .compatible = "100ask,leds",
    },
    {/* Sentinel */}};

MODULE_DEVICE_TABLE(of, led_of_match);

static struct platform_driver led_driver = {
    .probe = led_probe,
    .driver = {
        .name = "led_driver",
        .of_match_table = led_of_match,
    },
};

module_platform_driver(led_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("LED Driver with Resource Size, Start and End Address Printing");
