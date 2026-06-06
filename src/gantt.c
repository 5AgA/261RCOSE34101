#include <stdio.h>
#include "../include/gantt.h"
#include "../include/types.h"

// 9. Gantt Chart
void print_gantt(FILE *fp, GanttEntry gantt[], int len) {
    if (len == 0) return;

    fprintf(fp, "\n[Gantt Chart]\n");

    // 위에 있는 테두리
    fprintf(fp, " ");
    for (int i = 0; i < len; i++) {
        int w = (gantt[i].end - gantt[i].start) * 2;
        for (int j = 0; j < w ; j++) fprintf(fp, "---");
        fprintf(fp, " ");
    }
    fprintf(fp, "\n|");

    // 프로세스 표시
    for (int i = 0; i < len; i++) {
        int w = (gantt[i].end - gantt[i].start) * 2;
        int total_width = w * 3;  // 테두리와 동일한 너비

        char label[8];
        int label_len;
        if (gantt[i].pid == -1)
            label_len = snprintf(label, sizeof(label), "__");
        else
            label_len = snprintf(label, sizeof(label), "P%d", gantt[i].pid);

        int total_pad = total_width - label_len;
        int left  = total_pad / 2;
        int right = total_pad - left;

        for (int j = 0; j < left;  j++) fprintf(fp, " ");
        fprintf(fp, "%s", label);
        for (int j = 0; j < right; j++) fprintf(fp, " ");
        fprintf(fp, "|");
    }
    fprintf(fp, "\n ");

    // 아래에 있는 테두리
    for (int i = 0; i < len; i++) {
        int w = (gantt[i].end - gantt[i].start) * 2;
        for (int j = 0; j < w ; j++) fprintf(fp, "---");
        fprintf(fp, " ");
    }
    fprintf(fp, "\n");

    // 시간 표시
    char tmp[16];
    int d = snprintf(tmp, sizeof(tmp), "%d", gantt[0].start);
    fprintf(fp, "%s", tmp);

    for (int i = 0; i < len; i++) {
        int w = (gantt[i].end - gantt[i].start) * 2;
        int col_width = w * 3 + 1;  // 테두리/레이블과 동일한 열 너비
        int spaces = col_width - d;
        if (spaces < 1) spaces = 1;
        for (int j = 0; j < spaces; j++) fprintf(fp, " ");
        d = snprintf(tmp, sizeof(tmp), "%d", gantt[i].end);
        fprintf(fp, "%s", tmp);
    }
    fprintf(fp, "\n");
}

void print_io_gantt(FILE *fp, GanttEntry io_gantt[], int io_gantt_len, Process proc[], int n) {
    fprintf(fp, "\n[I/O Gantt Chart]\n");

    if (io_gantt_len == 0) {
        fprintf(fp, "(no I/O operations)\n");
        return;
    }

    int max_time = 0;
    for (int i = 0; i < io_gantt_len; i++)
        if (io_gantt[i].end > max_time) max_time = io_gantt[i].end;

    // 10단위 눈금
    fprintf(fp, "     ");
    for (int t = 0; t <= max_time; t += 10)
        fprintf(fp, "%-10d", t);
    fprintf(fp, "\n     ");
    for (int t = 0; t <= max_time; t++)
        fprintf(fp, "%d", t % 10);
    fprintf(fp, "\n");

    // 프로세스별 타임라인 ('=' = I/O 중, '_' = 아님)
    for (int i = 0; i < n; i++) {
        if (proc[i].io_count == 0) continue;
        int pid = proc[i].pid;
        fprintf(fp, "P%-3d ", pid);
        for (int t = 0; t < max_time; t++) {
            int in_io = 0;
            for (int j = 0; j < io_gantt_len; j++) {
                if (io_gantt[j].pid == pid && t >= io_gantt[j].start && t < io_gantt[j].end) {
                    in_io = 1; break;
                }
            }
            fprintf(fp, in_io ? "=" : "_");
        }
        fprintf(fp, "\n");
    }
}
