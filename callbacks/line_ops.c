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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/line_ops.c,v 1.30 2001/05/27 00:19:38 stever Exp $";
#endif


#include  <display.h>
#include  <bicpl/splines.h>

private  BOOLEAN  get_current_lines(
    display_struct    *display,
    lines_struct      **lines )
{
    BOOLEAN          found;
    object_struct    *current_object;

    if( get_current_object( display, &current_object ) &&
        get_object_type( current_object ) == LINES )
    {
        *lines = get_lines_ptr( current_object );
        found = TRUE;
    }
    else
        found = FALSE;

    return( found );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( smooth_current_lines )
{
    Status          status;
    lines_struct    *lines;
    Real            smooth_distance;

    status = OK;

    if( get_current_lines( display, &lines ) )
    {
        print( "Enter smoothing distance: " );

        status = input_real( stdin, &smooth_distance );

        (void) input_newline( stdin );

        if( status == OK )
        {
            smooth_lines( lines, smooth_distance );
            set_update_required( display, NORMAL_PLANES );
        }
    }

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(smooth_current_lines )
{
    return( current_object_is_this_type( display, LINES ) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( make_current_line_tube )
{
    Status          status;
    lines_struct    *lines;
    Real            radius;
    int             n_around;

    status = OK;

    if( get_current_lines( display, &lines ) )
    {
        print( "Enter n_around radius: " );

        status = input_int( stdin, &n_around );

        if( status == OK )
            status = input_real( stdin, &radius );

        (void) input_newline( stdin );

        if( status == OK )
            convert_lines_to_tubes_objects( display, lines, n_around, radius );

        if( status == OK )
            graphics_models_have_changed( display );
    }

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(make_current_line_tube )
{
    return( current_object_is_this_type( display, LINES ) );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( convert_line_to_spline_points )
{
    object_struct   *object;
    lines_struct    *lines;
    lines_struct    new_lines;
    render_struct   *render;

    if( get_current_lines( display, &lines ) )
    {
        render = get_main_render( display );

        create_line_spline( lines, render->n_curve_segments, &new_lines );

        object = create_object( LINES );

        *(get_lines_ptr(object)) = new_lines;
        add_object_to_current_model( display, object );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(convert_line_to_spline_points )
{
    return( current_object_is_this_type( display, LINES ) );
}
 
/* ARGSUSED */

public  DEF_MENU_FUNCTION( make_line_circle )
{
    Point             centre;
    Real              x_size, y_size;
    int               plane_axis, n_around;
    object_struct     *object;

    print( "Enter x_centre, y_centre, z_centre, plane_axis, x_size, y_size\n" );
    print( "      n_around: " );
    
    if( input_float( stdin, &Point_x(centre) ) == OK &&
        input_float( stdin, &Point_y(centre) ) == OK &&
        input_float( stdin, &Point_z(centre) ) == OK &&
        input_int( stdin, &plane_axis ) == OK &&
        input_real( stdin, &x_size ) == OK &&
        input_real( stdin, &y_size ) == OK &&
        input_int( stdin, &n_around ) == OK )
    {
        object = create_object( LINES );

        create_line_circle( &centre, plane_axis, x_size, y_size,
                            n_around, get_lines_ptr(object) );

        get_lines_ptr(object)->colours[0] = WHITE;
        add_object_to_current_model( display, object );
    }

    (void) input_newline( stdin );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(make_line_circle )
{
    return( TRUE );
}


/* ARGSUSED */

public  DEF_MENU_FUNCTION( subdivide_current_lines )
{
    lines_struct      *lines;

    if( get_current_lines( display, &lines ) )
    {
        subdivide_lines( lines );

        graphics_models_have_changed( display );
    }

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(subdivide_current_lines )
{
    return( current_object_is_this_type( display, LINES ) );
}

private  void  convert_to_lines(
    display_struct   *display,
    BOOLEAN          closed )
{
    lines_struct            *lines;
    int                     i, c, n_markers, curr_index, max_index;
    Real                    dist, curr_dist, ratio, next_dist;
    Real                    desired_dist;
    Point                   *markers, point;
    Point                   p1, p2, p3, p4;
    int                     n_points;
    object_struct           *object, *current_object;
    object_traverse_struct  object_traverse;
    BOOLEAN                 interpolate;

    if( !get_current_object( display, &current_object ) )
    {
        print( "Current object is not a marker or model containing markers.\n");
        return;
    }

    n_markers = 0;
    markers = NULL;

    initialize_object_traverse( &object_traverse, FALSE, 1, &current_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MARKER )
        {
            ADD_ELEMENT_TO_ARRAY( markers, n_markers,
                                  get_marker_ptr(object)->position,
                                  DEFAULT_CHUNK_SIZE );
        }
    }

    if( n_markers >= 4 )
    {
        dist = 0.0;
        for_less( i, 0, n_markers-1 )
            dist += distance_between_points( &markers[i], &markers[i+1] );

        if( closed )
            dist += distance_between_points( &markers[n_markers-1],
                                             &markers[0] );

        print( "Enter number of points desired: " );

        if( input_int( stdin, &n_points ) == OK )
        {
            interpolate = (n_points >= 2);
            if( !interpolate )
                n_points = n_markers;

            object = create_object( LINES );
            lines = get_lines_ptr( object );
            initialize_lines( lines, WHITE );
            lines->n_points = n_points;
            lines->n_items = 1;

            ALLOC( lines->points, lines->n_points );
            ALLOC( lines->end_indices, 1 );
            lines->end_indices[0] = n_points;
            if( closed )
                ++lines->end_indices[0];
            ALLOC( lines->indices, lines->end_indices[0] );

            for_less( i, 0, lines->end_indices[0] )
                lines->indices[i] = i % n_points;

            if( interpolate )
            {
                curr_index = 0;
                curr_dist = 0.0;
                next_dist = distance_between_points( &markers[0], &markers[1] );

                if( closed )
                    max_index = n_markers-1;
                else
                    max_index = n_markers-2;

                for_less( i, 0, n_points )
                {
                    if( closed )
                        desired_dist = (Real) i / (Real) n_points * dist;
                    else
                        desired_dist = (Real) i / (Real) (n_points-1) * dist;

                    while( curr_index < max_index && desired_dist >= next_dist )
                    {
                        ++curr_index;
                        curr_dist = next_dist;
                        next_dist += distance_between_points(
                                       &markers[curr_index],
                                       &markers[(curr_index+1)%n_markers] );
                    }

                    if( curr_index == 0 && closed )
                        p1 = markers[n_markers-1];
                    else if( curr_index == 0 && !closed )
                        p1 = markers[0];
                    else
                        p1 = markers[curr_index-1];

                    p2 = markers[curr_index];

                    if( curr_index + 1 > n_markers - 1 && closed )
                        p3 = markers[(curr_index+1) % n_markers];
                    else if( curr_index + 1 > n_markers - 1 && !closed )
                        p3 = markers[n_markers-1];
                    else
                        p3 = markers[curr_index+1];

                    if( curr_index + 2 > n_markers - 1 && closed )
                        p4 = markers[(curr_index+2) % n_markers];
                    else if( curr_index + 2 > n_markers - 1 && !closed )
                        p4 = markers[n_markers-1];
                    else
                        p4 = markers[curr_index+2];

                    ratio = (desired_dist - curr_dist) /(next_dist - curr_dist);
                    if( ratio < 0.0 || ratio > 1.0 )
                        handle_internal_error( "Dang.\n" );

                    for_less( c, 0, N_DIMENSIONS )
                    {
                        Point_coord(point,c) = (float) cubic_interpolate( ratio,
                          (Real) Point_coord(p1,c), (Real) Point_coord(p2,c),
                          (Real) Point_coord(p3,c), (Real) Point_coord(p4,c) );
                    }
                    lines->points[i] = point;
                }
            }
            else
            {
                for_less( i, 0, n_points )
                    lines->points[i] = markers[i];
            }

            add_object_to_current_model( display, object );
        }


        (void) input_newline( stdin );
    }

    if( n_markers > 0 )
        FREE( markers );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( convert_markers_to_lines )
{
    convert_to_lines( display, FALSE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(convert_markers_to_lines )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( convert_markers_to_closed_lines )
{
    convert_to_lines( display, TRUE );

    return( OK );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(convert_markers_to_closed_lines )
{
    return( TRUE );
}

/* ARGSUSED */

public  DEF_MENU_FUNCTION( set_line_widths )
{
    Status          status;
    lines_struct    *lines;
    Real            width;

    status = OK;

    if( get_current_lines( display, &lines ) )
    {
        print( "Enter line width: " );
        if( input_real( stdin, &width ) == OK )
        {
            lines->line_thickness = (float) width;
            set_update_required( display, NORMAL_PLANES );
        }

        (void) input_newline( stdin );
    }

    return( status );
}

/* ARGSUSED */

public  DEF_MENU_UPDATE(set_line_widths )
{
    return( current_object_is_this_type( display, LINES ) );
}
