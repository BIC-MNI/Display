#include  <display.h>

public  void  initialize_deformation(
    deformation_struct  *deform )
{
    deform->deforming_object = (object_struct *) 0;
    deform->in_progress = FALSE;

    initialize_deformation_parameters( &deform->deform );
    deform->deform.degrees_continuity = Volume_continuity;

/*
    set_default_line_annealing_parameters( &deform->anneal );
*/
}

public  void  delete_deformation(
    deformation_struct  *deform )
{
    delete_deformation_parameters( &deform->deform );
}

private  DEF_EVENT_FUNCTION( deform_object )    /* ARGSUSED */
{
    polygons_struct   *polygons;
    Real              end_time;

    end_time = current_realtime_seconds() + Max_background_seconds;

    do
    {
        display->three_d.deform.deform.deform_data.volume =
                                                 get_volume( display );
        display->three_d.deform.deform.deform_data.label_volume =
                                                 get_label_volume( display );

        if( display->three_d.deform.using_simulated_annealing )
        {
/*
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
*/
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
                polygons = get_polygons_ptr(display->three_d.deform.
                                            deforming_object);
                delete_the_bintree( (bintree_struct **) (&polygons->bintree) );
                deform_polygons_one_iteration( polygons,
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
    BOOLEAN         use_simulated_annealling )
{
    BOOLEAN        in_progress;

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
        display->three_d.deform.deform.deform_data.label_volume =
                                                 get_label_volume( display );

        display->three_d.deform.using_simulated_annealing =
                                         use_simulated_annealling;

        if( object != display->three_d.deform.deforming_object )
        {
            display->three_d.deform.deforming_object = object;
            display->three_d.deform.iteration = 0;
        }

        if( use_simulated_annealling )
        {
/*
            initialize_deform_line_annealing( 
                  get_lines_ptr(display->three_d.deform.deforming_object),
                  &display->three_d.deform.deform,
                  &display->three_d.deform.anneal );
*/
        }

        if( object->object_type == POLYGONS &&
            check_correct_deformation_polygons( get_polygons_ptr(object),
                                                &display->three_d.deform.
                                                deform.deformation_model ) ||
            object->object_type == LINES &&
            check_correct_deformation_lines( get_lines_ptr(object),
                                             &display->three_d.deform.
                                             deform.deformation_model ) )
        {
            display->three_d.deform.in_progress = TRUE;
            add_action_table_function( &display->action_table, NO_EVENT,
                                       deform_object );
        }
    }
}

public  void  turn_off_deformation(
    display_struct  *display )
{
    display_struct  *slice_window;

    if( display->three_d.deform.in_progress )
    {
        print( "Stopping deformation.\n" );

/*
        if( display->three_d.deform.using_simulated_annealing )
            terminate_deform_line_annealing( &display->three_d.deform.deform,
                                             &display->three_d.deform.anneal );
*/

        display->three_d.deform.in_progress = FALSE;
        remove_action_table_function( &display->action_table, NO_EVENT,
                                      deform_object );
        if( display->three_d.deform.deforming_object->object_type == POLYGONS &&
            get_slice_window( display, &slice_window ) )
        {
            print( "Scanning polygons to voxels: \n" );

            set_all_volume_label_data( get_label_volume(display), 0 );

            scan_polygons_to_voxels(
                  get_polygons_ptr(display->three_d.deform.deforming_object),
                  get_volume(slice_window),
                  get_label_volume(slice_window),
                  get_current_paint_label(slice_window),
                  Max_polygon_scan_distance );

            print( "--- done.\n" );

            compute_polygon_normals(
                get_polygons_ptr(display->three_d.deform.deforming_object) );

            set_slice_window_all_update( display->associated[SLICE_WINDOW],
                                         UPDATE_LABELS );
        }
    }
}
