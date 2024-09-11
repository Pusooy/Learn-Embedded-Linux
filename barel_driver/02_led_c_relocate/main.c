#define CCM_CCGR1 (volatile unsigned long *)0x20C406C
#define IOMUXC_SW_MUX_CTL_PAD_GPIO5_IO03 (volatile unsigned long *)0x2290014
#define IOMUXC_SW_PAD_CTL_PAD_GPIO5_IO03 (volatile unsigned long *)0x2290058
#define GPIO5_GDIR (volatile unsigned long *)0x020AC004
#define GPIO5_DR (volatile unsigned long *)0x020AC000

#define uint32_t unsigned int

void copy_data();
void led_init();
void __attribute__((section(".delay"))) delay(uint32_t second);

int main()
{

    copy_data();
    led_init();

    while (1)
    {
        *(GPIO5_DR) = 0x0;
        delay(50);
        *(GPIO5_DR) = (1 << 3);
        delay(50);
    }
    return 0;
}

void led_init()
{
    *(CCM_CCGR1) = 0xFFFFFFFF;                     // 开启GPIO1的时钟
    *(IOMUXC_SW_MUX_CTL_PAD_GPIO5_IO03) = 0x5;     // 设置PAD复用功能为GPIO
    *(IOMUXC_SW_PAD_CTL_PAD_GPIO5_IO03) = 0x1F838; // 设置PAD属性
    *(GPIO5_GDIR) = (1 << 3);                      // 设置GPIO为输出模式
    *(GPIO5_DR) = 0x0;                             // 设置输出电平为低电平
}

void __attribute__((section(".delay"))) delay(uint32_t second)
{
    uint32_t count = 0xFFFFF * second;
    volatile uint32_t i = 0;
    for (i = 0; i < count; ++i)
    {
        __asm("NOP"); /* 调用nop空指令 */
    }
}

void copy_data()
{
    extern unsigned long _delay_start_addr;
    extern unsigned long _delay_end_addr;
    extern unsigned long _delay_at_start_addr;
    unsigned long *src, *dst;

    src = &_delay_at_start_addr;
    dst = &_delay_start_addr;
    while (dst < &_delay_end_addr)
    {
        *dst++ = *src++;
    }
}
