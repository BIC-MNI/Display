
#include  <def_stdio.h>
#include  <def_string.h>
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_alloc.h>

public  Status  initialize_three_d_window( graphics )
    graphics_struct   *graphics;
{
    static   Vector        line_of_sight = { 0.0, 0.0, -1.0 };
    static                 Vector    horizontal = { 1.0, 0.0, 0.0 };
    void                   initialize_view();
    void                   adjust_view_for_aspect();
    void                   G_define_3D_view();
    void                   initialize_lights();
    void                   G_define_light();
    void                   G_set_light_state();
    void                   initialize_three_d_events();
    Status                 status;
    Status                 initialize_current_object();
    Status                 initialize_surface_extraction();
    Status                 initialize_cursor();
    void                   initialize_surface_edit();
    void                   reset_view_parameters();
    three_d_window_struct  *three_d;
    void                   update_view();

    three_d = &graphics->three_d;

    initialize_view( &three_d->view, &line_of_sight, &horizontal );
    three_d->view.perspective_flag = Initial_perspective_flag;
    adjust_view_for_aspect( &three_d->view, &graphics->window );
    G_define_3D_view( &graphics->window, &three_d->view );

    initialize_lights( three_d->lights );

    G_define_light( &graphics->window, &three_d->lights[0], 0 );
    G_define_light( &graphics->window, &three_d->lights[1], 1 );

    G_set_light_state( &graphics->window, 0, three_d->lights[0].state );
    G_set_light_state( &graphics->window, 1, three_d->lights[1].state );

    fill_Point( three_d->min_limit, 0.0, 0.0, 0.0 );
    fill_Point( three_d->max_limit, 1.0, 1.0, 1.0 );

    ADD_POINTS( three_d->centre_of_objects, three_d->min_limit,
                three_d->max_limit );
    SCALE_POINT( three_d->centre_of_objects, three_d->centre_of_objects, 0.5 );

    initialize_three_d_events( graphics );

    reset_view_parameters( graphics, &Default_line_of_sight,
                           &Default_horizontal );

    update_view( graphics );

    initialize_surface_edit( &graphics->three_d.surface_edit );

    status = initialize_surface_extraction( graphics );

    if( status == OK )
    {
        status = initialize_current_object( &graphics->three_d.current_object );
    }

    if( status == OK )
    {
        status = initialize_cursor( graphics );
    }

    return( status );
}

private  void  initialize_three_d_events( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( handle_resize_three_d );
    void                 add_action_table_function();
    void                 initialize_virtual_spaceball();
    void                 initialize_picking_polygon();

    initialize_virtual_spaceball( graphics );

    initialize_picking_polygon( graphics );

    add_action_table_function( &graphics->action_table, WINDOW_RESIZE_EVENT,
                               handle_resize_three_d );
}

private  DEF_EVENT_FUNCTION( handle_resize_three_d )
    /* ARGSUSED */
{
    void   adjust_view_for_aspect();
    void   update_view();
    void   set_update_required();

    adjust_view_for_aspect( &graphics->three_d.view, &graphics->window );

    update_view( graphics );

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

public  Status  delete_three_d( graphics )
    graphics_struct  *graphics;
{
    Status    status;
    Status    terminate_current_object();
    Status    delete_surface_extraction();
    Status    delete_surface_edit();

    status = terminate_current_object( &graphics->three_d.current_object );

    if( status == OK )
    {
        status = delete_surface_edit( &graphics->three_d.surface_edit );
    }

    if( status == OK )
    {
        status = delete_surface_extraction( graphics );
    }

    return( status );
}
