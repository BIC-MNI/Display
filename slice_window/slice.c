
#include  <def_display.h>

public  Boolean   get_slice_window_volume(
    display_struct   *display,
    Volume           *volume )
{
    Boolean  volume_set;

    if( display->associated[SLICE_WINDOW] != (display_struct *) NULL )
    {
        *volume = display->associated[SLICE_WINDOW]->slice.volume;
        volume_set = TRUE;
    }
    else
    {
        *volume = (Volume) NULL;
        volume_set = FALSE;
    }

    return( volume_set );
}

public  Volume   get_volume(
    display_struct   *display )
{
    Volume      volume;

    (void) get_slice_window_volume( display, &volume );

    return( volume );
}

public  Boolean  get_slice_window(
    display_struct   *display,
    display_struct   **slice_window )
{
    Boolean  exists;

    exists = FALSE;

    if( display != (display_struct *) NULL &&
        display->associated[SLICE_WINDOW] != (display_struct *) NULL )
    {
        *slice_window = display->associated[SLICE_WINDOW];
        exists = TRUE;
    }

    return( exists );
}

private  void  set_cursor_colour(
    display_struct   *slice_window )
{
    Real      indices[N_DIMENSIONS];
    int       int_indices[N_DIMENSIONS];
    Colour    col;
    Real      value;

    if( get_isosurface_value( slice_window->associated[THREE_D_WINDOW], &value))
    {
        get_current_voxel( slice_window, &indices[X], &indices[Y], &indices[Z]);
        int_indices[X] = ROUND( indices[X] );
        int_indices[Y] = ROUND( indices[Y] );
        int_indices[Z] = ROUND( indices[Z] );

        if( cube_is_within_volume( get_volume(slice_window), int_indices ) &&
            voxel_contains_value( get_volume(slice_window), int_indices[X],
                                  int_indices[Y], int_indices[Z], value ))
        {
            if( get_cursor_bitplanes() )
                col = Cursor_colour_on_surface;
            else
                col = Cursor_rgb_colour_on_surface;
        }
        else
        {
            if( get_cursor_bitplanes() )
                col = Cursor_colour_off_surface;
            else
                col = Cursor_rgb_colour_off_surface;
        }

        update_cursor_colour( slice_window->associated[THREE_D_WINDOW], col );
    }
}

public  Boolean  update_cursor_from_voxel(
    display_struct    *slice_window )
{
    Real              x, y, z;
    Real              x_w, y_w, z_w;
    Boolean           changed;
    Point             new_origin;
    display_struct    *display;

    display = slice_window->associated[THREE_D_WINDOW];

    get_current_voxel( slice_window, &x, &y, &z );

    convert_voxel_to_world( get_volume(slice_window),
                            x, y, z, &x_w, &y_w, &z_w );
    fill_Point( new_origin, x_w, y_w, z_w );

    if( !EQUAL_POINTS( new_origin, display->three_d.cursor.origin ) )
    {
        display->three_d.cursor.origin = new_origin;

        set_cursor_colour( slice_window );

        update_cursor( display );

        changed = TRUE;
    }
    else
    {
        changed = FALSE;
    }

    return( changed );
}

public  Boolean  update_voxel_from_cursor(
    display_struct    *slice_window )
{
    Real              x, y, z;
    Boolean           changed;
    display_struct    *display;

    changed = FALSE;

    if( get_slice_window(slice_window,&slice_window) )
    {
        display = slice_window->associated[THREE_D_WINDOW];

        if( get_voxel_corresponding_to_point( slice_window,
                                    &display->three_d.cursor.origin,
                                    &x, &y, &z ) )
        {
            changed = set_current_voxel( slice_window,
                                         ROUND( x ), ROUND( y ), ROUND( z ) );

            set_cursor_colour( slice_window );
        }
    }

    return( changed );
}

public  void  set_slice_window_update(
    display_struct   *slice_window,
    int              view_index )
{
    if( slice_window != (display_struct *) NULL )
    {
        slice_window->slice.slice_views[view_index].update_flag = TRUE;
        set_update_required( slice_window, NORMAL_PLANES );
    }
}

public  void  update_slice_window(
    display_struct   *display )
{
    int   c;

    for_less( c, 0, 3 )
    {
        if( display->slice.slice_views[c].update_flag )
        {
            rebuild_slice_pixels( display, c );
            display->slice.slice_views[c].update_flag = FALSE;
        }
    }

}

public  void  create_slice_window(
    display_struct   *display,
    char             filename[],
    Volume           volume )
{
    display_struct   *slice_window, *menu_window;
    int              sizes[N_DIMENSIONS];
    String           title;

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

    slice_window->slice.original_volume = volume;

    set_slice_window_volume( slice_window,
                             slice_window->slice.original_volume );

    set_slice_window_update( slice_window, 0 );
    set_slice_window_update( slice_window, 1 );
    set_slice_window_update( slice_window, 2 );
}

public  void  initialize_slice_window(
    display_struct    *slice_window )
{
    int        c;

    slice_window->slice.volume = (Volume) NULL;

    initialize_slice_window_events( slice_window );
    initialize_voxel_labeling( slice_window );

    for_less( c, 0, N_DIMENSIONS )
    {
        slice_window->slice.slice_views[c].filter_type =
                           (Filter_types) Default_filter_type;
        slice_window->slice.slice_views[c].filter_width = Default_filter_width;
        slice_window->slice.slice_views[c].update_flag = TRUE;
    }

    slice_window->slice.next_to_update = X;

    slice_window->slice.display_labels = Initial_display_labels;

    initialize_slice_colour_coding( slice_window );

    initialize_slice_models( slice_window );

    initialize_segmenting( &slice_window->slice.segmenting );
    initialize_atlas( &slice_window->slice.atlas );
    initialize_slice_window_view( slice_window );

    slice_window->slice.x_brush_radius = Default_x_brush_radius;
    slice_window->slice.y_brush_radius = Default_y_brush_radius;
    slice_window->slice.z_brush_radius = Default_z_brush_radius;
    slice_window->slice.current_paint_label = Default_paint_label;

    set_atlas_state( slice_window, Default_atlas_state );
}

private  void  free_slice_window(
    slice_window_struct   *slice );

public  void  delete_slice_window(
    slice_window_struct   *slice )
{
    free_slice_window( slice );

    delete_volume( slice->original_volume );
}

private  void  free_slice_window(
    slice_window_struct   *slice )
{
    delete_slice_colour_coding( slice );

    if( slice->volume != (Volume) NULL &&
        slice->volume != slice->original_volume )
    {
        delete_volume( slice->volume );
    }
}

public  void  set_slice_window_volume(
    display_struct    *slice_window,
    Volume            volume )
{
    int        c;
    Real       thickness[N_DIMENSIONS];

    free_slice_window( &slice_window->slice );

    if( slice_window->slice.volume != (Volume) NULL )
    {
        delete_voxel_flags( &slice_window->associated[THREE_D_WINDOW]->
                            three_d.surface_extraction.voxels_queued );

        delete_voxel_done_flags( slice_window->associated[THREE_D_WINDOW]->
                                 three_d.surface_extraction.voxel_done_flags );
    }

    slice_window->slice.volume = volume;

    for_less( c, 0, N_DIMENSIONS )
        slice_window->slice.slice_views[c].update_flag = TRUE;

    slice_window->slice.next_to_update = X;

    set_colour_coding_for_new_volume( slice_window );

    initialize_slice_window_view( slice_window );

    get_volume_separations( volume, thickness );

    slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[X] =
                          ABS( thickness[X] );
    slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Y] =
                          ABS( thickness[Y] );
    slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Z] =
                          ABS( thickness[Z] );

    update_cursor_size( slice_window->associated[THREE_D_WINDOW] );

    initialize_voxel_flags( &slice_window->associated[THREE_D_WINDOW]
                            ->three_d.surface_extraction.voxels_queued,
                            get_n_voxels(volume) );

    initialize_voxel_done_flags( &slice_window->associated[THREE_D_WINDOW]
                                  ->three_d.surface_extraction.voxel_done_flags,
                                  get_n_voxels(volume) );

    set_atlas_state( slice_window, Default_atlas_state );

    rebuild_slice_models( slice_window );
}
