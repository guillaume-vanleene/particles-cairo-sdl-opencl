#include "./SDL_render.h"

#include <SDL3/SDL_render.h>


void init_sdl(widget_t& widget)
{
    SDL_Init(SDL_INIT_VIDEO);

    widget.window = SDL_CreateWindow("Cairo SDL Particles",
                                            640,
                                            480,
                                            SDL_WINDOW_RESIZABLE);
    widget.renderer = SDL_CreateRenderer(widget.window, NULL);

    SDL_GetWindowSize(widget.window, &widget.w_width, &widget.w_height);
    
    SDL_GetRenderOutputSize(widget.renderer, &widget.r_width, &widget.r_height);
        
    SDL_PixelFormat cairo_format = SDL_GetPixelFormatForMasks(32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0);
    
    widget.texture = SDL_CreateTexture(
        widget.renderer,
        cairo_format,
        SDL_TEXTUREACCESS_STREAMING,
        widget.r_width,
        widget.r_height);
}

void exit_SDL(widget_t& widget)
{
    //SDL_DestroySurface(widget.sdl_surface);
    SDL_DestroyTexture(widget.texture);
    SDL_DestroyRenderer(widget.renderer);
    SDL_DestroyWindow(widget.window);
    
    SDL_Quit();   
}

void prepare_drawing(widget_t& widget)
{
    SDL_LockTexture(widget.texture, NULL, &widget.pixels, &widget.pitch);

    int cairo_x_multiplier = widget.r_width / widget.w_width;
    int cairo_y_multiplier = widget.r_height / widget.w_height;
    
    widget.cr_surface = cairo_image_surface_create_for_data((unsigned char *)widget.pixels,
                                                                        CAIRO_FORMAT_RGB24,
                                                                        widget.r_width,
                                                                        widget.r_height,
                                                                        widget.pitch);
    
    cairo_surface_set_device_scale(widget.cr_surface, cairo_x_multiplier, cairo_y_multiplier);

    widget.cr = cairo_create(widget.cr_surface);

    SDL_SetRenderDrawColor(widget.renderer, 255, 0, 0, 255);
    SDL_RenderClear(widget.renderer);

    cairo_set_source_rgb (widget.cr, 0, 0, 0);
    cairo_paint (widget.cr);
}

void render(widget_t& widget)
{
    SDL_UnlockTexture(widget.texture);
    cairo_destroy(widget.cr);
    cairo_surface_destroy(widget.cr_surface);
    SDL_RenderTexture(widget.renderer, widget.texture, NULL, NULL);
    SDL_RenderPresent(widget.renderer);
}

void handle_window_resized(widget_t& widget)
{
    SDL_GetWindowSize(widget.window, &widget.w_width, &widget.w_height);
    SDL_GetRenderOutputSize(widget.renderer, &widget.r_width, &widget.r_height);
    
    SDL_DestroyTexture(widget.texture);

    SDL_PixelFormat cairo_format = SDL_GetPixelFormatForMasks(
        32,
        0x00ff0000,
        0x0000ff00,
        0x000000ff,
        0);
    
    widget.texture = SDL_CreateTexture(
        widget.renderer,
        cairo_format,
        SDL_TEXTUREACCESS_STREAMING,
        widget.r_width,
        widget.r_height);
}