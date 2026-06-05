#include <limits.h>
#include "../include/types.h"
#include "../include/gantt.h"

void sjf_non_preemptive(Process proc[], int n, GanttEntry gantt[], int *gantt_len) {
    int completed = 0, time = 0, prev_pid = -2;
    *gantt_len = 0;

    while (completed < n) {

        // 2. I/O operation - waiting queue 처리 (매 tick)
        for (int i = 0; i < n; i++) {
            if (proc[i].state == WAITING) {
                // TODO: io_remaining 1 감소
                // io_remaining == 0 이면:
                //   io_done++
                //   io_remaining = io_burst
                //   state = READY
            }
        }

        // SJF-NP: 도착했고 READY 상태인 것 중 cpu_burst가 가장 짧은 프로세스 선택
        int sel = -1, min_burst = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (proc[i].arrival_time <= time &&
                proc[i].state == READY &&
                proc[i].remaining_cpu > 0 &&
                proc[i].cpu_burst < min_burst) {
                min_burst = proc[i].cpu_burst;
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
        // TODO: interval 계산 (cpu_burst / (io_count + 1))
        // TODO: cpu_done % interval == 0 && io_done < io_count 이면
        //   io_remaining = io_burst
        //   state = WAITING
        //   prev_pid = -2

        // 완료 체크
        // TODO: remaining_cpu == 0 이면
        //   completion_time = time
        //   state = TERMINATED
        //   completed++
        //   prev_pid = -2
    }
}

void sjf_preemptive(Process proc[], int n, GanttEntry gantt[], int *gantt_len) {
    int completed = 0, time = 0, prev_pid = -2;
    *gantt_len = 0;

    while (completed < n) {

        // 2. I/O operation - waiting queue 처리 (매 tick)
        for (int i = 0; i < n; i++) {
            if (proc[i].state == WAITING) {
                // TODO: io_remaining 1 감소
                // io_remaining == 0 이면:
                //   io_done++
                //   io_remaining = io_burst
                //   state = READY
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
        // TODO: interval 계산 (cpu_burst / (io_count + 1))
        // TODO: cpu_done % interval == 0 && io_done < io_count 이면
        //   io_remaining = io_burst
        //   state = WAITING
        //   prev_pid = -2

        // 완료 체크
        // TODO: remaining_cpu == 0 이면
        //   completion_time = time
        //   state = TERMINATED
        //   completed++
        //   prev_pid = -2
    }
}
