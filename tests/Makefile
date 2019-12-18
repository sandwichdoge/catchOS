CC=gcc
CFLAGS=-I../include
OBJECTS=main.o \
		test_string_h.o \
		../utils/string.c

all: main.out

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

main.out: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o *.out