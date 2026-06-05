#include <limits.h>
#include "../include/types.h"
#include "../include/gantt.h"

// 비선점 SJF
void sjf_non_preemptive(Process proc[], int n, GanttEntry gantt[], int *gantt_len) {
    int completed = 0, time = 0;
    int visited[MAX_PROCESSES] = {0};
    *gantt_len = 0;

    while (completed < n) {
        int sel = -1, min_burst = INT_MAX;

        for (int i = 0; i < n; i++) {
            // 도착할 프로세스들 중에서 cpu burst가 가장 작은 프로세스 찾기
            if (!visited[i] && proc[i].arrival_time <= time &&
                proc[i].cpu_burst < min_burst) {
                min_burst = proc[i].cpu_burst;
                sel = i;
            }
        }

        // 도착한 프로세스가 없을 경우
        if (sel == -1) {
            // 다음 프로세스 도착시간
            int next = INT_MAX;

            for (int i = 0; i < n; i++)
                // 곧 방문해야하는 프로세스 찾기
                if (!visited[i] && proc[i].arrival_time < next)
                    next = proc[i].arrival_time;

            // idle한 시간 기록
            gantt[*gantt_len] = (GanttEntry){ -1, time, next };
            (*gantt_len)++;

            // 다음 프로세스 도착 시간으로 설정
            time = next;
            continue;
        }

        // cpu burst 만큼 기록 - 한번 실행한 프로세스는 중간에 그만두지 않음
        gantt[*gantt_len] = (GanttEntry){
            proc[sel].pid, time, time + proc[sel].cpu_burst
        };
        (*gantt_len)++;
        time += proc[sel].cpu_burst;
        proc[sel].completion_time = time;
        visited[sel] = 1;

        // 프로세스 하나 완료할 때 마다 증가
        completed++;
    }
}

// 선점 SJF
void sjf_preemptive(Process proc[], int n, GanttEntry gantt[], int *gantt_len) {
    // prev_pid : 이전 프로세스 아이디 저장
    int completed = 0, time = 0, prev_pid = -2;
    *gantt_len = 0;

    while (completed < n) {
        int sel = -1, min_rem = INT_MAX;

        // 아직 완료 안된 프로세스들 중에서 shortest job 선택
        for (int i = 0; i < n; i++) {
            if (proc[i].arrival_time <= time && proc[i].remaining_cpu > 0 &&
                proc[i].remaining_cpu < min_rem) {
                min_rem = proc[i].remaining_cpu;
                sel = i;
            }
        }

        // 아직 도착할 프로세스가 없다면,
        if (sel == -1) {
            // 이전 tick에서도 idle 했다면, end를 늘리기
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

        // 작업시간이 가장 작은 프로세스가 이전 프로세스와 다르다면 기록
        if (proc[sel].pid != prev_pid) {
            gantt[*gantt_len] = (GanttEntry){ proc[sel].pid, time, time + 1 };
            (*gantt_len)++;
        } else {
        // 독보적이면 end를 늘리기
            gantt[*gantt_len - 1].end++;
        }

        // 한 칸 만 소모
        proc[sel].remaining_cpu--;
        time++;
        prev_pid = proc[sel].pid;

        // 만약 cpu 이용을 마쳤다면
        if (proc[sel].remaining_cpu == 0) {
            proc[sel].completion_time = time;
            completed++;
        }
    }
}
