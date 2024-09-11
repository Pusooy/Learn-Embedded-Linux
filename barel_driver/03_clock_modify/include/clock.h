#ifndef CLOCK_H // 如果未定义 CLOCK_H
#define CLOCK_H // 定义 CLOCK_H

#define uint32_t unsigned int

// CCM Arm Clock Root Register (CACRR)
#define CCM_CACRR (volatile unsigned long *)0x020C4010
#define CCM_CCSR (volatile unsigned long *)0x020C400C
#define CCM_ANALOG_PLL_ARM (volatile unsigned long *)0x020C8000

// 设置pll1_sw_clk->arm_clk_root分频系数为2
void set_arm_podf_2div(void);
// 设置设置step_clk时钟来源为osc_clk
void set_step_sel_osc(void);
// 设置pll1_sw_clk时钟来源: 0则pll1_main_clk 1则osc_clk
void set_pll1_sw_clk_sel(uint32_t value);
// 设置pll1_div_sel分频系数为8,则pll1_main_clk为96MHz
void set_pll1_div_sel_8(void);
// 设置pll1_div_sel分频系数为108,则pll1_main_clk1296MHz
void set_pll1_div_sel_108(void);
// 获取pll1_lock状态, 0则未锁定 1则锁定
uint32_t get_pll1_lock_status(void);

void set_arm_clk_root_81mhz(void);
void set_arm_clk_root_648mhz(void);

#endif // CLOCK_H

/*
24MHz OSC --> ARM_CLK_ROOT 路径：
切换前: OSC -> PLL1 -> pll1_main_clk -> CCSR:pll1_sw_clk_sel -> pll1_sw_clk -> CACRR[ARM_PODF] -> ARM_CLK_ROOT
切换中: OSC -> osc_clk -> CCSR:step_sel -> step_clk -> CCSR:pll1_sw_clk_sel -> pll1_sw_clk -> CACRR[ARM_PODF] -> ARM_CLK_ROOT
切换后: OSC -> PLL1 -> pll1_main_clk -> CCSR:pll1_sw_clk_sel -> pll1_sw_clk -> CACRR[ARM_PODF] -> ARM_CLK_ROOT
*/