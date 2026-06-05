#include <stdio.h>
#include "../include/gantt.h"

// 9. Gantt Chart
void print_gantt(FILE *fp, GanttEntry gantt[], int len) {
    if (len == 0) return;

    fprintf(fp, "\n[Gantt Chart]\n");

    // 위에 있는 테두리
    fprintf(fp, " ");
    for (int i = 0; i < len; i++) {
        int w = (gantt[i].end - gantt[i].start) * 2;
        for (int j = 0; j < w ; j++) fprintf(fp, "-");
        fprintf(fp, " ");
    }
    fprintf(fp, "\n|");

    // 프로세스 표시
    for (int i = 0; i < len; i++) {
        int w      = (gantt[i].end - gantt[i].start) * 2;
        int spaces = w - 2;
        int left   = spaces / 2;
        int right  = spaces - left;

        for (int j = 0; j < left;  j++) fprintf(fp, " ");
        if (gantt[i].pid == -1)
            fprintf(fp, "__");
        else
            fprintf(fp, "P%d", gantt[i].pid);
        for (int j = 0; j < right; j++) fprintf(fp, " ");
        fprintf(fp, "|");
    }
    fprintf(fp, "\n ");

    // 아래에 있는 테두리
    for (int i = 0; i < len; i++) {
        int w = (gantt[i].end - gantt[i].start) * 2;
        for (int j = 0; j < w ; j++) fprintf(fp, "-");
        fprintf(fp, " ");
    }
    fprintf(fp, "\n");

    // 시간 표시
    fprintf(fp, "%d", gantt[0].start);
    for (int i = 0; i < len; i++) {
        int w = (gantt[i].end - gantt[i].start) * 2;
        for (int j = 0; j < w ; j++) fprintf(fp, " ");

        // 끝 지점만 숫자 표시
        fprintf(fp, "%d", gantt[i].end);
    }
    fprintf(fp, "\n");
}
