
#include  <def_display.h>

private  display_struct  **windows = (display_struct **) 0;
private  int             n_windows = 0;

private  void  initialize_graphics_window(
    display_struct   *display );
private  void  update_graphics_overlay_planes_only(
    display_struct       *display,
    Boolean              display_flag );
private  void  update_graphics_normal_planes_only(
    display_struct               *display,
    update_interrupted_struct    *interrupt );
private  void  terminate_graphics_window(
    display_struct   *display );

/* --------------------------------------------------------------------- */

public  int  get_list_of_windows(
    display_struct  ***display )
{
    *display = windows;

    return( n_windows );
}

public  display_struct  *lookup_window(
    window_struct   *window )
{
    int              i;
    display_struct   *display;

    display = (display_struct *) 0;

    for_less( i, 0, n_windows )
    {
        if( windows[i]->window == window )
        {
            display = windows[i];
            break;
        }
    }

    return( display );
}

private  void  get_new_display(
    display_struct   **display )
{
    SET_ARRAY_SIZE( windows, n_windows, n_windows+1, DEFAULT_CHUNK_SIZE );

    ALLOC( windows[n_windows], 1 );

    *display = windows[n_windows];
    ++n_windows;
}

private  void  free_display(
    display_struct   *display )
{
    int      ind, i;

    for_less( ind, 0, n_windows )
    {
        if( windows[ind] == display )
        {
            break;
        }
    }

    for_less( i, ind, n_windows-1 )
    {
        windows[i] = windows[i+1];
    }

    --n_windows;

    FREE( display );
}

private  void  delete_windows_list( void )
{
    if( windows != (display_struct **) 0 )
        FREE( windows );
}

public  void  initialize_graphics( void )
{
}

public  void  terminate_graphics( void )
{
    display_struct    **graphics_windows;

    while( get_list_of_windows( &graphics_windows ) > 0 )
    {
        delete_graphics_window( graphics_windows[0] );
    }

    delete_windows_list();

    G_terminate();
}

public  Status  create_graphics_window(
    window_types      window_type,
    display_struct    **display,
    char              title[],
    int               width,
    int               height )
{
    Status   status;

    get_new_display( display );

    status = G_create_window( title, -1, -1, width, height, 
                              &(*display)->window );

    G_set_background_colour( (*display)->window, Initial_background_colour );

    if( status == OK )
    {
        (*display)->window_type = window_type;
        initialize_graphics_window( *display );
    }
    else
    {
        print( "Cannot open window.\n" );
    }

    return( status );
}

public  model_struct  *get_graphics_model(
    display_struct    *display,
    int               model_index )
{
    return( get_model_ptr(display->models[model_index]) );
}

public  model_info_struct  *get_model_info(
    model_struct   *model )
{
    return( (model_info_struct *) model->extra_ptr );
}

public  void  create_model_after_current(
    display_struct   *display )
{
    model_struct   *model;
    object_struct  *new_model;

    model = get_current_model( display );

    new_model = create_object( MODEL );

    initialize_display_model( get_model_ptr(new_model) );

    (void) strcpy( get_model_ptr(new_model)->filename, "Created" );

    add_object_to_model( model, new_model );
}

public  void  initialize_display_model(
    model_struct   *model )
{
    model_info_struct  *model_info;

    initialize_model( model );

    ALLOC( model_info, 1 );

    assign_model_extra_ptr( model, (void *) model_info );

    model_info->view_type = MODEL_VIEW;
    model_info->bitplanes = NORMAL_PLANES;
    initialize_render( &model_info->render );
    make_identity_transform( &model_info->transform );
}

public  void  terminate_display_model(
    model_struct   *model )
{
    model_info_struct  *model_info;

    model_info = (model_info_struct *) get_model_extra_ptr( model );

    FREE( model_info );
}

private  void  initialize_graphics_window(
    display_struct   *display )
{
    int                 i;
    View_types          view_type;
    model_struct        *model;
    model_info_struct   *model_info;

    display->associated[THREE_D_WINDOW] = (display_struct *) 0;
    display->associated[MENU_WINDOW] = (display_struct *) 0;
    display->associated[SLICE_WINDOW] = (display_struct *) 0;

    display->models_changed_id = 0;

    initialize_action_table( &display->action_table );

    initialize_resize_events( display );
    initialize_menu_actions( display );

    for_less( i, 0, N_MODELS )
    {
        display->models[i] = create_object( MODEL );
        model = get_graphics_model( display, i );
        initialize_display_model( model );
        model_info = get_model_info( model );

        model->n_objects = 0;

        switch( display->window_type )
        {
        case THREE_D_WINDOW:   view_type = MODEL_VIEW;   break;
        case MENU_WINDOW:      view_type = PIXEL_VIEW;   break;
        case SLICE_WINDOW:     view_type = PIXEL_VIEW;   break;
        }

        model_info->view_type = view_type;

        if( display->window_type == THREE_D_WINDOW &&
            (i == OVERLAY_MODEL || i == CURSOR_MODEL) )
        {
            model_info->bitplanes = OVERLAY_PLANES;
        }
        else
        {
            model_info->bitplanes = NORMAL_PLANES;
        }

        model->n_objects = 0;
        (void) strcpy( model->filename, "Top Level" );

        initialize_render( &model_info->render );

        if( display->window_type == THREE_D_WINDOW && i == THREED_MODEL )
        {
            model_info->render.render_lines_as_curves =
                                           Initial_line_curves_flag;
        }

        make_identity_transform( &model_info->transform );
    }

    if( display->window_type == THREE_D_WINDOW )
        initialize_three_d_window( display );

    if( display->window_type == SLICE_WINDOW )
        initialize_slice_window( display );

    if( display->window_type == MENU_WINDOW )
        initialize_menu_window( display );

    display->frame_number = 0;
    display->update_required[NORMAL_PLANES] = FALSE;
    display->update_required[OVERLAY_PLANES] = FALSE;
    display->update_interrupted.last_was_interrupted = FALSE;
}

public  void  set_update_required(
    display_struct   *display,
    Bitplane_types   which_bitplanes )
{
    display->update_required[which_bitplanes] = TRUE;
}

public  Boolean  graphics_normal_planes_update_required(
    display_struct   *display )
{
    return( display->update_required[NORMAL_PLANES] );
}

public  Boolean  graphics_update_required(
    display_struct   *display )
{
    return( display->update_required[NORMAL_PLANES] ||
            display->update_required[OVERLAY_PLANES] );
}

public  void  graphics_models_have_changed(
    display_struct  *display )
{
    rebuild_selected_list( display, display->associated[MENU_WINDOW] );

    set_update_required( display, NORMAL_PLANES );
    set_update_required( display->associated[MENU_WINDOW], NORMAL_PLANES );

    ++display->models_changed_id;
}

private  void  display_frame_info(
    display_struct   *display,
    int              frame_number,
    Real             update_time )
{
    text_struct   frame_text;
    String        frame_time_str;
    model_struct  *model;

    (void) sprintf( frame_text.string, "%d: ", frame_number );

    format_time( frame_time_str, "%g %s", update_time );

    (void) strcat( frame_text.string, frame_time_str );

    fill_Point( frame_text.origin, Frame_info_x, Frame_info_y, 0.0 );
    frame_text.colour = WHITE;

    G_set_view_type( display->window, PIXEL_VIEW );

    model = get_graphics_model( display, THREED_MODEL );

    set_render_info( display->window, &get_model_info(model)->render );
    G_draw_text( display->window, &frame_text );
}

public  void  update_graphics(
    display_struct               *display,
    update_interrupted_struct    *interrupt )
{
    if( interrupt->last_was_interrupted ||
        display->update_required[NORMAL_PLANES] )
    {
        update_graphics_normal_planes_only( display, interrupt );

        update_graphics_overlay_planes_only( display,
                                             !interrupt->current_interrupted );
    }
    else if( display->update_required[OVERLAY_PLANES] )
    {
        update_graphics_overlay_planes_only( display, TRUE );
    }
}

private  void  update_graphics_overlay_planes_only(
    display_struct       *display,
    Boolean              display_flag )
{
    int           i;

    G_set_bitplanes( display->window, OVERLAY_PLANES );

    if( display_flag )
    {
        for_less( i, 0, N_MODELS )
        {
            display_objects( display->window, display->models[i],
                             (update_interrupted_struct *) 0, OVERLAY_PLANES );
        }
    }

    G_update_window( display->window );

    G_set_bitplanes( display->window, NORMAL_PLANES );

    display->update_required[OVERLAY_PLANES] = FALSE;
}

private  void  update_graphics_normal_planes_only(
    display_struct               *display,
    update_interrupted_struct    *interrupt )
{
    int           i;
    Real          start, end;

    if( interrupt->last_was_interrupted )
    {
        G_append_to_last_update( display->window );
    }

    if( display->window_type == SLICE_WINDOW )
        update_slice_window( display );

    start = current_realtime_seconds();

    interrupt->interrupt_at = start + Maximum_display_time;
    G_set_interrupt_time( display->window, interrupt->interrupt_at );

    interrupt->current_interrupted = FALSE;

    for_less( i, 0, N_MODELS )
    {
        display_objects( display->window, display->models[i],
                         interrupt, NORMAL_PLANES );

        if( interrupt->current_interrupted )
            break;
    }

    interrupt->last_was_interrupted = interrupt->current_interrupted;

    end = current_realtime_seconds();

    ++display->frame_number;

    if( !interrupt->current_interrupted && Display_frame_info )
        display_frame_info( display, display->frame_number, end - start );

    G_update_window( display->window );

    display->update_required[NORMAL_PLANES] = FALSE;
}

public  void  delete_graphics_window(
    display_struct   *display )
{
    (void) G_delete_window( display->window );

    terminate_graphics_window( display );

    free_display( display );
}

private  void  terminate_graphics_window(
    display_struct   *display )
{
    int      i;

    for_less( i, 0, N_MODELS )
        delete_object( display->models[i] );

    if( display->window_type == THREE_D_WINDOW )
        delete_three_d( display );

    if( display->window_type == MENU_WINDOW )
        delete_menu( &display->menu );

    if( display->window_type == SLICE_WINDOW )
        delete_slice_window( &display->slice );
}

public  void  update_view(
    display_struct  *display )
{
    G_set_3D_view( display->window,
                   &display->three_d.view.origin,
                   &display->three_d.view.line_of_sight,
                   &display->three_d.view.y_axis,
                   display->three_d.view.front_distance,
                   display->three_d.view.back_distance,
                   display->three_d.view.perspective_flag,
                   display->three_d.view.perspective_distance,
                   display->three_d.view.window_width,
                   display->three_d.view.window_height );
    G_set_modeling_transform( display->window,
                              &display->three_d.view.modeling_transform );
}

public  void  reset_view_parameters(
    display_struct   *display,
    Vector           *line_of_sight,
    Vector           *horizontal )
{
    initialize_view( &display->three_d.view,
                     line_of_sight, horizontal );
    display->three_d.view.perspective_flag = Initial_perspective_flag;
    set_model_scale( &display->three_d.view,
                     Initial_x_scale, Initial_y_scale, Initial_z_scale );
    adjust_view_for_aspect( &display->three_d.view, display->window );
    fit_view_to_domain( &display->three_d.view,
                        &display->three_d.min_limit,
                        &display->three_d.max_limit );
}

public  Real  size_of_domain(
    display_struct   *display )
{
    Vector   diff;

    SUB_POINTS( diff, display->three_d.max_limit, display->three_d.min_limit );

    return( MAGNITUDE(diff) );
}
