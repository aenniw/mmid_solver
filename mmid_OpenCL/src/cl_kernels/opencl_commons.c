#pragma ide diagnostic ignored "TemplateArgumentsIssues"
#define __CL_ENABLE_EXCEPTIONS

#include "opencl_commons.h"

char CL_OPTIONS[255] = {'\0'};

cl::Program
createProgram(cl::Context &context, const char *program_data, const size_t len, std::vector<cl::Device> &devices) {
    cl::Program program(context, cl::Program::Sources(1, {program_data, len}));
    try {
        program.build(devices, CL_OPTIONS);
    } catch (cl::Error e) {
        for (std::vector<cl::Device>::iterator device = devices.begin();
             device != devices.end(); ++device) {
            std::cerr << "Device: " << device->getInfo<CL_DEVICE_NAME>() << std::endl
                      << "Program: " << program.getInfo<CL_PROGRAM_SOURCE>() << std::endl
                      << "ERROR: " << std::endl
                      << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(*device) << std::endl;
        }
        exit(1);
    }
    return program;
}

cl::Kernel createKernel(cl::Program &program) {
    return cl::Kernel(program,
                      program.getInfo<CL_PROGRAM_KERNEL_NAMES>().c_str());
}

std::vector<cl::Context> createContexts(cl_device_type deviceType) {
    std::vector<cl::Context> contexts;
    try {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        std::cout << "Platforms found: " << platforms.size() << std::endl;
        for (std::vector<cl::Platform>::iterator platform = platforms.begin();
             platform != platforms.end(); ++platform) {
            std::cout << "\tPLATFORM " << platform->getInfo<CL_PLATFORM_NAME>() << std::endl;
            std::vector<cl::Device> platformDevices;
            platform->getDevices(deviceType, &platformDevices);
            contexts.push_back(cl::Context(platformDevices));
        }
    } catch (cl::Error e) {
        std::cerr << e.what() << ": Error code " << e.err() << std::endl;
        exit(1);
    }
    return contexts;
}

#pragma clang diagnostic pop