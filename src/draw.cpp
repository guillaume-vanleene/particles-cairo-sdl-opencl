#include "draw.h"

#include <SDL3/SDL_timer.h>
#include <math.h>
#include <stdio.h>

#include <cairo/cairo.h>

#include "utility.h"


void draw(widget_t& widget, particle_list_t& particles)
{
    cairo_t *cr = widget.cr;
    cairo_set_source_rgb (widget.cr, 0, 0, 0);
    cairo_paint (widget.cr);
    cairo_fill(cr);

    for(auto p: particles) {
        cairo_set_source_rgba(cr, 1, 0, 0, 1);
        cairo_set_line_width(cr, 10);
        
        cairo_arc(cr, p.pos.x, p.pos.y, p.radius, 0, 2*M_PI);
        cairo_fill(cr);
    }

    printf("\n\n");
}