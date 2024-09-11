SECTIONS {
    . = 0x80100000;  /* 起始地址 */

    . = ALIGN(4);
    .text : 
    {
        *(.text)  /* 主程序代码 */
    }

    . = ALIGN(4);
    .data : 
    {
        *(.data)  /* 已初始化的数据 */
    }

    . = ALIGN(4);
    .bss : 
    {
        *(.bss)  /* 未初始化的数据 */
    }

    . = ALIGN(4);
    _delay_at_start_addr = .;  /* 记录延迟段的装载地址 */
    
    .delay 0x00900000 : AT(_delay_at_start_addr) 
    {
        _delay_start_addr = .;  /* 延迟段的起始地址 */
        *(.delay)               /* 延迟段内容 */
        _delay_end_addr = .;    /* 延迟段的结束地址 */
    }
}
