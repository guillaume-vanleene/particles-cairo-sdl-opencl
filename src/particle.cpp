#include "particle.h"
#include "opencl_interface.h"
#include "utility.h"

#include <iostream>
#include <ranges>


const double g = 9.81;
const double Rf = 10e1;
const double Mf = 10e1;

typedef struct {
    bool on;
    point_t pos;
} mouse_force_t;

mouse_force_t mouse_force;
 
particle_t create_particle(point_t pos, vector_t vel, double radius)
{
    particle_t p = {.pos = pos, .vel = vel, .radius = radius};
    return p;
}

void update(widget_t& widget, cli_t& cli, particle_list_t& particles, double δt, double time_since_start)
{
    vector_t acc[particles.size()];

    memset(acc, 0, particles.size() * sizeof(vector_t));

    opencl_write_buffer(cli, "acc", particles.size() * sizeof(vector_t), acc);
    opencl_write_buffer(cli, "t", sizeof(double), &time_since_start);
    opencl_write_buffer(cli, "particles", particles.size() * sizeof(particle_t), particles.data());
    
    check_boundaries(widget, cli, particles, 0.8);

    EError err = apply_interaction_forces(cli, particles, acc, time_since_start);
    if(err != EError::OK) {
        std::cout << "error : " << err << "\n";
        return;
    }

    opencl_read_buffer(cli, "acc", particles.size() * sizeof(vector_t), acc);
    
    for(int i = 0; i < particles.size(); i++)
    {
        particle_t* p = &particles[i];
        p->vel = add_vec(p->vel, mult_vec(acc[i], δt));
        p->pos = add_vec(p->pos, mult_vec(p->vel, δt));
    }
}

EError check_boundaries(widget_t& widget, cli_t& cli, particle_list_t& particles, double elasticity)
{
    opencl_write_buffer(cli, "elasticity", sizeof(double), &elasticity);

    vector_t boundaries = create_vector(widget.r_width, widget.r_height);
    opencl_write_buffer(cli, "boundaries", sizeof(vector_t), &boundaries);

    opencl_run_boundaries_check(cli, particles);

    opencl_read_buffer(cli, "particles", particles.size() * sizeof(particle_t), particles.data());

    return EError::OK;
}

void check_collision(particle_list_t& particles)
{
    
}

EError apply_interaction_forces(cli_t& cli, particle_list_t& particles, vector_t acc[], double time_since_start)
{
    EError err;
    err = opencl_run_grav(cli, particles);
    if(err != EError::OK)
        return err;
    err = opencl_run_custom_force(cli, particles);
    if(err != EError::OK)
        return err;

    return EError::OK;

    for (int i : std::views::iota(1, (int)particles.size()))
    {
        // Gravitational force

        particle_t p_i = particles[i];
        
        //acc[i].y += g;

        if (mouse_force.on) {
            vector_t direction = sub_vec(mouse_force.pos, p_i.pos);
            double distance = length_vec(direction);
            distance = std::max(distance, 0.1);

            acc[i] = add_vec(acc[i], neg_vec(mult_vec(direction, Mf/distance)));
        }

        for (int j : std::views::iota(i+1, (int)particles.size()))
        {
            if (i == j)
                continue;
            
            particle_t p_j = particles[j];

            vector_t direction = sub_vec(p_j.pos, p_i.pos);
            double distance = length_vec(direction);
            distance = std::max(distance, 0.1);
            //acc[i] = add_vec(acc[i], neg_vec(mult_vec(direction, Rf/(pow(distance, 3)/distance))));
            //acc[j] = add_vec(acc[j], mult_vec(direction, Rf/(pow(distance, 3)/distance)));
        }
    }
    return EError::OK;
}

void activate_mouse_force(bool on, point_t pos)
{
    mouse_force.on = on;
    mouse_force.pos = pos;
}

void spawn_random_particle(particle_list_t& particles, point_t pos)
{
    particle_t p = {
        .pos = pos,
        .vel = create_vector(rand_double_range(-50, 50), rand_double_range(-50, 50)),
        .radius = 5
    };
    particles.emplace_back(p);
}

void spawn_start_particules(widget_t& widget, particle_list_t& particles)
{
    const int nw = 20;
    const int nh = 20;

    const double offset = 20;

    const double w = widget.r_width  - offset * 2;
    const double h = widget.r_height - offset * 2;
    
    const double step_x = w / (nw+1);
    const double step_y = h / (nh+1);

    double x = offset + step_x;
    double y = offset + step_y;

    for(int i = 0; i < nw; i++)
    {
        for(int j = 0; j < nh; j++)
        {
            point_t pos = create_vector(x, y);
            
            particle_t p = {
                .pos = pos,
                .vel = create_vector(rand_double_range(-50, 50), rand_double_range(-50, 50)),
                .radius = 5
            };
            particles.emplace_back(p);

            y += step_y;
        }
        y = offset + step_y;
        x += step_x;
    }
}

void free_particles(particle_list_t& particles)
{
    particles.erase(particles.begin(), particles.end());
}