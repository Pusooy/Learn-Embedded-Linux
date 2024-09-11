#include "../include/clock.h"

void set_arm_podf_2div(void)
{
    *(CCM_CACRR) = 0x00000001;
}

void set_step_sel(uint32_t value)
{
    uint32_t reg_value = *(CCM_CCSR) & ~((0xFF) << 8); // 清除旧值
    *(CCM_CCSR) = reg_value | (value << 8);            // 设置新值
}

void set_pll1_sw_clk_sel(uint32_t value)
{
    uint32_t reg_value = *(CCM_CCSR) & ~((0x3) << 2); // 清除旧值
    *(CCM_CCSR) = reg_value | (value << 2);           // 设置新值
}

void set_pll1_div_sel_8(void)
{
    uint32_t reg_value = *(CCM_ANALOG_PLL_ARM) & ~0x7F; // 清除低7位
    *(CCM_ANALOG_PLL_ARM) = reg_value | 8;              // 设置新值8
}

void set_pll1_div_sel_108(void)
{
    uint32_t reg_value = *(CCM_ANALOG_PLL_ARM) & ~0x7F; // 清除低7位
    *(CCM_ANALOG_PLL_ARM) = reg_value | 108;            // 设置新值108
}

uint32_t get_pll1_lock_status(void)
{
    unsigned long value = *CCM_ANALOG_PLL_ARM;
    unsigned long highest_bit = (value >> 31) & 0x1;
    return highest_bit;
}

void set_arm_clk_root_81mhz(void)
{
    set_arm_podf_2div();
    set_step_sel(0);
    set_pll1_sw_clk_sel(1);
    set_pll1_div_sel_8();
    while (get_pll1_lock_status() == 0)
        ;
    set_pll1_sw_clk_sel(0);
}

void set_arm_clk_root_648mhz(void)
{
    set_arm_podf_2div();
    set_step_sel(0);
    set_pll1_sw_clk_sel(1);
    set_pll1_div_sel_108();
    while (get_pll1_lock_status() == 0)
        ;
    set_pll1_sw_clk_sel(0);
}
