
#include  <def_graphics.h>
#include  <def_files.h>

private  Boolean  get_current_lines( graphics, lines )
    graphics_struct   *graphics;
    lines_struct      **lines;
{
    Boolean          found;
    object_struct    *current_object;

    if( get_current_object( graphics, &current_object ) &&
        current_object->object_type == LINES )
    {
        *lines = current_object->ptr.lines;
        found = TRUE;
    }
    else
        found = FALSE;

    return( found );
}

public  DEF_MENU_FUNCTION( smooth_current_lines )   /* ARGSUSED */
{
    Status          status;
    Status          smooth_lines();
    lines_struct    *lines;
    Boolean         get_current_object();
    void            set_update_required();
    Real            smooth_distance;

    status = OK;

    if( get_current_lines( graphics, &lines ) )
    {
        PRINT( "Enter smoothing distance: " );

        status = input_real( stdin, &smooth_distance );

        (void) input_newline( stdin );

        if( status == OK )
            status = smooth_lines( lines, smooth_distance );

        if( status == OK )
            set_update_required( graphics, NORMAL_PLANES );
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
    Status          convert_lines_to_tubes();
    lines_struct    *lines;
    Boolean         get_current_object();
    void            graphics_models_have_changed();
    Real            radius;
    int             n_around;

    status = OK;

    if( get_current_lines( graphics, &lines ) )
    {
        PRINT( "Enter n_around radius: " );

        status = input_int( stdin, &n_around );

        if( status == OK )
            status = input_real( stdin, &radius );

        (void) input_newline( stdin );

        if( status == OK )
            status = convert_lines_to_tubes( graphics, lines,
                                             n_around, radius );

        if( status == OK )
            graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(make_current_line_tube )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( convert_line_to_spline_points )   /* ARGSUSED */
{
    Status          status;
    Status          create_line_spline();
    Status          create_object();
    Status          add_object_to_current_model();
    object_struct   *object;
    lines_struct    *lines;
    Boolean         get_current_object();
    lines_struct    new_lines;
    render_struct   *render;
    render_struct   *get_main_render();

    status = OK;

    if( get_current_lines( graphics, &lines ) )
    {
        render = get_main_render( graphics );

        status = create_line_spline( lines, render->n_curve_segments,
                                     &new_lines );

        if( status == OK )
            status = create_object( &object, LINES );

        if( status == OK )
        {
            *(object->ptr.lines) = new_lines;
            status = add_object_to_current_model( graphics, object );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(convert_line_to_spline_points )   /* ARGSUSED */
{
    return( OK );
}
 
public  DEF_MENU_FUNCTION( deform_line_to_volume )   /* ARGSUSED */
{
    Status            status;
    Status            deform_lines();
    Real              boundary_factor, max_step, search_width;
    Real              min_isovalue, max_isovalue;
    Real              stop_threshold, min_size;
    int               max_iterations, n_samples;
    volume_struct     *volume;
    lines_struct      *lines;
    void              set_update_required();

    status = OK;

    if( get_current_lines( graphics, &lines ) &&
        get_current_volume( graphics, &volume ) )
    {
        PRINT( "Enter boundary_factor, max_step, search_width,\n" );
        PRINT( "      n_samples, min_size, min_isovalue, max_isovalue,\n" );
        PRINT( "      max_iterations, stop_threshold: " );

        if( input_real( stdin, &boundary_factor ) == OK &&
            input_real( stdin, &max_step ) == OK &&
            input_real( stdin, &search_width ) == OK &&
            input_int( stdin, &n_samples ) == OK &&
            input_real( stdin, &min_size ) == OK &&
            input_real( stdin, &min_isovalue ) == OK &&
            input_real( stdin, &max_isovalue ) == OK &&
            input_int( stdin, &max_iterations ) == OK &&
            input_real( stdin, &stop_threshold ) == OK )
        {
            status = deform_lines( lines, volume,
                                   boundary_factor, max_step, search_width,
                                   n_samples, min_size,
                                   min_isovalue, max_isovalue,
                                   max_iterations, stop_threshold );

            set_update_required( graphics, NORMAL_PLANES );

            PRINT( "Done deforming lines.\n" );
        }

        (void) input_newline( stdin );
    }

    return( status );
}

public  DEF_MENU_UPDATE(deform_line_to_volume )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_line_circle )   /* ARGSUSED */
{
    Status            status;
    Status            create_line_circle();
    Status            create_object();
    Status            add_object_to_current_model();
    Point             centre;
    Real              x_size, y_size;
    int               plane_axis, n_around;
    object_struct     *object;
    void              set_object_colour();

    status = OK;

    PRINT( "Enter x_centre, y_centre, z_centre, plane_axis, x_size, y_size\n" );
    PRINT( "      n_around: " );
    
    if( input_real( stdin, &Point_x(centre) ) == OK &&
        input_real( stdin, &Point_y(centre) ) == OK &&
        input_real( stdin, &Point_z(centre) ) == OK &&
        input_int( stdin, &plane_axis ) == OK &&
        input_real( stdin, &x_size ) == OK &&
        input_real( stdin, &y_size ) == OK &&
        input_int( stdin, &n_around ) == OK )
    {
        status = create_object( &object, LINES );

        if( status == OK )
            status = create_line_circle( &centre, plane_axis, x_size, y_size,
                                         n_around, object->ptr.lines );

        if( status == OK )
        {
            object->ptr.lines->colours[0] = WHITE;
            status = add_object_to_current_model( graphics, object );
        }
    }

    (void) input_newline( stdin );

    return( status );
}

public  DEF_MENU_UPDATE(make_line_circle )   /* ARGSUSED */
{
    return( OK );
}


public  DEF_MENU_FUNCTION( subdivide_current_lines )   /* ARGSUSED */
{
    Status            status;
    Status            subdivide_lines();
    lines_struct      *lines;
    void              graphics_models_have_changed();

    status = OK;

    if( get_current_lines( graphics, &lines ) )
    {
        status = subdivide_lines( lines );

        if( status == OK )
            graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(subdivide_current_lines )   /* ARGSUSED */
{
    return( OK );
}
