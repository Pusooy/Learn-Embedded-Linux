//时钟控制寄存器
#define CCM_CCGR1 (volatile unsigned long*)0x20C406C
//GPIO1_04复用功能选择寄存器
#define IOMUXC_SW_MUX_CTL_PAD_GPIO5_IO03 (volatile unsigned long*)0x2290014
//PAD属性设置寄存器
#define IOMUXC_SW_PAD_CTL_PAD_GPIO5_IO03 (volatile unsigned long*)0x2290058
//GPIO方向设置寄存器
#define GPIO5_GDIR (volatile unsigned long*)0x020AC004
//GPIO输出状态寄存器
#define GPIO5_DR (volatile unsigned long*)0x020AC000

#define uint32_t  unsigned int

/*简单延时函数*/
void delay(uint32_t count)
{
   volatile uint32_t i = 0;
   for (i = 0; i < count; ++i)
   {
      __asm("NOP"); /* 调用nop空指令 */
   }
}

int main()
{
   *(CCM_CCGR1) = 0xFFFFFFFF;                     //开启GPIO1的时钟
   *(IOMUXC_SW_MUX_CTL_PAD_GPIO5_IO03) = 0x5;     //设置PAD复用功能为GPIO
   *(IOMUXC_SW_PAD_CTL_PAD_GPIO5_IO03) = 0x1F838; //设置PAD属性
   *(GPIO5_GDIR) = (1 << 3);                          //设置GPIO为输出模式
   *(GPIO5_DR) = 0x0;                             //设置输出电平为低电平

   while(1)
   {
      *(GPIO5_DR) = 0x0;
      delay(0xFFFFFF);
      *(GPIO5_DR) = (1 << 3);
      delay(0xFFFFFF);
   }
   return 0;
}