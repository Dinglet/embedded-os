obj-m += lab3-1_driver.o
 
# KDIR = /lib/modules/$(shell uname -r)/build
KDIR = /home/ding/linux

all: driver

driver:
	# make -C $(KDIR)  M=$(shell pwd) modules
	make -C $(KDIR)  M=$(shell pwd) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules

clean:
	# make -C $(KDIR)  M=$(shell pwd) clean
	make -C $(KDIR)  M=$(shell pwd) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- clean
