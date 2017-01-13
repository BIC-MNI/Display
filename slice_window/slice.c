/**
 * \file slice.c
 * \brief Basic functions for creating the slice window.
 *
 * \copyright
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

static  void  initialize_slice_window(
    display_struct    *slice_window );

static void initialize_ratio (display_struct* slice_window);

/**
 * Create the slice view window.
 * 
 * \param display A pointer to the 3D view window.
 * \param volume The initial volume loaded.
 */
static void create_slice_window(
    display_struct   *display,
    VIO_Volume           volume )
{
    display_struct   *slice_window;
    int              sizes[VIO_MAX_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    (void) create_graphics_window( SLICE_WINDOW, Slice_double_buffer_flag,
                                   &slice_window,
                                   "Display: Slice View",
                                   Initial_slice_window_x,
                                   Initial_slice_window_y,
                                   Initial_slice_window_width,
                                   Initial_slice_window_height);

    initialize_slice_window( slice_window );

    if( !Use_transparency_hardware )
        G_set_transparency_state( slice_window->window, FALSE );
}

/**
 * Initialize the slice view window data structures.
 *
 * \param slice_window A pointer to the slice view window.
 */
static  void  initialize_slice_window(
    display_struct    *slice_window )
{
    int        i, view;

    G_set_automatic_clear_state( slice_window->window, FALSE );

    slice_window->slice.n_volumes = 0;
    slice_window->slice.current_volume_index = -1;

    slice_window->slice.cursor_visibility = TRUE;

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
    slice_window->slice.volume_scale_step = Initial_volume_scale_step;

    slice_window->slice.incremental_update_allowed = Initial_incremental_update;

    initialize_slice_histogram( slice_window );
    initialize_colour_bar( slice_window );
    initialize_slice_models( slice_window );
    initialize_atlas( &slice_window->slice.atlas );
    initialize_voxel_labeling( slice_window );
    initialize_crop_box( slice_window );

    slice_window->slice.x_brush_radius = Default_x_brush_radius;
    slice_window->slice.y_brush_radius = Default_y_brush_radius;
    slice_window->slice.z_brush_radius = Default_z_brush_radius;
    slice_window->slice.current_paint_label = Default_paint_label;
    slice_window->slice.current_erase_label = 0;

    slice_window->slice.toggle_undo_feature = Initial_undo_feature;

    set_atlas_state( slice_window, Default_atlas_state );

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
        slice_window->slice.slice_views[view].update_outline_flag = FALSE;
        slice_window->slice.slice_views[view].sub_region_specified = FALSE;
        slice_window->slice.slice_views[view].prev_sub_region_specified = FALSE;
        slice_window->slice.slice_views[view].x_min = 0;
        slice_window->slice.slice_views[view].x_max = -1;
        slice_window->slice.slice_views[view].y_min = 0;
        slice_window->slice.slice_views[view].y_max = -1;

        slice_window->slice.slice_views[view].n_atlas_pixels_alloced = 0;
        slice_window->slice.slice_views[view].n_composite_pixels_alloced = 0;
    }

    initialize_ratio( slice_window );
    initialize_intensity_plot( slice_window );
}

/**
 * \brief Initialize the voxel ratio display, if specified.
 *
 * The voxel ratio shows the ratio between two of the loaded volumes
 * as part of the "probe" text display that shows the coordinates and
 * values associated with the current mouse position. The ratio is
 * specified as a comma-separated pair of numbers that correspond to
 * the zero-based indices of the loaded volumes, so a Ratio_volume_index
 * value of "1,2" would cause us to display the ratio between a voxel in
 * the second volume divided by the corresponding voxel in the third volume.
 *
 * \param slice_window A pointer to the slice view window.
 */
static void initialize_ratio (display_struct* slice_window)
{
  model_struct      *model;
  int               retcode;
  text_struct       *text;

  slice_window->slice.ratio_enabled = FALSE;

  if( string_length(Ratio_volume_index) )
  {
    retcode = sscanf(Ratio_volume_index, Ratio_volume_index_format,
                     &slice_window->slice.ratio_volume_numerator,
                     &slice_window->slice.ratio_volume_denominator);
    if( retcode != 2 )
      fprintf(stderr, "Error: can not parse %s with %s\n",
              Ratio_volume_index, Ratio_volume_index_format);
    else
    {
      slice_window->slice.ratio_enabled = TRUE;
      model = get_graphics_model( slice_window, SLICE_READOUT_MODEL );
      text = get_text_ptr( model->objects[RATIO_PROBE_INDEX] );
      text->colour = Slice_probe_ratio_colour;
    }
  }
}

/**
 * Delete all of the data structures associated with a loaded volume.
 *
 * \param slice_window A pointer to the slice view window.
 * \param volume_index The zero-based index of the volume to remove.
 */
static  void  delete_slice_window_volume_stuff(
    display_struct   *slice_window,
    int              volume_index )
{
    int            i;
    display_struct *display = get_three_d_window( slice_window );
    VIO_Volume     volume = get_nth_volume( slice_window, volume_index );
    VIO_Volume     label_volume = get_nth_label_volume( slice_window,
                                                        volume_index );

    tell_surface_extraction_volume_deleted( display, volume, label_volume );

    clear_histogram( slice_window );
    delete_slice_colour_coding( &slice_window->slice, volume_index );

    delete_general_transform( &slice_window->slice.volumes[volume_index].
                              original_transform );
    delete_volume( slice_window->slice.volumes[volume_index].volume );
    delete_slice_models_for_volume( slice_window, volume_index );
    delete_string( slice_window->slice.volumes[volume_index].filename );
    delete_string( slice_window->slice.volumes[volume_index].description );

    for_less( i, volume_index, slice_window->slice.n_volumes-1 )
        slice_window->slice.volumes[i] = slice_window->slice.volumes[i+1];

    SET_ARRAY_SIZE( slice_window->slice.volumes, slice_window->slice.n_volumes,
                    slice_window->slice.n_volumes-1, 1 );
    --slice_window->slice.n_volumes;
}

/**
 * Remove a particular volume from the slice view window.
 *
 * \param slice_window A pointer to the slice view window.
 * \param volume_index The zero-based index of the volume to remove.
 */
void  delete_slice_window_volume(
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

/**
 * Delete all of the structures associated with the slice window.
 *
 * \param slice_window A pointer to the slice view window.
 */
  void  delete_slice_window(
    display_struct   *slice_window )
{
    shutdown_slice_undo( slice_window );

    while( slice_window->slice.n_volumes > 0 )
    {
        delete_slice_window_volume_stuff( slice_window,
                                          slice_window->slice.n_volumes-1 );
    }

    delete_render_storage( slice_window->slice.render_storage );

    delete_slice_histogram( &slice_window->slice );

    delete_atlas( &slice_window->slice.atlas );
    delete_voxel_labeling( &slice_window->slice );
    delete_crop_box( slice_window );
}

/**
 * Get the file name associated with a particular volume index.
 * 
 * The stored pointer to the string is returned, so it should not be 
 * freed or changed without updating the structure.
 *
 * \param slice_window A pointer to the slice view window.
 * \param volume_index The zero-based index of the volume.
 * \returns A pointer to the volume file name.
 */
  VIO_STR  get_volume_filename(
    display_struct    *slice_window,
    int               volume_index )
{
    return( slice_window->slice.volumes[volume_index].filename );
}

/**
 * Creates the slice window if it does not already exist, then adds a
 * new volume to the slice window.
 * \param display A pointer to a top-level display_struct.
 * \param filename The file (or path) name of the volume.
 * \param description Descriptive text to associate with the volume.
 * \param volume The loaded volume.
 */
  void  add_slice_window_volume(
    display_struct    *display,
    VIO_STR            filename,
    VIO_STR            description,
    VIO_Volume            volume )
{
    display_struct         *slice_window;
    int                    new_volume_index;
    int                    cur_volume_index;
    int                    axis, view, sizes[VIO_MAX_DIMENSIONS];
    loaded_volume_struct   *info;
    VIO_Real               current_voxel[VIO_MAX_DIMENSIONS];

    if( !slice_window_exists(display) )
    {
        create_slice_window( display, volume );
        initialize_slice_object_outline(display);
    }

    if ( !get_slice_window( display, &slice_window ) )
    {
        HANDLE_INTERNAL_ERROR("Failed to create slice window.");
        return;
    }

    SET_ARRAY_SIZE( slice_window->slice.volumes,
                    slice_window->slice.n_volumes,
                    slice_window->slice.n_volumes+1, 1 );

    new_volume_index = slice_window->slice.n_volumes++;
    info = &slice_window->slice.volumes[new_volume_index];

    info->volume = volume;
    copy_general_transform( get_voxel_to_world_transform(volume),
                            &info->original_transform );
    info->filename = create_string( filename );
    info->description = create_string( description );
    info->display_labels = Initial_display_labels;

    get_volume_sizes( volume, sizes );

    /* The first volume is always opaque. However, subsequent volumes are
     * not. That way the user can see she has loaded multiple volumes.
     */
    if (new_volume_index == 0)
    {
        info->opacity = 1.0;
    }
    else
    {
        info->opacity = 0.66;
    }

    for_less( view, 0, N_SLICE_VIEWS )
    {
        info->views[view].visibility = TRUE;
        info->views[view].update_flag = TRUE;
        info->views[view].update_labels_flag = TRUE;
        info->views[view].filter_type = (VIO_Filter_types) Default_filter_type;
        info->views[view].filter_width = Default_filter_width;
        info->views[view].n_pixels_redraw = Initial_n_pixels_redraw;
        info->views[view].update_in_progress[0] = FALSE;
        info->views[view].update_in_progress[1] = FALSE;
    }

    initialize_slice_models_for_volume( slice_window, new_volume_index );
    initialize_slice_colour_coding( slice_window, new_volume_index );
    initialize_slice_window_view( slice_window, new_volume_index );
    initialize_slice_undo( &slice_window->slice.volumes[new_volume_index].undo );


    /* Initialize the current_voxel field.
     */
    for_less( axis, 0, VIO_MAX_DIMENSIONS )
        info->current_voxel[axis] = 0.0;

    /* Set the initial voxel position. Normally this is set to the centre
     * of the first loaded volume in voxel space.
     */
    if( slice_window->slice.n_volumes == 1 )
    {
        for_less( axis, 0, VIO_N_DIMENSIONS )
            info->current_voxel[axis] = (sizes[axis] - 1.0) / 2.0;
    }
    else
    {
        cur_volume_index = get_current_volume_index(slice_window);

        get_current_voxel( slice_window, cur_volume_index, current_voxel );

        for_less( axis, 0, VIO_N_DIMENSIONS )
            info->current_voxel[axis] = -1.0e20;

        set_current_voxel( slice_window, cur_volume_index, current_voxel );
        update_all_slice_axes_views( slice_window, cur_volume_index );
    }

    set_current_volume_index( slice_window, new_volume_index );

    set_slice_window_all_update( slice_window, -1, UPDATE_BOTH );
}

/**
 * Reset the slice (and label) pixel objects. We reset the pixels
 * (essentially deleting them) when we make a major change such as
 * altering the order of the volumes.
 *
 * \param slice_window A pointer to the slice window.
 * \param volume_index The index of the volume whose pixels should be reset.
 */
static void
reset_slice_pixels( display_struct *slice_window, int volume_index )
{
  int view_index;
  loaded_volume_struct *lvs_ptr;

  lvs_ptr = &slice_window->slice.volumes[volume_index];

  for (view_index = 0; view_index < N_SLICE_VIEWS; view_index++)
  {
    object_struct *object_ptr;
    pixels_struct *pixels_ptr;

    object_ptr = get_slice_pixels_object( slice_window, volume_index,
                                          view_index );
    pixels_ptr = get_pixels_ptr( object_ptr );
    delete_pixels( pixels_ptr );
    lvs_ptr->views[view_index].n_pixels_alloced = 0;

    object_ptr = get_label_slice_pixels_object( slice_window, volume_index,
                                                view_index );
    pixels_ptr = get_pixels_ptr( object_ptr );
    delete_pixels( pixels_ptr );
    lvs_ptr->views[view_index].n_label_pixels_alloced = 0;
  }
}

/**
 * Move the volume at volume_index to the "top" of the stack of
 * volumes from a visual perspective, meaning the volume will be moved
 * to the end of the volume list.
 *
 * \param display A pointer to a top-level window.
 * \param volume_index The zero-based index of the volume to move.
 */
void
move_slice_window_volume(display_struct *display,
                         int            volume_index)
{
  display_struct       *slice_window;
  int                  last_index;
  loaded_volume_struct loaded_volume_temp;

  if ( !get_slice_window( display, &slice_window ) )
  {
    return;
  }

  /* Get the index of the last volume - it will be the last one drawn
   * and therefore the top of the stack from a visual perspective.
   */
  last_index = slice_window->slice.n_volumes - 1;
  if ( volume_index == last_index )
  {
    return;
  }

  /* Exchange the two volume with the last volume structures. */
  loaded_volume_temp = slice_window->slice.volumes[last_index];

  slice_window->slice.volumes[last_index] =
    slice_window->slice.volumes[volume_index];

  slice_window->slice.volumes[volume_index] = loaded_volume_temp;

  /* Reset the pixel objects associated with these volumes, so they
   * will be rebuilt with the proper size (and contents).
   */
  reset_slice_pixels( slice_window, last_index );
  reset_slice_pixels( slice_window, volume_index );

  /* Trigger redrawing of the slice views.
   */
  update_all_slice_axes_views( slice_window, volume_index );
  update_all_slice_axes_views( slice_window, last_index );
  set_current_volume_index( slice_window, last_index );
  set_slice_window_all_update( slice_window, -1, UPDATE_BOTH );
}

/**
 * Set the currently selected volume. 
 *
 * \param slice_window A pointer to the slice window.
 * \param volume_index The zero-based index of the newly selected volume.
 */
void  set_current_volume_index(
    display_struct  *slice_window,
    int             volume_index )
{
    VIO_BOOL        first;
    int             view;
    VIO_Real        separations[VIO_MAX_DIMENSIONS];
    display_struct  *display;

    if( slice_window->slice.current_volume_index < 0 )
        first = TRUE;
    else if( volume_index >= 0 )
        first = FALSE;

    slice_window->slice.current_volume_index = volume_index;

    if( volume_index >= 0 )
    {
        get_volume_separations( get_nth_volume(slice_window, volume_index),
                                separations );

        display = get_three_d_window( slice_window );

        display->three_d.cursor.box_size[VIO_X] = VIO_FABS( separations[VIO_X] );
        display->three_d.cursor.box_size[VIO_Y] = VIO_FABS( separations[VIO_Y] );
        display->three_d.cursor.box_size[VIO_Z] = VIO_FABS( separations[VIO_Z] );

        update_cursor_size( display );

        if( first )
        {
            for_less( view, 0, N_SLICE_VIEWS )
                reset_slice_view( slice_window, view );
        }

        G_set_window_title( slice_window->window,
                            slice_window->slice.volumes[volume_index].description);
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

/**
 * Get the number of loaded volumes.
 *
 * \param display A pointer to a top-level window.
 * \returns The number of loaded volumes.
 */
int  get_n_volumes(
    display_struct  *display )
{
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
        return( slice_window->slice.n_volumes );
    else
        return( 0 );
}

/**
 * Get the zero-based index of the current volume.
 *
 * \param display A pointer to a top-level window.
 * \returns The zero-based index of the current volume or -1 on failure.
 */
int   get_current_volume_index(
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

/**
 * Get a handle to the current loaded volume.
 *
 * \param display A pointer to a top-level window.
 * \param volume  A pointer to a location that will receive the volume
 * handle.
 * \returns TRUE if the volume is present.
 */
  VIO_BOOL   get_slice_window_volume(
    display_struct   *display,
    VIO_Volume        *volume )
{
    VIO_BOOL         volume_exists;
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
        *volume = (VIO_Volume) NULL;
        volume_exists = FALSE;
    }

    return( volume_exists );
}

/**
 * Get a loaded volume by index, where the first volume is at index zero.
 *
 * \param display A pointer to a top-level window.
 * \param volume_index The zero-based index of the desired volume.
 * \returns A handle to the volume, or NULL on failure.
 */
VIO_Volume  get_nth_volume(
    display_struct   *display,
    int              volume_index )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        volume_index >= 0 &&
        volume_index < slice_window->slice.n_volumes )
    {
        return( slice_window->slice.volumes[volume_index].volume );
    }
    else
        return( (VIO_Volume) NULL );
}

/**
 * Get a handle to the current loaded volume.
 *
 * \param display A pointer to a top-level window.
 * \returns A handle to the volume, or NULL on failure.
 */
VIO_Volume   get_volume(
    display_struct   *display )
{
    VIO_Volume      volume;

    (void) get_slice_window_volume( display, &volume );

    return( volume );
}

/**
 * Check whether the slice window was created.
 *
 * \param display A pointer to a top-level window.
 * \returns TRUE if the slice window was created.
 */
VIO_BOOL  slice_window_exists(
    display_struct   *display )
{
    return( get_display_by_type( SLICE_WINDOW ) != NULL );
}

/**
 * Get a pointer to the slice window.
 * 
 * \param display A pointer to a top-level window.
 * \param slice_window A pointer to a location to receive the slice window 
 * pointer.
 * \returns TRUE if successful.
 */
VIO_BOOL  get_slice_window(
    display_struct   *display,
    display_struct   **slice_window )
{
  *slice_window = get_display_by_type( SLICE_WINDOW );

    return( *slice_window != NULL );
}

/**
 * Compute the total range of all loaded volumes, in world coordinates.
 * 
 * Returns two points corresponding to the minumum and maximum points of the
 * rectangular prism.
 *
 * \param display A pointer to the slice view window.
 * \param min_limit A point that will contain the minimum coordinate.
 * \param max_limit A point that will contain the minimum coordinate.
 * \returns TRUE if the values were computed, FALSE if no volume is loaded.
 */
  VIO_BOOL  get_range_of_volumes(
    display_struct   *display,
    VIO_Point            *min_limit,
    VIO_Point            *max_limit )
{
    int              n_volumes, sizes[VIO_MAX_DIMENSIONS], dx, dy, dz, volume_index;
    int              dim;
    VIO_Volume           volume;
    VIO_Real         voxel[VIO_MAX_DIMENSIONS], world[VIO_N_DIMENSIONS];
    VIO_BOOL          first;

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
            voxel[VIO_X] = -0.5 + (VIO_Real) dx * (VIO_Real) sizes[VIO_X];
            voxel[VIO_Y] = -0.5 + (VIO_Real) dy * (VIO_Real) sizes[VIO_Y];
            voxel[VIO_Z] = -0.5 + (VIO_Real) dz * (VIO_Real) sizes[VIO_Z];

            convert_voxel_to_world( volume, voxel,
                                    &world[VIO_X], &world[VIO_Y], &world[VIO_Z] );

            for_less( dim, 0, VIO_N_DIMENSIONS )
            {
                if( first || world[dim] < (VIO_Real) Point_coord(*min_limit,dim) )
                    Point_coord(*min_limit,dim) = (VIO_Point_coord_type) world[dim];
                if( first || world[dim] > (VIO_Real) Point_coord(*max_limit,dim) )
                    Point_coord(*max_limit,dim) = (VIO_Point_coord_type) world[dim];
            }

            first = FALSE;
        }
    }

    return( TRUE );
}

/**
 * Set a flag indicating it's time to redraw the crosshair cursor
 * for the slice views.
 *
 * \param slice_window A pointer to the slice view window.
 * \param view_index The view index to update, or -1 if all views should
 * be updated.
 */
void  set_slice_cursor_update(
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

/**
 * Set a flag indicating it's time to redraw the text (cursor position)
 * for the slice views.
 *
 * \param slice_window A pointer to the slice view window.
 * \param view_index The view index to update, or -1 if all views should
 * be updated.
 */
void  set_slice_text_update(
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

/**
 * Set a flag indicating it's time to redraw the object outlines in 
 * the slice views.
 *
 * \param slice_window A pointer to the slice view window.
 * \param view_index The view index to update, or -1 if all views should
 * be updated.
 */
void
set_slice_outline_update(
                         display_struct   *slice_window,
                         int              view_index )
{
  if( view_index >= 0 )
  {
    slice_window->slice.slice_views[view_index].update_outline_flag = TRUE;
  }
  else
  {
    for_less( view_index, 0, N_SLICE_VIEWS )
    {
      slice_window->slice.slice_views[view_index].update_outline_flag = TRUE;
    }
  }
}

/**
 * Set a flag indicating it's time to redraw the slice cross section.
 *
 * The slice cross section is the projection of the arbitrary (oblique)
 * plane in the three orthogonal slice views.
 *
 * \param slice_window A pointer to the slice view window.
 * \param view_index The view index to update, or -1 if all views should
 * be updated.
 */
void  set_slice_cross_section_update(
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

/**
 * Set a flag indicating it is time to update the crop box drawn in the
 * slice view window.
 *
 * \param slice_window A pointer to the slice view window.
 * \param view_index The view index to update, or -1 if all views should
 * be updated.
 */
void  set_crop_box_update(
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

/**
 * Set a flag indicating it's time to redraw the slice dividers.
 *
 * \param slice_window A pointer to the slice view window.
 */
static void  set_slice_dividers_update(
    display_struct   *slice_window )
{
    slice_window->slice.update_slice_dividers_flag = TRUE;
}

/**
 * Set a flag indicating it's time to redraw the slice view information text.
 *
 * \param slice_window A pointer to the slice view window.
 */
void  set_probe_update(
    display_struct   *slice_window )
{
    slice_window->slice.update_probe_flag = TRUE;
}

/**
 * Set a flag indicating it's time to redraw the slice view colour bar.
 *
 * \param slice_window A pointer to the slice view window.
 */
  void  set_colour_bar_update(
    display_struct   *slice_window )
{
    slice_window->slice.update_colour_bar_flag = TRUE;
}

/**
 * Set a flag indicating it's time to redraw the slice view atlas.
 *
 * \param slice_window A pointer to the slice view window.
 * \param view_index The index of the view to update, or -1 if all
 * visible views should be updated.
 */
  void  set_atlas_update(
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

/**
 * Trigger update of each of the slice graphical models.
 *
 * \param slice_window A pointer to the slice view window.
 */
void  update_all_slice_models(
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
    set_slice_outline_update( slice_window, -1 );
}

/**
 * Set a flag indicating it's time to update the overall slice view.
 *
 * \param slice_window A pointer to the slice view window.
 * \param volume_index The volume for which the update is required, or -1
 * if all volumes should be updated.
 * \param view_index The index of the view to update.
 * \param type Either UPDATE_SLICE, UPDATE_LABELS, or UPDATE_BOTH to
 * indicate which aspect of the display to update.
 */
void  set_slice_window_update(
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

/**
 * Set a flag indicating it's time to update all of the slice views.
 *
 * \param slice_window A pointer to the slice view window.
 * \param volume_index The volume for which the update is required, or -1
 * if all volumes should be updated.
 * \param type Either UPDATE_SLICE, UPDATE_LABELS, or UPDATE_BOTH to
 * indicate which aspect of the display to update.
 */
  void  set_slice_window_all_update(
    display_struct   *slice_window,
    int              volume_index,
    Update_types     type )
{
    int  view;

    for_less( view, 0, N_SLICE_VIEWS )
        set_slice_window_update( slice_window, volume_index, view, type );
}

/**
 * Sets a flag indicating that a particular viewport of the slice view
 * window should be updated.
 * \param slice_window A pointer to the slice view window.
 * \param model_number One of FULL_WINDOW_MODEL, SLICE_READOUT_MODEL, 
 * COLOUR_BAR_MODEL, INTENSITY_PLOT_MODEL, SLICE_MODEL1, SLICE_MODEL2,
 * SLICE_MODEL3, or SLICE_MODEL4.
 */
void  set_slice_viewport_update(
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

/**
 * Check whether any aspect of a slice viewport has changed, and therefore
 * needs to be updated.
 *
 * \param slice_window A pointer to the slice view window.
 * \param view The index of the view to check.
 * \returns TRUE if the slice viewport needs to be updated.
 */
static  VIO_BOOL  slice_viewport_has_changed(
    display_struct   *slice_window,
    int              view )
{
    VIO_BOOL  changed;
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

    if( slice_window->slice.slice_views[view].update_outline_flag )
        changed = TRUE;

    return( changed );
}

/**
 * Check whether the slice update is continuing (i.e. was previously
 * interrupted).
 *
 * \param slice_window A pointer to the slice view window.
 * \param view The index of the view to check.
 * \returns TRUE if an update of this view was previously interrupted.
 */
static  VIO_BOOL  is_slice_continuing(
    display_struct   *slice_window,
    int              view )
{
    VIO_BOOL  continuing;
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

/**
 * Check whether the partial redraw of the slice view should be interrupted.
 * \param end_time The final time allowed for rendering, negative or zero
 * if unlimited.
 * \param current_time The current time in seconds.
 * \returns TRUE if the end_time is positive and the current time is greater
 * than the end time.
 */
static  VIO_BOOL  time_is_up(
    VIO_Real    end_time,
    VIO_Real    current_time )
{
    return( end_time >= 0.0 && current_time > end_time );
}

/**
 * Render slice data into the appropriate pixel objects. 
 * 
 * Most of the complexity comes from attempts to perform minimal updates,
 * and allowing the updates to be restarted if they take more than a 
 * given amount of time.
 *
 * \param slice_window A pointer to the slice view window.
 * \param viewport_has_changed An array of flags that indicate which
 * of the four slice viewports have actually changed.
 */
static  void  render_more_slices(
    display_struct   *slice_window,
    VIO_BOOL          viewport_has_changed[] )
{
    VIO_BOOL  first_render, did_one, finished;
    VIO_BOOL  interrupted;
    VIO_BOOL  viewport_pixels_modified[N_SLICE_VIEWS];
    VIO_BOOL  no_viewport_changed, incremental_flag;
    VIO_BOOL  *update_flag_ptr, *update_in_progress;
    int      view, v, v_index, view_index, which_volume;
    int      slice_is_visible, n_pixels_drawn;
    int      current_update_volume, current_update_view, n_volumes;
    VIO_Real     update_time, end_time, current_time, prev_time;
    VIO_Real     time_to_create;

    no_viewport_changed = TRUE;
    for_less( view, 0, N_SLICE_VIEWS )
    {
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

    /*--- if currently painting or incremental update not desired, then
          don't interrupt the slice recreation */

    if( slice_window->slice.painting_view_index >= 0 ||
        !slice_window->slice.incremental_update_allowed || update_time <= 0.0 )
    {
        end_time = -1.0;
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
                        slice_is_visible = get_labels_visibility( slice_window,
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
                                                     0.5*(VIO_Real)n_pixels_drawn));
                                    }
                                }


                                if( !finished &&
                                     time_is_up( end_time, current_time ) )
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
    }
}

/**
 * Actually rebuild the requested parts of the slice window.
 *
 * Called from the timer_function() in event_loop.c
 *
 * \param slice_window A pointer to the slice view window.
 */
void  update_slice_window(
    display_struct   *slice_window )
{
    VIO_BOOL  viewport_has_changed[N_SLICE_VIEWS];
    VIO_BOOL  one_buffer_flag[N_SLICE_VIEWS];
    VIO_BOOL  slice_is_continuing[N_SLICE_VIEWS];
    VIO_BOOL  original_sub_region_specified[N_SLICE_VIEWS];
    VIO_BOOL  prev_use_sub_region;
    int      x_sub_min, x_sub_max;
    int      y_sub_min, y_sub_max;
    int      view;
    VIO_BOOL render_intensity_plot = FALSE;

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
            (slice_window->slice.slice_views[view].prev_sub_region_specified &&
             (viewport_has_changed[view] ||
              slice_window->slice.viewport_update_flags[SLICE_MODEL1+view][0])))
        {
            slice_window->slice.slice_views[view].use_sub_region =
                                       original_sub_region_specified[view];

            if( viewport_has_changed[view] ||
                (slice_is_continuing[view] && !prev_use_sub_region &&
                 slice_window->slice.slice_views[view].sub_region_specified) ||
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
            original_sub_region_specified[view] &&
            slice_window->slice.incremental_update_allowed )
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
        rebuild_volume_cross_section( slice_window );
        slice_window->slice.update_colour_bar_flag = FALSE;
    }

    for_less( view, 0, N_SLICE_VIEWS )
    {
        if( slice_window->slice.slice_views[view].update_cursor_flag )
        {
            rebuild_slice_cursor( slice_window, view );
            rebuild_slice_field_of_view( slice_window, view );
            rebuild_slice_rulers( slice_window, view );
            rebuild_slice_scalebar( slice_window, view );
            render_intensity_plot = TRUE; /* For later */
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

        if( slice_window->slice.slice_views[view].update_outline_flag )
        {
            rebuild_slice_object_outline(slice_window, view );
            slice_window->slice.slice_views[view].update_outline_flag = FALSE;
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
        else
        {
            x_sub_min = 0.0;
            x_sub_max = 0.0;
            y_sub_min = 0.0;
            y_sub_max = 0.0;
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
            render_intensity_plot = TRUE;
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

    if (render_intensity_plot)
      rebuild_intensity_plot( slice_window );
}

/**
 * Set the subviewport to be updated for a given slice view.
 *
 * This is used, for example, when painting requires that we
 * update a small portion of the overall slice view.
 *
 * The coordinates are given in pixel units relative to the
 * position of the overall slice viewport.
 *
 * \param slice_window A pointer to the slice view window.
 * \param view_index The index of the view.
 * \param x_min The minimum x coordinate.
 * \param x_max The maximum x coordinate.
 * \param y_min The minimum y coordinate.
 * \param y_max The maximum y coordinate.
 */
  void  set_slice_composite_update(
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

/**
 * Get the portion of the slice viewport that is scheduled to be updated.
 *
 * The returned coordinates are given in pixel units relative to the
 * position of the overall slice viewport.
 *
 * \param slice_window A pointer to the slice view window.
 * \param view_index The index of the view.
 * \param x_min A pointer that will hold the minimum x coordinate.
 * \param x_max A pointer that will hold the maximum x coordinate.
 * \param y_min A pointer that will hold the minimum y coordinate.
 * \param y_max A pointer that will hold the maximum y coordinate.
 * \returns TRUE if the subviewport is active.
 */
VIO_BOOL  get_slice_subviewport(
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
