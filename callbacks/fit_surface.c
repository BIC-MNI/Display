
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
        if( scanf( "%lf", &tmp_parameters[i] ) != 1 )
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

public  DEF_MENU_FUNCTION(fit_surface)   /* ARGSUSED */
{
    int                  n_parameters, n_fitting_evaluations;
    Status               status;
    minimization_struct  minimization;
    Status               initialize_amoeba();
    void                 amoeba();
    Status               terminate_amoeba();
    void                 display_parameters();

    n_parameters = graphics->three_d.surface_fitting.surface_representation->
           get_num_parameters( graphics->three_d.surface_fitting.descriptors );

    PRINT( "Starting parameters: " );
    display_parameters( &graphics->three_d.surface_fitting,
                        graphics->three_d.surface_fitting.parameters );

    graphics->three_d.surface_fitting.n_samples = N_fitting_samples;
    graphics->three_d.surface_fitting.gradient_strength_factor =
                                      Gradient_strength_factor;
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
        status = initialize_amoeba( &minimization, (void *) graphics,
                                graphics->three_d.surface_fitting.parameters,
                                n_parameters );

    if( status == OK )
        amoeba( &minimization, n_parameters, Fitting_tolerance,
                Max_fitting_evaluations, &n_fitting_evaluations, 
                graphics->three_d.surface_fitting.parameters );

    if( status == OK )
        status = terminate_amoeba( &minimization );

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
    (void) scanf( "%s", filename );

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
    (void) scanf( "%s", filename );

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
            (void) scanf( "%lf", &new_descriptors[i] );
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
