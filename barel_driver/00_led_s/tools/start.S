	
.text
.global  _start
_start: 				

//设置栈
	ldr  sp,=0x80100000

	bl clean_bss
	
	bl main

halt:
	b  halt 

clean_bss:
	/* 清除BSS段 */
	ldr r1, =__bss_start
	ldr r2, =__bss_end
	mov r3, #0
clean:
	str r3, [r1]
	add r1, r1, #4
	cmp r1, r2
	bne clean
	
	mov pc, lr
