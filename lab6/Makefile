all: server client

server: server.c
	$(CC) $(CFLAGS) -pthread -o $@ $^

client: client.c
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f server client
	rm -f *.o
