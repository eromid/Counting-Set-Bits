MAKE=make
CC=gcc
CFLAGS=-Wall -Werror -Wextra -O3 -std=c99
TARGET=count_bits
CLEAN_PATTERNS=*.o

all: count_bits

count_bits: count_bits.o
	$(CC) $(CFLAGS) -o $(TARGET) count_bits.o $(OBJECTS) $(LINKED_LIBS)

count_bits.o: count_bits.c
	$(CC) $(CFLAGS) -c -o count_bits.o count_bits.c

sclean:
	rm -f $(CLEAN_PATTERNS)

veryclean: clean
	rm -f $(TARGET)
