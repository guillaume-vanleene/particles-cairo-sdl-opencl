#include "opencl_interface.h"

#include <CL/cl_ext.h>
#include <iostream>
#include <string>

#include <CL/opencl.hpp>

#include "utility.h"


#define CHECK_SUCCESS(var, message) if (var != CL_SUCCESS) { printf(message); printf("\ncode : %d\n", var); return false; }

#define CP(n) printf("CP n° : %d\n", n);

EError init_opencl(cli_t& cli)
{
    cli.sources_dir = "../../src/opencl";

    cl_int cl_err;
    
    cl_uint numPlatforms;

    // Find number of platforms
    cli.platform = cl::Platform::getDefault(&cl_err);
    if(cl_err != CL_SUCCESS)
        return EError::CL_NO_PLATFORM;

    cli.device = cl::Device::getDefault(&cl_err);
    if(cl_err != CL_SUCCESS)
        return EError::CL_NO_DEVICE;

    cli.ctx = cl::Context(cli.device, nullptr, nullptr, nullptr, &cl_err);
    if(cl_err != CL_SUCCESS)
        return EError::CL_CONTEXT_CREATION;

    cli.cmd_queue = cl::CommandQueue(cli.ctx, cli.device, 0, &cl_err);
    if(cl_err != CL_SUCCESS)
        return EError::CL_NO_DEVICE;

    cli.buffers = std::map<std::string, cl::Buffer>();
    cli.kernels = std::map<std::string, cl::Kernel>();

    EError err;
    err = opencl_create_buffer(cli, "acc", 400 * sizeof(vector_t));
    if(err != EError::OK)
        return err;

    err = opencl_create_buffer(cli, "t", sizeof(double), true);
    if(err != EError::OK)
        return err;

    err = opencl_create_buffer(cli, "particles", 400 * sizeof(particle_t));
    if(err != EError::OK)
        return err;

    err = opencl_create_buffer(cli, "elasticity", sizeof(double), true);
    if(err != EError::OK)
        return err;

    err = opencl_create_buffer(cli, "boundaries", sizeof(vector_t), true);
    if(err != EError::OK)
        return err;
    
    return opencl_build_program(cli, "interactions");
}

EError opencl_build_program(cli_t& cli, const char* file_name)
{
    std::string file_source = std::format("{}/{}.cl", cli.sources_dir, file_name);

    std::string src;
    EError err = read_file(src, file_source.c_str());
    if(err != EError::OK)
        return err;

    cl_int cl_err = CL_SUCCESS;

    cli.program = cl::Program(cli.ctx, src.c_str(), false, &cl_err);
    if(cl_err != CL_SUCCESS)
        return EError::CL_PROGRAM_CREATION;

    std::string options = std::format("-I {}/", cli.sources_dir);
    
    cl_err = cli.program.build(options.c_str(), nullptr);
    
    if(cl_err != CL_SUCCESS) {
        char err_data[1000];
        auto info = cli.program.getBuildInfo(cli.device, CL_PROGRAM_BUILD_LOG, &err_data);
        printf("%s\n", err_data);
        return EError::CL_BUILD_PROGRAM;
    }

    size_t n;
    cl_err = cli.program.getInfo(CL_PROGRAM_NUM_KERNELS, &n);
    if(cl_err != CL_SUCCESS) {
        return (EError)cl_err;
    }
    if(n == 0)
        return EError::OK;

    char kernels_name[1000];
    cl_err = cli.program.getInfo(CL_PROGRAM_KERNEL_NAMES, &kernels_name);
    if(cl_err != CL_SUCCESS) {
        printf("RIP\n");
        return (EError)cl_err;
    }

    std::string ker_name;
    int i = 0;
    while(true) {
        if(kernels_name[i] == ';' || kernels_name[i] == '\0') {
            cl::Kernel ker = cl::Kernel(cli.program, ker_name, &cl_err);
            if(cl_err != CL_SUCCESS)
                return EError::CL_KERNEL_CREATION;

            cli.kernels.insert({ker_name, ker});

            ker_name.clear();

            if(kernels_name[i] == '\0')
                break;
        }
        else
            ker_name += kernels_name[i];
        i += 1;
    }

    return EError::OK;
}

EError opencl_create_buffer(cli_t& cli, const char* name, size_t size, bool read_only)
{
    cl_int err;

    cl::Buffer buffer = cl::Buffer(cli.ctx, read_only ? CL_MEM_READ_ONLY : CL_MEM_READ_WRITE, size, nullptr, &err);
    if(err != CL_SUCCESS)
        return EError::CL_BUFFER_CREATION;
    
    cli.buffers.insert({name, buffer});

    return EError::OK;
}

EError opencl_write_buffer(cli_t& cli, const char* name, size_t size, void* data)
{
    cl::Buffer buffer;

    if(auto iter = cli.buffers.find(name); iter != cli.buffers.end())
        buffer = iter->second;
    else
        return EError::CL_BUFFER_INVALID;

    cl_int cl_err;

    cl_err = cli.cmd_queue.enqueueWriteBuffer(buffer,
        CL_TRUE,
        0,
        size,
        data);
    
    if(cl_err != CL_SUCCESS)
        return EError::CL_WRITE_BUFFER;
    
    return EError::OK;
}

EError opencl_read_buffer(cli_t& cli, const char* name, size_t size, void* data)
{
    cl::Buffer buffer;

    if(auto iter = cli.buffers.find(name); iter != cli.buffers.end())
        buffer = iter->second;
    else
        return EError::CL_BUFFER_INVALID;

    cl_int cl_err;

    cl_err = cli.cmd_queue.enqueueReadBuffer(buffer,
        CL_TRUE,
        0,
        size,
        data);
            
    if(cl_err != CL_SUCCESS)
        return EError::CL_READ_BUFFER;
    
    return EError::OK;
}

EError opencl_run_grav(cli_t& cli, particle_list_t& particles)
{
    cl_int cl_err = CL_SUCCESS;
    cl::Buffer acc_buffer;
    cl::Kernel ker;

    if(auto iter = cli.buffers.find("acc"); iter != cli.buffers.end())
        acc_buffer = iter->second;
    else
        return EError::CL_BUFFER_INVALID;

    if(auto iter = cli.kernels.find("add_grav_force"); iter != cli.kernels.end())
        ker = iter->second;
    else
        return EError::CL_KERNEL_INVALID;

    ker.setArg(0, acc_buffer);
    
    cl_err = cli.cmd_queue.enqueueNDRangeKernel(ker, cl::NullRange, particles.size());
    if(cl_err != CL_SUCCESS)
        return EError::CL_KERNEL_RUN;

    return EError::OK;
}

EError opencl_run_boundaries_check(cli_t& cli, particle_list_t& particles)
{
    cl_int cl_err = CL_SUCCESS;
    cl::Buffer particles_buffer;
    cl::Buffer boundaries_buffer;
    cl::Buffer elasticity_buffer;
    cl::Kernel ker;

    if(auto iter = cli.buffers.find("particles"); iter != cli.buffers.end())
        particles_buffer = iter->second;
    else
        return EError::CL_BUFFER_INVALID;

    if(auto iter = cli.buffers.find("elasticity"); iter != cli.buffers.end())
        elasticity_buffer = iter->second;
    else
        return EError::CL_BUFFER_INVALID;

    if(auto iter = cli.buffers.find("boundaries"); iter != cli.buffers.end())
        boundaries_buffer = iter->second;
    else
        return EError::CL_BUFFER_INVALID;

    if(auto iter = cli.kernels.find("check_boundaries"); iter != cli.kernels.end())
        ker = iter->second;
    else
        return EError::CL_KERNEL_INVALID;

    ker.setArg(0, particles_buffer);
    ker.setArg(1, boundaries_buffer);
    ker.setArg(2, elasticity_buffer);
    
    cl_err = cli.cmd_queue.enqueueNDRangeKernel(ker, cl::NullRange, particles.size());
    
    if(cl_err != CL_SUCCESS)
        return EError::CL_KERNEL_RUN;
    
    return EError::OK;
}

EError opencl_run_custom_force(cli_t& cli, particle_list_t& particles)
{
    cl_int cl_err = CL_SUCCESS;
    cl::Buffer acc_buffer;
    cl::Buffer particles_buffer;
    cl::Buffer t_buffer;
    cl::Kernel ker;

    if(auto iter = cli.buffers.find("acc"); iter != cli.buffers.end())
        acc_buffer = iter->second;
    else
        return EError::CL_BUFFER_INVALID;

    if(auto iter = cli.buffers.find("particles"); iter != cli.buffers.end())
        particles_buffer = iter->second;
    else
        return EError::CL_BUFFER_INVALID;

    if(auto iter = cli.buffers.find("t"); iter != cli.buffers.end())
        t_buffer = iter->second;
    else
        return EError::CL_BUFFER_INVALID;

    if(auto iter = cli.kernels.find("add_custom_force"); iter != cli.kernels.end())
        ker = iter->second;
    else
        return EError::CL_KERNEL_INVALID;

    ker.setArg(0, acc_buffer);
    ker.setArg(1, particles_buffer);
    ker.setArg(2, t_buffer);
    
    cl_err = cli.cmd_queue.enqueueNDRangeKernel(ker, cl::NullRange, particles.size());
    
    if(cl_err != CL_SUCCESS)
        return EError::CL_KERNEL_RUN;

    return EError::OK;
}