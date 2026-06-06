#include <stdlib.h>
#include <limits.h>
#include "../include/types.h"
#include "../include/gantt.h"

static int cmp_arrival(const void *a, const void *b) {
    return ((Process *)a)->arrival_time - ((Process *)b)->arrival_time;
}

void fcfs(Process proc[], int n, GanttEntry gantt[], int *gantt_len, GanttEntry io_gantt[], int *io_gantt_len) {
    qsort(proc, n, sizeof(Process), cmp_arrival);

    int time = 0, completed = 0, prev_pid = -2;
    int io_start[MAX_PROCESSES] = {0};
    *gantt_len = 0;
    *io_gantt_len = 0;

    while (completed < n) {

        // 2. I/O operation - waiting queue 처리 (매 tick)
        for (int i = 0; i < n; i++) {
            if (proc[i].state == WAITING) {
                proc[i].io_remaining--;

                // I/O 작업 1회 완료되면
                if(proc[i].io_remaining == 0) {
                    io_gantt[*io_gantt_len] = (GanttEntry){proc[i].pid, io_start[i], io_start[i] + proc[i].io_burst};
                    (*io_gantt_len)++;
                    proc[i].io_done++;
                    proc[i].io_remaining = proc[i].io_burst;
                    proc[i].state = READY;
                }
            }
        }

        // FCFS: 도착했고 READY 상태인 첫 번째 프로세스 선택
        int sel = -1;
        for (int i = 0; i < n; i++) {
            if (proc[i].arrival_time <= time &&
                (proc[i].state == READY || proc[i].state == RUNNING) &&
                proc[i].remaining_cpu > 0) {
                sel = i;
                break;
            }
        }

        // idle 처리
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

        // Gantt chart 기록 (이전과 같은 pid면 end만 늘리기)
        if (proc[sel].pid != prev_pid) {
            gantt[*gantt_len] = (GanttEntry){ proc[sel].pid, time, time + 1 };
            (*gantt_len)++;
        } else {
            gantt[*gantt_len - 1].end++;
        }

        // 1틱 실행
        proc[sel].remaining_cpu--;
        proc[sel].cpu_done++;
        proc[sel].state = RUNNING;
        time++;
        prev_pid = proc[sel].pid;

        // 2. I/O operation - I/O 트리거 체크
        int interval = (proc[sel].io_count > 0) ? proc[sel].cpu_burst / (proc[sel].io_count + 1) : INT_MAX;
        interval = (interval == 0) ? 1 : interval;

        if (proc[sel].cpu_done % interval == 0 &&
            proc[sel].io_done < proc[sel].io_count) {
            proc[sel].state = WAITING;
            prev_pid = -2;
            io_start[sel] = time;
        }
        
        // 완료 체크
        if(proc[sel].remaining_cpu == 0) {
            proc[sel].completion_time = time;
            proc[sel].state = TERMINATED;
            completed++;
            prev_pid = -2;
        }
    }
}
