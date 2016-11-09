#ifndef ROOTPROJECT_KERNEL_EDIT_H
#define ROOTPROJECT_KERNEL_EDIT_H

#include <CL/cl.hpp>
#include <iostream>

cl::Program
createProgram(cl::Context &context, const char *program_data, const size_t len, std::vector<cl::Device> &devices);

cl::Kernel createKernel(cl::Program &program);

std::vector<cl::Context> createContexts(cl_device_type deviceType);

#endif //ROOTPROJECT_KERNEL_EDIT_H
