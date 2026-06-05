#include <limits.h>
#include "../include/types.h"
#include "../include/gantt.h"

// 비선점 우선순위
void priority_non_preemptive(Process proc[], int n, GanttEntry gantt[], int *gantt_len) {
    int completed = 0, time = 0;
    int visited[MAX_PROCESSES] = {0};
    *gantt_len = 0;

    while (completed < n) {
        int sel = -1, highest = INT_MAX;

        // 숫자가 작을 수록 우선순위가 높음 - 우선순위가 같으면 먼저 온 것 부터 처리
        for (int i = 0; i < n; i++) {
            if (!visited[i] && proc[i].arrival_time <= time &&
                proc[i].priority < highest) {
                highest = proc[i].priority;
                sel = i;
            }
        }

        // 도착한 프로세스가 없을 때
        if (sel == -1) {
            int next = INT_MAX;

            // 현재로부터 가장 먼저 오는 프로세스 찾기
            for (int i = 0; i < n; i++)
                if (!visited[i] && proc[i].arrival_time < next)
                    next = proc[i].arrival_time;

            // 다음 프로세스까지 건너뛰기
            gantt[*gantt_len] = (GanttEntry){ -1, time, next };
            (*gantt_len)++;
            time = next;
            continue;
        }

        // 끝날때까지 점유
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

// 선점 우선순위
void priority_preemptive(Process proc[], int n, GanttEntry gantt[], int *gantt_len) {
    int completed = 0, time = 0, prev_pid = -2;
    *gantt_len = 0;

    while (completed < n) {
        int sel = -1, highest = INT_MAX;

        // 완료되지 않은 프로세스들 중에서 우선순위가 높은 것 찾기
        for (int i = 0; i < n; i++) {
            if (proc[i].arrival_time <= time && proc[i].remaining_cpu > 0 &&
                proc[i].priority < highest) {
                highest = proc[i].priority;
                sel = i;
            }
        }

        // cpu가 idle 상태라면
        if (sel == -1) {
            // 이전 tick에서도 idle 상태였다면 end point만 늘리기
            if (*gantt_len > 0 && gantt[*gantt_len - 1].pid == -1)
                gantt[*gantt_len - 1].end++;
            // 처음 idle을 만났을 때
            else {
                gantt[*gantt_len] = (GanttEntry){ -1, time, time + 1 };
                (*gantt_len)++;
            }
            time++;
            continue;
        }

        // 이전 프로세스랑 다르다면 새로 기록
        if (proc[sel].pid != prev_pid) {
            gantt[*gantt_len] = (GanttEntry){ proc[sel].pid, time, time + 1 };
            (*gantt_len)++;
        } else {
        // 이전 프로세스랑 같으면 end point만 늘리기
            gantt[*gantt_len - 1].end++;
        }

        // 한 칸 씩 소모
        proc[sel].remaining_cpu--;
        time++;
        prev_pid = proc[sel].pid;

        // 모두 소모하면 완료 기록
        if (proc[sel].remaining_cpu == 0) {
            proc[sel].completion_time = time;
            completed++;
        }
    }
}
