/**
 * \file draw_immed.c
 * \brief Immediate drawing functions.
 *
 * Used for drawing "immediately" on the display.
 *
 * \copyright
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

void  draw_2d_line(
    display_struct    *display,
    View_types        view_type,
    VIO_Colour        colour,
    VIO_Real          x1,
    VIO_Real          y1,
    VIO_Real          x2,
    VIO_Real          y2 )
{
    static    VIO_Point end_points[2];
    static    int       end_indices[] = { 2 };
    static    int       indices[]     = { 0, 1 };
    static    lines_struct  lines = {
                                        ONE_COLOUR,
                                        NULL,
                                        1.0f,
                                        2,
                                        end_points,
                                        1,
                                        end_indices,
                                        indices,
                                        NULL
                                    };

    G_set_view_type( display->window, view_type );

    fill_Point( end_points[0], x1, y1, 0.0 );
    fill_Point( end_points[1], x2, y2, 0.0 );

    ALLOC( lines.colours, 1 );

    lines.colours[0] = colour;

    set_render_info( display->window, get_main_render(display) );

    G_draw_lines( display->window, &lines );

    FREE( lines.colours );
}

void  draw_2d_rectangle(
    display_struct    *display,
    View_types        view_type,
    VIO_Colour        colour,
    VIO_Real          x1,
    VIO_Real          y1,
    VIO_Real          x2,
    VIO_Real          y2 )
{
    static    VIO_Point corners[4];
    static    int       end_indices[] = { 5 };
    static    int       indices[]     = { 0, 1, 2, 3, 0 };
    static    lines_struct  lines = {
                                        ONE_COLOUR,
                                        (VIO_Colour *) 0,
                                        1.0f,
                                        4,
                                        corners,
                                        1,
                                        end_indices,
                                        indices,
                                        NULL
                                    };

    G_set_view_type( display->window, view_type );

    fill_Point( corners[0], x1, y1, 0.0 );
    fill_Point( corners[1], x2, y1, 0.0 );
    fill_Point( corners[2], x2, y2, 0.0 );
    fill_Point( corners[3], x1, y2, 0.0 );

    ALLOC( lines.colours, 1 );

    lines.colours[0] = colour;

    set_render_info( display->window, get_main_render(display) );
    G_draw_lines( display->window, &lines );

    FREE( lines.colours );
}

void
draw_polygons( display_struct    *display,
               polygons_struct   *polygons )
{
    G_set_view_type( display->window, MODEL_VIEW );

    set_render_info( display->window, get_main_render(display) );

    G_draw_polygons( display->window, polygons );
}

render_struct  *
get_main_render( display_struct *display )
{
    return( &get_model_info(get_model_ptr(display->models[THREED_MODEL]))
                                              ->render );
}

void
draw_text_3d( display_struct *display,
              VIO_Point      *origin,
              VIO_Colour     colour,
              VIO_STR        str )
{
    text_struct    text;

    G_set_view_type( display->window, MODEL_VIEW );

    initialize_text( &text, origin, colour, FIXED_FONT, 0.0 );

    text.origin = *origin;
    text.string = create_string( str );

    G_draw_text( display->window, &text );

    delete_text( &text );
}
