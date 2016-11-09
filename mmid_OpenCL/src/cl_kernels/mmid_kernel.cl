void swap(void **p, void **q) {
    void *t = *p;
    *p = *q;
    *q = t;
}

kernel void test_kernel(global const float *k, global const float *m,
                        global float *v_p, global float *u_p,
                        global float *v, global float *u) {
    private const int i = get_local_id(0);
    for(int step = 0; step < steps; step++) {
        private float sumU = 0, sumV = 0, sumV_part = 0.5 * dt * v_p[i];
        #pragma unroll
        for (long j = i == 0 ? 0 : i - INTERACT; j <= i + INTERACT && j < N; j++) {
            float tmp_u = k[i * N + j] * u_p[j];
            sumV -= tmp_u + k[i * N + j] * sumV_part;
            sumU -= tmp_u;
        }
        u[i] = u_p[i] + dt * (v_p[i] + 0.5 * dt * sumU / m[i]);
        v[i] = v_p[i] + dt * sumV / m[i];
        swap((void **) &v, (void **) &v_p);
        swap((void **) &u, (void **) &u_p);
        barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
    }
}
