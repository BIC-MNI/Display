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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/slice_window/slice.c,v 1.114 1996-09-24 19:30:44 david Exp $";
#endif


#include  <display.h>

private  void  initialize_slice_window(
    display_struct    *slice_window );

public  void  create_slice_window(
    display_struct   *display,
    STRING           filename,
    Volume           volume )
{
    display_struct   *slice_window, *menu_window;
    int              sizes[N_DIMENSIONS];
    char             title[EXTREMELY_LARGE_STRING_SIZE];

    get_volume_sizes( volume, sizes );

    (void) sprintf( title, "%s [%d * %d * %d]", filename,
                    sizes[X], sizes[Y], sizes[Z] );

    (void) create_graphics_window( SLICE_WINDOW, Slice_double_buffer_flag,
                                   &slice_window, title, 0, 0 );

    menu_window = display->associated[MENU_WINDOW];

    slice_window->associated[THREE_D_WINDOW] = display;
    slice_window->associated[MENU_WINDOW] = menu_window;
    slice_window->associated[SLICE_WINDOW] = slice_window;
    display->associated[SLICE_WINDOW] = slice_window;
    menu_window->associated[SLICE_WINDOW] = slice_window;

    initialize_slice_window( slice_window );

    if( !Use_transparency_hardware )
        G_set_transparency_state( display->window, OFF );
}

public  void  update_all_slice_models(
    display_struct   *slice_window )
{
    set_slice_cursor_update( slice_window, -1 );
    set_slice_text_update( slice_window, -1 );
    set_slice_cross_section_update( slice_window, -1 );
    set_crop_box_update( slice_window, -1 );
    set_slice_dividers_update( slice_window );
    set_probe_update( slice_window );
    set_colour_bar_update( slice_window );
    set_atlas_update( slice_window, -1 );
}

private  void  initialize_slice_window(
    display_struct    *slice_window )
{
    int        i, view;

    G_set_automatic_clear_state( slice_window->window, OFF );

    slice_window->slice.n_volumes = 0;
    slice_window->slice.current_volume_index = -1;

    slice_window->slice.cursor_visibility = ON;

    slice_window->slice.share_labels_flag = Initial_share_labels;
    slice_window->slice.crop_labels_on_output_flag =
                                           Initial_crop_labels_on_output;
    slice_window->slice.degrees_continuity = Initial_slice_continuity;
    slice_window->slice.allowable_slice_update_time = Initial_slice_update_time;
    slice_window->slice.total_slice_update_time1 =
                                           Initial_total_slice_update_time1;
    slice_window->slice.total_slice_update_time2 =
                                           Initial_total_slice_update_time2;
    slice_window->slice.current_update_volume = 0;
    slice_window->slice.current_update_view = 0;

    initialize_slice_window_events( slice_window );

    update_all_slice_models( slice_window );

    slice_window->slice.x_split = Slice_divider_x_position;
    slice_window->slice.y_split = Slice_divider_y_position;

    slice_window->slice.volume_rotation_step = Initial_volume_rotation_step;
    slice_window->slice.volume_translation_step =
                                         Initial_volume_translation_step;

    slice_window->slice.incremental_update_allowed = Initial_incremental_update;

    initialize_slice_histogram( slice_window );
    initialize_colour_bar( slice_window );
    initialize_slice_models( slice_window );
    initialize_segmenting( &slice_window->slice.segmenting );
    initialize_atlas( &slice_window->slice.atlas );
    initialize_voxel_labeling( slice_window );
    initialize_crop_box( slice_window );

    slice_window->slice.x_brush_radius = Default_x_brush_radius;
    slice_window->slice.y_brush_radius = Default_y_brush_radius;
    slice_window->slice.z_brush_radius = Default_z_brush_radius;
    slice_window->slice.current_paint_label = Default_paint_label;
    slice_window->slice.current_erase_label = 0;

    set_atlas_state( slice_window, Default_atlas_state );

    initialize_slice_undo( &slice_window->slice.undo );

    slice_window->slice.cross_section_index = OBLIQUE_VIEW_INDEX;
    slice_window->slice.cross_section_visibility = FALSE;
    slice_window->slice.cross_section_vector_present = FALSE;

    slice_window->slice.render_storage = initialize_render_storage();

    for_less( i, 0, N_MODELS )
    {
        slice_window->slice.viewport_update_flags[i][0] = TRUE;
        slice_window->slice.viewport_update_flags[i][1] = TRUE;
    }

    for_less( view, 0, N_SLICE_VIEWS )
    {
        slice_window->slice.slice_views[view].update_cursor_flag = FALSE;
        slice_window->slice.slice_views[view].update_text_flag = FALSE;
        slice_window->slice.slice_views[view].update_cross_section_flag = FALSE;
        slice_window->slice.slice_views[view].update_crop_flag = FALSE;
        slice_window->slice.slice_views[view].update_atlas_flag = FALSE;
        slice_window->slice.slice_views[view].update_composite_flag = FALSE;
        slice_window->slice.slice_views[view].sub_region_specified = FALSE;
        slice_window->slice.slice_views[view].prev_sub_region_specified = FALSE;
        slice_window->slice.slice_views[view].x_min = 0;
        slice_window->slice.slice_views[view].x_max = -1;
        slice_window->slice.slice_views[view].y_min = 0;
        slice_window->slice.slice_views[view].y_max = -1;

        slice_window->slice.slice_views[view].n_atlas_pixels_alloced = 0;
        slice_window->slice.slice_views[view].n_composite_pixels_alloced = 0;

        rebuild_slice_unfinished_flag( slice_window, view );
    }
}

private  void  delete_slice_window_volume_stuff(
    display_struct   *slice_window,
    int              volume_index )
{
    int              i;
    display_struct   *display;

    display = get_three_d_window( slice_window );

    tell_surface_extraction_volume_deleted( display, get_volume(slice_window),
                                            get_label_volume(slice_window) );
    clear_histogram( slice_window );
    delete_slice_colour_coding( &slice_window->slice, volume_index );

    delete_general_transform( &slice_window->slice.volumes[volume_index].
                              original_transform );
    delete_volume( slice_window->slice.volumes[volume_index].volume );
    delete_slice_colour_coding( &slice_window->slice, volume_index );
    delete_slice_models_for_volume( slice_window, volume_index );
    delete_string( slice_window->slice.volumes[volume_index].filename );

    for_less( i, volume_index, slice_window->slice.n_volumes-1 )
        slice_window->slice.volumes[i] = slice_window->slice.volumes[i+1];

    SET_ARRAY_SIZE( slice_window->slice.volumes, slice_window->slice.n_volumes,
                    slice_window->slice.n_volumes-1, 1 );
    --slice_window->slice.n_volumes;
}

public  void  delete_slice_window_volume(
    display_struct   *slice_window,
    int              volume_index )
{
    int   current_volume_index;

    delete_slice_window_volume_stuff( slice_window, volume_index );

    current_volume_index = get_current_volume_index(slice_window);

    if( current_volume_index >= slice_window->slice.n_volumes )
        current_volume_index = slice_window->slice.n_volumes-1;

    set_current_volume_index( slice_window, current_volume_index );
}

public  void  delete_slice_window(
    display_struct   *slice_window )
{
    while( slice_window->slice.n_volumes > 0 )
    {
        delete_slice_window_volume_stuff( slice_window,
                                          slice_window->slice.n_volumes-1 );
    }

    delete_render_storage( slice_window->slice.render_storage );

    delete_slice_undo( &slice_window->slice.undo, -1 );
    delete_slice_histogram( &slice_window->slice );

    delete_atlas( &slice_window->slice.atlas );
    delete_voxel_labeling( &slice_window->slice );
    delete_crop_box( slice_window );
}

public  STRING  get_volume_filename(
    display_struct    *slice_window,
    int               volume_index )
{
    return( slice_window->slice.volumes[volume_index].filename );
}

public  void  add_slice_window_volume(
    display_struct    *display,
    STRING            filename,
    Volume            volume )
{
    display_struct         *slice_window;
    int                    volume_index, axis, view, sizes[MAX_DIMENSIONS];
    loaded_volume_struct   *info;
    Real                   current_voxel[MAX_DIMENSIONS];

    if( !slice_window_exists(display) )
    {
        create_slice_window( display, filename, volume );
    }

    (void) get_slice_window( display, &slice_window );

    SET_ARRAY_SIZE( slice_window->slice.volumes,
                    slice_window->slice.n_volumes,
                    slice_window->slice.n_volumes+1, 1 );

    ++slice_window->slice.n_volumes;
    volume_index = slice_window->slice.n_volumes-1;
    info = &slice_window->slice.volumes[volume_index];

    info->volume = volume;
    copy_general_transform( get_voxel_to_world_transform(volume),
                            &info->original_transform );
    info->filename = create_string( filename );
    info->display_labels = Initial_display_labels;
    info->opacity = 1.0;

    get_volume_sizes( volume, sizes );

    for_less( view, 0, N_SLICE_VIEWS )
    {
        info->views[view].visibility = TRUE;
        info->views[view].update_flag = TRUE;
        info->views[view].update_labels_flag = TRUE;
        info->views[view].filter_type = (Filter_types) Default_filter_type;
        info->views[view].filter_width = Default_filter_width;
        info->views[view].n_pixels_redraw = Initial_n_pixels_redraw;
        info->views[view].update_in_progress[0] = FALSE;
        info->views[view].update_in_progress[1] = FALSE;
    }

    initialize_slice_models_for_volume( slice_window, volume_index );
    initialize_slice_colour_coding( slice_window, volume_index );
    initialize_slice_window_view( slice_window, volume_index );

    if( slice_window->slice.n_volumes == 1 )
    {
        for_less( axis, 0, N_DIMENSIONS )
            slice_window->slice.volumes[0].current_voxel[axis] =
                                               (Real) (sizes[axis] - 1) / 2.0;
    }
    else
    {
        get_current_voxel( slice_window, get_current_volume_index(slice_window),
                           current_voxel );

        for_less( axis, 0, N_DIMENSIONS )
            slice_window->slice.volumes[slice_window->slice.n_volumes - 1].
                              current_voxel[axis] = -1.0e20;

        (void) set_current_voxel( slice_window,
                                  get_current_volume_index(slice_window),
                                  current_voxel );
        update_all_slice_axes_views( slice_window,
                                     get_current_volume_index(slice_window) );
    }

    set_current_volume_index( slice_window, slice_window->slice.n_volumes - 1 );

    set_slice_window_all_update( slice_window, -1, UPDATE_BOTH );
}

public  void  set_current_volume_index(
    display_struct  *slice_window,
    int             volume_index )
{
    BOOLEAN         first;
    int             view;
    Real            separations[MAX_DIMENSIONS];
    display_struct  *display;

    if( slice_window->slice.current_volume_index < 0 )
        first = TRUE;
    else if( volume_index >= 0 )
        first = FALSE;

    slice_window->slice.current_volume_index = volume_index;

    if( volume_index >= 0 )
    {
        get_volume_separations( get_nth_volume(slice_window,volume_index),
                                separations );

        display = get_three_d_window( slice_window );

        display->three_d.cursor.box_size[X] = FABS( separations[X] );
        display->three_d.cursor.box_size[Y] = FABS( separations[Y] );
        display->three_d.cursor.box_size[Z] = FABS( separations[Z] );

        update_cursor_size( display );

        if( first )
        {
            for_less( view, 0, N_SLICE_VIEWS )
                reset_slice_view( slice_window, view );
        }

        G_set_window_title( slice_window->window,
                            slice_window->slice.volumes[volume_index].filename);
    }
    else
    {
        G_set_window_title( slice_window->window, "No Volume Loaded" );
    }

    update_all_slice_models( slice_window );

    rebuild_volume_cross_section( slice_window );
    rebuild_volume_outline( slice_window );

    set_slice_window_all_update( slice_window, volume_index, UPDATE_BOTH );
}

public  int  get_n_volumes(
    display_struct  *display )
{
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
        return( slice_window->slice.n_volumes );
    else
        return( 0 );
}

public  int   get_current_volume_index(
    display_struct   *display )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        slice_window->slice.n_volumes > 0 &&
        slice_window->slice.current_volume_index >= 0 )
    {
        return( slice_window->slice.current_volume_index );
    }
    else
        return( -1 );
}

public  BOOLEAN   get_slice_window_volume(
    display_struct   *display,
    Volume           *volume )
{
    BOOLEAN          volume_exists;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        slice_window->slice.n_volumes > 0 )
    {
        *volume = slice_window->slice.volumes[slice_window->slice.
                                              current_volume_index].volume;
        volume_exists = TRUE;
    }
    else
    {
        *volume = (Volume) NULL;
        volume_exists = FALSE;
    }

    return( volume_exists );
}

public  Volume  get_nth_volume(
    display_struct   *display,
    int              volume_index )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        return( slice_window->slice.volumes[volume_index].volume );
    }
    else
        return( (Volume) NULL );
}

public  Volume   get_volume(
    display_struct   *display )
{
    Volume      volume;

    (void) get_slice_window_volume( display, &volume );

    return( volume );
}

public  BOOLEAN  slice_window_exists(
    display_struct   *display )
{
    return( display != (display_struct *) NULL &&
            display->associated[SLICE_WINDOW] != (display_struct *) NULL );
}

public  BOOLEAN  get_slice_window(
    display_struct   *display,
    display_struct   **slice_window )
{
    BOOLEAN  exists;

    exists = FALSE;

    if( slice_window_exists( display ) )
    {
        *slice_window = display->associated[SLICE_WINDOW];
        exists = TRUE;
    }

    return( exists );
}

public  BOOLEAN  get_range_of_volumes(
    display_struct   *display,
    Point            *min_limit,
    Point            *max_limit )
{
    int              n_volumes, sizes[N_DIMENSIONS], dx, dy, dz, volume_index;
    int              dim;
    Volume           volume;
    Real             voxel[N_DIMENSIONS], world[N_DIMENSIONS];
    BOOLEAN          first;

    n_volumes = get_n_volumes( display );

    if( n_volumes == 0 )
        return( FALSE );

    fill_Point( *min_limit, 0.0, 0.0, 0.0 );
    fill_Point( *max_limit, 0.0, 0.0, 0.0 );
    first = TRUE;

    for_less( volume_index, 0, n_volumes )
    {
        volume = get_nth_volume( display, volume_index );

        get_volume_sizes( volume, sizes );

        for_less( dx, 0, 2 )
        for_less( dy, 0, 2 )
        for_less( dz, 0, 2 )
        {
            voxel[X] = -0.5 + (Real) dx * (Real) sizes[X];
            voxel[Y] = -0.5 + (Real) dy * (Real) sizes[Y];
            voxel[Z] = -0.5 + (Real) dz * (Real) sizes[Z];

            convert_voxel_to_world( volume, voxel,
                                    &world[X], &world[Y], &world[Z] );

            for_less( dim, 0, N_DIMENSIONS )
            {
                if( first || world[dim] < (Real) Point_coord(*min_limit,dim) )
                    Point_coord(*min_limit,dim) = (Point_coord_type) world[dim];
                if( first || world[dim] > (Real) Point_coord(*max_limit,dim) )
                    Point_coord(*max_limit,dim) = (Point_coord_type) world[dim];
            }

            first = FALSE;
        }
    }

    return( TRUE );
}

public  void  set_slice_cursor_update(
    display_struct   *slice_window,
    int              view_index )
{
    if( view_index >= 0 )
    {
        slice_window->slice.slice_views[view_index].update_cursor_flag = TRUE;
    }
    else
    {
        for_less( view_index, 0, N_SLICE_VIEWS )
            slice_window->slice.slice_views[view_index].update_cursor_flag=TRUE;
    }
}

public  void  set_slice_text_update(
    display_struct   *slice_window,
    int              view_index )
{
    if( view_index >= 0 )
    {
        slice_window->slice.slice_views[view_index].update_text_flag = TRUE;
    }
    else
    {
        for_less( view_index, 0, N_SLICE_VIEWS )
        {
            slice_window->slice.slice_views[view_index].update_text_flag =TRUE;
        }
    }
}

public  void  set_slice_cross_section_update(
    display_struct   *slice_window,
    int              view_index )
{
    if( view_index >= 0 )
    {
        slice_window->slice.slice_views[view_index].update_cross_section_flag =
                                                 TRUE;
    }
    else
    {
        for_less( view_index, 0, N_SLICE_VIEWS )
        {
            slice_window->slice.slice_views[view_index].
                                       update_cross_section_flag =TRUE;
        }
    }
}

public  void  set_crop_box_update(
    display_struct   *slice_window,
    int              view_index )
{
    if( view_index >= 0 )
    {
        slice_window->slice.slice_views[view_index].update_crop_flag = TRUE;
    }
    else
    {
        for_less( view_index, 0, N_SLICE_VIEWS )
        {
            slice_window->slice.slice_views[view_index].update_crop_flag =TRUE;
        }
    }
}

public  void  set_slice_dividers_update(
    display_struct   *slice_window )
{
    slice_window->slice.update_slice_dividers_flag = TRUE;
}

public  void  set_probe_update(
    display_struct   *slice_window )
{
    slice_window->slice.update_probe_flag = TRUE;
}

public  void  set_colour_bar_update(
    display_struct   *slice_window )
{
    slice_window->slice.update_colour_bar_flag = TRUE;
}

public  void  set_atlas_update(
    display_struct   *slice_window,
    int              view_index )
{
    if( view_index >= 0 )
    {
        slice_window->slice.slice_views[view_index].update_atlas_flag = TRUE;
    }
    else
    {
        for_less( view_index, 0, N_SLICE_VIEWS )
        {
            slice_window->slice.slice_views[view_index].update_atlas_flag =TRUE;
        }
    }
}

public  void  set_slice_window_update(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    Update_types     type )
{
    int   v, v_min, v_max;

    if( slice_window != (display_struct *) NULL )
    {
        if( volume_index < 0 || volume_index >= slice_window->slice.n_volumes )
        {
            v_min = 0;
            v_max = slice_window->slice.n_volumes;
            set_atlas_update( slice_window, view_index );
        }
        else
        {
            v_min = volume_index;
            v_max = volume_index + 1;
        }

        for_less( v, v_min, v_max )
        {
            if( type == UPDATE_SLICE || type == UPDATE_BOTH )
            {
                slice_window->slice.volumes[v].views[view_index].update_flag =
                                                                     TRUE;
            }

            if( type == UPDATE_LABELS || type == UPDATE_BOTH )
            {
                slice_window->slice.volumes[v].views[view_index].
                                                      update_labels_flag = TRUE;
            }
        }
    }
}

public  void  set_slice_window_all_update(
    display_struct   *slice_window,
    int              volume_index,
    Update_types     type )
{
    int  view;

    for_less( view, 0, N_SLICE_VIEWS )
        set_slice_window_update( slice_window, volume_index, view, type );
}

public  void  set_slice_viewport_update(
    display_struct   *slice_window,
    int              model_number )
{
    int   i;

    slice_window->slice.viewport_update_flags[model_number][0] = TRUE;
    slice_window->slice.viewport_update_flags[model_number][1] = TRUE;

    if( model_number == FULL_WINDOW_MODEL )
    {
        for_less( i, 0, N_MODELS )
        {
            if( get_model_bitplanes( get_graphics_model(slice_window,i) ) ==
                                                            NORMAL_PLANES )
            {
                slice_window->slice.viewport_update_flags[i][0] = TRUE;
                slice_window->slice.viewport_update_flags[i][1] = TRUE;
            }
        }
    }

    set_update_required( slice_window, get_model_bitplanes(
                              get_graphics_model(slice_window,model_number)) );
}

private  BOOLEAN  slice_viewport_has_changed(
    display_struct   *slice_window,
    int              view )
{
    BOOLEAN  changed;
    int      v;

    changed = FALSE;

    for_less( v, 0, slice_window->slice.n_volumes )
    {
        if( slice_window->slice.volumes[v].views[view].update_flag )
            changed = TRUE;

        if( slice_window->slice.volumes[v].views[view].update_labels_flag )
            changed = TRUE;
    }

    if( slice_window->slice.slice_views[view].update_cursor_flag )
        changed = TRUE;

    if( slice_window->slice.slice_views[view].update_text_flag )
        changed = TRUE;

    if( slice_window->slice.slice_views[view].update_cross_section_flag )
        changed = TRUE;

    if( slice_window->slice.slice_views[view].update_crop_flag )
        changed = TRUE;

    if( slice_window->slice.slice_views[view].update_atlas_flag )
        changed = TRUE;

    if( slice_window->slice.slice_views[view].update_composite_flag )
        changed = TRUE;

    return( changed );
}

private  BOOLEAN  is_slice_continuing(
    display_struct   *slice_window,
    int              view )
{
    BOOLEAN  continuing;
    int      v;

    continuing = FALSE;

    for_less( v, 0, slice_window->slice.n_volumes )
    {
        if( slice_window->slice.volumes[v].views[view].update_in_progress[0] ||
            slice_window->slice.volumes[v].views[view].update_in_progress[1] )
            continuing = TRUE;
    }

    return( continuing );
}

private  BOOLEAN  time_is_up(
    Real    event_time,
    Real    end_time,
    Real    current_time )
{
    return( end_time >= 0.0 &&
            (current_time > end_time ||
             current_time > event_time && G_events_pending()) );
}

private  void  render_more_slices(
    display_struct   *slice_window,
    BOOLEAN          viewport_has_changed[] )
{
    BOOLEAN  first_render, did_one, finished;
    BOOLEAN  interrupted, view_was_interrupted[N_SLICE_VIEWS];
    BOOLEAN  viewport_pixels_modified[N_SLICE_VIEWS];
    BOOLEAN  no_viewport_changed, incremental_flag;
    BOOLEAN  *update_flag_ptr, *update_in_progress;
    int      view, v, v_index, view_index, which_volume;
    int      slice_is_visible, n_pixels_drawn;
    int      current_update_volume, current_update_view, n_volumes;
    Real     update_time, end_time, current_time, event_time, prev_time;
    Real     time_to_create;

    no_viewport_changed = TRUE;
    for_less( view, 0, N_SLICE_VIEWS )
    {
        view_was_interrupted[view] = FALSE;
        viewport_pixels_modified[view] = FALSE;

        if( viewport_has_changed[view] )
            no_viewport_changed = FALSE;
    }

    if( no_viewport_changed )
        update_time = slice_window->slice.total_slice_update_time2;
    else
        update_time = slice_window->slice.total_slice_update_time1;

    current_time = current_realtime_seconds();
    end_time = current_time + update_time;
    event_time = current_time + Slice_event_check_time;

    /*--- if currently painting or incremental update not desired, then
          don't interrupt the slice recreation */

    if( slice_window->slice.painting_view_index >= 0 ||
        !slice_window->slice.incremental_update_allowed || update_time <= 0.0 )
    {
        end_time = -1.0;
        event_time = -1.0;
        incremental_flag = FALSE;
    }
    else
        incremental_flag = TRUE;

    interrupted = FALSE;
    n_volumes = slice_window->slice.n_volumes;
    current_update_volume = slice_window->slice.current_update_volume;
    current_update_view = slice_window->slice.current_update_view;

    if( current_update_volume >= n_volumes )
        current_update_volume = 0;
    if( current_update_view >= N_SLICE_VIEWS )
        current_update_view = 0;

    prev_time = current_time;

    for_less( v_index, 0, n_volumes )
    {
        v = (v_index + current_update_volume) % n_volumes;

        for_less( which_volume, 0, 2 )
        {
            first_render = TRUE;
            do
            {
                did_one = FALSE;

                for_less( view_index, 0, N_SLICE_VIEWS )
                {
                    view = (view_index + current_update_view) % N_SLICE_VIEWS;

                    update_in_progress =
                             slice_window->slice.volumes[v].
                                              views[view].update_in_progress;
                    if( which_volume == 0 )
                    {
                        update_flag_ptr = &slice_window->slice.volumes[v].
                                              views[view].update_flag;
                        slice_is_visible = get_slice_visibility( slice_window,
                                                                 v, view );
                    }
                    else
                    {
                        update_flag_ptr = &slice_window->slice.volumes[v].
                                              views[view].update_labels_flag;
                        slice_is_visible = get_label_visibility( slice_window,
                                                                 v, view );
                    }

                    if( (*update_flag_ptr || update_in_progress[which_volume])&&
                        slice_is_visible )
                    {
                        if( !interrupted || first_render )
                        {
                            if( which_volume == 0 )
                            {
                                n_pixels_drawn =
                                      rebuild_slice_pixels_for_volume(
                                          slice_window, v, view,
                                          incremental_flag, interrupted,
                                          !(*update_flag_ptr), &finished );
                            }
                            else
                            {
                                n_pixels_drawn =
                                      rebuild_label_slice_pixels_for_volume(
                                          slice_window, v, view,
                                          incremental_flag, interrupted,
                                          !(*update_flag_ptr), &finished );
                            }

                            update_in_progress[which_volume] = !finished;

                            if( n_pixels_drawn > 0 )
                            {
                                viewport_pixels_modified[view] = TRUE;
                                did_one = TRUE;
                                current_time = current_realtime_seconds();

                                time_to_create = current_time - prev_time;
                                prev_time = current_time;

                                if( incremental_flag &&
                                    n_pixels_drawn >= slice_window->slice.
                                       volumes[v].views[view].n_pixels_redraw/2)
                                {
                                    if( time_to_create < slice_window->slice.
                                         allowable_slice_update_time / 0.5 )
                                    {
                                        slice_window->slice.
                                        volumes[v].views[view].
                                          n_pixels_redraw = MAX( 1,
                                                           2 * n_pixels_drawn );
                                    }
                                    else if( time_to_create > slice_window->
                                      slice.allowable_slice_update_time * 2.0 )
                                    {
                                        slice_window->slice.
                                        volumes[v].views[view].
                                             n_pixels_redraw = (int)
                                               (MAX( 1.0,
                                                     0.5*(Real)n_pixels_drawn));
                                    }
                                }


                                if( !finished &&
                                     time_is_up( event_time, end_time,
                                                 current_time ) )
                                {
                                    if( !interrupted )
                                    {
                                        slice_window->slice.current_update_view=
                                                   (view + 1) % N_SLICE_VIEWS;
#ifdef  UPDATE_VOLUME_INDEX
                                        if( view == N_SLICE_VIEWS-1)
                                        {
                                            slice_window->slice.
                                                 current_update_volume =
                                                           (v+1) % n_volumes;
                                        }
                                        else
                                        {
                                            slice_window->slice.
                                                 current_update_volume = v;
                                        }
#endif

                                        interrupted = TRUE;
                                    }
                                }
                            }
                        }
                        else
                            update_in_progress[which_volume] = TRUE;
                    }
                    else
                    {
                        update_in_progress[which_volume] = FALSE;
                    }

                    *update_flag_ptr = FALSE;
                }
            }
            while( !interrupted && did_one );

            first_render = FALSE;
        }
    }

    for_less( view, 0, N_SLICE_VIEWS )
    {
        interrupted = FALSE;
        for_less( v, 0, n_volumes )
        {
            if( slice_window->slice.volumes[v].
                                         views[view].update_in_progress[0] ||
                slice_window->slice.volumes[v].
                                         views[view].update_in_progress[1] )
            {
                interrupted = TRUE;
            }
        }

        if( viewport_pixels_modified[view] )
            composite_volume_and_labels( slice_window, view );

        set_slice_unfinished_flag_visibility( slice_window, view, interrupted );
    }
}

public  void  update_slice_window(
    display_struct   *slice_window )
{
    BOOLEAN  viewport_has_changed[N_SLICE_VIEWS];
    BOOLEAN  one_buffer_flag[N_SLICE_VIEWS];
    BOOLEAN  slice_is_continuing[N_SLICE_VIEWS];
    BOOLEAN  original_sub_region_specified[N_SLICE_VIEWS];
    BOOLEAN  prev_use_sub_region;
    int      x_sub_min, x_sub_max;
    int      y_sub_min, y_sub_max;
    int      view;

    for_less( view, 0, N_SLICE_VIEWS )
    {
        viewport_has_changed[view] = slice_viewport_has_changed( slice_window,
                                                                 view );
        prev_use_sub_region = slice_window->slice.slice_views[view].
                              use_sub_region;

        slice_is_continuing[view] = !viewport_has_changed[view] &&
                                    is_slice_continuing( slice_window, view );

        if( viewport_has_changed[view] )
            slice_window->slice.slice_views[view].use_sub_region = FALSE;

        one_buffer_flag[view] = !viewport_has_changed[view] &&
                 slice_window->slice.slice_views[view].sub_region_specified;

        original_sub_region_specified[view] = !viewport_has_changed[view] &&
                slice_window->slice.slice_views[view].sub_region_specified;

        if( slice_window->slice.slice_views[view].sub_region_specified ||
            slice_window->slice.slice_views[view].prev_sub_region_specified &&
            (viewport_has_changed[view] ||
             slice_window->slice.viewport_update_flags[SLICE_MODEL1+view][0]) )
        {
            slice_window->slice.slice_views[view].use_sub_region =
                                       original_sub_region_specified[view];

            if( viewport_has_changed[view] ||
                slice_is_continuing[view] && !prev_use_sub_region &&
                slice_window->slice.slice_views[view].sub_region_specified ||
                slice_window->slice.viewport_update_flags[SLICE_MODEL1+view][0])
            {
                slice_window->slice.slice_views[view].use_sub_region = FALSE;
            }

            set_slice_window_update( slice_window, -1, view, UPDATE_BOTH );
            viewport_has_changed[view] = TRUE;
        }

        slice_is_continuing[view] = !viewport_has_changed[view] &&
                                    is_slice_continuing( slice_window, view );

        if( slice_is_continuing[view] )
        {
            one_buffer_flag[view] = slice_window->slice.slice_views[view].
                                                           use_sub_region;
            if( slice_window->slice.slice_views[view].use_sub_region &&
                prev_use_sub_region )
            {
                slice_window->slice.slice_views[view].prev_x_min =
                     slice_window->slice.slice_views[view].x_min;
                slice_window->slice.slice_views[view].prev_x_max =
                     slice_window->slice.slice_views[view].x_max;
                slice_window->slice.slice_views[view].prev_y_min =
                     slice_window->slice.slice_views[view].y_min;
                slice_window->slice.slice_views[view].prev_y_max =
                     slice_window->slice.slice_views[view].y_max;
            }
        }

        /*--- for now this is an effective way to get around a bug,
              but the real solution is a simple change in the above logic,
              The bug occurs when slice painting is started while the
              slices are being updated and the system is in double buffer
              mode. */

        if( !slice_window->slice.slice_views[view].prev_sub_region_specified &&
            original_sub_region_specified[view] )
            one_buffer_flag[view] = FALSE;
    }

    if( slice_window->slice.update_slice_dividers_flag )
    {
        rebuild_slice_divider( slice_window );
        slice_window->slice.update_slice_dividers_flag = FALSE;
    }

    if( slice_window->slice.update_probe_flag )
    {
        rebuild_probe( slice_window );
        slice_window->slice.update_probe_flag = FALSE;
    }

    if( slice_window->slice.update_colour_bar_flag )
    {
        rebuild_colour_bar( slice_window );
        slice_window->slice.update_colour_bar_flag = FALSE;
    }

    for_less( view, 0, N_SLICE_VIEWS )
    {
        if( slice_window->slice.slice_views[view].update_cursor_flag )
        {
            rebuild_slice_cursor( slice_window, view );
            slice_window->slice.slice_views[view].update_cursor_flag = FALSE;
        }

        if( slice_window->slice.slice_views[view].update_text_flag )
        {
            rebuild_slice_text( slice_window, view );
            slice_window->slice.slice_views[view].update_text_flag = FALSE;
        }

        if( slice_window->slice.slice_views[view].update_cross_section_flag )
        {
            rebuild_slice_cross_section( slice_window, view );
            slice_window->slice.slice_views[view].update_cross_section_flag =
                                                             FALSE;
        }

        if( slice_window->slice.slice_views[view].update_crop_flag )
        {
            rebuild_slice_crop_box( slice_window, view );
            slice_window->slice.slice_views[view].update_crop_flag = FALSE;
        }


        if( original_sub_region_specified[view] )
        {
            x_sub_min = slice_window->slice.slice_views[view].x_min;
            x_sub_max = slice_window->slice.slice_views[view].x_max;
            y_sub_min = slice_window->slice.slice_views[view].y_min;
            y_sub_max = slice_window->slice.slice_views[view].y_max;

            if( G_get_double_buffer_state( slice_window->window ) &&
                slice_window->slice.slice_views[view].prev_sub_region_specified)
            {
                if( slice_window->slice.slice_views[view].prev_x_min <
                    slice_window->slice.slice_views[view].x_min )
                {
                    slice_window->slice.slice_views[view].x_min =
                      slice_window->slice.slice_views[view].prev_x_min;
                }
                if( slice_window->slice.slice_views[view].prev_x_max >
                    slice_window->slice.slice_views[view].x_max )
                {
                    slice_window->slice.slice_views[view].x_max =
                      slice_window->slice.slice_views[view].prev_x_max;
                }
                if( slice_window->slice.slice_views[view].prev_y_min <
                    slice_window->slice.slice_views[view].y_min )
                {
                    slice_window->slice.slice_views[view].y_min =
                      slice_window->slice.slice_views[view].prev_y_min;
                }
                if( slice_window->slice.slice_views[view].prev_y_max >
                    slice_window->slice.slice_views[view].y_max )
                {
                    slice_window->slice.slice_views[view].y_max =
                      slice_window->slice.slice_views[view].prev_y_max;
                }
            }
        }

        update_slice_pixel_visibilities( slice_window, view );

        if( slice_window->slice.slice_views[view].update_atlas_flag )
        {
            rebuild_atlas_slice_pixels( slice_window, view );
            slice_window->slice.slice_views[view].update_atlas_flag = FALSE;
        }

        if( viewport_has_changed[view] ||
            slice_is_continuing[view] ||
            slice_window->slice.slice_views[view].update_composite_flag )
        {
            composite_volume_and_labels( slice_window, view );
            slice_window->slice.slice_views[view].update_composite_flag = FALSE;
        }

        if( one_buffer_flag[view] )
        {
            slice_window->slice.viewport_update_flags[SLICE_MODEL1+view][0] =
                                                                TRUE;
            set_update_required( slice_window, get_model_bitplanes(
                         get_graphics_model(slice_window,SLICE_MODEL1+ view)) );
        }
        else if( viewport_has_changed[view] || slice_is_continuing[view] )
        {
            set_slice_viewport_update( slice_window, SLICE_MODEL1 + view );
        }

        if( viewport_has_changed[view] )
        {
            slice_window->slice.slice_views[view].prev_sub_region_specified =
                                         original_sub_region_specified[view];
            slice_window->slice.slice_views[view].prev_x_min = x_sub_min;
            slice_window->slice.slice_views[view].prev_x_max = x_sub_max;
            slice_window->slice.slice_views[view].prev_y_min = y_sub_min;
            slice_window->slice.slice_views[view].prev_y_max = y_sub_max;
        }
        else if( slice_is_continuing[view] )
        {
            slice_window->slice.slice_views[view].prev_x_min =
                 slice_window->slice.slice_views[view].x_min;
            slice_window->slice.slice_views[view].prev_x_max =
                 slice_window->slice.slice_views[view].x_max;
            slice_window->slice.slice_views[view].prev_y_min =
                 slice_window->slice.slice_views[view].y_min;
            slice_window->slice.slice_views[view].prev_y_max =
                 slice_window->slice.slice_views[view].y_max;
        }

        slice_window->slice.slice_views[view].sub_region_specified = FALSE;
    }

    render_more_slices( slice_window, viewport_has_changed );
}

public  void  set_slice_composite_update(
    display_struct   *slice_window,
    int              view_index,
    int              x_min,
    int              x_max,
    int              y_min,
    int              y_max )
{
    if( slice_window->slice.slice_views[view_index].sub_region_specified )
    {
        x_min = MIN( slice_window->slice.slice_views[view_index].x_min,
                     x_min );
        x_max = MAX( slice_window->slice.slice_views[view_index].x_max,
                     x_max );
        y_min = MIN( slice_window->slice.slice_views[view_index].y_min,
                     y_min );
        y_max = MAX( slice_window->slice.slice_views[view_index].y_max,
                     y_max );
    }

    slice_window->slice.slice_views[view_index].sub_region_specified = TRUE;

    slice_window->slice.slice_views[view_index].x_min = x_min;
    slice_window->slice.slice_views[view_index].x_max = x_max;
    slice_window->slice.slice_views[view_index].y_min = y_min;
    slice_window->slice.slice_views[view_index].y_max = y_max;
}

public  BOOLEAN  get_slice_subviewport(
    display_struct   *slice_window,
    int              view_index,
    int              *x_min,
    int              *x_max,
    int              *y_min,
    int              *y_max )
{
    int  x_min_viewport, x_max_viewport, y_min_viewport, y_max_viewport;

    get_slice_viewport( slice_window, view_index,
                        &x_min_viewport, &x_max_viewport,
                        &y_min_viewport, &y_max_viewport );

    if( slice_window->slice.slice_views[view_index].use_sub_region )
    {
        *x_min = slice_window->slice.slice_views[view_index].x_min;
        *x_max = slice_window->slice.slice_views[view_index].x_max;
        *y_min = slice_window->slice.slice_views[view_index].y_min;
        *y_max = slice_window->slice.slice_views[view_index].y_max;

        *x_min = MAX( *x_min, 0 );
        *x_max = MIN( *x_max, x_max_viewport - x_min_viewport );
        *y_min = MAX( *y_min, 0 );
        *y_max = MIN( *y_max, y_max_viewport - y_min_viewport );

        return( TRUE );
    }
    else
    {
        *x_min = 0;
        *x_max = x_max_viewport - x_min_viewport;
        *y_min = 0;
        *y_max = y_max_viewport - y_min_viewport;
        return( FALSE );
    }
}
