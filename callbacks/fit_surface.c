
#include  <def_display.h>

public  DEF_MENU_FUNCTION(set_model_parameters)   /* ARGSUSED */
{
    int      i, n_parameters, ch;
    double   *tmp_parameters;

    n_parameters = display->three_d.surface_fitting.surface_representation->
           get_num_parameters( display->three_d.surface_fitting.descriptors );
    print( "Current parameters: " );
    display_parameters( &display->three_d.surface_fitting,
                        display->three_d.surface_fitting.parameters );
    print( "\n" );

    ALLOC( tmp_parameters, n_parameters );

    print( "Enter new values of parameters: " );
    for_less( i, 0, n_parameters )
    {
        if( input_double( stdin, &tmp_parameters[i] ) != OK )
        {
            tmp_parameters[i] = display->three_d.surface_fitting.parameters[i];
            ch = getchar();
            if( ch < 'a' || ch > 'z' )
                break;
        }
    }

    if( i == n_parameters )
    {
        for_less( i, 0, n_parameters )
            display->three_d.surface_fitting.parameters[i] = tmp_parameters[i];
        print( "New parameters: " );
        display_parameters( &display->three_d.surface_fitting,
                            display->three_d.surface_fitting.parameters );
    }

    FREE( tmp_parameters );

    return( OK );
}

public  DEF_MENU_UPDATE(set_model_parameters)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(delete_all_surface_points)   /* ARGSUSED */
{
    delete_surface_fitting_points( &display->three_d.surface_fitting );

    return( OK );
}

public  DEF_MENU_UPDATE(delete_all_surface_points)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(add_surface_point)   /* ARGSUSED */
{
    int      x, y, z, axis_index;
    Real     x_w, y_w, z_w;
    Point    point;

    if( get_voxel_under_mouse( display, &x, &y, &z, &axis_index ) )
    {
        convert_voxel_to_world(
                         &display->associated[SLICE_WINDOW]->slice.volume,
                                (Real) x, (Real) y, (Real) z,
                                &x_w, &y_w, &z_w );
        fill_Point( point, x_w, y_w, z_w );

        set_voxel_label_flag( &display->associated[SLICE_WINDOW]->slice.volume,
                              x, y, z, TRUE );
        set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
    }
    else
        point = display->three_d.cursor.origin;

    add_surface_fitting_point( &display->three_d.surface_fitting, &point );

    return( OK );
}

public  DEF_MENU_UPDATE(add_surface_point)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(delete_surface_point)   /* ARGSUSED */
{
    int      x, y, z, axis_index;
    Real     x_w, y_w, z_w;
    Point    point;

    if( get_voxel_under_mouse( display, &x, &y, &z, &axis_index ) )
    {
        convert_voxel_to_world(
                         &display->associated[SLICE_WINDOW]->slice.volume,
                         (Real) x, (Real) y, (Real) z,
                         &x_w, &y_w, &z_w );
        fill_Point( point, x_w, y_w, z_w );
        set_voxel_label_flag( &display->associated[SLICE_WINDOW]->slice.volume,
                              x, y, z, FALSE );
        set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
    }
    else
        point = display->three_d.cursor.origin;

    delete_surface_fitting_point( &display->three_d.surface_fitting, &point );

    return( OK );
}

public  DEF_MENU_UPDATE(delete_surface_point)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(show_all_surface_points)   /* ARGSUSED */
{
    int            i;
    Real           x, y, z;
    volume_struct  *volume;

    if( get_slice_window_volume( display, &volume ) )
    {
        for_less( i, 0, display->three_d.surface_fitting.n_surface_points )
        {
            convert_world_to_voxel( volume,
                Point_x(display->three_d.surface_fitting.surface_points[i]),
                Point_y(display->three_d.surface_fitting.surface_points[i]),
                Point_z(display->three_d.surface_fitting.surface_points[i]),
                &x, &y, &z );

            set_voxel_label_flag( volume, ROUND(x), ROUND(y), ROUND(z), TRUE );
        }

        set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(show_all_surface_points)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(save_surface_points)   /* ARGSUSED */
{
    int      i;
    FILE     *file;
    String   filename;
    Status   status;

    print( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    (void) input_newline( stdin );

    if( status == OK )
        status = open_file_with_default_suffix( filename, "obj", WRITE_FILE,
                                                ASCII_FORMAT, &file );

    if( status == OK )
    {
        for_less( i, 0, display->three_d.surface_fitting.n_surface_points )
        {
            status = io_point( file, WRITE_FILE, ASCII_FORMAT,
                       &display->three_d.surface_fitting.surface_points[i] );

            if( status == OK )
                status = io_newline( file, WRITE_FILE, ASCII_FORMAT );
        }
    }

    if( status == OK )
        status = close_file( file );

    return( status );
}

public  DEF_MENU_UPDATE(save_surface_points)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(load_surface_points)   /* ARGSUSED */
{
    Real     x, y, z;
    FILE     *file;
    Point    point;
    String   filename;
    Status   status;

    print( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    (void) input_newline( stdin );

    if( status == OK )
        status = open_file_with_default_suffix( filename, "obj", READ_FILE,
                                                ASCII_FORMAT, &file );

    while( status == OK )
    {
        status = io_point( file, READ_FILE, ASCII_FORMAT, &point );

        if( status == OK )
        {
            add_surface_fitting_point(
                             &display->three_d.surface_fitting, &point );

            convert_world_to_voxel(
                         &display->associated[SLICE_WINDOW]->slice.volume,
                         Point_x(point), Point_y(point), Point_z(point),
                         &x, &y, &z );

            set_voxel_label_flag(
                           &display->associated[SLICE_WINDOW]->slice.volume,
                           ROUND(x), ROUND(y), ROUND(z), TRUE );
        }
    }

    set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
    set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
    set_slice_window_update( display->associated[SLICE_WINDOW], 2 );

    if( status == OK )
        status = close_file( file );

    return( OK );
}

public  DEF_MENU_UPDATE(load_surface_points)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(fit_surface)   /* ARGSUSED */
{
    int                  i, n_parameters;
    const    double      TOLERANCE = 1.0e-4;

    n_parameters = display->three_d.surface_fitting.surface_representation->
           get_num_parameters( display->three_d.surface_fitting.descriptors );

    print( "Starting parameters: " );
    display_parameters( &display->three_d.surface_fitting,
                        display->three_d.surface_fitting.parameters );

    display->three_d.surface_fitting.n_samples = N_fitting_samples;
    display->three_d.surface_fitting.isovalue_factor = Isovalue_factor;
    display->three_d.surface_fitting.isovalue = Fitting_isovalue;
    display->three_d.surface_fitting.gradient_strength_factor =
                                      Gradient_strength_factor;
    display->three_d.surface_fitting.gradient_strength_exponent =
                                      Gradient_strength_exponent;
    display->three_d.surface_fitting.curvature_factor = Curvature_factor;
    display->three_d.surface_fitting.surface_point_distance_factor =
                                      Surface_point_distance_factor;
    display->three_d.surface_fitting.surface_point_distance_threshold =
                                      Surface_point_distance_threshold;

    if( display->three_d.surface_fitting.n_surface_points > 0 )
        ALLOC( display->three_d.surface_fitting.surface_point_distances,
               display->three_d.surface_fitting.n_surface_points );

    switch( (Minimization_methods) Minimization_method )
    {
    case DOWNHILL_SIMPLEX:
        apply_simplex_minimization( n_parameters,
                              display->three_d.surface_fitting.parameters,
                              evaluate_graphics_fit, (void *) display );
        break;

    case ONE_PARAMETER_MINIMIZATION:
        for_less( i, 0, n_parameters )
        {
            display->three_d.surface_fitting.max_parameter_deltas[i] =
                    Max_parameter_delta;
        }

        apply_one_parameter_minimization(
                   display->three_d.surface_fitting.surface_representation,
                   display->three_d.surface_fitting.descriptors,
                   1, TOLERANCE, n_parameters,
                   display->three_d.surface_fitting.parameters,
                   display->three_d.surface_fitting.max_parameter_deltas,
                   display->three_d.surface_fitting.parameter_deltas,
                   evaluate_graphics_surface_point_distances,
                   evaluate_graphics_fit_with_range, (void *) display );
        break;

    default:
        print( "Unrecognized minimization.\n" );
    }
  
    if( display->three_d.surface_fitting.n_surface_points > 0 )
    {
        FREE( display->three_d.surface_fitting.surface_point_distances );
    }

    print( "Resulting parameters: " );
    display_parameters( &display->three_d.surface_fitting,
                        display->three_d.surface_fitting.parameters );
       
    return( OK );
}

public  DEF_MENU_UPDATE(fit_surface)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(create_surface_model)   /* ARGSUSED */
{
    surface_fitting_struct  *surface_fitting;
    object_struct           *object;

    surface_fitting = &display->three_d.surface_fitting;

    create_model_of_surface( surface_fitting->surface_representation,
                             surface_fitting->descriptors,
                             surface_fitting->parameters,
                             Surface_model_resolution,
                             Surface_model_resolution, &object );

    add_object_to_current_model( display, object );

    return( OK );
}

public  DEF_MENU_UPDATE(create_surface_model)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(scan_model_to_voxels)   /* ARGSUSED */
{
    volume_struct  *volume;

    if( get_slice_window_volume( display, &volume ) )
    {
        set_all_voxel_label_flags( volume, FALSE );

        print( "Scanning to voxels " );
        (void) fflush( stdout );
        scan_to_voxels(
                 display->three_d.surface_fitting.surface_representation,
                 display->three_d.surface_fitting.descriptors,
                 volume, display->three_d.surface_fitting.parameters,
                 Max_voxel_scan_distance,
                 Max_parametric_scan_distance );
        print( " done.\n" );

        set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(scan_model_to_voxels)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(load_model_parameters)   /* ARGSUSED */
{
    Status   status;
    int      i, n_parameters;
    double   *tmp_parameters;
    String   filename;
    FILE     *file;

    print( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    (void) input_newline( stdin );

    if( status == OK )
        status = open_file( filename, READ_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        n_parameters = display->three_d.surface_fitting.
           surface_representation->
           get_num_parameters( display->three_d.surface_fitting.descriptors );

        ALLOC( tmp_parameters, n_parameters );
    }

    if( status == OK )
    {
        for_less( i, 0, n_parameters )
        {
            status = io_double( file, READ_FILE, ASCII_FORMAT,
                                &tmp_parameters[i] );
            if( status != OK )  break;
        }
    }

    if( status == OK )
        status = close_file( file );

    if( status == OK )
    {
        for_less( i, 0, n_parameters )
            display->three_d.surface_fitting.parameters[i] = tmp_parameters[i];

        FREE( tmp_parameters );
    }

    return( status );
}

public  DEF_MENU_UPDATE(load_model_parameters)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(save_model_parameters)   /* ARGSUSED */
{
    Status   status;
    int      i, n_parameters;
    String   filename;
    FILE     *file;

    print( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    (void) input_newline( stdin );

    if( status == OK )
        status = open_file( filename, WRITE_FILE, ASCII_FORMAT, &file );

    n_parameters = display->three_d.surface_fitting.
           surface_representation->
           get_num_parameters( display->three_d.surface_fitting.descriptors );

    if( status == OK )
    {
        for_less( i, 0, n_parameters )
        {
            status = io_double( file, WRITE_FILE, ASCII_FORMAT,
                         &display->three_d.surface_fitting.parameters[i] );
            if( status == OK && i % 4 == 3 )
                status = io_newline( file, WRITE_FILE, ASCII_FORMAT );

            if( status != OK )  break;
        }
    }

    if( status == OK )
        status = close_file( file );

    return( status );
}

public  DEF_MENU_UPDATE(save_model_parameters)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(convert_to_new_representation)   /* ARGSUSED */
{
    Status                    status;
    int                       i;
    double                    *new_descriptors;
    Surface_representations   new_rep;
    surface_rep_struct        *surface_rep;
    char                      ch;

    print( "Enter new surface representation type ['q' or 's']: " );

    status = input_nonwhite_character( stdin, &ch );

    if( status == OK )
    {
        switch( ch )
        {
        case 'q':
        case 'Q':   new_rep = SUPERQUADRIC;   break;

        case 's':
        case 'S':   new_rep = SPLINE;   break;

        default:    print( "Invalid type: %c.\n", ch );
                    status = ERROR;
                    break;
        }
    }

    if( status == OK && !lookup_surface_representation( new_rep, &surface_rep ))
    {
        print( "Could not find representation.\n" );
        status = ERROR;
    }

    if( status == OK )
    {
        ALLOC( new_descriptors, surface_rep->n_descriptors );

        for_less( i, 0, surface_rep->n_descriptors )
        {
            print( "Enter descriptor[%d]: ", i+1 );
            status = input_double( stdin, &new_descriptors[i] );
        }
    }

    if( status == OK )
    {
        convert_to_new_surface_representation(
                       &display->three_d.surface_fitting, surface_rep,
                       new_descriptors );

        FREE( new_descriptors );
    }
   
    return( status );
}

public  DEF_MENU_UPDATE(convert_to_new_representation)   /* ARGSUSED */
{
    return( OK );
}
