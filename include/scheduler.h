#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"

// 3~8. Algorithm
void fcfs                   (Process proc[], int n, GanttEntry gantt[], int *gantt_len);
void sjf_non_preemptive     (Process proc[], int n, GanttEntry gantt[], int *gantt_len);
void sjf_preemptive         (Process proc[], int n, GanttEntry gantt[], int *gantt_len);
void priority_non_preemptive(Process proc[], int n, GanttEntry gantt[], int *gantt_len);
void priority_preemptive    (Process proc[], int n, GanttEntry gantt[], int *gantt_len);
void round_robin            (Process proc[], int n, int quantum,
                             GanttEntry gantt[], int *gantt_len);

#endif
