
#include  <display.h>

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
            convert_lines_to_tubes( display, lines, n_around, radius );

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
