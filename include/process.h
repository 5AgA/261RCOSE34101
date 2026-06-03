#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

void create_process  (Process proc[], int n);
void copy_processes  (Process dst[], Process src[], int n);
void print_processes (Process proc[], int n);

#endif
