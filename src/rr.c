#include <limits.h>
#include "../include/types.h"
#include "../include/queue.h"
#include "../include/gantt.h"

void round_robin(Process proc[], int n, int quantum, GanttEntry gantt[], int *gantt_len, GanttEntry io_gantt[], int *io_gantt_len) {
    Queue ready_q;
    queue_init(&ready_q);

    int time = 0, completed = 0;
    int in_queue[MAX_PROCESSES] = {0};
    int io_start[MAX_PROCESSES] = {0};
    *gantt_len = 0;
    *io_gantt_len = 0;

    // 도착 순서대로 정렬
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (proc[i].arrival_time > proc[j].arrival_time) {
                Process tmp = proc[i]; proc[i] = proc[j]; proc[j] = tmp;
            }

    while (completed < n) {

        // 도착한 프로세스 레디큐에 대기 (WAITING/TERMINATED 상태 제외)
        for (int i = 0; i < n; i++) {
            if (!in_queue[i] &&
                proc[i].arrival_time <= time &&
                proc[i].state != WAITING &&
                proc[i].state != TERMINATED &&
                proc[i].remaining_cpu > 0) {
                enqueue(&ready_q, &proc[i]);
                in_queue[i] = 1;
            }
        }

        // 도착한 프로세스가 없다면 idle
        if (queue_empty(&ready_q)) {
            int next = INT_MAX;
            int has_waiting = 0;
            for (int i = 0; i < n; i++)
                if (proc[i].state != WAITING &&
                    proc[i].state != TERMINATED &&
                    proc[i].remaining_cpu > 0 &&
                    proc[i].arrival_time < next)
                    next = proc[i].arrival_time;

            // 2. I/O operation - waiting 중인 프로세스가 있으면 1틱씩 처리
            for (int i = 0; i < n; i++) {
                if (proc[i].state == WAITING) {
                    has_waiting = 1;
                    proc[i].io_remaining--;

                    if (proc[i].io_remaining == 0) {
                        io_gantt[*io_gantt_len] = (GanttEntry){proc[i].pid, io_start[i], io_start[i] + proc[i].io_burst};
                        (*io_gantt_len)++;
                        proc[i].io_done++;
                        proc[i].io_remaining = proc[i].io_burst;
                        proc[i].state = READY;

                        // I/O 작업이 끝났기 때문에, 다시 READY 큐에 넣을 수 있게 함
                        in_queue[i] = 0;
                    }
                }
            }

            // waiting 큐가 없으면 바로 스킵
            if (has_waiting) {
                if (*gantt_len > 0 && gantt[*gantt_len - 1].pid == -1)
                    gantt[*gantt_len - 1].end++;
                else {
                    gantt[*gantt_len] = (GanttEntry){ -1, time, time + 1 };
                    (*gantt_len)++;
                }
                time++;
            } else {
                gantt[*gantt_len] = (GanttEntry){ -1, time, next };
                (*gantt_len)++;
                time = next;
            }
            continue;
        }

        // 실행 시킬 프로세스 디큐
        Process *cur = dequeue(&ready_q);
        int idx = cur - proc;
        int interval = (cur->io_count > 0) ? cur->cpu_burst / (cur->io_count + 1) : INT_MAX;
        if (interval == 0) interval = 1;
        int run = (cur->remaining_cpu < quantum) ? cur->remaining_cpu : quantum;

        if (cur->io_done < cur->io_count) {
            int before_trigger = interval - (cur->cpu_done % interval);
            if (before_trigger < run) run = before_trigger;
        }

        gantt[*gantt_len] = (GanttEntry){ cur->pid, time, time + run };
        (*gantt_len)++;

        // 현재 프로세스를 run만큼 실행
        cur->remaining_cpu -= run;
        cur->cpu_done      += run;
        time               += run;

        // 실행 후 새로 도착한 프로세스 enqueue (WAITING/TERMINATED 상태 제외)
        for (int i = 0; i < n; i++) {
            if (!in_queue[i] &&
                proc[i].arrival_time <= time &&
                proc[i].state != WAITING &&
                proc[i].state != TERMINATED &&
                proc[i].remaining_cpu > 0) {
                enqueue(&ready_q, &proc[i]);
                in_queue[i] = 1;
            }
        }

        // 2. I/O operation - 다른 프로세스들 I/O 작업 (run틱 만큼 경과)
        for (int i = 0; i < n; i++) {
            if (proc[i].state == WAITING) {
                proc[i].io_remaining -= run;
                
                if(proc[i].io_remaining <= 0) {
                    io_gantt[*io_gantt_len] = (GanttEntry){proc[i].pid, io_start[i], io_start[i] + proc[i].io_burst};
                    (*io_gantt_len)++;
                    proc[i].io_done++;
                    proc[i].io_remaining = proc[i].io_burst;
                    proc[i].state = READY;
                    in_queue[i] = 0;
                }
            }
        }

        // 2. I/O operation - 현재 프로세스의 I/O 트리거 체크
        if (cur->io_count > 0 && cur->io_done < cur->io_count &&
            cur->cpu_done % interval == 0) {
            cur->state = WAITING;
            in_queue[idx] = 0;
            io_start[idx] = time;
        }

        // 완료 체크
        if (cur->remaining_cpu == 0) {
            cur->completion_time = time;
            cur->state = TERMINATED;
            completed++;
        } else if (cur->state != WAITING) {
            in_queue[idx] = 0;  // 다음 루프에서 큐에 재삽입
        }

    }
}
