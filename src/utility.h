#pragma once

#include <sys/time.h>

#include <vector>
#include <string>
#include <expected>


enum EError : size_t {
    OK = 0,
    READING_FILE = 1,

    CL_NO_PLATFORM        = 201,
    CL_NO_DEVICE          = 202,
    CL_CONTEXT_CREATION   = 203,
    CL_PROGRAM_CREATION   = 204,
    CL_CMD_QUEUE_CREATION = 205,
    CL_BUILD_PROGRAM      = 206,
    CL_BUFFER_CREATION    = 207,
    CL_BUFFER_INVALID     = 208,
    CL_READ_BUFFER        = 209,
    CL_WRITE_BUFFER       = 210,
    CL_KERNEL_CREATION    = 211,
    CL_KERNEL_INVALID     = 212,
    CL_KERNEL_RUN         = 213,
};

#define CHRONO(id, code)    timeval_t start_##id;                                          \
                            start_##id = get_time();                                       \
                            code;                                                          \
                            double δt_##id = compute_δt(start_##id, get_time());           \
                            printf(" - chrono : %d -> %.4f ms\n", id, δt_##id*1000); \

typedef struct {
    double x;
    double y;
} point_t;

typedef point_t vector_t;

typedef struct {
    point_t pos;
    vector_t vel;

    double radius;
} particle_t;

typedef std::vector<particle_t> particle_list_t;

extern const vector_t NULL_VEC;

typedef struct timeval timeval_t;

vector_t create_vector(double x, double y);
vector_t mult_vec(vector_t vec, double val);
vector_t add_vec(vector_t vec1, vector_t vec2);
vector_t sub_vec(vector_t vec1, vector_t vec2);
vector_t neg_vec(vector_t vec);
double length_vec(vector_t vec);


timeval_t get_time();

double compute_δt(timeval_t t_start, timeval_t t_end);

double rand_double_range(double a, double b);
double rand_double();

EError read_file(std::string& content, const char* src);