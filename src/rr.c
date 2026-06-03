#include <limits.h>
#include "../include/types.h"
#include "../include/queue.h"
#include "../include/gantt.h"

void round_robin(Process proc[], int n, int quantum, GanttEntry gantt[], int *gantt_len) {
    Queue ready_q;
    queue_init(&ready_q);

    int time = 0, completed = 0;
    int in_queue[MAX_PROCESSES] = {0};
    *gantt_len = 0;

    /* sort by arrival so we enqueue in order */
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (proc[i].arrival_time > proc[j].arrival_time) {
                Process tmp = proc[i]; proc[i] = proc[j]; proc[j] = tmp;
            }

    while (completed < n) {
        /* enqueue all processes that have arrived */
        for (int i = 0; i < n; i++) {
            if (!in_queue[i] && proc[i].arrival_time <= time &&
                proc[i].remaining_cpu > 0) {
                enqueue(&ready_q, &proc[i]);
                in_queue[i] = 1;
            }
        }

        if (queue_empty(&ready_q)) {
            int next = INT_MAX;
            for (int i = 0; i < n; i++)
                if (!in_queue[i] && proc[i].remaining_cpu > 0 &&
                    proc[i].arrival_time < next)
                    next = proc[i].arrival_time;
            gantt[*gantt_len] = (GanttEntry){ -1, time, next };
            (*gantt_len)++;
            time = next;
            continue;
        }

        Process *cur = dequeue(&ready_q);
        int run = (cur->remaining_cpu < quantum) ? cur->remaining_cpu : quantum;

        gantt[*gantt_len] = (GanttEntry){ cur->pid, time, time + run };
        (*gantt_len)++;
        time               += run;
        cur->remaining_cpu -= run;

        /* enqueue processes that arrived during this slice */
        for (int i = 0; i < n; i++) {
            if (!in_queue[i] && proc[i].arrival_time <= time &&
                proc[i].remaining_cpu > 0) {
                enqueue(&ready_q, &proc[i]);
                in_queue[i] = 1;
            }
        }

        if (cur->remaining_cpu == 0) {
            cur->completion_time = time;
            completed++;
        } else {
            enqueue(&ready_q, cur);
        }
    }
}
