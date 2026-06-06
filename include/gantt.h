#ifndef GANTT_H
#define GANTT_H

#include <stdio.h>
#include "types.h"

void print_gantt   (FILE *fp, GanttEntry gantt[], int len);
void print_io_gantt(FILE *fp, GanttEntry io_gantt[], int io_gantt_len, Process proc[], int n);

#endif
