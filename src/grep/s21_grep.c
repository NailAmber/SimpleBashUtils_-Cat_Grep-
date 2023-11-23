#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct flags {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
};

int get_options(int argc, char **argv, int *num, int *check, char **templates,
                char **files, struct flags *flag, int *temp_count,
                int *files_count);
int check_temp_files(int files_count, char **files);
char *search_regex_in_line(char *search_line, char *expression,
                           struct flags flag, regmatch_t *pmatch);
char *search_regex(char *search_line, char **templates, char **files,
                   int temp_count, int files_count, struct flags flag,
                   int *pmatch_all);
void sgrep(int argc, struct flags flag, char **argv, char **templates,
           int temp_count, char **files, int files_count);
int get_options_switch(int *num, struct flags *flag, char **templates,
                       int *temp_count, char **files, int *files_count);
void search_from_temp_flag_o(char **result, char **templates, struct flags flag,
                             regmatch_t *pmatch, int *pmatch_all,
                             char *search_line, int i);
int search_from_temp(int temp_count, struct flags flag, char *search_line,
                     char **result, char **templates, regmatch_t *pmatch,
                     int *pmatch_all);
void search_from_file(int files_count, char **files, char *search_line,
                      struct flags flag, regmatch_t *pmatch, char **result,
                      int *pmatch_all);

int main(int argc, char **argv) {
  int num = 0;
  int check = 0;
  struct flags flag = {0};
  char *templates[argc];
  char *files[argc];
  int temp_count = 0;
  int files_count = 0;
  if (!get_options(argc, argv, &num, &check, templates, files, &flag,
                   &temp_count, &files_count) &&
      !check_temp_files(files_count, files)) {
    if (!(flag.v && flag.o && !flag.l && !flag.c)) {
      if (flag.c) {
        flag.n = 0;
        flag.o = 0;
      }

      if (flag.l) {
        flag.o = 0;
      }
      if (flag.v) {
        flag.o = 0;
      }
      sgrep(argc, flag, argv, templates, temp_count, files, files_count);
    }
  }
  return 0;
}

int sgrep_printing(char **res, struct flags flag, char **argv, int many_files,
                   int *line_number, char *line, int *pmatch_all,
                   int *out_count) {
  int find = 1;
  if (*res != NULL) {
    if (flag.l) {
      printf("%s\n", argv[optind]);
      find = 0;
    }
    if (flag.c && find) {
      *out_count += 1;

    } else if (find) {
      if (many_files && !flag.h) {
        printf("%s:", argv[optind]);
      }
      if (flag.n && find) {
        printf("%d:", *line_number);
      }
      printf("%s", *res);
    }
  }

  if (flag.o) {
    for (size_t i = 0; i < strlen(line); i++) {
      if (many_files && pmatch_all[i] > -1 && !flag.c) {
        printf("%s:", argv[optind]);
      }
      if (flag.n && pmatch_all[i] > -1) {
        printf("%d:", *line_number);
      }
      if (pmatch_all[i] > -1) {
        if (!flag.c) {
          printf("%.*s\n", (int)(pmatch_all[i] - i), line + i);
        }
      }
    }
  }
  return find;
}

void sgrep_main_loop(int *line_size, char **line, size_t *buffer_size,
                     FILE **fp, int *line_number, char **res, char **templates,
                     char **files, int temp_count, int files_count,
                     struct flags flag, char **argv, int *out_count,
                     int many_files) {
  int find = 1;
  while ((*line_size = getline(line, buffer_size, *fp)) != EOF && find) {
    *line_number += 1;
    int pmatch_all[strlen(*line)];
    for (size_t i = 0; i < strlen(*line); i++) {
      pmatch_all[i] = -1;
    }
    *res = search_regex(*line, templates, files, temp_count, files_count, flag,
                        pmatch_all);
    find = sgrep_printing(res, flag, argv, many_files, line_number, *line,
                          pmatch_all, out_count);
  }
}

void post_printing(struct flags flag, int many_files, char **argv,
                   int out_count, char *res, FILE *fp) {
  if (flag.c && many_files && !flag.l) {
    printf("%s:", argv[optind]);
  }
  if (flag.c && !flag.l) {
    printf("%d\n", out_count);
  }
  if (!flag.o && !flag.l && !flag.c && res && res[strlen(res) - 1] != '\n') {
    printf("\n");
  }
  fclose(fp);
}

void sgrep(int argc, struct flags flag, char **argv, char **templates,
           int temp_count, char **files, int files_count) {
  int line_size = 0;
  char *line = NULL;
  size_t buffer_size = 0;
  char *res = NULL;
  int many_files = 0;
  if (optind < argc - 1 && !flag.h) {
    many_files = 1;
  }
  for (; optind < argc; optind++) {
    FILE *fp = fopen(argv[optind], "r");
    if (fp != NULL) {
      int out_count = 0;
      int line_number = 0;
      sgrep_main_loop(&line_size, &line, &buffer_size, &fp, &line_number, &res,
                      templates, files, temp_count, files_count, flag, argv,
                      &out_count, many_files);
      post_printing(flag, many_files, argv, out_count, res, fp);
    } else {
      fprintf(stderr, "Error\n");
    }
    free(line);
    line = NULL;
  }
}

int get_options_switch(int *num, struct flags *flag, char **templates,
                       int *temp_count, char **files, int *files_count) {
  int errors = 0;
  switch (*num) {
    case 'e':
      flag->e = 1;
      templates[*temp_count] = optarg;
      *temp_count = *temp_count + 1;
      break;
    case 'i':
      flag->i = 1;
      break;
    case 'v':
      flag->v = 1;
      break;
    case 'c':
      flag->c = 1;
      break;
    case 'l':
      flag->l = 1;
      break;
    case 'n':
      flag->n = 1;
      break;
    case 'h':
      flag->h = 1;
      break;
    case 's':
      flag->s = 1;
      break;
    case 'f':
      flag->f = 1;
      files[*files_count] = optarg;
      *files_count = *files_count + 1;
      break;
    case 'o':
      flag->o = 1;
      break;
    default:
      fprintf(stderr, "Error\n");
      errors++;
  }
  return errors;
}

int get_options(int argc, char **argv, int *num, int *check, char **templates,
                char **files, struct flags *flag, int *temp_count,
                int *files_count) {
  int errors = 0;
  while ((*num = getopt(argc, argv, "e:ivclnhsf:o")) != -1 && !*check) {
    errors += get_options_switch(num, flag, templates, temp_count, files,
                                 files_count);
  }
  if (flag->e == 0 && flag->f == 0 && optind < argc - 1) {
    templates[*temp_count] = argv[optind];
    *temp_count = *temp_count + 1;
    optind++;
  } else if (flag->e == 0 && flag->f == 0) {
    fprintf(stderr, "Error\n");
    errors++;
  }
  return errors;
}

int check_temp_files(int files_count, char **files) {
  int errors = 0;
  FILE *fp = NULL;
  for (int i = 0; i < files_count; i++) {
    fp = fopen(files[i], "r");
    if (fp == NULL) {
      fprintf(stderr, "Error\n");
      errors++;
    } else {
      fclose(fp);
    }
  }
  return errors;
}

char *search_regex_in_line(char *search_line, char *expression,
                           struct flags flag, regmatch_t *pmatch) {
  char *result = NULL;
  regex_t regex_expression;
  int error = 0;
  char message[200];

  if (flag.i) {
    error = regcomp(&regex_expression, expression, REG_EXTENDED | REG_ICASE);

  } else {
    error = regcomp(&regex_expression, expression, REG_EXTENDED);
  }
  if (error != 0) {
    regerror(error, &regex_expression, message, 200);
    printf("%s\n", message);
  } else {
    if ((error = regexec(&regex_expression, search_line, 1, pmatch, 0)) == 0) {
      result = search_line;
    } else if (error != REG_NOMATCH) {
      regerror(error, &regex_expression, message, 200);
    }
    regfree(&regex_expression);
    if (strlen(expression) == 1 && expression[0] == '.' &&
        strlen(search_line) == 1 && search_line[0] == '\n') {
      result = NULL;
    }
  }
  return result;
}

void search_from_temp_flag_o(char **result, char **templates, struct flags flag,
                             regmatch_t *pmatch, int *pmatch_all,
                             char *search_line, int i) {
  char *temp_line = search_line;

  while ((*result = search_regex_in_line(temp_line, templates[i], flag,
                                         pmatch)) != NULL) {
    int len = strlen(search_line) - strlen(temp_line);
    temp_line += pmatch[0].rm_eo;
    if (pmatch_all[pmatch[0].rm_so + len] != -2 &&
        pmatch_all[pmatch[0].rm_so + len] < pmatch[0].rm_eo + len) {
      pmatch_all[pmatch[0].rm_so + len] = pmatch[0].rm_eo + len;
      for (int j = pmatch[0].rm_so + len + 1; j < pmatch[0].rm_eo + len; j++) {
        pmatch_all[j] = -2;
      }
    }
  }
  *result = NULL;
}

int search_from_temp(int temp_count, struct flags flag, char *search_line,
                     char **result, char **templates, regmatch_t *pmatch,
                     int *pmatch_all) {
  int find = 0;
  for (int i = 0; i < temp_count; i++) {
    if (flag.o) {
      search_from_temp_flag_o(result, templates, flag, pmatch, pmatch_all,
                              search_line, i);

    } else {
      *result = search_regex_in_line(search_line, templates[i], flag, pmatch);
      if (*result != NULL && !flag.v && !flag.o) {
        find = 1;
        i = temp_count;
      } else if (*result != NULL && flag.v) {
        *result = NULL;
        find = 1;
        i = temp_count;
      } else if (*result == NULL && flag.v) {
        *result = search_line;
      }
    }
  }

  return find;
}

void search_from_file_flag_o(char *search_line, char **result, char *line,
                             struct flags flag, regmatch_t *pmatch,
                             int *pmatch_all) {
  char *temp_line = search_line;
  while ((*result = search_regex_in_line(temp_line, line, flag, pmatch)) !=
         NULL) {
    int len = strlen(search_line) - strlen(temp_line);
    temp_line += pmatch[0].rm_eo;
    if (pmatch_all[pmatch[0].rm_so + len] != -2 &&
        pmatch_all[pmatch[0].rm_so + len] < pmatch[0].rm_eo + len) {
      pmatch_all[pmatch[0].rm_so + len] = pmatch[0].rm_eo + len;
      for (int j = pmatch[0].rm_so + len + 1; j < pmatch[0].rm_eo + len; j++) {
        pmatch_all[j] = -2;
      }
    }
  }
}

int search_from_file_choose_flag(struct flags flag, char *search_line,
                                 char **result, char *line, regmatch_t *pmatch,
                                 int *pmatch_all) {
  int find = 1;
  if (flag.o) {
    search_from_file_flag_o(search_line, result, line, flag, pmatch,
                            pmatch_all);
    *result = NULL;
  } else {
    *result = search_regex_in_line(search_line, line, flag, pmatch);

    if (*result != NULL && !flag.v && !flag.o) {
      find = 0;
    } else if (*result != NULL && flag.v) {
      *result = NULL;
      find = 0;
    } else if (*result == NULL && flag.v) {
      *result = search_line;
    }
  }
  return find;
}

void search_from_file(int files_count, char **files, char *search_line,
                      struct flags flag, regmatch_t *pmatch, char **result,
                      int *pmatch_all) {
  size_t char_count = 0;
  size_t buff_size = 0;
  for (int i = 0; i < files_count; i++) {
    char *line = NULL;
    FILE *fp = fopen(files[i], "r");
    int find = 1;
    while ((char_count = getline(&line, &buff_size, fp)) != (size_t)-1 &&
           find) {
      if (line[strlen(line) - 1] == '\n') {
        line[strlen(line) - 1] = 0;
      }
      find = search_from_file_choose_flag(flag, search_line, result, line,
                                          pmatch, pmatch_all);
    }
    free(line);
    line = NULL;
    fclose(fp);
  }
}

char *search_regex(char *search_line, char **templates, char **files,
                   int temp_count, int files_count, struct flags flag,
                   int *pmatch_all) {
  char *result = NULL;

  regmatch_t pmatch[1];

  if (!search_from_temp(temp_count, flag, search_line, &result, templates,
                        pmatch, pmatch_all)) {
    search_from_file(files_count, files, search_line, flag, pmatch, &result,
                     pmatch_all);
  }
  return result;
}