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
            for (int i = 0; i < n; i++)
                if (proc[i].state != WAITING &&
                    proc[i].state != TERMINATED &&
                    proc[i].remaining_cpu > 0 &&
                    proc[i].arrival_time < next)
                    next = proc[i].arrival_time;

            // 2. I/O operation - waiting 중인 프로세스가 있으면 1틱씩 처리
            // TODO: next 점프 대신 1틱씩 waiting queue를 처리하면서 대기
            //   for each WAITING process: io_remaining--
            //   io_remaining == 0 이면: io_done++, io_remaining=io_burst, state=READY, in_queue[i]=0
            //   time++
            // (waiting 중인 프로세스가 없으면 기존처럼 next로 점프)

            gantt[*gantt_len] = (GanttEntry){ -1, time, next };
            (*gantt_len)++;
            time = next;
            continue;
        }

        // 실행 시킬 프로세스 디큐
        Process *cur = dequeue(&ready_q);
        int idx = cur->pid - 1;

        // quantum만큼 실행하되 I/O 트리거 시점을 넘지 않도록 조절
        // TODO: run = min(remaining_cpu, quantum) 계산
        // TODO: I/O 트리거 시점(interval의 배수)까지 남은 틱을 계산해서
        //       run이 그 시점을 넘으면 run을 줄이기
        int run = (cur->remaining_cpu < quantum) ? cur->remaining_cpu : quantum;

        gantt[*gantt_len] = (GanttEntry){ cur->pid, time, time + run };
        (*gantt_len)++;

        // run만큼 실행
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

        // 2. I/O operation - waiting queue 처리 (run틱 경과)
        for (int i = 0; i < n; i++) {
            if (proc[i].state == WAITING) {
                // TODO: io_remaining을 run만큼 감소
                // io_remaining <= 0 이면:
                //   io_done++
                //   io_remaining = io_burst
                //   state = READY
                //   in_queue[i] = 0  ← 레디큐에 다시 들어올 수 있도록
            }
        }

        // 2. I/O operation - I/O 트리거 체크
        // TODO: interval 계산 (cpu_burst / (io_count + 1))
        // TODO: cpu_done % interval == 0 && io_done < io_count 이면
        //   io_remaining = io_burst
        //   state = WAITING
        //   in_queue[idx] = 0  ← 레디큐에 다시 넣지 않음
        // else (I/O 없이 quantum 소진):
        //   완료 체크: remaining_cpu == 0 이면 completion_time=time, state=TERMINATED, completed++
        //   아니면: 레디큐 뒤에 재삽입, in_queue[idx] = 0
        (void)idx;

        if (cur->remaining_cpu == 0) {
            cur->completion_time = time;
            completed++;
        } else {
            enqueue(&ready_q, cur);
        }
    }
}
