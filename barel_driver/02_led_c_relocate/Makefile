PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

main.imx : main.c start.S ./tools/imx6ull.lds
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o main.o main.c
	
	$(LD) -T ./tools/imx6ull.lds -g start.o main.o -o main.elf 
	
	$(OBJCOPY) -O binary -S main.elf  main.bin
	$(OBJCOPY) -O ihex -S main.elf main.hex
	$(OBJDUMP) -D -m arm  main.elf  > main.dis	

	mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d main.bin main.imx
	rm -f *.o main.elf main.bin

clean:
	rm -f main.dis main.imx main.hex

