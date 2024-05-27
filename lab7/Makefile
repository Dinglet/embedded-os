all: game guess

game: game.o
	$(CC) $(CFLAGS) -o $@ $^

guess: guess.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f game guess
	rm -f *.o
