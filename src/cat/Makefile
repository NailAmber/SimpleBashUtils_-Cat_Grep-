CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: s21_cat

s21_cat: s21_cat.c
	$(CC) $(CFLAGS) $< parser.c -o s21_cat

test:
	sh test.sh

clean:
	-rm -f s21_cat