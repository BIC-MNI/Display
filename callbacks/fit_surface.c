
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_math.h>
#include  <def_stdio.h>
#include  <def_minimization.h>

public  DEF_MENU_FUNCTION(set_model_parameters)   /* ARGSUSED */
{
    Status   status;
    int      i, n_parameters, ch;
    double   *tmp_parameters;
    void     display_parameters();

    n_parameters = graphics->three_d.surface_fitting.n_parameters;

    PRINT( "Current parameters: " );
    display_parameters( graphics->three_d.surface_fitting.parameters );
    PRINT( "\n" );

    ALLOC1( status, tmp_parameters, n_parameters, double );

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
        display_parameters( graphics->three_d.surface_fitting.parameters );
    }

    if( status == OK )
        FREE1( status, tmp_parameters );

    return( status );
}

public  DEF_MENU_UPDATE(set_model_parameters)   /* ARGSUSED */
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

    n_parameters = get_num_parameters();

    PRINT( "Starting parameters: " );
    display_parameters( graphics->three_d.surface_fitting.parameters );

    graphics->three_d.surface_fitting.n_samples = N_fitting_samples;
    graphics->three_d.surface_fitting.curvature_factor = Curvature_factor;
    graphics->three_d.surface_fitting.surface_point_distance_factor =
                                      Surface_point_distance_factor;
    graphics->three_d.surface_fitting.surface_point_distance_threshold =
                                      Surface_point_distance_threshold;

    status = OK;

    if( graphics->three_d.surface_fitting.n_surface_points > 0 )
        ALLOC1( status,
                graphics->three_d.surface_fitting.surface_point_distances,
                graphics->three_d.surface_fitting.n_surface_points,
                Real );

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
        FREE1( status,
                graphics->three_d.surface_fitting.surface_point_distances );

    PRINT( "Resulting parameters: " );
    display_parameters( graphics->three_d.surface_fitting.parameters );
       
    return( status );
}

public  DEF_MENU_UPDATE(fit_surface)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(create_surface_model)   /* ARGSUSED */
{
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
        scan_to_voxels( volume, graphics->three_d.surface_fitting.parameters,
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
