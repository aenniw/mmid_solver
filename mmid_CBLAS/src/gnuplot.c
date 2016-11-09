#include "gnuplot.h"

const char *plot_u[] = {
        "gnuplot -p -e \"set title 'Offset';set xrange [0:*]; set yrange [*<-1:1<*]; plot for [i=1:",
        "] 'plot_'.i.'.dat' using 1:2 w lines title 'U entry '.i\""};
const char *plot_v[] = {
        "gnuplot -p -e \"set title 'Speed';set xrange [0:*]; set yrange [*<-1:1<*]; plot for [i=1:",
        "] 'plot_'.i.'.dat' using 1:3 w lines title 'V entry '.i\""};
const char plot_e[] = "gnuplot -p -e \"set title 'Energy';set xrange [0:*]; set yrange [*<-1:1<*]; set yrange [*<-1:1<*]; plot 'E_plot.dat' using 1:2 w lines title 'Total Energy'\"";


int lenOfNumber(long n) {
    int len = 1;
    while (n / 10 > 0) {
        n /= 10;
        len++;
    }
    return len;
}

FILE *createPlot(const char *title, const char *comment) {
    char fileName[strlen(title) + 5];
    memset(fileName, '\0', sizeof(fileName));
    strcpy(fileName, title);
    strcpy(fileName + strlen(title), ".dat");
    FILE *gnuplot = fopen(fileName, "w");
    fprintf(gnuplot, "#%s\n", comment);
    return gnuplot;
}

FILE **createPlots(const char *title, const char *comment, long start, long end) {
    if (start < 0 || end < 0 || start >= end)
        return NULL;
    const long amount = end - start;
    FILE **gnuplots = malloc(sizeof(FILE *) * amount);
    const int len_n = lenOfNumber(end);
    char fileName[strlen(title) + 6 + len_n], number[len_n + 1];
    for (long i = 0; i < amount; i++) {
        memset(fileName, '\0', sizeof(fileName));
        memset(number, '\0', sizeof(number));
        sprintf(number, "%ld", i + start + 1);
        strcat(fileName, title);
        strcat(fileName + strlen(title), "_");
        strcat(fileName + strlen(title), number);
        strcat(fileName + strlen(title), ".dat");
        gnuplots[i] = fopen(fileName, "w");
        fprintf(gnuplots[i], "#%s\n", comment);
    }
    return gnuplots;
}

void *flushPlot(FILE *gnuplot) {
    if (gnuplot == NULL)
        return NULL;
    fflush(gnuplot);
    fclose(gnuplot);
    return NULL;
}

void *flushPlots(FILE **gnuplots, long len) {
    if (gnuplots == NULL || len-- <= 0)
        return NULL;
    do {
        flushPlot(gnuplots[len--]);
    } while (len >= 0);
    free(gnuplots);
    return NULL;
}

int printPlots(long n) {
    if (n <= 0)
        return 1;
    const int len_n = lenOfNumber(n);
    char plot_u_local[strlen(len_n + plot_u[0]) + strlen(plot_u[1]) + 1], plot_v_local[
            len_n + strlen(plot_v[0]) + strlen(plot_v[1]) + 1], number[len_n + 1];
    memset(plot_u_local, '\0', sizeof(plot_u_local));
    memset(plot_v_local, '\0', sizeof(plot_v_local));
    memset(number, '\0', sizeof(number));
    sprintf(number, "%ld", n);

    strcat(plot_u_local, plot_u[0]);
    strcat(plot_u_local, number);
    strcat(plot_u_local, plot_u[1]);
    strcat(plot_v_local, plot_v[0]);
    strcat(plot_v_local, number);
    strcat(plot_v_local, plot_v[1]);

    return system(plot_v_local) + system(plot_u_local) + system(plot_e);
}