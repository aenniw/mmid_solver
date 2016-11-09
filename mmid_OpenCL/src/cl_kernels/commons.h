#ifndef ROOTPROJECT_COMMONS_H
#define ROOTPROJECT_COMMONS_H
#define INTERACT 1
//#define LOGGING
#include <iostream>
#include <CL/cl_platform.h>
#include <cmath>

extern int N;
extern cl_float *m, *k, *v, *u;

char *addDefine(const char *kernel, const char *name, const int value);

char *addDefine(const char *kernel, const char *name, const float value);

char *prepareKernel(int N, const cl_float dt, const int steps, const char *kern);

int confirm();

cl_float get_array(int x, int y, cl_float *array);

void set_array(int x, int y, cl_float val, cl_float *array);

cl_float get_k(int i, int j, int n, cl_float *k);

cl_float getPeriod(const cl_float *m, cl_float *k, int n);

void initializeData();

void releaseData();

#endif //ROOTPROJECT_COMMONS_H
