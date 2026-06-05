#include <stdlib.h>
#include "../include/types.h"
#include "../include/gantt.h"

static int cmp_arrival(const void *a, const void *b) {
    // 음수면 a가 빠른 것, 양수면 b가 빠른 것
    return ((Process *)a)->arrival_time - ((Process *)b)->arrival_time;
}

void fcfs(Process proc[], int n, GanttEntry gantt[], int *gantt_len) {
    // 도착 순서대로 퀵 sort
    qsort(proc, n, sizeof(Process), cmp_arrival);

    int time = 0;
    *gantt_len = 0;

    for (int i = 0; i < n; i++) {
        // 도착 전까지 idle
        if (time < proc[i].arrival_time) {
            // 다음 도착할 프로세스까지 건너 뛰기
            gantt[*gantt_len] = (GanttEntry){ -1, time, proc[i].arrival_time };
            (*gantt_len)++;
            time = proc[i].arrival_time;
        }

        // 도착 후에 
        gantt[*gantt_len] = (GanttEntry){ proc[i].pid, time, time + proc[i].cpu_burst };
        (*gantt_len)++;
        time += proc[i].cpu_burst;
        proc[i].completion_time = time;
    }
}
