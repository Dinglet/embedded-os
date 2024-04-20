obj-m := leds.o
obj-m += segments.o

KDIR = /lib/modules/$(shell uname -r)/build

CC = $(CROSS_COMPILE)gcc

all: hw1 driver

hw1: hw1.c Shop.c DeliveryApp.c OrderMenu.c Cart.c
	$(CC) -o $@ $^ -lpthread

driver: leds.c segments.c
ifndef CROSS_COMPILE
	make -C $(KDIR)  M=$(shell pwd) modules
else
	make -C $(KDIR)  M=$(shell pwd) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
endif

clean:
	rm -f hw1
	make -C $(KDIR)  M=$(shell pwd) clean
