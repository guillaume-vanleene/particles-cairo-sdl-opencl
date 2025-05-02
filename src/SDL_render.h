#pragma once

#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL.h>
#include <cairo/cairo.h>


struct widget_t {
    SDL_Window * window;
    SDL_Renderer* renderer;
    SDL_Surface* sdl_surface;
    SDL_Texture* texture;

    cairo_surface_t* cr_surface;
    cairo_t* cr;

    void *pixels;
    int pitch;

    int w_height;
    int w_width;

    int r_height;
    int r_width;
} ;

void init_sdl(widget_t& widget);
void exit_SDL(widget_t& widget);

void prepare_drawing(widget_t& widget);

void render(widget_t& widget);

void handle_window_resized(widget_t& widget);