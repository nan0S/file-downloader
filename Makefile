CC = gcc
CFLAGS = -Wall -Wextra
DEPS = init.h server.h segment.h
OBJ = transport.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

transport: $(OBJ)
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f *.o
distclean:
	rm -f *.o transport
