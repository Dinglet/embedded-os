CC = $(CROSS_COMPILE)gcc

all: hw2

hw2: hw2.o DeliveryManager.o DeliveryApp.o Shop.o DeliveryApp.o OrderMenu.o Cart.o common.o
	$(CC) $(CFLAGS) -pthread -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f hw2
	rm -f *.o
