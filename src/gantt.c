#include <stdio.h>
#include "../include/gantt.h"

// 9. Gantt Chart
void print_gantt(FILE *fp, GanttEntry gantt[], int len) {
    if (len == 0) return;

    fprintf(fp, "\n[Gantt Chart]\n");

    /* top border */
    fprintf(fp, " ");
    for (int i = 0; i < len; i++) {
        int w = (gantt[i].end - gantt[i].start) * 2;
        for (int j = 0; j < w - 1; j++) fprintf(fp, "-");
        fprintf(fp, " ");
    }
    fprintf(fp, "\n|");

    /* process labels */
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

    /* bottom border */
    for (int i = 0; i < len; i++) {
        int w = (gantt[i].end - gantt[i].start) * 2;
        for (int j = 0; j < w - 1; j++) fprintf(fp, "-");
        fprintf(fp, " ");
    }
    fprintf(fp, "\n");

    /* time labels */
    fprintf(fp, "%d", gantt[0].start);
    for (int i = 0; i < len; i++) {
        int w = (gantt[i].end - gantt[i].start) * 2;
        for (int j = 0; j < w - 1; j++) fprintf(fp, " ");
        fprintf(fp, "%d", gantt[i].end);
    }
    fprintf(fp, "\n");
}
