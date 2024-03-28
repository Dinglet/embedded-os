obj-m += lab3-1_driver.o
 
# KDIR = /lib/modules/$(shell uname -r)/build
KDIR = /home/ding/linux

all: driver writer

driver:
	# make -C $(KDIR)  M=$(shell pwd) modules
	make -C $(KDIR)  M=$(shell pwd) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules

writer: lab3-1_writer.c
	aarch64-linux-gnu-gcc -o writer lab3-1_writer.c

clean:
	# make -C $(KDIR)  M=$(shell pwd) clean
	make -C $(KDIR)  M=$(shell pwd) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- clean
