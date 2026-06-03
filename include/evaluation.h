#ifndef EVALUATION_H
#define EVALUATION_H

#include <stdio.h>
#include "types.h"

void calculate_times  (Process proc[], int n);
void print_evaluation (FILE *fp, Process proc[], int n, const char *algo_name);
void print_comparison (FILE *fp, Process results[][MAX_PROCESSES], int n,
                       const char *names[], int algo_count);

#endif
