#include <limits.h>
#include "../include/types.h"
#include "../include/gantt.h"

void sjf_non_preemptive(Process proc[], int n, GanttEntry gantt[], int *gantt_len, GanttEntry io_gantt[], int *io_gantt_len) {
    int completed = 0, time = 0, prev_pid = -2;
    int io_start[MAX_PROCESSES] = {0};
    *gantt_len = 0;
    *io_gantt_len = 0;

    while (completed < n) {

        // 2. I/O operation - waiting queue 처리 (매 tick)
        for (int i = 0; i < n; i++) {
            if (proc[i].state == WAITING) {
                proc[i].io_remaining--;

                if(proc[i].io_remaining == 0) {
                    io_gantt[*io_gantt_len] = (GanttEntry){proc[i].pid, io_start[i], io_start[i] + proc[i].io_burst};
                    (*io_gantt_len)++;
                    proc[i].io_done++;
                    proc[i].io_remaining = proc[i].io_burst;
                    proc[i].state = READY;
                }
            }
        }

        // SJF-NP: 이미 실행 중인 프로세스 유지 (non-preemptive), 없으면 cpu_burst 최소 선택
        int sel = -1, min_burst = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (proc[i].state == RUNNING) { sel = i; break; }
        }
        if (sel == -1) {
            for (int i = 0; i < n; i++) {
                if (proc[i].arrival_time <= time &&
                    proc[i].state == READY &&
                    proc[i].remaining_cpu > 0 &&
                    proc[i].remaining_cpu < min_burst) {
                    min_burst = proc[i].remaining_cpu;
                    sel = i;
                }
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

        // Gantt chart 기록
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

void sjf_preemptive(Process proc[], int n, GanttEntry gantt[], int *gantt_len, GanttEntry io_gantt[], int *io_gantt_len) {
    int completed = 0, time = 0, prev_pid = -2;
    int io_start[MAX_PROCESSES] = {0};
    *gantt_len = 0;
    *io_gantt_len = 0;

    while (completed < n) {

        // 2. I/O operation - waiting queue 처리 (매 tick)
        for (int i = 0; i < n; i++) {
            if (proc[i].state == WAITING) {
                proc[i].io_remaining--;

                if(proc[i].io_remaining == 0) {
                    io_gantt[*io_gantt_len] = (GanttEntry){proc[i].pid, io_start[i], io_start[i] + proc[i].io_burst};
                    (*io_gantt_len)++;
                    proc[i].io_done++;
                    proc[i].io_remaining = proc[i].io_burst;
                    proc[i].state = READY;
                }
            }
        }

        // SJF-P: 도착했고 WAITING/TERMINATED 아닌 것 중 remaining_cpu가 가장 짧은 프로세스
        int sel = -1, min_rem = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (proc[i].arrival_time <= time &&
                proc[i].state != WAITING &&
                proc[i].state != TERMINATED &&
                proc[i].remaining_cpu > 0 &&
                proc[i].remaining_cpu < min_rem) {
                min_rem = proc[i].remaining_cpu;
                sel = i;
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

        // Gantt chart 기록
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
