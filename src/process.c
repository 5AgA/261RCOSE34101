#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/process.h"

void create_process(Process proc[], int n) {
    printf("\n[Process Generation - Random]\n");
    printf("%-5s %-10s %-12s %-12s %-12s %-10s\n",
           "PID", "Arrival", "CPU Burst", "IO Burst", "IO Count", "Priority");
    printf("----------------------------------------------------------------\n");

    // n개의 프로세스를 생성
    for (int i = 0; i < n; i++) {
        proc[i].pid          = i + 1;
        proc[i].arrival_time = rand() % 10;       /* 0 ~ 9  */
        proc[i].cpu_burst    = rand() % 10 + 3;   /* 3 ~ 12 */
        proc[i].io_burst     = rand() % 5  + 1;   /* 1 ~ 5  */
        proc[i].io_count     = rand() % 3  + 1;   /* 1 ~ 3  */
        proc[i].priority     = rand() % 5  + 1;   /* 1(high) ~ 5(low) */

        proc[i].remaining_cpu = proc[i].cpu_burst;
        proc[i].io_remaining  = proc[i].io_burst;
        proc[i].io_done       = 0;
        proc[i].waiting_time  = 0;
        proc[i].turnaround_time = 0;
        proc[i].completion_time = 0;
        proc[i].start_time    = -1;
        proc[i].state         = READY;

        printf("P%-4d %-10d %-12d %-12d %-12d %-10d\n",
               proc[i].pid, proc[i].arrival_time, proc[i].cpu_burst,
               proc[i].io_burst, proc[i].io_count, proc[i].priority);
    }
}

void copy_processes(Process dst[], Process src[], int n) {
    // 알고리즘 수행을 위해 복사본 생성
    memcpy(dst, src, sizeof(Process) * n);
    
    for (int i = 0; i < n; i++) {
        dst[i].remaining_cpu    = src[i].cpu_burst;
        dst[i].io_remaining     = src[i].io_burst;
        dst[i].io_done          = 0;
        dst[i].waiting_time     = 0;
        dst[i].turnaround_time  = 0;
        dst[i].completion_time  = 0;
        dst[i].start_time       = -1;
        dst[i].state            = READY;
    }
}

void print_processes(Process proc[], int n) {
    printf("\n[Process Table]\n");
    printf("%-5s %-10s %-12s %-12s %-12s %-10s\n",
           "PID", "Arrival", "CPU Burst", "IO Burst", "IO Count", "Priority");
    printf("----------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("P%-4d %-10d %-12d %-12d %-12d %-10d\n",
               proc[i].pid, proc[i].arrival_time, proc[i].cpu_burst,
               proc[i].io_burst, proc[i].io_count, proc[i].priority);
    }
}
