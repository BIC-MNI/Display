/* ----------------------------------------------------------------------------
@COPYRIGHT  :
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
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/slice_window/colour_bar.c,v 1.27 2001-05-27 00:19:52 stever Exp $";
#endif

#include  <display.h>

private  Real  get_y_pos(
    Real    value,
    Real    min_value,
    Real    max_value,
    Real    bottom,
    Real    top );

typedef enum
{
    HISTOGRAM,
    BAR,
    TICKS,
    FIRST_TEXT
} Colour_bar_objects;

public  void  initialize_colour_bar(
    display_struct    *slice_window )
{
    int               n_vertices;
    object_struct     *object;
    quadmesh_struct   *quadmesh;
    lines_struct      *lines;
    model_struct      *model;
    model_info_struct *model_info;
    colour_bar_struct *colour_bar;

    colour_bar = &slice_window->slice.colour_bar;

    colour_bar->top_offset = Colour_bar_top_offset;
    colour_bar->bottom_offset = Colour_bar_bottom_offset;
    colour_bar->left_offset = Colour_bar_left_offset;
    colour_bar->bar_width = Colour_bar_width;
    colour_bar->tick_width = Colour_bar_tick_width;
    colour_bar->desired_n_intervals = Colour_bar_desired_intervals;

    model = get_graphics_model( slice_window, COLOUR_BAR_MODEL );
    model_info = get_model_info( model );

    model_info->render.shaded_mode = ON;
    model_info->render.shading_type = GOURAUD_SHADING;
    model_info->render.master_light_switch = OFF;
    model_info->render.backface_flag = OFF;

    object = create_object( QUADMESH );

    quadmesh = get_quadmesh_ptr( object );
    initialize_quadmesh( quadmesh, WHITE, NULL, Colour_bar_resolution, 2 );
    FREE( quadmesh->normals );
    quadmesh->normals = (Vector *) NULL;

    quadmesh->colour_flag = PER_VERTEX_COLOURS;
    n_vertices = 2 * Colour_bar_resolution;
    REALLOC( quadmesh->colours, n_vertices );

    quadmesh->normals = (Vector *) NULL;

    add_object_to_model( model, object );  

    object = create_object( LINES );

    lines = get_lines_ptr( object );
    initialize_lines( lines, WHITE );
    delete_lines( lines );
    lines->colour_flag = PER_ITEM_COLOURS;
    lines->line_thickness = 1.0f;
    lines->n_points = 0;
    lines->n_items = 0;

    add_object_to_model( model, object );  
}

typedef  struct
{
    int      priority;
    Real     value;
} number_entry;

public  void  rebuild_colour_bar(
    display_struct   *slice_window )
{
    int                 i, volume_index;
    int                 x_min, x_max, y_min, y_max;
    Real                x, y, bottom, top, range, delta;
    Real                ratio, last_y, next_y, value, min_value, max_value;
    Real                start_threshold, end_threshold;
    Real                x_tick_start, x_tick_end, mult_value;
    Point               point;
    char                buffer[EXTREMELY_LARGE_STRING_SIZE];
    Colour              colour;
    colour_bar_struct   *colour_bar;
    lines_struct        *lines;
    object_struct       *object;
    text_struct         *text;
    quadmesh_struct     *quadmesh;
    Volume              volume;
    int                 n_numbers;
    number_entry        entry, *numbers;
    model_struct        *model;

    object = slice_window->models[COLOUR_BAR_MODEL];

    if( !get_slice_window_volume( slice_window, &volume ) ||
        is_an_rgb_volume(volume) )
    {
        set_object_visibility( object, OFF );
        return;
    }
    else
        set_object_visibility( object, ON );

    colour_bar = &slice_window->slice.colour_bar;

    get_volume_real_range( volume, &min_value, &max_value );

    volume_index = get_current_volume_index( slice_window );

    get_colour_coding_min_max( &slice_window->slice.
                               volumes[volume_index].colour_coding,
                               &start_threshold,
                               &end_threshold );

    model = get_graphics_model( slice_window, COLOUR_BAR_MODEL );

    /* rebuild the points */

    get_slice_model_viewport( slice_window, COLOUR_BAR_MODEL,
                              &x_min, &x_max, &y_min, &y_max );

    quadmesh = get_quadmesh_ptr( model->objects[BAR] );

    bottom = colour_bar->bottom_offset;
    top = (Real) y_max - (Real) y_min - colour_bar->top_offset;

    for_less( i, 0, quadmesh->m )
    {
        ratio = (Real) i / (Real) (quadmesh->m-1);

        /* set the points */

        y = INTERPOLATE( ratio, bottom, top );

        fill_Point( quadmesh->points[IJ(i,1,2)],
                    colour_bar->left_offset, y, 0.0 );

        fill_Point( quadmesh->points[IJ(i,0,2)],
                    colour_bar->left_offset + colour_bar->bar_width,
                    y, 0.0 );

        /* set the colours */

        value = INTERPOLATE( ratio, min_value, max_value );

        colour = get_colour_code( &slice_window->slice.volumes[volume_index].
                                  colour_coding, value );

        quadmesh->colours[IJ(i,0,2)] = colour;
        quadmesh->colours[IJ(i,1,2)] = colour;
    }

    /* now rebuild the tick marks and numbers */

    lines = get_lines_ptr( model->objects[TICKS] );

    delete_lines( lines );

    lines->n_points = 0;
    lines->n_items = 0;

    x_tick_start = colour_bar->left_offset + colour_bar->bar_width;
    x_tick_end = x_tick_start + colour_bar->tick_width;

    while( model->n_objects > FIRST_TEXT )
    {
        delete_object( model->objects[FIRST_TEXT] );
        remove_ith_object_from_model( model, FIRST_TEXT );
    }

    range = max_value - min_value;

    delta = get_good_round_value( range /
                                  (Real) Colour_bar_desired_intervals );

    n_numbers = 0;
    numbers = NULL;

    mult_value = round_to_nearest_multiple( min_value, delta );
    while( mult_value <= min_value && delta > 0.0 )
         mult_value = round_to_nearest_multiple( mult_value + delta, delta );

    value = min_value;
    while( value <= max_value )
    {
        entry.value = value;

        if( (value == start_threshold || value == end_threshold) )
            entry.priority = 2;
        else if( value == min_value || value == max_value )
            entry.priority = 1;
        else
            entry.priority = 0;
           
        ADD_ELEMENT_TO_ARRAY( numbers, n_numbers, entry, DEFAULT_CHUNK_SIZE );

        if( value < start_threshold && mult_value >= start_threshold )
            value = start_threshold;
        else if( value < end_threshold && mult_value >= end_threshold )
            value = end_threshold;
        else if( value < max_value && mult_value >= max_value )
            value = max_value;
        else
        {
            value = mult_value;
            mult_value = round_to_nearest_multiple( mult_value + delta, delta );
        }

        if( delta <= 0.0 )
            break;
    }

    last_y = 0.0;

    for_less( i, 0, n_numbers )
    {
        y = get_y_pos( numbers[i].value, min_value, max_value, bottom,
                       top );

        if( i < n_numbers-1 )
            next_y = get_y_pos( numbers[i+1].value, min_value, max_value,
                                bottom, top );

        if( (i == 0 || y - last_y > Colour_bar_closest_text) &&
            (i == n_numbers-1 ||
                next_y - y > Colour_bar_closest_text ||
                numbers[i].priority > numbers[i+1].priority ) )
        {
            if( numbers[i].priority == 2 && numbers[i].value == start_threshold)
                colour = Colour_bar_min_limit_colour;
            else if( numbers[i].priority == 2 &&
                     numbers[i].value == end_threshold )
                colour = Colour_bar_max_limit_colour;
            else if( numbers[i].priority == 1 )
                colour = Colour_bar_range_colour;
            else
                colour = Colour_bar_text_colour;

            SET_ARRAY_SIZE( lines->colours, lines->n_items,
                            lines->n_items+1, DEFAULT_CHUNK_SIZE );

            lines->colours[lines->n_items] = colour;

            start_new_line( lines );

            if( numbers[i].priority == 2 )
                x = colour_bar->left_offset;
            else
                x = x_tick_start;

            fill_Point( point, x, y, 0.0 );

            add_point_to_line( lines, &point );

            if( Histogram_extra_width > 0.0 &&
                slice_window->slice.unscaled_histogram_lines.n_points > 0 &&
                (numbers[i].value == start_threshold ||
                 numbers[i].value == end_threshold) )
            {
                fill_Point( point, x_tick_end + Histogram_extra_width, y, 0.0 );
            }
            else
            {
                fill_Point( point, x_tick_end, y, 0.0 );
            }
            add_point_to_line( lines, &point );

            object = create_object( TEXT );

            text = get_text_ptr( object );
            text->font = (Font_types) Colour_bar_text_font;
            text->size = Colour_bar_text_size;
            text->colour = colour;
            fill_Point( text->origin, x_tick_end,
                        y - G_get_text_height( text->font, text->size ) / 2.0,
                        0.0 );
            (void) sprintf( buffer, Colour_bar_number_format,
                            numbers[i].value );

            text->string = create_string( buffer );

            add_object_to_model( model, object );

            last_y = y;
        }
    }

    if( n_numbers > 0 )
        FREE( numbers );

    set_slice_viewport_update( slice_window, COLOUR_BAR_MODEL );
}

private  Real  get_y_pos(
    Real    value,
    Real    min_value,
    Real    max_value,
    Real    bottom,
    Real    top )
{
    if( min_value != max_value )
        return( INTERPOLATE( (value - min_value) / (max_value - min_value ),
                             bottom, top ) );
    else
        return( 0.0 );
}

public  int  get_colour_bar_y_pos(
    display_struct      *slice_window,
    Real                value )
{
    Volume              volume;
    int                 x_min, y_min, x_max, y_max;
    Real                top, bottom, min_value, max_value;
    colour_bar_struct   *colour_bar;

    if( !get_slice_window_volume( slice_window, &volume ) ||
        is_an_rgb_volume( volume ) )
        return( -1 );

    colour_bar = &slice_window->slice.colour_bar;

    get_volume_real_range( volume, &min_value, &max_value );
    get_slice_model_viewport( slice_window, COLOUR_BAR_MODEL,
                              &x_min, &x_max, &y_min, &y_max );
    
    bottom = (Real) colour_bar->bottom_offset;
    top    = (Real) y_max - (Real) y_min - colour_bar->top_offset;

    return( ROUND(get_y_pos( value, min_value, max_value, bottom, top )) );
}

public  BOOLEAN  mouse_within_colour_bar(
    display_struct      *slice_window,
    Real                x,
    Real                y,
    Real                *ratio )
{
    int                 x_min, x_max, y_min, y_max;
    Real                top, bottom;
    BOOLEAN             within;
    colour_bar_struct   *colour_bar;
    Volume              volume;

    if( !get_slice_window_volume( slice_window, &volume ) ||
        is_an_rgb_volume( volume ) )
        return( FALSE );

    get_slice_model_viewport( slice_window, COLOUR_BAR_MODEL,
                              &x_min, &x_max, &y_min, &y_max );

    x -= (Real) x_min;
    y -= (Real) y_min;

    colour_bar = &slice_window->slice.colour_bar;

    bottom = (Real) colour_bar->bottom_offset;
    top = (Real) y_max - (Real) y_min - colour_bar->top_offset;

    within = y >= bottom && y <= top && colour_bar->left_offset <= x &&
             (x <= colour_bar->left_offset +
                   colour_bar->bar_width + colour_bar->tick_width);

    if( bottom == top )
        *ratio = 0.0;
    else
        *ratio = (y - bottom) / (top - bottom);

    return( within );
}

public  void  get_histogram_space(
    display_struct      *slice_window,
    int                 *x1,
    int                 *x2 )
{
    int                 x_min, x_max, y_min, y_max;
    colour_bar_struct   *colour_bar;

    colour_bar = &slice_window->slice.colour_bar;
    get_slice_model_viewport( slice_window, COLOUR_BAR_MODEL,
                              &x_min, &x_max, &y_min, &y_max );

    *x1 = ROUND( colour_bar->left_offset + colour_bar->bar_width +
                 colour_bar->tick_width );
    *x2 = x_max - x_min;
}
