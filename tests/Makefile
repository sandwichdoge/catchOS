CFLAGS=-I../include -I./googletest/googletest/include
OBJECTS=main.o \
		test_string_utils.o \
		../kernel/utils/string.c
GTEST_DIR=./googletest/googletest

all: buildlib main.out
	./main.out

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

main.out: $(OBJECTS)
	$(CXX) $(CFLAGS) $^ -o $@ -pthread libgtest.a

buildlib: libgtest.a

libgtest.a:
	g++ -std=c++11 -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o
	rm -f gtest-all.o

clean:
	rm -f *.o *.out
