
#include  <display.h>

public  BOOLEAN   get_slice_window_volume(
    display_struct   *display,
    Volume           *volume )
{
    BOOLEAN  volume_set;

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

public  Volume  get_label_volume(
    display_struct   *display )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        return( slice_window->slice.labels );
    else
        return( (Volume) NULL );
}

public  BOOLEAN  get_slice_window(
    display_struct   *display,
    display_struct   **slice_window )
{
    BOOLEAN  exists;

    exists = FALSE;

    if( display != (display_struct *) NULL &&
        display->associated[SLICE_WINDOW] != (display_struct *) NULL )
    {
        *slice_window = display->associated[SLICE_WINDOW];
        exists = TRUE;
    }

    return( exists );
}

public  BOOLEAN  update_cursor_from_voxel(
    display_struct    *slice_window )
{
    Real              voxel[MAX_DIMENSIONS];
    Real              x_w, y_w, z_w;
    BOOLEAN           changed;
    Point             new_origin;
    display_struct    *display;

    display = slice_window->associated[THREE_D_WINDOW];

    get_current_voxel( slice_window, voxel );

    convert_voxel_to_world( get_volume(slice_window), voxel, &x_w, &y_w, &z_w );
    fill_Point( new_origin, x_w, y_w, z_w );

    if( !EQUAL_POINTS( new_origin, display->three_d.cursor.origin ) )
    {
        display->three_d.cursor.origin = new_origin;

        update_cursor( display );

        changed = TRUE;
    }
    else
    {
        changed = FALSE;
    }

    return( changed );
}

public  BOOLEAN  update_voxel_from_cursor(
    display_struct    *slice_window )
{
    Real              voxel[MAX_DIMENSIONS];
    BOOLEAN           changed;
    display_struct    *display;

    changed = FALSE;

    if( get_slice_window(slice_window,&slice_window) )
    {
        display = slice_window->associated[THREE_D_WINDOW];

        if( get_voxel_corresponding_to_point( slice_window,
                                    &display->three_d.cursor.origin,
                                    voxel ) )
        {
            changed = set_current_voxel( slice_window, voxel );
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

public  void  set_slice_window_all_update(
    display_struct   *slice_window )
{
    int  view;

    for_less( view, 0, N_SLICE_VIEWS )
        set_slice_window_update( slice_window, view );
}

public  void  update_slice_window(
    display_struct   *display )
{
    int   view;

    for_less( view, 0, N_SLICE_VIEWS )
    {
        if( display->slice.slice_views[view].update_flag )
        {
            rebuild_slice_pixels( display, view );
            display->slice.slice_views[view].update_flag = FALSE;
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

    slice_window->slice.original_volume = volume;
    slice_window->slice.original_labels = create_label_volume( volume );

    set_slice_window_volume( slice_window,
                             slice_window->slice.original_volume );

    set_slice_window_all_update( slice_window );
}

public  void  initialize_slice_window(
    display_struct    *slice_window )
{
    int        view;

    slice_window->slice.volume = (Volume) NULL;

    initialize_slice_histogram( slice_window );
    initialize_slice_window_events( slice_window );
    initialize_voxel_labeling( slice_window );

    for_less( view, 0, N_SLICE_VIEWS )
    {
        slice_window->slice.slice_views[view].filter_type =
                           (Filter_types) Default_filter_type;
        slice_window->slice.slice_views[view].filter_width =
                                          Default_filter_width;
        slice_window->slice.slice_views[view].update_flag = TRUE;
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

    initialize_slice_undo( &slice_window->slice.undo );
}

private  void  free_slice_window(
    slice_window_struct   *slice );

public  void  delete_slice_window(
    slice_window_struct   *slice )
{
    free_slice_window( slice );

    delete_slice_histogram( slice );

    delete_volume( slice->original_volume );
    delete_volume( slice->original_labels );

    delete_atlas( &slice->atlas );
}

private  void  free_slice_window(
    slice_window_struct   *slice )
{
    delete_slice_colour_coding( slice );
    delete_slice_undo( &slice->undo );

    if( slice->volume != (Volume) NULL &&
        slice->volume != slice->original_volume )
    {
        delete_volume( slice->volume );
        delete_volume( slice->labels );
    }
}

public  void  set_slice_window_volume(
    display_struct    *slice_window,
    Volume            volume )
{
    int        view, sizes[MAX_DIMENSIONS];
    Real       separations[N_DIMENSIONS];

    free_slice_window( &slice_window->slice );

    if( slice_window->slice.volume != (Volume) NULL )
    {
        delete_voxel_flags( &slice_window->associated[THREE_D_WINDOW]->
                            three_d.surface_extraction.voxels_queued );

        delete_voxel_done_flags( slice_window->associated[THREE_D_WINDOW]->
                                 three_d.surface_extraction.voxel_done_flags );
    }

    slice_window->slice.volume = volume;
    get_volume_sizes( volume, sizes );

    if( volume == slice_window->slice.original_volume )
        slice_window->slice.labels = slice_window->slice.original_labels;
    else
        slice_window->slice.labels = create_label_volume( volume );

    for_less( view, 0, N_SLICE_VIEWS )
        slice_window->slice.slice_views[view].update_flag = TRUE;

    slice_window->slice.next_to_update = X;

    set_colour_coding_for_new_volume( slice_window );

    initialize_slice_window_view( slice_window );

    get_volume_separations( volume, separations );

    slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[X] =
                          ABS( separations[X] );
    slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Y] =
                          ABS( separations[Y] );
    slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Z] =
                          ABS( separations[Z] );

    update_cursor_size( slice_window->associated[THREE_D_WINDOW] );

    initialize_voxel_flags( &slice_window->associated[THREE_D_WINDOW]
                            ->three_d.surface_extraction.voxels_queued,
                            get_n_voxels(volume) );

    initialize_voxel_done_flags( &slice_window->associated[THREE_D_WINDOW]
                                  ->three_d.surface_extraction.voxel_done_flags,
                                  get_n_voxels(volume) );

    set_atlas_state( slice_window, Default_atlas_state );

    rebuild_slice_models( slice_window );

    rebuild_volume_cross_section( slice_window );
    rebuild_volume_outline( slice_window );

    clear_histogram( slice_window );

    delete_slice_undo( &slice_window->slice.undo );
}

public  void  set_voxel_label_flag(
    Volume   label_volume,
    int      voxel[],
    BOOLEAN  value )
{
    set_voxel_label_bit( label_volume, voxel, get_label_bit(), value );
}

public  BOOLEAN  get_voxel_label_flag(
    Volume   label_volume,
    int      voxel[] )
{
    return( get_voxel_label_bit( label_volume, voxel, get_label_bit() ) );
}
