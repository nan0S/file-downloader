TARGET = transport
EXENAME = transport

CC = gcc
CFLAGS = -Wall -Wextra
DEPS = init.h server.h segment.h
OBJS = $(TARGET).o

all: $(TARGET)

install: $(TARGET) clean

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXENAME) $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o
distclean: clean
	rm -f $(EXENAME)

.PHONY: clean

