CC = $(CROSS_COMPILE)gcc

all: hw1

hw1: hw1.c Shop.c DeliveryApp.c OrderMenu.c Cart.c
	$(CC) -o $@ $^ -lpthread

clean:
	rm -f hw1
