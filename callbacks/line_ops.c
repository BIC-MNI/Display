
#include  <display.h>
#include  <splines.h>

private  BOOLEAN  get_current_lines(
    display_struct    *display,
    lines_struct      **lines )
{
    BOOLEAN          found;
    object_struct    *current_object;

    if( get_current_object( display, &current_object ) &&
        current_object->object_type == LINES )
    {
        *lines = get_lines_ptr( current_object );
        found = TRUE;
    }
    else
        found = FALSE;

    return( found );
}

public  DEF_MENU_FUNCTION( smooth_current_lines )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(smooth_current_lines )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_current_line_tube )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(make_current_line_tube )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( convert_line_to_spline_points )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(convert_line_to_spline_points )   /* ARGSUSED */
{
    return( OK );
}
 
public  DEF_MENU_FUNCTION( make_line_circle )   /* ARGSUSED */
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

public  DEF_MENU_UPDATE(make_line_circle )   /* ARGSUSED */
{
    return( OK );
}


public  DEF_MENU_FUNCTION( subdivide_current_lines )   /* ARGSUSED */
{
    lines_struct      *lines;

    if( get_current_lines( display, &lines ) )
    {
        subdivide_lines( lines );

        graphics_models_have_changed( display );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(subdivide_current_lines )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( convert_markers_to_lines )   /* ARGSUSED */
{
    lines_struct            *lines;
    int                     i, c, n_markers, curr_index;
    Real                    dist, curr_dist, ratio, next_dist;
    Real                    desired_dist;
    Point                   *markers, point;
    int                     n_points;
    object_struct           *object, *current_object;
    object_traverse_struct  object_traverse;


    current_object = get_current_model_object( display );

    n_markers = 0;

    initialize_object_traverse( &object_traverse, 1, &current_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MARKER )
        {
            ADD_ELEMENT_TO_ARRAY( markers, n_markers,
                                  get_marker_ptr(object)->position,
                                  DEFAULT_CHUNK_SIZE );
        }
    }

    if( n_markers >= 3 )
    {
        dist = 0.0;
        for_less( i, 0, n_markers )
            dist += distance_between_points( &markers[i],
                                             &markers[(i+1)%n_markers] );

        print( "Enter number of points desired: " );

        if( input_int( stdin, &n_points ) == OK )
        {
            object = create_object( LINES );
            lines = get_lines_ptr( object );
            initialize_lines( lines, WHITE );
            lines->n_points = n_points;
            lines->n_items = 1;

            ALLOC( lines->points, lines->n_points );
            ALLOC( lines->end_indices, 1 );
            ALLOC( lines->indices, lines->n_points+1 );

            lines->end_indices[0] = n_points+1;

            for_less( i, 0, n_points+1 )
                lines->indices[i] = i % n_points;

            curr_index = 0;
            curr_dist = 0.0;
            next_dist = distance_between_points( &markers[0], &markers[1] );

            for_less( i, 0, n_points )
            {
                desired_dist = (Real) i / (Real) n_points * dist;

                while( desired_dist >= next_dist )
                {
                    ++curr_index;
                    curr_dist = next_dist;
                    next_dist += distance_between_points( &markers[curr_index],
                                          &markers[(curr_index+1)%n_markers] );
                }

                ratio = (desired_dist - curr_dist) / (next_dist - curr_dist);
                for_less( c, 0, N_DIMENSIONS )
                {
                    Point_coord(point,c) = CUBIC_UNIVAR( 
                     Point_coord(markers[(curr_index-1+n_markers)%n_markers],c),
                     Point_coord(markers[curr_index],c),
                     Point_coord(markers[(curr_index+1)%n_markers],c),
                     Point_coord(markers[(curr_index+2)%n_markers],c),
                     ratio );
                }
                lines->points[i] = point;
            }

            add_object_to_current_model( display, object );
        }


        (void) input_newline( stdin );
    }

    if( n_markers > 0 )
        FREE( markers );

    return( OK );
}

public  DEF_MENU_UPDATE(convert_markers_to_lines )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_line_widths )   /* ARGSUSED */
{
    Status          status;
    lines_struct    *lines;
    int             width;

    status = OK;

    if( get_current_lines( display, &lines ) )
    {
        print( "Enter line width: " );
        if( input_int( stdin, &width ) == OK )
        {
            lines->line_thickness = width;
            set_update_required( display, NORMAL_PLANES );
        }

        (void) input_newline( stdin );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_line_widths )   /* ARGSUSED */
{
    return( OK );
}
