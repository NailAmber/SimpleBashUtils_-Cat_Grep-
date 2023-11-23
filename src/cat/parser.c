#include <string.h>

struct flags {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
};

int single_option(struct flags *flag, char **argv, int elem) {
  int error = 0;
  for (unsigned int count = 1; count < strlen(argv[elem]); count++) {
    switch (argv[elem][count]) {
      case 'b':
        flag->b = 1;
        break;
      case 'E':
        flag->e = 1;
        break;
      case 'v':
        flag->v = 1;
        break;
      case 'e':
        flag->e = 1;
        flag->v = 1;
        break;
      case 'n':
        flag->n = 1;
        break;
      case 's':
        flag->s = 1;
        break;
      case 'T':
        flag->t = 1;
        break;
      case 't':
        flag->t = 1;
        flag->v = 1;
        break;
      default:
        error++;
        break;
    }
  }
  argv[elem] = "\0";
  return error;
}

int parser(struct flags *flag, int argc, char **argv) {
  int error = 0;
  for (int i = 1; i < argc; i++) {
    if (strlen(argv[i]) > 1 && argv[i][0] == '-' && argv[i][1] == '-') {
      if (!strcmp(argv[i], "--number-nonblank")) {
        flag->b = 1;
        argv[i] = "\0";
        continue;
      }
      if (!strcmp(argv[i], "--number")) {
        flag->n = 1;
        argv[i] = "\0";
        continue;
      }
      if (!strcmp(argv[i], "--squeeze-blank")) {
        flag->s = 1;
        argv[i] = "\0";
        continue;
      }
      if (!strcmp(argv[i], "--")) {
        continue;
      }
      error++;
    } else if (argv[i][0] == '-')
      error += single_option(flag, argv, i);
  }
  return error;
}
