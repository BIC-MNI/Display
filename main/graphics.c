
#include  <stdio.h>
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_alloc.h>

private  graphics_struct  **windows;
private  int              n_windows = 0;

public  int  get_list_of_windows( graphics )
    graphics_struct  ***graphics;
{
    *graphics = windows;

    return( n_windows );
}

public  graphics_struct  *lookup_window( window_id )
    Window_id   window_id;
{
    int              i;
    graphics_struct  *graphics;

    graphics = (graphics_struct *) 0;

    for_less( i, 0, n_windows )
    {
        if( windows[i]->window.window_id == window_id )
        {
            graphics = windows[i];
            break;
        }
    }

    return( graphics );
}

private  Status  get_new_graphics( graphics )
    graphics_struct   **graphics;
{
    Status   status;

    CHECK_ALLOC( status, windows, n_windows, n_windows+1, graphics_struct *,
                 DEFAULT_CHUNK_SIZE );

    if( status == OK )
    {
        CALLOC( status, windows[n_windows], 1, graphics_struct );
    }

    if( status == OK )
    {
        *graphics = windows[n_windows];
        ++n_windows;
    }

    return( status );
}

private  Status  free_graphics( graphics )
    graphics_struct   *graphics;
{
    int      ind, i;
    Status   status;

    status = ERROR;

    for_less( ind, 0, n_windows )
    {
        if( windows[ind] == graphics )
        {
            status = OK;
            break;
        }
    }

    if( status == OK )
    {
        for_less( i, ind, n_windows-1 )
        {
            windows[i] = windows[i+1];
        }

        --n_windows;

        FREE( status, graphics );
    }

    return( status );
}

public  Status  create_graphics_window( graphics )
    graphics_struct   **graphics;
{
    Status   status;
    void     initialize_graphics_window();
    Status   get_new_graphics();
    Status   G_create_window();

    status = get_new_graphics( graphics );

    if( status == OK )
    {
        status = G_create_window( "Window Title", &(*graphics)->window );
    }

    if( status == OK )
    {
        initialize_graphics_window( *graphics );
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

    initialize_render( &graphics->model.render );
    initialize_objects( &graphics->model.objects );

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

    G_set_view_type( &graphics->window, PIXEL_VIEW );

    G_draw_text( &graphics->window, &frame_number, &graphics->model.render );
}

public  void  update_graphics( graphics )
    graphics_struct   *graphics;
{
    void          G_update_window();
    void          display_objects();
    void          display_frame_number();

    display_objects( &graphics->window, &graphics->model );

    ++graphics->frame_number;

    if( Display_frame_number )
    {
        display_frame_number( graphics );
    }

    G_update_window( &graphics->window );

    graphics->update_required = FALSE;
}

public  Status  delete_graphics_window( graphics )
    graphics_struct   *graphics;
{
    Status   status;
    Status   G_delete_window();
    Status   free_graphics();
    Status   terminate_graphics_window();

    status = G_delete_window( &graphics->window );

    if( status == OK )
    {
        status = terminate_graphics_window( graphics );
    }

    if( status == OK )
    {
        status = free_graphics( graphics );
    }

    return( status );
}

private  Status  terminate_graphics_window( graphics )
    graphics_struct   *graphics;
{
    Status   status;
    Status   delete_object_struct();

    status = delete_object_struct( &graphics->model.objects );

    return( status );
}

public  void  update_view( graphics )
    graphics_struct  *graphics;
{
    void   G_define_view();

    G_define_view( &graphics->window, &graphics->view );
}

public  void  reset_view_parameters( graphics )
    graphics_struct  *graphics;
{
    void   adjust_view_for_aspect();
    Real   G_get_window_aspect();
    void   initialize_view();
    void   fit_view_to_domain();

    initialize_view( &graphics->view );
    adjust_view_for_aspect( &graphics->view,
                            G_get_window_aspect(&graphics->window) );
    fit_view_to_domain( &graphics->view, &graphics->min_limit,
                        &graphics->max_limit );
}

public  void  transform_model( graphics, transform )
    graphics_struct   *graphics;
    Transform         *transform;
{
    void  concat_transforms();

    concat_transforms( &graphics->view.modeling_transform,
                       &graphics->view.modeling_transform,
                       transform );
}
