
#include  <def_display.h>

public  Boolean   get_slice_window_volume(
    display_struct   *display,
    volume_struct    **volume )
{
    Boolean  volume_set;

    if( display->associated[SLICE_WINDOW] != (display_struct *) NULL )
    {
        *volume = display->associated[SLICE_WINDOW]->slice.volume;
        volume_set = TRUE;
    }
    else
    {
        *volume = (volume_struct *) 0;
        volume_set = FALSE;
    }

    return( volume_set );
}

public  volume_struct   *get_volume(
    display_struct   *display )
{
    volume_struct  *volume;

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
    int       x, y, z;
    Real      value;

    if( get_isosurface_value( slice_window->associated[THREE_D_WINDOW], &value))
    {
        get_current_voxel( slice_window, &x, &y, &z );

        if( cube_is_within_volume( get_volume(slice_window), x, y, z ) &&
            voxel_contains_value( get_volume(slice_window), x, y, z, value ))
        {
            update_cursor_colour( slice_window->associated[THREE_D_WINDOW],
                                  &Cursor_colour_on_surface );
        }
        else
        {
            update_cursor_colour( slice_window->associated[THREE_D_WINDOW],
                                  &Cursor_colour_off_surface );
        }
    }
}

public  Boolean  update_cursor_from_voxel(
    display_struct    *slice_window )
{
    int               x, y, z;
    Real              x_w, y_w, z_w;
    Boolean           changed;
    Point             new_origin;
    display_struct    *display;

    display = slice_window->associated[THREE_D_WINDOW];

    get_current_voxel( slice_window, &x, &y, &z );

    convert_voxel_to_world( get_volume(slice_window),
                            (Real) x, (Real) y, (Real) z, &x_w, &y_w, &z_w );
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
    display_struct   *display,
    int              view_index )
{
#ifndef  BUG
    if( display != (display_struct  *) 0 )
    {
#endif
        display->slice.slice_views[view_index].update_flag = TRUE;
        set_update_required( display, NORMAL_PLANES );
#ifndef  BUG
    }
#endif
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
    volume_struct    *volume )
{
    display_struct   *slice_window, *menu_window;
    int              nx, ny, nz;
    String           title;

    get_volume_size( volume, &nx, &ny, &nz );

    (void) sprintf( title, "%s [%d * %d * %d]", volume->filename,
                    nx, ny, nz );

    (void) create_graphics_window( SLICE_WINDOW, &slice_window, title, 0, 0 );

    menu_window = display->associated[MENU_WINDOW];

    slice_window->associated[THREE_D_WINDOW] = display;
    slice_window->associated[MENU_WINDOW] = menu_window;
    slice_window->associated[SLICE_WINDOW] = slice_window;
    display->associated[SLICE_WINDOW] = slice_window;
    menu_window->associated[SLICE_WINDOW] = slice_window;

    slice_window->slice.original_volume = *volume;

    set_slice_window_volume( slice_window,
                             &slice_window->slice.original_volume );

    set_slice_window_update( slice_window, 0 );
    set_slice_window_update( slice_window, 1 );
    set_slice_window_update( slice_window, 2 );
}

public  void  initialize_slice_window(
    display_struct    *slice_window )
{
    int        c;

    slice_window->slice.volume = (volume_struct *) NULL;

    slice_window->slice.temporary_indices_alloced = 0;

    initialize_slice_window_events( slice_window );

    for_less( c, 0, N_DIMENSIONS )
        slice_window->slice.slice_views[c].update_flag = TRUE;

    slice_window->slice.next_to_update = X;

    initialize_slice_colour_coding( slice_window );

    initialize_slice_models( slice_window );

    initialize_segmenting( &slice_window->slice.segmenting );
    initialize_atlas( &slice_window->slice.atlas );
    initialize_slice_window_view( slice_window );

    set_atlas_state( slice_window, Default_atlas_state );
}

private  void  free_slice_window(
    slice_window_struct   *slice );

public  void  delete_slice_window(
    slice_window_struct   *slice )
{
    free_slice_window( slice );

    delete_volume( &slice->original_volume );
}

private  void  free_slice_window(
    slice_window_struct   *slice )
{
    delete_slice_colour_coding( slice );

    if( slice->temporary_indices_alloced > 0 )
    {
        FREE( slice->temporary_indices );
    }

    if( slice->volume != (volume_struct *) NULL &&
        slice->volume != &slice->original_volume )
    {
        delete_volume( slice->volume );
        FREE( slice->volume );
    }
}

public  void  set_slice_window_volume(
    display_struct    *slice_window,
    volume_struct     *volume )
{
    int        c;
    Real       thickness[N_DIMENSIONS];

    free_slice_window( &slice_window->slice );

    if( slice_window->slice.volume != (volume_struct *) NULL )
    {
        delete_voxel_flags( &slice_window->associated[THREE_D_WINDOW]->
                            three_d.surface_extraction.voxels_queued );

        delete_voxel_done_flags( slice_window->associated[THREE_D_WINDOW]->
                                 three_d.surface_extraction.voxel_done_flags );
    }

    slice_window->slice.volume = volume;

    slice_window->slice.temporary_indices_alloced = 0;

    for_less( c, 0, N_DIMENSIONS )
        slice_window->slice.slice_views[c].update_flag = TRUE;

    slice_window->slice.next_to_update = X;

    set_colour_coding_for_new_volume( slice_window );

    initialize_slice_window_view( slice_window );

    get_volume_slice_thickness( volume, &thickness[X], &thickness[Y],
                                        &thickness[Z] );

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
