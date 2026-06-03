#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../include/types.h"
#include "../include/process.h"
#include "../include/scheduler.h"
#include "../include/gantt.h"
#include "../include/evaluation.h"

#define MIN_PROCESSES  3
#define MAX_PROCESSES_COUNT 10
#define RESULTS_DIR    "results"

// 알고리즘 설정
#define ALGO_COUNT 6
static const char *ALGO_NAMES[ALGO_COUNT] = {
    "FCFS",
    "Non-Preemptive SJF",
    "Preemptive SJF",
    "Non-Preemptive Priority",
    "Preemptive Priority",
    "Round Robin"
};
static const char *ALGO_FILES[ALGO_COUNT] = {
    RESULTS_DIR "/fcfs.txt",
    RESULTS_DIR "/sjf_np.txt",
    RESULTS_DIR "/sjf_p.txt",
    RESULTS_DIR "/priority_np.txt",
    RESULTS_DIR "/priority_p.txt",
    RESULTS_DIR "/rr.txt"
};

// 결과 기록 초기화
static void clear_results_dir(void) {
    mkdir(RESULTS_DIR, 0755);

    DIR *d = opendir(RESULTS_DIR);
    if (!d) return;

    struct dirent *entry;
    char path[512];
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        snprintf(path, sizeof(path), "%s/%s", RESULTS_DIR, entry->d_name);
        remove(path);
    }
    closedir(d);
}

static FILE *open_result_file(const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "Error: cannot open %s\n", filepath);
        exit(1);
    }
    return fp;
}

// 프로세스 테이블을 파일에도 함께 기록
static void write_process_table(FILE *fp, Process proc[], int n) {
    fprintf(fp, "[Process Table]\n");
    fprintf(fp, "%-5s %-10s %-12s %-12s %-12s %-10s\n",
            "PID", "Arrival", "CPU Burst", "IO Burst", "IO Count", "Priority");
    fprintf(fp, "----------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "P%-4d %-10d %-12d %-12d %-12d %-10d\n",
                proc[i].pid, proc[i].arrival_time, proc[i].cpu_burst,
                proc[i].io_burst, proc[i].io_count, proc[i].priority);
    }
    fprintf(fp, "\n");
}

static void run_one(Process original[], int n, int choice) {
    Process    proc[MAX_PROCESSES];
    GanttEntry gantt[MAX_GANTT];
    int        gantt_len = 0;

    copy_processes(proc, original, n);

    switch (choice) {
        case 1: fcfs                   (proc, n, gantt, &gantt_len); break;
        case 2: sjf_non_preemptive     (proc, n, gantt, &gantt_len); break;
        case 3: sjf_preemptive         (proc, n, gantt, &gantt_len); break;
        case 4: priority_non_preemptive(proc, n, gantt, &gantt_len); break;
        case 5: priority_preemptive    (proc, n, gantt, &gantt_len); break;
        case 6: round_robin            (proc, n, TIME_QUANTUM, gantt, &gantt_len); break;
        default: return;
    }

    const char *filepath = ALGO_FILES[choice - 1];
    FILE *fp = open_result_file(filepath);
    write_process_table(fp, original, n);
    print_gantt(fp, gantt, gantt_len);
    print_evaluation(fp, proc, n, ALGO_NAMES[choice - 1]);
    fclose(fp);

    printf("  -> Saved: %s\n", filepath);
}

static void run_all(Process original[], int n) {
    Process    results[ALGO_COUNT][MAX_PROCESSES];
    GanttEntry gantt[MAX_GANTT];
    int        gantt_len;

    for (int a = 0; a < ALGO_COUNT; a++) {
        copy_processes(results[a], original, n);
        gantt_len = 0;

        switch (a) {
            case 0: fcfs                   (results[a], n, gantt, &gantt_len); break;
            case 1: sjf_non_preemptive     (results[a], n, gantt, &gantt_len); break;
            case 2: sjf_preemptive         (results[a], n, gantt, &gantt_len); break;
            case 3: priority_non_preemptive(results[a], n, gantt, &gantt_len); break;
            case 4: priority_preemptive    (results[a], n, gantt, &gantt_len); break;
            case 5: round_robin            (results[a], n, TIME_QUANTUM, gantt, &gantt_len); break;
        }

        calculate_times(results[a], n);

        FILE *fp = open_result_file(ALGO_FILES[a]);
        write_process_table(fp, original, n);
        print_gantt(fp, gantt, gantt_len);
        print_evaluation(fp, results[a], n, ALGO_NAMES[a]);
        fclose(fp);

        printf("  -> Saved: %s\n", ALGO_FILES[a]);
    }

    /* 비교표는 별도 파일 */
    const char *cmp_path = RESULTS_DIR "/comparison.txt";
    FILE *fp = open_result_file(cmp_path);
    write_process_table(fp, original, n);
    print_comparison(fp, results, n, ALGO_NAMES, ALGO_COUNT);
    fclose(fp);

    printf("  -> Saved: %s\n", cmp_path);
}

int main(void) {
    clear_results_dir();
    srand((unsigned int)time(NULL));

    Process original[MAX_PROCESSES];
    int     n = MIN_PROCESSES + rand() % (MAX_PROCESSES_COUNT - MIN_PROCESSES + 1);
    int     choice;

    create_process(original, n);

    while (1) {
        printf("\n======================================\n");
        printf("     CPU Scheduling Simulator\n");
        printf("======================================\n");
        printf(" 1. FCFS\n");
        printf(" 2. Non-Preemptive SJF\n");
        printf(" 3. Preemptive SJF\n");
        printf(" 4. Non-Preemptive Priority\n");
        printf(" 5. Preemptive Priority\n");
        printf(" 6. Round Robin (quantum=%d)\n", TIME_QUANTUM);
        printf("--------------------------------------\n");
        printf(" 7. Run All + Compare\n");
        printf(" 8. Show Process Table\n");
        printf(" 9. Generate New Processes\n");
        printf(" 0. Exit\n");
        printf("--------------------------------------\n");
        printf("Choose: ");

        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 0:
                printf("Bye.\n");
                return 0;
            case 1: case 2: case 3: case 4: case 5: case 6:
                run_one(original, n, choice);
                break;
            case 7:
                run_all(original, n);
                break;
            case 8:
                print_processes(original, n);
                break;
            case 9:
                clear_results_dir();
                n = MIN_PROCESSES + rand() % (MAX_PROCESSES_COUNT - MIN_PROCESSES + 1);
                create_process(original, n);
                break;
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}
