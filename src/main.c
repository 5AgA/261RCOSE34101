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
#define MAX_BATCH  1000
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
static const char *ALGO_DIRS[ALGO_COUNT] = {
    RESULTS_DIR "/fcfs",
    RESULTS_DIR "/sjf_np",
    RESULTS_DIR "/sjf_p",
    RESULTS_DIR "/priority_np",
    RESULTS_DIR "/priority_p",
    RESULTS_DIR "/rr"
};

// 디렉터리 안 파일 전부 삭제 (서브디렉터리는 건드리지 않음)
static void clear_dir_files(const char *dir) {
    DIR *d = opendir(dir);
    if (!d) return;
    struct dirent *entry;
    char path[512];
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
        remove(path);  // 파일만 삭제, 디렉터리는 silently 무시
    }
    closedir(d);
}

// results/ 및 모든 알고리즘 서브디렉터리 초기화
static void clear_results_dir(void) {
    mkdir(RESULTS_DIR, 0755);
    clear_dir_files(RESULTS_DIR);
    for (int a = 0; a < ALGO_COUNT; a++) {
        mkdir(ALGO_DIRS[a], 0755);
        clear_dir_files(ALGO_DIRS[a]);
    }
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

// 알고리즘 하나만 실행
static void run_one(Process original[], int n, int choice) {
    Process    proc[MAX_PROCESSES];
    GanttEntry gantt[MAX_GANTT];
    GanttEntry io_gantt[MAX_GANTT];
    int        gantt_len = 0, io_gantt_len = 0;

    copy_processes(proc, original, n);

    switch (choice) {
        case 1: fcfs                   (proc, n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
        case 2: sjf_non_preemptive     (proc, n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
        case 3: sjf_preemptive         (proc, n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
        case 4: priority_non_preemptive(proc, n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
        case 5: priority_preemptive    (proc, n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
        case 6: round_robin            (proc, n, TIME_QUANTUM, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
        default: return;
    }

    const char *filepath = ALGO_FILES[choice - 1];
    FILE *fp = open_result_file(filepath);
    write_process_table(fp, original, n);
    print_gantt(fp, gantt, gantt_len);
    print_io_gantt(fp, io_gantt, io_gantt_len, proc, n);
    print_evaluation(fp, proc, n, ALGO_NAMES[choice - 1]);
    fclose(fp);
}

// 모든 알고리즘 실행
static void run_all(Process original[], int n) {
    Process    results[ALGO_COUNT][MAX_PROCESSES];
    GanttEntry gantt[MAX_GANTT];
    GanttEntry io_gantt[MAX_GANTT];
    int        gantt_len, io_gantt_len;

    for (int a = 0; a < ALGO_COUNT; a++) {
        copy_processes(results[a], original, n);
        gantt_len = 0; io_gantt_len = 0;

        switch (a) {
            case 0: fcfs                   (results[a], n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
            case 1: sjf_non_preemptive     (results[a], n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
            case 2: sjf_preemptive         (results[a], n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
            case 3: priority_non_preemptive(results[a], n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
            case 4: priority_preemptive    (results[a], n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
            case 5: round_robin            (results[a], n, TIME_QUANTUM, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
        }

        calculate_times(results[a], n);

        FILE *fp = open_result_file(ALGO_FILES[a]);
        write_process_table(fp, original, n);
        print_gantt(fp, gantt, gantt_len);
        print_io_gantt(fp, io_gantt, io_gantt_len, results[a], n);
        print_evaluation(fp, results[a], n, ALGO_NAMES[a]);
        fclose(fp);
    }

    // 비교표 파일 + 콘솔 출력
    const char *cmp_path = RESULTS_DIR "/comparison.txt";
    FILE *fp = open_result_file(cmp_path);
    write_process_table(fp, original, n);
    print_comparison(fp, results, n, ALGO_NAMES, ALGO_COUNT);
    fclose(fp);

    print_comparison(stdout, results, n, ALGO_NAMES, ALGO_COUNT);
    printf("\n  -> Saved: %s\n", cmp_path);
}

typedef struct {
    double sum_wt,  min_wt,  max_wt;
    double sum_tat, min_tat, max_tat;
    int    win_wt, win_tat;
    int    cnt;
} AlgoStats;

// 정합성 검사: 음수 대기시간, completion < arrival+cpu, TAT 불일치
static int check_validity(Process proc[], int n, int tc, int algo_idx, FILE *err_fp) {
    int errors = 0;
    for (int i = 0; i < n; i++) {
        if (proc[i].waiting_time < 0) {
            fprintf(err_fp, "[case_%04d][%s] P%d: waiting_time=%d (<0)\n",
                    tc, ALGO_NAMES[algo_idx], proc[i].pid, proc[i].waiting_time);
            errors++;
        }
        if (proc[i].completion_time < proc[i].arrival_time + proc[i].cpu_burst) {
            fprintf(err_fp, "[case_%04d][%s] P%d: completion=%d < arrival+cpu=%d\n",
                    tc, ALGO_NAMES[algo_idx], proc[i].pid,
                    proc[i].completion_time,
                    proc[i].arrival_time + proc[i].cpu_burst);
            errors++;
        }
        if (proc[i].turnaround_time != proc[i].completion_time - proc[i].arrival_time) {
            fprintf(err_fp, "[case_%04d][%s] P%d: TAT inconsistent (%d != CT%d - AT%d)\n",
                    tc, ALGO_NAMES[algo_idx], proc[i].pid,
                    proc[i].turnaround_time, proc[i].completion_time, proc[i].arrival_time);
            errors++;
        }
    }
    return errors;
}

// 배치 테스트: count개의 랜덤 프로세스 세트를 생성해 알고리즘별 폴더에 저장
static void run_batch(int count) {
    clear_results_dir();

    AlgoStats stats[ALGO_COUNT];
    for (int a = 0; a < ALGO_COUNT; a++) {
        stats[a] = (AlgoStats){ 0, 1e18, -1e18, 0, 1e18, -1e18, 0, 0, 0 };
    }

    int total_errors = 0, error_cases = 0;
    const char *err_path = RESULTS_DIR "/batch_errors.txt";
    FILE *err_fp = open_result_file(err_path);
    fprintf(err_fp, "# Batch Validation Errors\n\n");

    printf("\n  Generating %d test cases...\n", count);

    for (int tc = 1; tc <= count; tc++) {
        Process    original[MAX_PROCESSES];
        Process    results[ALGO_COUNT][MAX_PROCESSES];
        GanttEntry gantt[MAX_GANTT];
        GanttEntry io_gantt[MAX_GANTT];
        int        gantt_len, io_gantt_len;
        double     avg_wt[ALGO_COUNT], avg_tat[ALGO_COUNT];

        int n = MIN_PROCESSES + rand() % (MAX_PROCESSES_COUNT - MIN_PROCESSES + 1);
        create_process(original, n);

        int case_errors = 0;

        for (int a = 0; a < ALGO_COUNT; a++) {
            copy_processes(results[a], original, n);
            gantt_len = 0; io_gantt_len = 0;

            switch (a) {
                case 0: fcfs                   (results[a], n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
                case 1: sjf_non_preemptive     (results[a], n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
                case 2: sjf_preemptive         (results[a], n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
                case 3: priority_non_preemptive(results[a], n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
                case 4: priority_preemptive    (results[a], n, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
                case 5: round_robin            (results[a], n, TIME_QUANTUM, gantt, &gantt_len, io_gantt, &io_gantt_len); break;
            }

            char path[512];
            snprintf(path, sizeof(path), "%s/case_%04d.txt", ALGO_DIRS[a], tc);
            FILE *fp = open_result_file(path);
            write_process_table(fp, original, n);
            print_gantt(fp, gantt, gantt_len);
            print_io_gantt(fp, io_gantt, io_gantt_len, results[a], n);
            print_evaluation(fp, results[a], n, ALGO_NAMES[a]);  // calculate_times 내부 호출
            fclose(fp);

            // 지표 수집 (calculate_times가 print_evaluation 내에서 이미 호출됨)
            double twt = 0, ttat = 0;
            for (int i = 0; i < n; i++) {
                twt  += results[a][i].waiting_time;
                ttat += results[a][i].turnaround_time;
            }
            avg_wt[a]  = twt  / n;
            avg_tat[a] = ttat / n;

            // 누적 통계 갱신
            stats[a].sum_wt  += avg_wt[a];
            stats[a].sum_tat += avg_tat[a];
            if (avg_wt[a]  < stats[a].min_wt)  stats[a].min_wt  = avg_wt[a];
            if (avg_wt[a]  > stats[a].max_wt)  stats[a].max_wt  = avg_wt[a];
            if (avg_tat[a] < stats[a].min_tat) stats[a].min_tat = avg_tat[a];
            if (avg_tat[a] > stats[a].max_tat) stats[a].max_tat = avg_tat[a];
            stats[a].cnt++;

            // 정합성 검사
            int errs = check_validity(results[a], n, tc, a, err_fp);
            case_errors  += errs;
            total_errors += errs;
        }

        if (case_errors > 0) error_cases++;

        // 우승 횟수 (avg_wt / avg_tat 최소 알고리즘)
        int best_wt = 0, best_tat = 0;
        for (int a = 1; a < ALGO_COUNT; a++) {
            if (avg_wt[a]  < avg_wt[best_wt])  best_wt  = a;
            if (avg_tat[a] < avg_tat[best_tat]) best_tat = a;
        }
        stats[best_wt].win_wt++;
        stats[best_tat].win_tat++;

        if (tc % 100 == 0 || tc == count)
            printf("  Progress: %4d / %d\n", tc, count);
    }

    fclose(err_fp);
    if (total_errors == 0) remove(err_path);

    // 요약 출력 (파일 + 콘솔 공통 함수)
    const char *sum_path = RESULTS_DIR "/batch_summary.txt";
    FILE *sfp = open_result_file(sum_path);
    for (int pass = 0; pass < 2; pass++) {
        FILE *out = (pass == 0) ? sfp : stdout;
        fprintf(out, "\n========== Batch Test Summary (%d cases) ==========\n\n", count);
        fprintf(out, "[Performance Metrics]\n");
        fprintf(out, "%-26s  %-28s  %-28s  %s\n",
                "Algorithm", "Avg WT (min/avg/max)", "Avg TAT (min/avg/max)", "Wins(WT/TAT)");
        fprintf(out, "%s\n",
                "-------------------------------------------------------------------------------------");
        for (int a = 0; a < ALGO_COUNT; a++) {
            double mean_wt  = stats[a].sum_wt  / stats[a].cnt;
            double mean_tat = stats[a].sum_tat / stats[a].cnt;
            char wt_str[32], tat_str[32];
            snprintf(wt_str,  sizeof(wt_str),  "%.2f / %.2f / %.2f",
                     stats[a].min_wt, mean_wt, stats[a].max_wt);
            snprintf(tat_str, sizeof(tat_str), "%.2f / %.2f / %.2f",
                     stats[a].min_tat, mean_tat, stats[a].max_tat);
            fprintf(out, "%-26s  %-28s  %-28s  %d / %d\n",
                    ALGO_NAMES[a], wt_str, tat_str,
                    stats[a].win_wt, stats[a].win_tat);
        }
        fprintf(out, "\n[Validation Results]\n");
        fprintf(out, "Total test cases  : %d\n", count);
        fprintf(out, "Error-free cases  : %d\n", count - error_cases);
        fprintf(out, "Cases with errors : %d\n", error_cases);
        if (total_errors > 0)
            fprintf(out, "Total violations  : %d  (see batch_errors.txt)\n", total_errors);
        else
            fprintf(out, "Total violations  : 0  (all checks passed)\n");
    }
    fclose(sfp);

    printf("\n  Summary saved: %s\n", sum_path);
    if (total_errors > 0)
        printf("  Errors  : %s (%d violations in %d cases)\n",
               err_path, total_errors, error_cases);
}

int main(void) {
    clear_results_dir();
    srand((unsigned int)time(NULL));

    Process original[MAX_PROCESSES];
    int     n = MIN_PROCESSES + rand() % (MAX_PROCESSES_COUNT - MIN_PROCESSES + 1);
    int     choice;

    create_process(original, n);
    print_processes(original, n);

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
        printf("10. Batch Test (1~%d cases)\n", MAX_BATCH);
        printf(" 0. Exit\n");
        printf("--------------------------------------\n");
        printf("Choose: ");

        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 0:
                clear_results_dir();
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
                print_processes(original, n);
                break;
            case 10: {
                int cnt = 1 + rand() % MAX_BATCH;
                run_batch(cnt);
                break;
            }
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}
