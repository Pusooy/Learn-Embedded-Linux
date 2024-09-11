PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

main.imx : main.c start.S
	$(CC) -nostdlib -g -c -o start.o start.S
	$(CC) -nostdlib -g -c -o main.o main.c
	
	$(LD) -T ./tools/imx6ull.lds -g main.o start.o -o main.elf 
	
	$(OBJCOPY) -O binary -S main.elf  main.bin
	$(OBJDUMP) -D -m arm  main.elf  > main.dis	

	mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d main.bin main.imx
	cat ./tools/1k.bin main.imx > main.img
	rm -f main.elf *.o main.bin main.hex main.img

clean:
	rm -f main.dis  main.bin main.elf main.imx main.img *.o

