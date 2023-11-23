#include <stdio.h>
#include <string.h>

#include "parser.h"

int sprint(const char *filename, struct flags flag, int *string_number,
           int *fill);
void allprint(struct flags flag, int argc, char **argv, int *fill,
              int *string_number, int *error);
void sqeuuze_func(FILE *fp, struct flags flag, char current_char,
                  char *next_char, int *fill, int *string_number);
void dollar_func(struct flags flag, char current_char);
void print_symbol(struct flags flag, char current_char);
void print_numbers(struct flags flag, char current_char, char next_char,
                   int *string_number);
void post_printing(struct flags flag, char current_char, int *fill);

int main(int argc, char **argv) {
  struct flags flag = {0};
  if (argc > 1) {
    int error = parser(&flag, argc, argv);
    int string_number = 1;
    int fill = 0;
    if (flag.b) {
      flag.n = 0;
    }
    if (!(error > 0)) {
      allprint(flag, argc, argv, &fill, &string_number, &error);
    }
  }

  return 0;
}

void allprint(struct flags flag, int argc, char **argv, int *fill,
              int *string_number, int *error) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '\0') {
      *error += sprint(argv[i], flag, string_number, fill);
    }
  }
}

int sprint(const char *filename, struct flags flag, int *string_number,
           int *fill) {
  int error = 0;
  FILE *fp;

  fp = fopen(filename, "r");

  if (fp == NULL) {
    error++;
  } else {
    char current_char = '\0';
    current_char = fgetc(fp);
    char next_char = '\0';
    if ((!*fill && (flag.n || (flag.b && current_char != '\n')) &&
         current_char != EOF)) {
      printf("%6d\t", *string_number);
      *string_number = *string_number + 1;
    }
    while ((next_char = fgetc(fp)) != EOF) {
      sqeuuze_func(fp, flag, current_char, &next_char, fill, string_number);
      dollar_func(flag, current_char);
      print_symbol(flag, current_char);
      if (current_char != '\n') {
        *fill = 1;
      }
      print_numbers(flag, current_char, next_char, string_number);
      current_char = next_char;
    }
    post_printing(flag, current_char, fill);
    fclose(fp);
  }
  return error;
}

void sqeuuze_func(FILE *fp, struct flags flag, char current_char,
                  char *next_char, int *fill, int *string_number) {
  if (flag.s && current_char == '\n' && *next_char == '\n') {
    while (*next_char == '\n') {
      *next_char = fgetc(fp);
    }
    if (*fill) {
      if (flag.e) {
        putchar('$');
      }
      if (!flag.n) {
        putchar('\n');
      }
    }
    *fill = 1;
    if (flag.n) {
      printf("\n%6d\t", *string_number);
      *string_number = *string_number + 1;
    }
  }
}

void dollar_func(struct flags flag, char current_char) {
  if (flag.e && current_char == '\n') {
    putchar('$');
  }
}

void print_symbol(struct flags flag, char current_char) {
  if (flag.t && current_char == '\t') {
    printf("^I");
  } else if (flag.v && current_char != '\n' && current_char != '\t' &&
             (current_char >= 0 && current_char < 32)) {
    printf("^%c", (current_char + 64));
  } else if (current_char > 126 && flag.v) {
    printf("^%c", (current_char - 64));

  } else if ((current_char < 0) && current_char != '\n' &&
             current_char != '\t' && flag.v) {
    if ((current_char + 62 + 66 >= 0 && current_char + 62 + 66 < 32) ||
        current_char + 62 + 66 > 126) {
      printf("M-^%c", current_char + 62 + 66 + 64);
    } else
      printf("M-%c", current_char + 62 + 66);
  } else {
    putchar(current_char);
  }
}

void print_numbers(struct flags flag, char current_char, char next_char,
                   int *string_number) {
  if (flag.n && current_char == '\n' && next_char != EOF) {
    printf("%6d\t", *string_number);
    *string_number = *string_number + 1;
  } else if (flag.b && current_char == '\n' && next_char != EOF &&
             next_char != '\n') {
    printf("%6d\t", *string_number);
    *string_number = *string_number + 1;
  }
}

void post_printing(struct flags flag, char current_char, int *fill) {
  if (current_char == '\n' && flag.e) {
    putchar('$');
  }
  if (current_char != EOF) {
    putchar(current_char);
  }
  if (current_char == '\n') {
    *fill = 0;
  } else {
    *fill = 1;
  }
}