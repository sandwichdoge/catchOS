CC=gcc
CXX=g++
AS=nasm
ASFLAGS=-f elf32
CFLAGS=-I../include -I./googletest/googletest/include -Wno-write-strings -DDEBUG -DTARGET_HOST -g -fsanitize=address -fpermissive
OBJECTS=main.o \
		test_string_utils.o \
		test_printf_utils.o \
		test_bitmap_utils.o \
		test_pageframe_alloc.o \
		test_spinlock.o \
		test_hashtable_utils.o \
		../kernel/utils/string.c \
		../kernel/utils/printf.o \
		../kernel/utils/bitmap.o \
		../kernel/utils/hashtable.o \
		../kernel/utils/list.o \
		../kernel/mmu/pageframe_alloc.o \
		../kernel/mmu/kheap.o \
		../kernel/mmu/mmu.o \
		../kernel/common/kinfo.o \
		../kernel/utils/rwlock.o \
		../kernel/utils/spinlock.o \
		../kernel/utils/atomic.o \
		../kernel/utils/debug.o \
		../kernel/drivers/cpu.o

GTEST_DIR=./googletest/googletest

all: cleanparent buildlib main.out
	./main.out

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@


main.out: $(OBJECTS)
	$(CXX) $(CFLAGS) $^ -o $@ -pthread libgtest.a

buildlib: libgtest.a

cleanparent:
	cd .. && make clean

libgtest.a:
	g++ -std=c++11 -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o
	rm -f gtest-all.o

clean:
	rm -f *.o *.out
