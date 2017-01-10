#include <cstring>
#include "commons.h"

extern char CL_OPTIONS[];

void prepareKernel(int N, const cl_float dt, const int steps) {
    sprintf(CL_OPTIONS, "-Werror -DN=%d -Ddt=%.10lf -Dsteps=%d -DINTERACT=%d", N, dt, steps, INTERACT);
}

int confirm() {
    int c = '\0';
    while (c != 'y' && c != 'n')
        c = getchar();
    if (c == 'n')
        return 0;
    return 1;
}

cl_float get_array(int x, int y, cl_float *array) { return array[x * N + y]; }

void set_array(int x, int y, cl_float val, cl_float *array) {
    array[x * N + y] = val;
}


cl_float get_k(int i, int j, int n, cl_float *k) {
    if ((i > j && i - j > INTERACT) || (i < j && j - i > INTERACT))
        return 0;
    else if (i != j)
        return -get_array(i, j, k);
    return (i > 0 ? -get_array(i - 1, j, k) : 0) + get_array(i, j, k) +
           (i < n - 1 ? -get_array(i + 1, j, k) : 0);
}

cl_float getPeriod(const cl_float *m, cl_float *k, int n) {
    if (m == NULL || n <= 0)
        return 0;
    cl_float max = m[n - 1] / get_array(n - 1, n - 1, k);
    for (n -= 2; n >= 0; n--) {
        if (m[n] / get_array(n, n, k) > max) {
            max = m[n] / get_array(n, n, k);
        }
    }
    return (cl_float) (2 * 3.1415926 * sqrt(max));
}

void initializeData() {
    fprintf(stdout, "Enter samples value: ");
    scanf("%d", &N);
    cl_float rand_offset = 1.2;
    m = (cl_float *) malloc(sizeof(cl_float) * N);
    k = (cl_float *) malloc(sizeof(cl_float) * N * N);
    v = (cl_float *) malloc(sizeof(cl_float) * 2 * N);
    u = (cl_float *) malloc(sizeof(cl_float) * 2 * N);
    cl_float m_base = 2, k_base = 0.3, v_base = 0.2, u_base = 0.1;
    //srand((unsigned) time(NULL));
#ifdef LOGGING
    fprintf(stdout, "--- Samples -----------------\n");
#endif
    for (int i = 0; i < N; i++) {
        m[i] = (rand() / (RAND_MAX / (m_base * rand_offset - m_base))) + m_base;
        set_array(i, i,
                  (rand() / (RAND_MAX / (k_base * rand_offset - k_base))) + k_base,
                  k);
        if (i > 0) {
            for (int j = 1; i - j >= 0; j++) {
                set_array(i - j, i,
                          get_array(i - j + 1, i, k) > get_array(i - j, i - 1, k)
                          ? get_array(i - j + 1, i, k) - get_array(i - j, i - 1, k)
                          : get_array(i - j, i - 1, k) - get_array(i - j + 1, i, k),
                          k);
                set_array(i, i - j, get_array(i - j, i, k), k);
            }
        }
        v[i] = (rand() / (RAND_MAX / (v_base * rand_offset - v_base))) + v_base;
        u[i] = (rand() / (RAND_MAX / (u_base * rand_offset - u_base))) + u_base;
#ifdef LOGGING
        fprintf(stdout, "%d. m=%+14.10lf v=%+14.10lf u=%+14.10lf\n", i + 1, m[i], v[i], u[i]);
#endif
    }
#ifdef LOGGING
    fprintf(stdout, "--- k-Matrix ----------------\n");
#endif
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            set_array(i, j, get_k(i, j, N, k), k);
#ifdef LOGGING
            fprintf(stdout, "%+14.10lf ", get_array(i, j, k));
#endif
        }
#ifdef LOGGING
        fprintf(stdout, "\n");
#endif
    }
}

void releaseData() {
    free(m);
    free(k);
    free(v);
    free(u);
}