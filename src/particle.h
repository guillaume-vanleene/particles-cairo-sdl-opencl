#pragma once

#include "utility.h"

#include "SDL_render.h"
#include "opencl_interface.h"


particle_t create_particle(point_t pos, vector_t vel, double radius);

void update(widget_t& widget, cli_t& cli, particle_list_t& particles, double δt, double time_since_start);

EError check_boundaries(widget_t& widget, cli_t& cli, particle_list_t& particles, double eslasticity);
void check_collision(particle_list_t& particles);
EError apply_interaction_forces(cli_t& cli, particle_list_t& particles, vector_t acc[], double time_since_start);

void activate_mouse_force(bool on, point_t pos);

void spawn_random_particle(particle_list_t& particles, point_t pos);
void spawn_start_particules(widget_t& widget, particle_list_t& particles);
void free_particles(particle_list_t& particles);