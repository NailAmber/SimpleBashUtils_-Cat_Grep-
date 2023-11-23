#ifndef PARSER_H
#define PARSER_H

struct flags {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
};

int parser(struct flags *flag, int argc, char **argv);
int single_option(struct flags *flag, char **argv, int elem);

#endif