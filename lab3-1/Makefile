obj-m += lab3-1_driver.o

KDIR = /lib/modules/$(shell uname -r)/build

CC = $(CROSS_COMPILE)gcc

all: driver writer

driver: lab3-1_driver.c
ifndef CROSS_COMPILE
	make -C $(KDIR)  M=$(shell pwd) modules
else
	make -C $(KDIR)  M=$(shell pwd) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
endif

writer: lab3-1_writer.c
	$(CC) -o writer lab3-1_writer.c

clean:
	make -C $(KDIR)  M=$(shell pwd) clean
	rm -f writer
