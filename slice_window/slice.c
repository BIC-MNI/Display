
#include  <display.h>

private  void  initialize_slice_window(
    display_struct    *slice_window );

public  void  create_slice_window(
    display_struct   *display,
    char             filename[],
    Volume           volume )
{
    display_struct   *slice_window, *menu_window;
    int              sizes[N_DIMENSIONS];
    STRING           title;

    get_volume_sizes( volume, sizes );

    (void) sprintf( title, "%s [%d * %d * %d]", filename,
                    sizes[X], sizes[Y], sizes[Z] );

    (void) create_graphics_window( SLICE_WINDOW, &slice_window, title, 0, 0 );

    menu_window = display->associated[MENU_WINDOW];

    slice_window->associated[THREE_D_WINDOW] = display;
    slice_window->associated[MENU_WINDOW] = menu_window;
    slice_window->associated[SLICE_WINDOW] = slice_window;
    display->associated[SLICE_WINDOW] = slice_window;
    menu_window->associated[SLICE_WINDOW] = slice_window;

    initialize_slice_window( slice_window );
}

private  void  initialize_slice_window(
    display_struct    *slice_window )
{
    int        i, view;

    G_set_automatic_clear_state( slice_window->window, OFF );

    slice_window->slice.n_volumes = 0;
    slice_window->slice.current_volume_index = -1;

    initialize_slice_window_events( slice_window );

    for_less( view, 0, N_SLICE_VIEWS )
    {
        slice_window->slice.slice_views[view].update_atlas_flag = TRUE;
        slice_window->slice.slice_views[view].update_composite_flag = TRUE;
    }

    slice_window->slice.x_split = Slice_divider_x_position;
    slice_window->slice.y_split = Slice_divider_y_position;

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
}

private  void  free_slice_window(
    slice_window_struct   *slice );

private  void  delete_slice_window_volume_stuff(
    display_struct   *slice_window,
    int              volume_index )
{
    int              i, which;
    display_struct   *display;

    display = get_three_d_window( slice_window );

    tell_surface_extraction_volume_deleted( display, get_volume(slice_window),
                                            get_label_volume(slice_window) );
    clear_histogram( slice_window );
    delete_slice_colour_coding( &slice_window->slice, volume_index );

    delete_volume( slice_window->slice.volumes[volume_index].volume );
    delete_slice_colour_coding( &slice_window->slice, volume_index );
    delete_slice_models_for_volume( slice_window, volume_index );

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
}

public  void  add_slice_window_volume(
    display_struct    *display,
    char              filename[],
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
    }

    initialize_slice_models_for_volume( slice_window, volume_index );
    initialize_slice_colour_coding( slice_window, volume_index );
    initialize_slice_window_view( slice_window, volume_index );

    if( slice_window->slice.n_volumes == 1 )
    {
        for_less( axis, 0, N_DIMENSIONS )
            slice_window->slice.volumes[0].current_voxel[axis] =
                                               (sizes[axis] - 1) / 2.0;
    }
    else
    {
        get_current_voxel( slice_window, get_current_volume_index(slice_window),
                           current_voxel );
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

        display->three_d.cursor.box_size[X] = ABS( separations[X] );
        display->three_d.cursor.box_size[Y] = ABS( separations[Y] );
        display->three_d.cursor.box_size[Z] = ABS( separations[Z] );

        update_cursor_size( display );
    }

    if( volume_index >= 0 )
    {
        if( first )
        {
            for_less( view, 0, N_SLICE_VIEWS )
                reset_slice_view( slice_window, view );
        }
    }

    rebuild_slice_models( slice_window );
    rebuild_volume_cross_section( slice_window );
    rebuild_volume_outline( slice_window );

    for_less( view, 0, N_SLICE_VIEWS )
    {
        rebuild_slice_text_and_cursor( slice_window, view );
        slice_view_has_changed( slice_window, view );
    }

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

public  void  set_atlas_update(
    display_struct   *slice_window,
    int              view_index )
{
    slice_window->slice.slice_views[view_index].update_atlas_flag = TRUE;
}


public  void  set_atlas_all_update(
    display_struct   *slice_window )
{
    int              view_index;

    for_less( view_index, 0, N_SLICE_VIEWS )
         set_atlas_update( slice_window, view_index );
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

        set_slice_viewport_update( slice_window, SLICE_MODEL1 + view_index );
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

public  void  update_slice_window(
    display_struct   *slice_window )
{
    BOOLEAN  changed;
    int      view, v;

    for_less( view, 0, N_SLICE_VIEWS )
    {
        changed = FALSE;

        update_slice_pixel_visibilities( slice_window, view );

        for_less( v, 0, slice_window->slice.n_volumes )
        {
            if( slice_window->slice.volumes[v].views[view].update_flag )
            {
                rebuild_slice_pixels_for_volume( slice_window, v, view );
                slice_window->slice.volumes[v].views[view].update_flag = FALSE;
                changed = TRUE;
            }

            if( slice_window->slice.volumes[v].views[view].update_labels_flag )
            {
                rebuild_label_slice_pixels_for_volume( slice_window, v, view );
                slice_window->slice.volumes[v].views[view].update_labels_flag =
                                                          FALSE;
                changed = TRUE;
            }
        }

        if( slice_window->slice.slice_views[view].update_atlas_flag )
        {
            rebuild_atlas_slice_pixels( slice_window, view );
            slice_window->slice.slice_views[view].update_atlas_flag = FALSE;
            changed = TRUE;
        }

        if( changed )
            rebuild_slice_text_and_cursor( slice_window, view );

        if( changed ||
            slice_window->slice.slice_views[view].update_composite_flag )
        {
            composite_volume_and_labels( slice_window, view );
            slice_window->slice.slice_views[view].update_composite_flag = FALSE;
            set_slice_viewport_update( slice_window, SLICE_MODEL1 + view );
        }
    }
}
