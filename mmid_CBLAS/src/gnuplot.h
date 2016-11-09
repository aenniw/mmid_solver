#ifndef ROOTPROJECT_GNUPLOT_H
#define ROOTPROJECT_GNUPLOT_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

FILE *createPlot(const char *title, const char *comment);

FILE **createPlots(const char *title, const char *comment, long start, long end);

void *flushPlot(FILE *gnuplot);

void *flushPlots(FILE **gnuplot, long len);

int printPlots(long n);

#endif //ROOTPROJECT_GNUPLOT_H
