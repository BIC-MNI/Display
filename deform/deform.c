#include  <def_display.h>

public  void  initialize_deformation(
    deformation_struct  *deform )
{
    deform->deforming_object = (object_struct *) 0;
    deform->in_progress = FALSE;

    deform->deform.deform_data.type = VOLUME_DATA;
    deform->deform.model_weight = 0.5;
    deform->deform.deformation_model.model_type = POINT_SPHERE_MODEL;
    deform->deform.deformation_model.min_curvature_offset = -0.3;
    deform->deform.deformation_model.max_curvature_offset = 0.3;
    deform->deform.deformation_model.position_constrained = FALSE;
    deform->deform.fractional_step = 0.4;
    deform->deform.max_step = 0.5;
    deform->deform.max_search_distance = 30.0;
    deform->deform.search_increment = 0.5;
    deform->deform.min_size = 0.01;
    deform->deform.boundary_definition.min_isovalue = 100.0;
    deform->deform.boundary_definition.max_isovalue = 100.0;
    deform->deform.boundary_definition.normal_direction = TOWARDS_LOWER;
    deform->deform.max_iterations = 1000000;
    deform->deform.stop_threshold = 0.0;

    set_default_line_annealing_parameters( &deform->anneal );
}

public  void  delete_deformation(
    deformation_struct  *deform )
{
    delete_deformation_parameters( &deform->deform );
}

private  DEF_EVENT_FUNCTION( deform_object )    /* ARGSUSED */
{
    Real  end_time;

    end_time = current_realtime_seconds() + Max_background_seconds;

    do
    {
        display->three_d.deform.deform.deform_data.volume =
                                                 get_volume( display );

        if( display->three_d.deform.using_simulated_annealing )
        {
            switch( display->three_d.deform.deforming_object->object_type )
            {
            case LINES:
                if( apply_simulated_annealing_deform_lines(
                     get_lines_ptr(display->three_d.deform.deforming_object),
                     &display->three_d.deform.deform,
                     &display->three_d.deform.anneal ) )
                {
                     turn_off_deformation( display );
                }
                                
                break;
            case POLYGONS:
                break;
            }
        }
        else
        {
            ++display->three_d.deform.iteration;
            switch( display->three_d.deform.deforming_object->object_type )
            {
            case LINES:
                deform_lines_one_iteration(
                     get_lines_ptr(display->three_d.deform.deforming_object),
                     &display->three_d.deform.deform,
                     display->three_d.deform.iteration );
                break;

            case POLYGONS:
                delete_polygons_bintree(
                   get_polygons_ptr(display->three_d.deform.deforming_object) );
                deform_polygons_one_iteration(
                     get_polygons_ptr(display->three_d.deform.deforming_object),
                     &display->three_d.deform.deform,
                     display->three_d.deform.iteration );
                break;
            }
        }
    } while( current_realtime_seconds() <= end_time );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  void  turn_on_deformation(
    display_struct  *display,
    object_struct   *object,
    Boolean         use_simulated_annealling )
{
    Boolean        in_progress;

    in_progress = FALSE;

    if( !display->three_d.deform.in_progress )
    {
        if( (object->object_type == POLYGONS && !use_simulated_annealling) ||
            object->object_type == LINES )
        {
            in_progress = TRUE;
        }
    }

    if( in_progress )
    {
        display->three_d.deform.deform.deform_data.volume =
                                                 get_volume( display );

        display->three_d.deform.using_simulated_annealing =
                                         use_simulated_annealling;

        if( object != display->three_d.deform.deforming_object )
        {
            display->three_d.deform.deforming_object = object;
            display->three_d.deform.iteration = 0;
        }

        if( use_simulated_annealling )
        {
            initialize_deform_line_annealing( 
                  get_lines_ptr(display->three_d.deform.deforming_object),
                  &display->three_d.deform.deform,
                  &display->three_d.deform.anneal );
        }

        display->three_d.deform.in_progress = TRUE;
        add_action_table_function( &display->action_table, NO_EVENT,
                                   deform_object );
    }
}

public  void  turn_off_deformation(
    display_struct  *display )
{
    if( display->three_d.deform.in_progress )
    {
        print( "Stopping deformation.\n" );

        if( display->three_d.deform.using_simulated_annealing )
            terminate_deform_line_annealing( &display->three_d.deform.deform,
                                             &display->three_d.deform.anneal );

        display->three_d.deform.in_progress = FALSE;
        remove_action_table_function( &display->action_table, NO_EVENT,
                                      deform_object );
        if( display->three_d.deform.deforming_object->object_type == POLYGONS )
        {
            print( "Scanning polygons to voxels: \n" );

            set_all_voxel_label_flags( get_volume(display), FALSE );

            scan_polygons_to_voxels(
                  get_polygons_ptr(display->three_d.deform.deforming_object),
                  get_volume(display),
                  Scanned_polygons_label | ACTIVE_BIT,
                  Max_polygon_scan_distance );

            print( "--- done.\n" );

            compute_polygon_normals(
                get_polygons_ptr(display->three_d.deform.deforming_object) );

            set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
            set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
            set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
        }
    }
}
