/*************************第一部分*************************/
.text            //代码段
.align 2         //设置字节对齐
.global _start   //定义全局变量

/*************************第二部分*************************/
_start:          //程序的开始
   b reset      //跳转到reset标号处

/*************************第三部分*************************/
reset:
   mrc     p15, 0, r0, c1, c0, 0     /*读取CP15系统控制寄存器   */
   bic     r0,  r0, #(0x1 << 12)     /*  清除第12位（I位）禁用 I Cache  */
   bic     r0,  r0, #(0x1 <<  2)     /*  清除第 2位（C位）禁用 D Cache  */
   bic     r0,  r0, #0x2             /*  清除第 1位（A位）禁止严格对齐   */
   bic     r0,  r0, #(0x1 << 11)     /*  清除第11位（Z位）分支预测   */
   bic     r0,  r0, #0x1             /*  清除第 0位（M位）禁用 MMU   */
   mcr     p15, 0, r0, c1, c0, 0     /*  将修改后的值写回CP15寄存器   */

/*************************第四部分*************************/
   /*跳转到light_led函数*/
   bl light_led
   /*进入死循环*/
 /*************************第五部分*************************/
loop:
   b loop


/*************************第六部分*************************/
/*CCM_CCGR1 时钟使能寄存器地址，默认时钟全部开启*/
#define gpio1_clock_enible_ccm_ccgr1  0x20C406C


/*IOMUXC_SW_MUX_CTL_PAD_GPIO5_IO03
寄存器地址，用于设置GPIO1_iIO04的复用功能*/
#define gpio5_io03_mux_ctl_register  0x2290014
/*IOMUXC_SW_PAD_CTL_PAD_GPIO5_IO03寄存器地址，用于设置GPIO的PAD属性*/
#define gpio5_io03_pad_ctl_register  0x2290058


/*GPIO5_GDIR寄存器，用于设置GPIO为输入或者输出*/
#define  gpio5_gdir_register  0x020AC004
/*GPIO5_DR寄存器，用于设置GPIO输出的电平状态*/
#define  gpio5_dr_register  0x020AC000



/*************************第七部分*************************/
light_led:
   /*开启GPIO5的时钟*/
   ldr r0, =gpio1_clock_enible_ccm_ccgr1
   ldr r1, =0xFFFFFFFF
   str r1, [r0]

/*************************第八部分*************************/
   /*将PAD引脚复用为GPIO*/
   ldr r0, =gpio5_io03_mux_ctl_register
   ldr r1, =0x5
   str r1, [r0]

/*************************第九部分*************************/
   /*设置GPIO PAD属性*/
   ldr r0, =gpio5_io03_pad_ctl_register
   ldr r1, =0x1F838
   str r1, [r0]
/*************************第十部分*************************/
   /*将GPIO_GDIR.[3] 设置为1， gpio5_io03设置为输出模式*/
   ldr r0, =gpio5_gdir_register
   ldr r1, =0x8
   str r1, [r0]

/*************************第十一部分*************************/
   /*将GPIO_DR 设置为0， gpio5全部输出为低电平*/
   ldr r0, =gpio5_dr_register
   ldr r1, =0x0
   str r1, [r0]

/*************************第十二部分*************************/
   /*跳出light_led函数，返回跳转位置*/
   mov pc, lr