
#include  <stdio.h>
#include  <def_graphics.h>
#include  <def_globals.h>

public  Status  create_graphics_window( graphics )
    graphics_struct   *graphics;
{
    Status   status;
    void     initialize_graphics_window();
    Status   G_create_window();

    status = G_create_window( "Window Title", &graphics->window );

    if( status == OK )
    {
        initialize_graphics_window( graphics );
    }
    else
    {
        PRINT_ERROR( "Cannot open window.\n" );
    }

    return( status );
}

private  void  initialize_graphics_window( graphics )
    graphics_struct   *graphics;
{
    void    initialize_view();
    void    adjust_view_for_aspect();
    Real    G_get_window_aspect();
    void    G_define_view();
    void    initialize_lights();
    void    G_define_light();
    void    G_set_light_state();
    void    initialize_action_table();
    void    initialize_render();
    void    initialize_objects();

    initialize_view( &graphics->view );
    adjust_view_for_aspect( &graphics->view,
                            G_get_window_aspect(&graphics->window) );
    G_define_view( &graphics->window, &graphics->view );

    initialize_lights( graphics->lights );

    G_define_light( &graphics->window, &graphics->lights[0], 0 );
    G_define_light( &graphics->window, &graphics->lights[1], 1 );

    G_set_light_state( &graphics->window, 0, graphics->lights[0].state );
    G_set_light_state( &graphics->window, 1, graphics->lights[1].state );

    initialize_action_table( &graphics->action_table );

    initialize_render( &graphics->render );
    initialize_objects( &graphics->objects );

    graphics->frame_number = 0;
    graphics->update_required = FALSE;
}

private  void  display_frame_number( graphics )
    graphics_struct   *graphics;
{
    void          G_set_view_type();
    void          G_draw_text();
    text_struct   frame_number;

    (void) sprintf( frame_number.text, "%d", graphics->frame_number );

    fill_Point( frame_number.origin, Frame_number_x, Frame_number_y, 0.0 );
    fill_Colour( frame_number.colour, 1.0, 1.0, 1.0 );

    G_set_view_type( &graphics->window, SCREEN_VIEW );

    G_draw_text( &graphics->window, &frame_number, &graphics->render );
}

public  void  update_graphics( graphics )
    graphics_struct   *graphics;
{
    void          G_update_window();
    void          display_objects();
    void          display_frame_number();

    display_objects( &graphics->window, graphics->objects, &graphics->render );

    ++graphics->frame_number;

    display_frame_number( graphics );

    G_update_window( &graphics->window );

    graphics->update_required = FALSE;
}

public  Status  delete_graphics_window( graphics )
    graphics_struct   *graphics;
{
    Status   status;
    Status   G_delete_window();
    Status   terminate_graphics_window();

    status = G_delete_window( &graphics->window );

    if( status == OK )
    {
        status = terminate_graphics_window( graphics );
    }

    return( status );
}

private  Status  terminate_graphics_window( graphics )
    graphics_struct   *graphics;
{
    Status   status;
    Status   delete_object_struct();

    status = delete_object_struct( &graphics->objects );

    return( status );
}

public  void  update_view( graphics )
    graphics_struct  *graphics;
{
    void   G_define_view();

    G_define_view( &graphics->window, &graphics->view );
}
