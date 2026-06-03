#ifndef TYPES_H
#define TYPES_H

#define MAX_PROCESSES 20
#define MAX_GANTT     2000
#define TIME_QUANTUM  2

typedef enum {
    NEW, READY, RUNNING, WAITING, TERMINATED
} State;

typedef struct {
    // 1. Method for Data Creation
    int pid;
    int arrival_time;
    int cpu_burst;
    int remaining_cpu;
    int io_burst;
    int io_remaining;
    int io_count;
    int io_done;
    int priority;

    // 10. Evaluation
    int waiting_time;
    int turnaround_time;
    int completion_time;
    int start_time;
    State state;
} Process;

typedef struct {
    int pid;    /* -1 = idle (CPU empty) */
    int start;
    int end;
} GanttEntry;

#endif
