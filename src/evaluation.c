#include <stdio.h>
#include "../include/evaluation.h"

// turnaround와 waiting 시간 계산
void calculate_times(Process proc[], int n) {
    for (int i = 0; i < n; i++) {
        proc[i].turnaround_time = proc[i].completion_time - proc[i].arrival_time;
        proc[i].waiting_time    = proc[i].turnaround_time - proc[i].cpu_burst;
    }
}

// 10. Evaluation
void print_evaluation(FILE *fp, Process proc[], int n, const char *algo_name) {
    calculate_times(proc, n);

    double total_wt = 0, total_tat = 0;

    fprintf(fp, "\n[%s - Result]\n", algo_name);
    fprintf(fp, "%-5s %-15s %-18s %-15s\n",
            "PID", "Completion", "Turnaround Time", "Waiting Time");
    fprintf(fp, "----------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "P%-4d %-15d %-18d %-15d\n",
                proc[i].pid, proc[i].completion_time,
                proc[i].turnaround_time, proc[i].waiting_time);
        total_wt  += proc[i].waiting_time;
        total_tat += proc[i].turnaround_time;
    }
    fprintf(fp, "----------------------------------------------------------\n");
    fprintf(fp, "Average Waiting Time    : %.2f\n", total_wt  / n);
    fprintf(fp, "Average Turnaround Time : %.2f\n", total_tat / n);
}

// 11. Additional Functions: Comparison
void print_comparison(FILE *fp, Process results[][MAX_PROCESSES], int n,
                      const char *names[], int algo_count) {
    fprintf(fp, "\n========== Algorithm Comparison ==========\n");
    fprintf(fp, "%-26s %-20s %-20s\n",
            "Algorithm", "Avg Waiting Time", "Avg Turnaround Time");
    fprintf(fp, "------------------------------------------------------------------\n");

    for (int a = 0; a < algo_count; a++) {
        double total_wt = 0, total_tat = 0;
        for (int i = 0; i < n; i++) {
            total_wt  += results[a][i].waiting_time;
            total_tat += results[a][i].turnaround_time;
        }
        fprintf(fp, "%-26s %-20.2f %-20.2f\n",
                names[a], total_wt / n, total_tat / n);
    }
}
