#pragma once

#include <CL/opencl.hpp>
#include <map>

#include <CL/cl.h>

#include "utility.h"


typedef struct {
    const char* name;
    cl_program program;
} gpu_program;

typedef struct {
    cl::Platform platform;
    cl::Device device;
    
    cl::Context ctx;
    cl::Program program;
    cl::CommandQueue cmd_queue;

    const char* sources_dir;

    std::map<std::string, cl::Buffer> buffers;

    std::map<std::string, cl::Kernel> kernels;
} cli_t;


EError init_opencl(cli_t& cli);
EError opencl_build_program(cli_t& cli, const char* file_name);

EError opencl_create_buffer(cli_t& cli, const char* name, size_t size, bool read_only = false);
EError opencl_write_buffer (cli_t& cli, const char* name, size_t size, void* data);
EError opencl_read_buffer (cli_t& cli, const char* name, size_t size, void* data);

EError opencl_run_grav(cli_t& cli, particle_list_t& particles);
EError opencl_run_boundaries_check(cli_t& cli, particle_list_t& particles);
EError opencl_run_custom_force(cli_t& cli, particle_list_t& particles);