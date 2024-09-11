PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld
AR=$(PREFIX)ar
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

led.imx : led.S
	$(CC) -nostdlib -g -c -o led.o led.S
	
	$(LD) -T ./tools/imx6ull.lds -g led.o -o led.elf 
	
	$(OBJCOPY) -O binary -S led.elf  led.bin
	$(OBJDUMP) -D -m arm  led.elf  > led.dis	
	mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d led.bin led.imx
	cat ./tools/1k.bin led.imx > led.img
	rm -f led.elf led.bin led.img *.o

clean:
	rm -f led.dis  led.bin led.elf led.imx led.img *.o


	
