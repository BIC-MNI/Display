
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_math.h>
#include  <def_files.h>
#include  <def_minimization.h>

public  DEF_MENU_FUNCTION(set_model_parameters)   /* ARGSUSED */
{
    Status   status;
    int      i, n_parameters, ch;
    double   *tmp_parameters;
    void     display_parameters();

    n_parameters = graphics->three_d.surface_fitting.surface_representation->
           get_num_parameters( graphics->three_d.surface_fitting.descriptors );
    PRINT( "Current parameters: " );
    display_parameters( &graphics->three_d.surface_fitting,
                        graphics->three_d.surface_fitting.parameters );
    PRINT( "\n" );

    ALLOC( status, tmp_parameters, n_parameters );

    PRINT( "Enter new values of parameters: " );
    for_less( i, 0, n_parameters )
    {
        if( input_double( stdin, &tmp_parameters[i] ) != OK )
        {
            tmp_parameters[i] = graphics->three_d.surface_fitting.parameters[i];
            ch = getchar();
            if( ch < 'a' || ch > 'z' )
                break;
        }
    }

    if( i == n_parameters )
    {
        for_less( i, 0, n_parameters )
            graphics->three_d.surface_fitting.parameters[i] = tmp_parameters[i];
        PRINT( "New parameters: " );
        display_parameters( &graphics->three_d.surface_fitting,
                            graphics->three_d.surface_fitting.parameters );
    }

    if( status == OK )
        FREE( status, tmp_parameters );

    return( status );
}

public  DEF_MENU_UPDATE(set_model_parameters)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(delete_all_surface_points)   /* ARGSUSED */
{
    Status   status;
    Status   delete_surface_fitting_points();

    status = delete_surface_fitting_points( &graphics->three_d.surface_fitting);

    return( status );
}

public  DEF_MENU_UPDATE(delete_all_surface_points)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(add_surface_point)   /* ARGSUSED */
{
    Status   status;
    int      x, y, z, axis_index;
    Point    point;
    void     convert_voxel_to_point();
    void     set_voxel_label_flag();
    Status   add_surface_fitting_point();
    void     set_slice_window_update();


    if( get_voxel_under_mouse( graphics, &x, &y, &z, &axis_index ) )
    {
        convert_voxel_to_point(
                         graphics->associated[SLICE_WINDOW]->slice.volume,
                                (Real) x, (Real) y, (Real) z, &point );

        set_voxel_label_flag( graphics->associated[SLICE_WINDOW]->slice.volume,
                              x, y, z, TRUE );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
    }
    else
        point = graphics->three_d.cursor.origin;

    status = add_surface_fitting_point( &graphics->three_d.surface_fitting,
                                        &point );

    return( status );
}

public  DEF_MENU_UPDATE(add_surface_point)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(delete_surface_point)   /* ARGSUSED */
{
    Status   status;
    int      x, y, z, axis_index;
    Point    point;
    void     convert_voxel_to_point();
    Status   delete_surface_fitting_point();
    void     set_slice_window_update();

    if( get_voxel_under_mouse( graphics, &x, &y, &z, &axis_index ) )
    {
        convert_voxel_to_point(
                         graphics->associated[SLICE_WINDOW]->slice.volume,
                                (Real) x, (Real) y, (Real) z, &point );
        set_voxel_label_flag( graphics->associated[SLICE_WINDOW]->slice.volume,
                              x, y, z, FALSE );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
    }
    else
        point = graphics->three_d.cursor.origin;

    status = delete_surface_fitting_point( &graphics->three_d.surface_fitting,
                                           &point );

    return( status );
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
    void           convert_point_to_voxel();
    void           set_voxel_label_flag();
    void           set_slice_window_update();
    void           set_all_voxel_label_flags();

    if( get_slice_window_volume( graphics, &volume ) )
    {
        for_less( i, 0, graphics->three_d.surface_fitting.n_surface_points )
        {
            convert_point_to_voxel( volume,
                Point_x(graphics->three_d.surface_fitting.surface_points[i]),
                Point_y(graphics->three_d.surface_fitting.surface_points[i]),
                Point_z(graphics->three_d.surface_fitting.surface_points[i]),
                &x, &y, &z );

            set_voxel_label_flag( volume, ROUND(x), ROUND(y), ROUND(z), TRUE );
        }

        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
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
    Status   open_file();
    Status   io_point();
    Status   io_newline();
    Status   close_file();

    PRINT( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    if( status == OK )
        status = input_newline( stdin );

    if( status == OK )
        status = open_file( filename, WRITE_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        for_less( i, 0, graphics->three_d.surface_fitting.n_surface_points )
        {
            status = io_point( file, WRITE_FILE, ASCII_FORMAT,
                       &graphics->three_d.surface_fitting.surface_points[i] );

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
    Status   status, input_status;
    Status   open_file();
    Status   io_point();
    Status   add_surface_fitting_point();
    Status   close_file();
    void     convert_point_to_voxel();
    void     set_voxel_label_flag();
    void     set_slice_window_update();

    PRINT( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    if( status == OK )
        status = input_newline( stdin );

    if( status == OK )
        status = open_file( filename, READ_FILE, ASCII_FORMAT, &file );

    while( status == OK )
    {
        input_status = io_point( file, READ_FILE, ASCII_FORMAT, &point );

        if( input_status == OK )
            status = add_surface_fitting_point(
                             &graphics->three_d.surface_fitting, &point );

        if( status == OK )
        {
            convert_point_to_voxel(
                         graphics->associated[SLICE_WINDOW]->slice.volume,
                         Point_x(point), Point_y(point), Point_z(point),
                         &x, &y, &z );

            set_voxel_label_flag(
                           graphics->associated[SLICE_WINDOW]->slice.volume,
                           ROUND(x), ROUND(y), ROUND(z), TRUE );
        }
    }

    set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
    set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
    set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );

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
    Status               status;
    Status               apply_simplex_minimization();
    void                 apply_one_parameter_minimization();
    double               evaluate_graphics_fit( void *, double [] );
    double               evaluate_graphics_fit_with_range( void *, double [],
                                  double, double, double, double, Real [] );
    void                 evaluate_graphics_surface_point_distances( void *,
                           double [], Real [], double, double, double, double );
    void                 display_parameters();
    const    double      TOLERANCE = 1.0e-4;

    n_parameters = graphics->three_d.surface_fitting.surface_representation->
           get_num_parameters( graphics->three_d.surface_fitting.descriptors );

    PRINT( "Starting parameters: " );
    display_parameters( &graphics->three_d.surface_fitting,
                        graphics->three_d.surface_fitting.parameters );

    graphics->three_d.surface_fitting.n_samples = N_fitting_samples;
    graphics->three_d.surface_fitting.isovalue_factor = Isovalue_factor;
    graphics->three_d.surface_fitting.isovalue = Fitting_isovalue;
    graphics->three_d.surface_fitting.gradient_strength_factor =
                                      Gradient_strength_factor;
    graphics->three_d.surface_fitting.gradient_strength_exponent =
                                      Gradient_strength_exponent;
    graphics->three_d.surface_fitting.curvature_factor = Curvature_factor;
    graphics->three_d.surface_fitting.surface_point_distance_factor =
                                      Surface_point_distance_factor;
    graphics->three_d.surface_fitting.surface_point_distance_threshold =
                                      Surface_point_distance_threshold;

    status = OK;

    if( graphics->three_d.surface_fitting.n_surface_points > 0 )
        ALLOC( status,
                graphics->three_d.surface_fitting.surface_point_distances,
                graphics->three_d.surface_fitting.n_surface_points );

    if( status == OK )
    {
        switch( (Minimization_methods) Minimization_method )
        {
        case DOWNHILL_SIMPLEX:
            status = apply_simplex_minimization( n_parameters,
                                  graphics->three_d.surface_fitting.parameters,
                                  evaluate_graphics_fit, (void *) graphics );
            break;

        case ONE_PARAMETER_MINIMIZATION:
            for_less( i, 0, n_parameters )
            {
                graphics->three_d.surface_fitting.max_parameter_deltas[i] =
                        Max_parameter_delta;
            }

            apply_one_parameter_minimization(
                       graphics->three_d.surface_fitting.surface_representation,
                       graphics->three_d.surface_fitting.descriptors,
                       1, TOLERANCE, n_parameters,
                       graphics->three_d.surface_fitting.parameters,
                       graphics->three_d.surface_fitting.max_parameter_deltas,
                       graphics->three_d.surface_fitting.parameter_deltas,
                       evaluate_graphics_surface_point_distances,
                       evaluate_graphics_fit_with_range, (void *) graphics );
            break;

        default:
            PRINT_ERROR( "Unrecognized minimization.\n" );
        }
    }
  
    if( status == OK && graphics->three_d.surface_fitting.n_surface_points > 0 )
        FREE( status,
              graphics->three_d.surface_fitting.surface_point_distances );

    PRINT( "Resulting parameters: " );
    display_parameters( &graphics->three_d.surface_fitting,
                        graphics->three_d.surface_fitting.parameters );
       
    return( status );
}

public  DEF_MENU_UPDATE(fit_surface)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(create_surface_model)   /* ARGSUSED */
{
    Status                  status;
    Status                  create_model_of_surface();
    Status                  add_object_to_model();
    surface_fitting_struct  *surface_fitting;
    object_struct           *object;
    model_struct            *get_current_model();
    void                    graphics_models_have_changed();

    surface_fitting = &graphics->three_d.surface_fitting;

    status = create_model_of_surface( surface_fitting->surface_representation,
                                      surface_fitting->descriptors,
                                      surface_fitting->parameters,
                                      Surface_model_resolution,
                                      Surface_model_resolution, &object );

    if( status == OK )
        status = add_object_to_model( get_current_model(graphics), object );

    if( status == OK )
        graphics_models_have_changed( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(create_surface_model)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(scan_model_to_voxels)   /* ARGSUSED */
{
    volume_struct  *volume;
    void           set_all_voxel_label_flags();
    void           scan_to_voxels();
    void           set_slice_window_update();

    if( get_slice_window_volume( graphics, &volume ) )
    {
        set_all_voxel_label_flags( volume, FALSE );

        PRINT( "Scanning to voxels " );
        (void) fflush( stdout );
        scan_to_voxels(
                 graphics->three_d.surface_fitting.surface_representation,
                 graphics->three_d.surface_fitting.descriptors,
                 volume, graphics->three_d.surface_fitting.parameters,
                 Max_voxel_scan_distance,
                 Max_parametric_scan_distance );
        PRINT( " done.\n" );

        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
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
    Status   open_file();
    Status   io_double();
    Status   close_file();

    PRINT( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    if( status == OK )
        status = input_newline( stdin );

    if( status == OK )
        status = open_file( filename, READ_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        n_parameters = graphics->three_d.surface_fitting.
           surface_representation->
           get_num_parameters( graphics->three_d.surface_fitting.descriptors );

        ALLOC( status, tmp_parameters, n_parameters );
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
            graphics->three_d.surface_fitting.parameters[i] = tmp_parameters[i];

        FREE( status, tmp_parameters );
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
    Status   open_file();
    Status   io_double();
    Status   io_newline();
    Status   close_file();

    PRINT( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    if( status == OK )
        status = input_newline( stdin );

    if( status == OK )
        status = open_file( filename, WRITE_FILE, ASCII_FORMAT, &file );

    n_parameters = graphics->three_d.surface_fitting.
           surface_representation->
           get_num_parameters( graphics->three_d.surface_fitting.descriptors );

    if( status == OK )
    {
        for_less( i, 0, n_parameters )
        {
            status = io_double( file, WRITE_FILE, ASCII_FORMAT,
                         &graphics->three_d.surface_fitting.parameters[i] );
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
    Status                    input_nonwhite_character();
    Status                    convert_to_new_surface_representation();
    int                       i;
    double                    *new_descriptors;
    Surface_representations   new_rep;
    surface_rep_struct        *surface_rep;
    char                      ch;

    PRINT( "Enter new surface representation type ['q' or 's']: " );

    status = input_nonwhite_character( stdin, &ch );

    if( status == OK )
    {
        switch( ch )
        {
        case 'q':
        case 'Q':   new_rep = SUPERQUADRIC;   break;

        case 's':
        case 'S':   new_rep = SPLINE;   break;

        default:    PRINT( "Invalid type: %c.\n", ch );
                    status = ERROR;
                    break;
        }
    }

    if( status == OK && !lookup_surface_representation( new_rep, &surface_rep ))
    {
        PRINT( "Could not find representation.\n" );
        status = ERROR;
    }

    if( status == OK )
    {
        ALLOC( status, new_descriptors, surface_rep->n_descriptors );

        for_less( i, 0, surface_rep->n_descriptors )
        {
            PRINT( "Enter descriptor[%d]: ", i+1 );
            status = input_double( stdin, &new_descriptors[i] );
        }
    }

    if( status == OK )
    {
        status = convert_to_new_surface_representation(
                       &graphics->three_d.surface_fitting, surface_rep,
                       new_descriptors );
    }

    if( status == OK )
        FREE( status, new_descriptors );
   
    return( status );
}

public  DEF_MENU_UPDATE(convert_to_new_representation)   /* ARGSUSED */
{
    return( OK );
}
