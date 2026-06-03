#include <limits.h>
#include "../include/types.h"
#include "../include/gantt.h"

void priority_non_preemptive(Process proc[], int n, GanttEntry gantt[], int *gantt_len) {
    int completed = 0, time = 0;
    int visited[MAX_PROCESSES] = {0};
    *gantt_len = 0;

    while (completed < n) {
        int sel = -1, highest = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!visited[i] && proc[i].arrival_time <= time &&
                proc[i].priority < highest) {
                highest = proc[i].priority;
                sel = i;
            }
        }

        if (sel == -1) {
            int next = INT_MAX;
            for (int i = 0; i < n; i++)
                if (!visited[i] && proc[i].arrival_time < next)
                    next = proc[i].arrival_time;
            gantt[*gantt_len] = (GanttEntry){ -1, time, next };
            (*gantt_len)++;
            time = next;
            continue;
        }

        gantt[*gantt_len] = (GanttEntry){
            proc[sel].pid, time, time + proc[sel].cpu_burst
        };
        (*gantt_len)++;
        time += proc[sel].cpu_burst;
        proc[sel].completion_time = time;
        visited[sel] = 1;
        completed++;
    }
}

void priority_preemptive(Process proc[], int n, GanttEntry gantt[], int *gantt_len) {
    int completed = 0, time = 0, prev_pid = -2;
    *gantt_len = 0;

    while (completed < n) {
        int sel = -1, highest = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (proc[i].arrival_time <= time && proc[i].remaining_cpu > 0 &&
                proc[i].priority < highest) {
                highest = proc[i].priority;
                sel = i;
            }
        }

        if (sel == -1) {
            if (*gantt_len > 0 && gantt[*gantt_len - 1].pid == -1)
                gantt[*gantt_len - 1].end++;
            else {
                gantt[*gantt_len] = (GanttEntry){ -1, time, time + 1 };
                (*gantt_len)++;
            }
            time++;
            continue;
        }

        if (proc[sel].pid != prev_pid) {
            gantt[*gantt_len] = (GanttEntry){ proc[sel].pid, time, time + 1 };
            (*gantt_len)++;
        } else {
            gantt[*gantt_len - 1].end++;
        }

        proc[sel].remaining_cpu--;
        time++;
        prev_pid = proc[sel].pid;

        if (proc[sel].remaining_cpu == 0) {
            proc[sel].completion_time = time;
            completed++;
        }
    }
}
