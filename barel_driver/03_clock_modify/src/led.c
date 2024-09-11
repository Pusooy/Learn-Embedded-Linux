#include "../include/led.h"

void led_init()
{
   *(CCM_CCGR1) = 0xFFFFFFFF;                     // 开启GPIO1的时钟
   *(IOMUXC_SW_MUX_CTL_PAD_GPIO5_IO03) = 0x5;     // 设置PAD复用功能为GPIO
   *(IOMUXC_SW_PAD_CTL_PAD_GPIO5_IO03) = 0x1F838; // 设置PAD属性
   *(GPIO5_GDIR) = (1 << 3);                      // 设置GPIO为输出模式
   *(GPIO5_DR) = 0x0;                             // 设置输出电平为低电平
}

void delay(uint32_t second)
{
   uint32_t count = 0x7FFFF * second;
   volatile uint32_t i = 0;
   for (i = 0; i < count; ++i)
   {
      __asm("NOP"); /* 调用nop空指令 */
   }
}

void led_toggle()
{
   // 切换GPIO引脚的状态
   *(GPIO5_DR) ^= (1 << 3);
}
