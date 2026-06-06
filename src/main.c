#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/config.h"
#include "../include/types.h"
#include "../include/process.h"
#include "../include/runner.h"

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
