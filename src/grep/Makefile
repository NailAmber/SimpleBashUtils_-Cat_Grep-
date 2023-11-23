CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: s21_grep

s21_grep: s21_grep.c
	$(CC) $(CFLAGS) $< -o s21_grep

test: 
	sh test.sh

clean:
	-rm -f s21_grep
	