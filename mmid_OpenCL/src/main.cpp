#pragma ide diagnostic ignored "TemplateArgumentsIssues"
#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>
#include <iostream>
#include <unistd.h>
#include "cl_kernels/commons.h"
#include "cl_kernels/opencl_commons.h"
#include "cl_kernels/kernels.h"

int N = 0, done = 0, INTERACT = 0;
cl_float *m = nullptr, *k = nullptr, *v = nullptr, *u = nullptr;

int main(void) {
    initializeData();
    const cl_float period = 100, //getPeriod(m, k, N);
            dt = 0.01;
    const int steps = (const int) roundf(period / dt);
    prepareKernel(N, dt, steps);
    std::vector<cl::Context> contexts = createContexts(CL_DEVICE_TYPE_ALL);
    std::cout << "Contexts generated:" << contexts.size() << std::endl << std::endl;
    // get Context devices info
    for (std::vector<cl::Context>::iterator context = contexts.begin(); context != contexts.end() && !done; ++context) {
        std::vector<cl::Device> devices = context->getInfo<CL_CONTEXT_DEVICES>();
        // MEM
        const size_t unitSize = sizeof(cl_float) * N;
        cl::Buffer k_buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            unitSize * N, (void *) k);
        cl::Buffer m_buffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            unitSize, (void *) m);
        cl::Buffer v_p_buffer(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                              unitSize, (void *) v);
        cl::Buffer u_p_buffer(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                              unitSize, (void *) u);
        cl::Buffer v_buffer(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                            unitSize, (void *) v);
        cl::Buffer u_buffer(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                            unitSize, (void *) u);
        try {
            // Compile Programs
            cl::Program program = createProgram(*context, mmid_kernel, mmid_kernel_len, devices);
            cl::Kernel kernel = createKernel(program);
            kernel.setArg(0, k_buffer);
            kernel.setArg(1, m_buffer);
            kernel.setArg(2, v_p_buffer);
            kernel.setArg(3, u_p_buffer);
            kernel.setArg(4, v_buffer);
            kernel.setArg(5, u_buffer);
            for (std::vector<cl::Device>::iterator device = devices.begin();
                 device != devices.end() && !done; ++device) {
                std::cout << "Device: " << device->getInfo<CL_DEVICE_NAME>()
                          << std::endl
                          << " Address bits: "
                          << device->getInfo<CL_DEVICE_ADDRESS_BITS>() << std::endl
                          << " Compute units: " << device->getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl
                          << " Constant buffer limit " << device->getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>()
                          << std::endl
                          << " Parameter limit " << device->getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>() << std::endl
                          << " Work-group limit " << device->getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl
                          << device->getInfo<CL_DEVICE_VERSION>() << std::endl
                          << "Use this device ? [y/n] " << std::flush;
                if (!confirm())
                    continue;
                size_t start = (size_t) time(NULL);
                cl::CommandQueue queue(*context, *device, CL_QUEUE_PROFILING_ENABLE);
                queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange((size_t) N));
                queue.enqueueReadBuffer(v_buffer, CL_TRUE, 0, unitSize, v);
                queue.enqueueReadBuffer(u_buffer, CL_TRUE, 0, unitSize, u);
                queue.finish();
                for (int i = 0; i < N; i++)
                    fprintf(stdout, "%+24.10lf %+24.10lf\n", u[i], v[i]);
                //END of execution
                printf("**time elapsed= %lds\n", time(NULL) - start);
                done = 1;
            }
            std::cout << std::endl;
        } catch (cl::Error e) {
            std::cerr << e.what() << ": Error code " << e.err() << std::endl;
            exit(1);
        }
    }
    releaseData();
    return 0;
}

#pragma clang diagnostic pop
