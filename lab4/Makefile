obj-m := mydev.o

KDIR = ~/linux

CC = $(CROSS_COMPILE)gcc

all: driver reader writer

driver:
	# make -C $(KDIR)  M=$(shell pwd) modules
	make -C $(KDIR)  M=$(shell pwd) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules

reader: reader.c
	$(CC) -o reader reader.c

writer: writer.c
	$(CC) -o writer writer.c

clean:
	make -C $(KDIR)  M=$(shell pwd) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- clean
	rm -f writer
	rm -f reader
