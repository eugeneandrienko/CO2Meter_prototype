TEST_SRC=../strings.c strings_test.c
TEST_SRC+=../numbers.c numbers_test.c
TEST_SRC+=tests.c
TEST_OBJS=strings.o strings_test.o
TEST_OBJS+=numbers.o numbers_test.o
TEST_OBJS+=tests.o

.PHONY: tests
tests:
	gcc -Wall -c $(TEST_SRC) -I/usr/include/CUnit/ -I. -I..
	gcc -Wall -o test $(TEST_OBJS) -lcunit
	rm -f $(TEST_OBJS)
	./test
	rm -f ./test

