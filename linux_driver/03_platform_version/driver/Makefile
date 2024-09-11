obj-m += led_driver.o
PWD ?= $(shell pwd)

all:
	bear make -C $(KDIR) M=$(PWD) modules    #make操作
	echo "Cleaning up intermediate files..."
	find . -type f ! -name 'Makefile' ! -name '*.ko' ! -name '*.c' ! -name '*.h' ! -name 'compile_commands.json' ! -name 'update_makefile.sh' -delete
	find . -name '*.mod.c' -delete
	sed -i 's/cc/arm-buildroot-linux-gnueabihf-gcc/g' compile_commands.json

clean:
	make -C $(KDIR) M=$(PWD) clean    #make clean操作s