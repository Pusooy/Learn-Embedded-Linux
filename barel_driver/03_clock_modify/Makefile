PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

# 自动查找 src 目录下的所有 .c 文件
C_SRCS := $(wildcard src/*.c)
# 将 .c 文件转换为对应的 .o 文件
C_OBJS := $(C_SRCS:.c=.o)

# 汇编文件的目标
ASM_SRCS := src/start.S
ASM_OBJS := $(ASM_SRCS:.S=.o)

# 最终目标
main.imx : $(ASM_OBJS) $(C_OBJS) main.o
	$(LD) -T ./tools/imx6ull.lds -g $(ASM_OBJS) main.o $(C_OBJS) -o main.elf -lgcc -L D:\linux\learn\imx6ull_barel_develop_on_windows\cortexA7_windows_tools\gcc-arm-linux-gnueabihf-6.2.1\lib\gcc\arm-linux-gnueabihf\6.2.1
	
	$(OBJCOPY) -O binary -S main.elf main.bin
	$(OBJDUMP) -D -m arm main.elf > main.dis	
	
	mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d main.bin main.imx
	rm -f *.o src/*.o main.elf main.bin

# 编译 C 文件
$(C_OBJS): %.o : %.c
	$(CC) -nostdlib -g -c -o $@ $<

# 编译汇编文件
$(ASM_OBJS): %.o : %.S
	$(CC) -nostdlib -g -c -o $@ $<

# 编译主文件
main.o: main.c
	$(CC) -nostdlib -g -c -o $@ $<

clean:
	rm -f main.dis main.imx main.hex $(C_OBJS) $(ASM_OBJS) main.o
