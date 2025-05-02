#include <vector>
#include <iostream>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>

#include "SDL_render.h"
#include "opencl_interface.h"
#include "draw.h"
#include "particle.h"
#include "utility.h"


int main(int argc, char *argv[])
{
    EError err;

    cli_t cli;
    err = init_opencl(cli);
    if(err != EError::OK) {
        std::cout << "error opencl init : " << err << "\n";
        return 1;
    }

    widget_t widget;

    init_sdl(widget);

    //reset_surface(&widget);

    std::vector<particle_t> particles;
    
    spawn_start_particules(widget, particles);

    bool quit = false;
    timeval_t t_start = get_time();
    timeval_t t_last = t_start;

    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                quit = true;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                handle_window_resized(widget);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == 1) {
                    point_t pos = create_vector(event.button.x, event.button.y);
                    //spawn_random_particle(particles, pos);
                }
                else if (event.button.button == 3) {
                    point_t pos = create_vector(event.button.x, event.button.y);
                    activate_mouse_force(true, pos);
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == 3) 
                    activate_mouse_force(false, NULL_VEC);
                break;
            default:
                break;
            }
        }
        
        timeval_t t_now = get_time();

        double δt = compute_δt(t_last, t_now);
        δt = std::min(δt, 0.01);
        t_last = t_now;

        double t = compute_δt(t_start, t_now);
        std::cout << "t : " << t << "\n";
        update(widget, cli, particles, δt, t);

        prepare_drawing(widget);

        draw(widget, particles);

        render(widget);
    }
    
    free_particles(particles);

    exit_SDL(widget);
    
    return 0;
}