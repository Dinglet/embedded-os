obj-m := mydev.o

KDIR = ~/linux

CC = $(CROSS_COMPILE)gcc

all: driver reader writer

driver: mydev.c
ifndef CROSS_COMPILE
	make -C $(KDIR)  M=$(shell pwd) modules
else
	make -C $(KDIR)  M=$(shell pwd) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
endif

reader: reader.c
	$(CC) -o reader reader.c

writer: writer.c
	$(CC) -o writer writer.c

clean:
	make -C $(KDIR)  M=$(shell pwd) clean
	rm -f writer
	rm -f reader
