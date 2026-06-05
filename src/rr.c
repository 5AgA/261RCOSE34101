#include <limits.h>
#include "../include/types.h"
#include "../include/queue.h"
#include "../include/gantt.h"

// 라운드 로빈 - quantum: 최대 점유 시간
void round_robin(Process proc[], int n, int quantum, GanttEntry gantt[], int *gantt_len) {
    Queue ready_q;
    queue_init(&ready_q);

    int time = 0, completed = 0;
    int in_queue[MAX_PROCESSES] = {0};
    *gantt_len = 0;

    // 도착 순서대로 정렬
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (proc[i].arrival_time > proc[j].arrival_time) {
                Process tmp = proc[i]; proc[i] = proc[j]; proc[j] = tmp;
            }

    while (completed < n) {
        // 도착한 프로세스 체크
        for (int i = 0; i < n; i++) {
            if (!in_queue[i] && proc[i].arrival_time <= time &&
                proc[i].remaining_cpu > 0) {
                enqueue(&ready_q, &proc[i]);
                in_queue[i] = 1;
            }
        }

        // 도착한 프로세스가 없다면
        if (queue_empty(&ready_q)) {
            int next = INT_MAX;
            // 다음 도착할 프로세스로 건너뛰기
            for (int i = 0; i < n; i++)
                if (!in_queue[i] && proc[i].remaining_cpu > 0 &&
                    proc[i].arrival_time < next)
                    next = proc[i].arrival_time;
            gantt[*gantt_len] = (GanttEntry){ -1, time, next };
            (*gantt_len)++;
            time = next;
            continue;
        }

        // 실행 시킬 프로세스 디큐
        Process *cur = dequeue(&ready_q);
        int run = (cur->remaining_cpu < quantum) ? cur->remaining_cpu : quantum;

        gantt[*gantt_len] = (GanttEntry){ cur->pid, time, time + run };
        (*gantt_len)++;
        time               += run;
        cur->remaining_cpu -= run;

        // quantum 실행 중에 도착한 프로세스 체크
        for (int i = 0; i < n; i++) {
            if (!in_queue[i] && proc[i].arrival_time <= time &&
                proc[i].remaining_cpu > 0) {
                enqueue(&ready_q, &proc[i]);
                in_queue[i] = 1;
            }
        }

        // 작업을 마쳤을 경우, 완료 표시
        if (cur->remaining_cpu == 0) {
            cur->completion_time = time;
            completed++;
        } else {
        // 작업 시간이 더 남을 경우, 큐 뒤에 추가
            enqueue(&ready_q, cur);
        }
    }
}
