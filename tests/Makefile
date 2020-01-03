CC=gcc
CFLAGS=-I../include
OBJECTS=main.o \
		test_string_util.o \
		test_alloc.o \
		test_algo.o \
		../kernel/utils/string.c

all: main.out
	./main.out

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

main.out: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o *.out