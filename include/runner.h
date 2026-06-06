#ifndef RUNNER_H
#define RUNNER_H

#include <stdio.h>
#include "types.h"

void clear_results_dir(void);
void run_one  (Process original[], int n, int choice);
void run_all  (Process original[], int n);
void run_batch(int count);

#endif
