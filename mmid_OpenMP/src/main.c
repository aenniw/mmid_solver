#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include "gnuplot.h"

//#define ENERGY_CHECK
//#define PLOTS
long INTERACT = 0;
double *m, **k, E[2] = {0, 0};
double *v[2], *u[2];

double get_k(long i, long j, long n) {
    if ((i > j && i - j > INTERACT) || (i < j && j - i > INTERACT))
        return 0;
    else if (i != j)
        return -k[i][j];
    return
            (i > 0 ? -k[i - 1][j] : 0) + k[i][j] + (i < n - 1 ? -k[i + 1][j] : 0);
}

void mmid_sub_task(const double dt, const double endT, const long n) {
    const int my_rank = omp_get_thread_num(),
            thread_count = omp_get_num_threads();
    const long startOffset = my_rank * n / thread_count;
    const long endOffset = my_rank == thread_count ? n : (my_rank + 1) * n / thread_count;
    FILE **plots = NULL, *e_plot = NULL;
#ifdef PLOTS
    plots = createPlots("plot", "t u v", startOffset, endOffset);
#endif
#ifdef ENERGY_CHECK
# pragma omp master
    e_plot = createPlot("E_plot", "t E");
#endif
    int step = 0;
    for (double t = 0; t < endT; t += dt, step = (step + 1) % 2) {
#pragma omp parallel for
        for (long i = startOffset; i < endOffset; i++) {
            double sumU = 0, sumV = 0, sumE = 0;
            // INTERACT stands for interaction between entities
            for (long j = i == 0 ? 0 : i - INTERACT; j <= i + INTERACT && j < n; j++) {
                sumV -= k[i][j] * (u[step][j] + 0.5 * dt * v[step][i]);
                sumU -= k[i][j] * u[step][j];
#ifdef ENERGY_CHECK
                sumE += 0.5 * k[i][j] * u[step][i] * u[step][j];
#endif
            }
#ifdef ENERGY_CHECK
# pragma omp atomic
            E[step] += 0.5 * m[i] * v[step][i] * v[step][i] + sumE;
#endif
            u[(step + 1) % 2][i] = u[step][i] + dt * (v[step][i] + 0.5 * dt * sumU / m[i]);
            v[(step + 1) % 2][i] = v[step][i] + dt * sumV / m[i];
#ifdef PLOTS
            fprintf(plots[i - startOffset], "%+24.10lf %+24.10lf %+24.10lf\n", t, u[step][i], v[step][i]);
#endif
        }
# pragma omp barrier
#ifdef ENERGY_CHECK
# pragma omp master
        {
            fprintf(e_plot, "%+24.10lf %+24.10lf\n", t, E[step]);
            E[step] = 0;
        }
#endif
    }
#ifdef ENERGY_CHECK
# pragma omp master
    flushPlot(e_plot);
#endif
#ifdef PLOTS
    flushPlots(plots, endOffset - startOffset);
#endif
}

double getPeriod(const double *m, double **k, long n) {
#define PI 3.14159265358979323846
    if (m == NULL || n <= 0)
        return 0;
    double max = m[n - 1] / k[n - 1][n - 1];
    for (n -= 2; n >= 0; n--) {
        if (m[n] / k[n][n] > max) {
            max = m[n] / k[n][n];
        }
    }
    return 2 * PI * sqrt(max);
}

int main(int argc, char **argv) {
    long N = 0;
    fprintf(stdout, "Enter samples value: ");
    scanf("%ld", &N);
    fprintf(stdout, "Enter interaction value: ");
    scanf("%ld", &INTERACT);
    float rand_offset = 1.2;
    m = malloc(sizeof(double) * N);
    k = malloc(sizeof(double *) * N);
    for (int i = 0; i < 2; i++) {
        v[i] = malloc(sizeof(double) * N);
        u[i] = malloc(sizeof(double) * N);
    }
    double m_base = 2, k_base = 0.3, v_base = 0.2, u_base = 0.1;
    //comment out for stable samples
    srand((unsigned) time(NULL));
    fprintf(stdout, "--- Samples -----------------\n");
    for (long i = 0; i < N; i++) {
        m[i] = (rand() / (RAND_MAX / (m_base * rand_offset - m_base))) + m_base;
        k[i] = malloc(sizeof(double) * N);
        k[i][i] = (rand() / (RAND_MAX / (k_base * rand_offset - k_base))) + k_base;
        if (i > 0) {
            for (long j = 1; i - j >= 0; j++) {
                k[i - j][i] = k[i - j + 1][i] > k[i - j][i - 1] ? k[i - j + 1][i] - k[i - j][i - 1] : k[i - j][i - 1] -
                                                                                                      k[i - j + 1][i];
                k[i][i - j] = k[i - j][i];
            }
        }
        v[0][i] = (rand() / (RAND_MAX / (v_base * rand_offset - v_base))) + v_base;
        u[0][i] = (rand() / (RAND_MAX / (u_base * rand_offset - u_base))) + u_base;
        fprintf(stdout, "%ld. m=%+14.10lf v=%+14.10lf u=%+14.10lf\n", i + 1, m[i], v[0][i], u[0][i]);
    }
    fprintf(stdout, "--- k-Matrix ----------------\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            k[i][j] = get_k(i, j, N);
            fprintf(stdout, "%+14.10lf ", k[i][j]);
        }
        fprintf(stdout, "\n");
    }
    //comment out for stable samples
    const double period = getPeriod(m, k, N);
    //Computation
# pragma omp parallel
    mmid_sub_task(0.01, period, N);
    for (int i = 0; i < N; i++)
        fprintf(stdout, "%+24.10lf %+24.10lf\n", u[1][i], v[1][i]);
    //Cleanup
    for (long i = 0; i < N; i++) {
        if (i < 2) {
            free(v[i]);
            free(u[i]);
        }
        free(k[i]);
    }
    free(k);
    free(m);
#if defined(PLOTS) || defined(ENERGY_CHECK)
    return printPlots(N);
#else
    return 0;
#endif
}