#include "common.h"

kernel void add_grav_force(global vector_t* acc)
{
    int i = get_global_id(0);
    acc[i].y += 1;
}

kernel void check_boundaries(global particle_t* particles, constant vector_t* boundaries, constant double* elasticity)
{
    int i = get_global_id(0);

    point_t p_edge;
    point_t p_edge_vector;

    // bottom
    p_edge_vector = create_vector(0, particles[i].radius);
    p_edge = add_vec(particles[i].pos, p_edge_vector);
    if (p_edge.y > boundaries->y) {
        particles[i].vel.y *= -*elasticity;
        particles[i].pos.y = boundaries->y - p_edge_vector.y;
    }

    // up
    p_edge_vector = create_vector(0, -particles[i].radius);
    p_edge = add_vec(particles[i].pos, p_edge_vector);
    if (p_edge.y < 0) {
        particles[i].vel.y *= -*elasticity;
        particles[i].pos.y = -p_edge_vector.y;
    }

    // left
    p_edge_vector = create_vector(-particles[i].radius, 0);
    p_edge = add_vec(particles[i].pos, p_edge_vector);
    if (p_edge.x < 0) {
        particles[i].vel.x *= -*elasticity;
        particles[i].pos.x = -p_edge_vector.x;
    }

    // right
    p_edge_vector = create_vector(particles[i].radius, 0);
    p_edge = add_vec(particles[i].pos, p_edge_vector);
    if (p_edge.x > boundaries->x) {
        particles[i].vel.x *= -*elasticity;
        particles[i].pos.x = boundaries->x - p_edge_vector.x;
    }
}

kernel void add_custom_force(global vector_t* acc, global particle_t* particles, constant double* t)
{
    int i = get_global_id(0);

    for(int j = 0; j < 400; j++) {
        vector_t direction = sub_vec(particles[j].pos, particles[i].pos);
        double distance = length_vec(direction);
        distance = max(distance, 0.1);
        
        acc[i] = add_vec(acc[i], mult_vec(direction, 5/distance));
    }
}