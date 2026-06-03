#include <stdlib.h>
#include "../include/types.h"
#include "../include/gantt.h"

static int cmp_arrival(const void *a, const void *b) {
    return ((Process *)a)->arrival_time - ((Process *)b)->arrival_time;
}

void fcfs(Process proc[], int n, GanttEntry gantt[], int *gantt_len) {
    qsort(proc, n, sizeof(Process), cmp_arrival);

    int time = 0;
    *gantt_len = 0;

    for (int i = 0; i < n; i++) {
        if (time < proc[i].arrival_time) {
            gantt[*gantt_len] = (GanttEntry){ -1, time, proc[i].arrival_time };
            (*gantt_len)++;
            time = proc[i].arrival_time;
        }

        gantt[*gantt_len] = (GanttEntry){ proc[i].pid, time, time + proc[i].cpu_burst };
        (*gantt_len)++;
        time += proc[i].cpu_burst;
        proc[i].completion_time = time;
    }
}
