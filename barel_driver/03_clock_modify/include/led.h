#ifndef LED_H // 如果未定义 LED_H
#define LED_H // 定义 LED_H

// 时钟控制寄存器
#define CCM_CCGR1 (volatile unsigned long *)0x20C406C
// GPIO1_04 复用功能选择寄存器
#define IOMUXC_SW_MUX_CTL_PAD_GPIO5_IO03 (volatile unsigned long *)0x2290014
// PAD 属性设置寄存器
#define IOMUXC_SW_PAD_CTL_PAD_GPIO5_IO03 (volatile unsigned long *)0x2290058
// GPIO 方向设置寄存器
#define GPIO5_GDIR (volatile unsigned long *)0x020AC004
// GPIO 输出状态寄存器
#define GPIO5_DR (volatile unsigned long *)0x020AC000

#define uint32_t unsigned int

void led_init();
void delay(uint32_t second);
void led_toggle();

#endif // LED_H
