#include "utility.h"

#include <cstdio>
#include <fstream>
#include <cmath>


const vector_t NULL_VEC = {.x = 0, .y = 0};


vector_t create_vector(double x, double y)
{
    vector_t v = {.x = x, .y = y};
    return v;
}

vector_t mult_vec(vector_t vec, double val)
{
    vec.x *= val;
    vec.y *= val;
    return vec;
}

vector_t add_vec(vector_t vec1, vector_t vec2)
{
    vec1.x += vec2.x;
    vec1.y += vec2.y;
    return vec1;
}

vector_t sub_vec(vector_t vec1, vector_t vec2)
{
    return add_vec(vec1, neg_vec(vec2));
}

vector_t neg_vec(vector_t vec)
{
    vec.x *= -1;
    vec.y *= -1;
    return vec;
}

double length_vec(vector_t vec)
{
    return sqrt(vec.x*vec.x + vec.y*vec.y);
}

timeval_t get_time()
{
    timeval_t tv;
    gettimeofday(&tv,NULL);
    return tv;
}

#include <iostream>
double compute_δt(timeval_t t_start, timeval_t t_end)
{
    double δt_s = t_end.tv_sec - t_start.tv_sec;
    double δt_μ = t_end.tv_usec - t_start.tv_usec;
    if (δt_μ < 0) {
        δt_s -= 1;
        δt_μ += 10e5;
    }
    return δt_s + δt_μ * 10e-7;
}

double rand_double_range(double a, double b)
{
    return ((double)rand()/(double)(RAND_MAX)) * (b - a) - a;
}

double rand_double()
{
    return rand_double_range(0, 1);
}

EError read_file(std::string& content, const char* src)
{
    std::ifstream f;

    std::string line;

    f.open(src);
    if (f.fail() or !f.is_open()) {
        perror("perror");
        return EError::READING_FILE;
    }
    
    while (std::getline(f, line))
    {
        content += line;
        content.push_back('\n');
    }

    f.close();
    
    return EError::OK;
}