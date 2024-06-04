CC = $(CROSS_COMPILE)gcc

all: hw3

hw3: hw3.o DeliveryManager.o DeliveryApp.o Command.o Shop.o Cart.o common.o libtasks.a
	$(CC) $(CFLAGS) -pthread -L. -ltasks -o $@ $^

libtasks.a: Task.o TaskList.o TaskConsumer.o TaskManager.o
	ar rcs $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f hw3
	rm -f *.o *.a
