#include  <def_display.h>

public  void  initialize_deformation(
    deformation_struct  *deform )
{
    deform->current_object = (object_struct *) 0;
    deform->in_progress = FALSE;

    deform->deform.deform_data.type = VOLUME_DATA;
    deform->deform.model_weight = 0.5;
    deform->deform.deformation_model.model_type = POINT_SPHERE_MODEL;
    deform->deform.deformation_model.min_curvature_offset = -0.3;
    deform->deform.deformation_model.min_curvature_offset = 0.3;
    deform->deform.deformation_model.fractional_step = 0.4;
    deform->deform.deformation_model.max_step = 0.5;
    deform->deform.deformation_model.max_search_distance = 5.0;
    deform->deform.deformation_model.search_increment = 0.5;
    deform->deform.deformation_model.min_size = 0.01;
    deform->deform.deformation_model.boundary_definition.min_isovalue = 100.0;
    deform->deform.deformation_model.boundary_definition.max_isovalue = 100.0;
    deform->deform.deformation_model.boundary_definition.gradient_flag = FALSE;
    deform->deform.deformation_model.boundary_definition.
                                   variable_threshold_flag = FALSE;
    deform->deform.deformation_model.max_iterations = 1000000;
    deform->deform.deformation_model.stop_threshold = 0.0;
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
        deform_object( &display->threed.deform.deform,
                       display->three_d.deform.current_object );
    } while( current_realtime_seconds() <= end_time );
}

public  void  turn_on_deformation(
    display_struct  *display )
{
    object_struct  *object;
    Boolean        in_progress;

    in_progress = FALSE;

    if( !display->three_d.deform.in_progress )
    {
        if( get_current_polygons( display, &object ) )
        {
            if( object->object_type == POLYGONS ||
                object->object_type == LINES )
            {
                in_progress = TRUE;
            }
        }
    }

    if( in_progress )
    {
        if( object != display->three_d.deform.current_object )
        {
            display->three_d.deform.current_object = object;
            display->three_d.deform.iteration = 0;
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
        display->three_d.deform.in_progress = FALSE;
        remove_action_table_function( &display->action_table, NO_EVENT,
                                      deform_object );
    }
}
