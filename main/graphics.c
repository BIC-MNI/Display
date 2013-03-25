/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif


#include  <display.h>

private  display_struct  **windows = (display_struct **) 0;
private  int             n_windows = 0;

private  void  initialize_graphics_window(
    display_struct   *display );
private  void  update_graphics_overlay_planes_only(
    display_struct       *display,
    VIO_BOOL              display_flag );
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
    Gwindow   window )
{
    int              i;
    display_struct   *display;

    display = NULL;

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

public  display_struct  *get_main_window( void )
{
    int   i;

    for_less( i, 0, n_windows )
    {
        if( windows[i]->window_type == THREE_D_WINDOW )
            return( windows[i] );
    }

    print( "Error:  get_main_window()\n" );
    return( 0 );
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
    int               n;
    display_struct    **graphics_windows;

    while( get_list_of_windows( &graphics_windows ) > 0 )
    {
        n = get_list_of_windows( &graphics_windows );
        delete_graphics_window( graphics_windows[n-1] );
    }

    delete_windows_list();

    G_terminate();
}

public  Status  create_graphics_window(
    window_types      window_type,
    VIO_BOOL           double_buffering,
    display_struct    **display,
    STRING            title,
    int               width,
    int               height )
{
    Status   status;

    get_new_display( display );

    status = G_create_window( title, -1, -1, width, height,
                              FALSE, double_buffering, TRUE, 2,
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

public  Bitplane_types  get_model_bitplanes(
    model_struct   *model )
{
    return( get_model_info(model)->bitplanes );
}

public  void  create_model_after_current(
    display_struct   *display )
{
    model_struct   *model;
    object_struct  *new_model;

    model = get_current_model( display );

    new_model = create_object( MODEL );

    initialize_display_model( get_model_ptr(new_model) );
    initialize_3D_model_info( model );

    replace_string( &get_model_ptr(new_model)->filename,
                    create_string("Created") );

    add_object_to_model( model, new_model );
}

public  void  initialize_model_info(
    model_struct   *model )
{
    model_info_struct  *model_info;

    ALLOC( model_info, 1 );

    assign_model_extra_ptr( model, (void *) model_info );

    model_info->view_type = MODEL_VIEW;
    model_info->bitplanes = NORMAL_PLANES;
    initialize_render( &model_info->render );
    make_identity_transform( &model_info->transform );
}

public  void  initialize_3D_model_info(
    model_struct   *model )
{
    model_info_struct  *model_info;

    model_info = (model_info_struct *) get_model_extra_ptr( model );

    initialize_render_3D( &model_info->render );
}

public  void  initialize_display_model(
    model_struct   *model )
{
    initialize_model( model );
    initialize_model_info( model );
}

public  void  terminate_display_model(
    model_struct   *model )
{
    model_info_struct  *model_info;

    model_info = (model_info_struct *) get_model_extra_ptr( model );

    FREE( model_info );
}

public  Bitplane_types  get_cursor_bitplanes( void )
{
    return( (Bitplane_types) Cursor_bitplanes );
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
    display->associated[MARKER_WINDOW] = (display_struct *) 0;

    display->models_changed_id = 0;

    initialize_action_table( &display->action_table );

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
        case MARKER_WINDOW:	   view_type = PIXEL_VIEW;   break;
        }

        model_info->view_type = view_type;

        if( display->window_type == THREE_D_WINDOW &&
            (i == OVERLAY_MODEL ||
             (i == CURSOR_MODEL && get_cursor_bitplanes() == OVERLAY_PLANES)) &&
            G_has_overlay_planes() )
        {
            model_info->bitplanes = OVERLAY_PLANES;
        }
        else if( display->window_type == SLICE_WINDOW &&
                 i == SLICE_READOUT_MODEL )
        {
            model_info->bitplanes = get_slice_readout_bitplanes();
        }
        else
        {
            model_info->bitplanes = NORMAL_PLANES;
        }

        model->n_objects = 0;
        replace_string( &model->filename, create_string("Top Level") );

        if( display->window_type == THREE_D_WINDOW && i == THREED_MODEL )
        {
            initialize_render_3D( &model_info->render );
        }

        if( display->window_type == THREE_D_WINDOW && i == OVERLAY_MODEL )
        {
            model_info->render.shaded_mode = FALSE;
            model_info->render.master_light_switch = FALSE;
        }

        if( display->window_type == THREE_D_WINDOW && i == MISCELLANEOUS_MODEL )
        {
            model_info->render.shaded_mode = TRUE;
            model_info->render.backface_flag = FALSE;
        }

        make_identity_transform( &model_info->transform );
    }

    if( display->window_type == THREE_D_WINDOW )
        initialize_three_d_window( display );

    if( display->window_type == MENU_WINDOW )
        initialize_menu_window( display );

    if( display->window_type == MARKER_WINDOW )
        initialize_menu_window( display );

    display->frame_number = 0;
    display->update_required[NORMAL_PLANES] = FALSE;
    display->update_required[OVERLAY_PLANES] = FALSE;
    display->update_interrupted.last_was_interrupted = FALSE;

    initialize_window_callbacks( display );
}

public  void  set_update_required(
    display_struct   *display,
    Bitplane_types   which_bitplanes )
{
    if( display->update_interrupted.last_was_interrupted )
    {
        display->update_interrupted.last_was_interrupted = FALSE;
        G_update_window( display->window );
    }

    display->update_required[which_bitplanes] = TRUE;
}

public  VIO_BOOL  graphics_normal_planes_update_required(
    display_struct   *display )
{
    return( display->update_required[NORMAL_PLANES] );
}

public  VIO_BOOL  graphics_update_required(
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
    char          buffer[EXTREMELY_LARGE_STRING_SIZE];
    STRING        frame_time_str;
    Point         origin;
    model_struct  *model;

    fill_Point( origin, Frame_info_x, Frame_info_y, 0.0 );

    initialize_text( &frame_text, &origin, WHITE, FIXED_FONT, 0.0 );

    (void) sprintf( buffer, "%d: ", frame_number );

    frame_time_str = format_time( "%g %s", update_time );

    frame_text.string = concat_strings( buffer, frame_time_str );

    G_set_view_type( display->window, PIXEL_VIEW );

    model = get_graphics_model( display, THREED_MODEL );

    set_render_info( display->window, &get_model_info(model)->render );
    G_draw_text( display->window, &frame_text );

    delete_string( frame_time_str );
    delete_text( &frame_text );
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

private  void  draw_in_viewports(
    display_struct               *display,
    update_interrupted_struct    *interrupt,
    Bitplane_types               bitplanes,
    VIO_BOOL                      *past_last_object )
{
    int           i, x_min, x_max, y_min, y_max;
    int           x_sub_min, x_sub_max, y_sub_min, y_sub_max;
    VIO_BOOL       draw_model, redrawing_whole_window;

    redrawing_whole_window = FALSE;

    for_less( i, 0, N_MODELS )
    {
        if( display->window_type == SLICE_WINDOW &&
            get_model_bitplanes(get_graphics_model(display,i)) == bitplanes )
        {
            draw_model = display->slice.viewport_update_flags[i][0];

            if( draw_model )
            {
                get_slice_model_viewport( display, i, &x_min, &x_max,
                                          &y_min, &y_max );

                if( i >= SLICE_MODEL1 && i < SLICE_MODEL1 + N_SLICE_VIEWS &&
                    get_slice_subviewport( display, i-SLICE_MODEL1,
                                           &x_sub_min, &x_sub_max,
                                           &y_sub_min, &y_sub_max ) )
                {
                    display->slice.slice_views[i-SLICE_MODEL1].
                                             sub_region_specified = FALSE;

                    if( !redrawing_whole_window )
                    {
                        G_set_viewport( display->window,
                                        x_min + x_sub_min, x_min + x_sub_max,
                                        y_min + y_sub_min, y_min + y_sub_max );
                        G_clear_viewport( display->window,
                                          display->window->background_colour );
                    }

                    G_set_viewport( display->window,
                                    x_min, x_max, y_min, y_max );
                }
                else
                {
                    G_set_viewport( display->window,
                                    x_min, x_max, y_min, y_max );
                    if( !redrawing_whole_window )
                        G_clear_viewport( display->window,
                                          display->window->background_colour );
                }

                if( i == FULL_WINDOW_MODEL )
                    redrawing_whole_window = TRUE;

                if( bitplanes == NORMAL_PLANES &&
                    G_get_double_buffer_state( display->window ) )
                {
                    display->slice.viewport_update_flags[i][0] =
                          display->slice.viewport_update_flags[i][1];
                    display->slice.viewport_update_flags[i][1] = FALSE;
                }
                else
                    display->slice.viewport_update_flags[i][0] = FALSE;
            }
        }
        else
            draw_model = TRUE;

        if( draw_model )
        {
            display_objects( display->window, display->models[i],
                             interrupt, bitplanes, past_last_object );
        }

        if( interrupt != NULL && interrupt->current_interrupted )
            break;
    }
}

private  void  update_graphics_overlay_planes_only(
    display_struct       *display,
    VIO_BOOL              display_flag )
{
    VIO_BOOL       past_last_object;

    if( G_has_overlay_planes() )
    {
        G_set_bitplanes( display->window, OVERLAY_PLANES );

        if( display_flag )
        {
            past_last_object = FALSE;

            draw_in_viewports( display, (update_interrupted_struct *) 0,
                               OVERLAY_PLANES, &past_last_object );
        }

        G_update_window( display->window );

        G_set_bitplanes( display->window, NORMAL_PLANES );
    }

    display->update_required[OVERLAY_PLANES] = FALSE;
}

private  void  update_graphics_normal_planes_only(
    display_struct               *display,
    update_interrupted_struct    *interrupt )
{
    Real          start, end;
    int           i;
    VIO_BOOL       past_last_object, out_of_date;

    start = current_realtime_seconds();

    G_start_interrupt_test( display->window );

    if( interrupt->last_was_interrupted )
        G_continue_last_update( display->window );

    interrupt->current_interrupted = FALSE;
    past_last_object = FALSE;

    draw_in_viewports( display, interrupt, NORMAL_PLANES, &past_last_object );

    interrupt->last_was_interrupted = interrupt->current_interrupted;

    end = current_realtime_seconds();

    ++display->frame_number;

    if( !interrupt->current_interrupted && Display_frame_info )
        display_frame_info( display, display->frame_number, end - start );

    if( !interrupt->current_interrupted )
        G_update_window( display->window );

    display->update_required[NORMAL_PLANES] = FALSE;

    if( display->window_type == SLICE_WINDOW &&
        !display->update_interrupted.last_was_interrupted &&
        G_get_double_buffer_state( display->window ) )
    {
        out_of_date = FALSE;
        for_less( i, 0, N_MODELS )
        {
            if( get_model_bitplanes(get_graphics_model(display,i)) ==
                                     NORMAL_PLANES &&
                display->slice.viewport_update_flags[i][0] )
            {
                out_of_date = TRUE;
            }
        }

        if( out_of_date )
            display->update_required[NORMAL_PLANES] = TRUE;
    }
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

    if( display->window_type == THREE_D_WINDOW )
        delete_three_d( display );

    if( display->window_type == MENU_WINDOW )
        delete_menu( &display->menu );

    if( display->window_type == SLICE_WINDOW )
        delete_slice_window( display );

    for_less( i, 0, N_MODELS )
        delete_object( display->models[i] );
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
                   display->three_d.view.stereo_flag,
                   display->three_d.view.eye_separation_ratio *
                   display->three_d.view.perspective_distance,
                   display->three_d.view.window_width,
                   display->three_d.view.window_height );
    G_set_modeling_transform( display->window,
                              &display->three_d.view.modeling_transform );
}

public  void  fit_view_to_visible_models(
    display_struct   *display )
{
    int     c;
    Point   min_limit, max_limit;
    Point   misc_min_limit, misc_max_limit;

    if( !get_range_of_object( display->models[THREED_MODEL],
                              TRUE, &min_limit, &max_limit ) )
    {
        if( !get_range_of_object( display->models[MISCELLANEOUS_MODEL],
                                  TRUE, &min_limit, &max_limit ) )
        {
            if( !get_range_of_volumes( display, &min_limit, &max_limit ) )
            {
                fill_Point( min_limit, -1.0, -1.0, -1.0 );
                fill_Point( max_limit, 1.0, 1.0, 1.0 );
            }
        }
    }
    else if( get_range_of_object( display->models[MISCELLANEOUS_MODEL],
                                  TRUE, &misc_min_limit, &misc_max_limit ) )
    {
        for_less( c, 0, N_DIMENSIONS )
        {
            if( Point_coord(misc_min_limit,c) < Point_coord(min_limit,c) )
                Point_coord(min_limit,c) = Point_coord(misc_min_limit,c);
            if( Point_coord(misc_max_limit,c) > Point_coord(max_limit,c) )
                Point_coord(max_limit,c) = Point_coord(misc_max_limit,c);
        }
    }

    fit_view_to_domain( &display->three_d.view, &min_limit, &max_limit );
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
    fit_view_to_visible_models( display );
}

public  Real  size_of_domain(
    display_struct   *display )
{
    Vector   diff;

    SUB_POINTS( diff, display->three_d.max_limit, display->three_d.min_limit );

    return( MAGNITUDE(diff) );
}
