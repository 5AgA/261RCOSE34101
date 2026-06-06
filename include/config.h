#ifndef CONFIG_H
#define CONFIG_H

#define MIN_PROCESSES       3
#define MAX_PROCESSES_COUNT 10
#define MAX_BATCH           1000
#define RESULTS_DIR         "results"
#define ALGO_COUNT          6

extern const char *ALGO_NAMES[ALGO_COUNT];
extern const char *ALGO_FILES[ALGO_COUNT];
extern const char *ALGO_DIRS[ALGO_COUNT];

#endif
