
#include  <def_display.h>

private  void  initialize_three_d_events(
    display_struct  *display );

public  void  initialize_three_d_window(
    display_struct   *display )
{
    static   Vector        line_of_sight = { 0.0, 0.0, -1.0 };
    static                 Vector    horizontal = { 1.0, 0.0, 0.0 };
    three_d_window_struct  *three_d;

    three_d = &display->three_d;

    three_d->default_marker_structure_id = Default_marker_structure_id;
    three_d->default_marker_patient_id = Default_marker_patient_id;
    three_d->default_marker_type = (int) Default_marker_type;
    three_d->default_marker_size = Default_marker_size;
    three_d->default_marker_colour = Default_marker_colour;
    (void) strcpy( three_d->default_marker_label, Default_marker_label );

    initialize_view( &three_d->view, &line_of_sight, &horizontal );
    three_d->view.perspective_flag = Initial_perspective_flag;
    adjust_view_for_aspect( &three_d->view, display->window );
    update_view( display );

    initialize_lights( three_d->lights );

    three_d->lights[0].colour = Directional_light_colour;
    three_d->lights[0].direction = Light_direction;

    G_set_ambient_light( display->window, Ambient_light_colour );
    G_define_light( display->window, 0,
                    three_d->lights[0].light_type,
                    three_d->lights[0].colour,
                    &three_d->lights[0].direction,
                    &three_d->lights[0].position,
                    three_d->lights[0].spot_exponent,
                    three_d->lights[0].spot_angle );

    G_set_light_state( display->window, 0, three_d->lights[0].state );

    fill_Point( three_d->min_limit, 0.0, 0.0, 0.0 );
    fill_Point( three_d->max_limit, 1.0, 1.0, 1.0 );

    ADD_POINTS( three_d->centre_of_objects, three_d->min_limit,
                three_d->max_limit );
    SCALE_POINT( three_d->centre_of_objects, three_d->centre_of_objects, 0.5 );

    initialize_three_d_events( display );

    reset_view_parameters( display, &Default_line_of_sight,
                           &Default_horizontal );

    update_view( display );

    initialize_surface_edit( &display->three_d.surface_edit );
    initialize_marker_segmentation( &display->three_d.marker_segmentation );

    initialize_surface_fitting( &display->three_d.surface_fitting );

    initialize_surface_extraction( display );

    initialize_current_object( display );

    initialize_cursor( display );

    initialize_surface_curve( display );
}

private    DEF_EVENT_FUNCTION( handle_resize_three_d );

private  void  initialize_three_d_events(
    display_struct  *display )
{
    initialize_virtual_spaceball( display );

    initialize_picking_object( display );

    add_action_table_function( &display->action_table, WINDOW_RESIZE_EVENT,
                               handle_resize_three_d );
}

private  DEF_EVENT_FUNCTION( handle_resize_three_d )    /* ARGSUSED */
{
    adjust_view_for_aspect( &display->three_d.view, display->window );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( OK );
}

public  void  delete_three_d(
    display_struct  *display )
{
    delete_marker_segmentation( &display->three_d.marker_segmentation );
    terminate_current_object( &display->three_d.current_object );
    delete_surface_edit( &display->three_d.surface_edit );
    delete_surface_extraction( display );
    delete_surface_fitting( &display->three_d.surface_fitting );
}

public  void  add_object_to_current_model(
    display_struct   *display,
    object_struct     *object )
{
    model_struct   *model;

    model = get_current_model( display );

    add_object_to_model( model, object );

    set_current_object_index( display, model->n_objects-1 );

    graphics_models_have_changed( display );
}
