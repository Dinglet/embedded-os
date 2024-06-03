CC = $(CROSS_COMPILE)gcc

all: hw3

hw3: hw3.o DeliveryManager.o DeliveryApp.o Command.o Shop.o Cart.o common.o
	$(CC) $(CFLAGS) -pthread -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f hw3
	rm -f *.o
