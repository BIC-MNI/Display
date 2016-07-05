/**
 * \file volume_ops.c
 * \brief Menu commands for manipulating and navigating voxel data.
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


/**
 * Move the time coordinate forward or backward by \c delta.
 * \param display A pointer to a top-level window.
 * \param delta The amount (positive or negative) by which to move
 * the cursor along the time axis.
 */
static void
change_current_time_by_one(
                           display_struct   *display,
                           int              delta
                           )
{
    display_struct  *slice_window;
    VIO_Volume      volume;
    VIO_Real        voxel[VIO_MAX_DIMENSIONS];
    VIO_Real        separations[VIO_MAX_DIMENSIONS];
    int             sizes[VIO_MAX_DIMENSIONS];
    int             volume_index;
    VIO_Real        new_position;

    if( !get_slice_window( display, &slice_window ))
        return;

    volume_index = get_current_volume_index( slice_window );
    if (volume_index < 0)
        return;

    volume = get_nth_volume( slice_window, volume_index );
    get_volume_sizes( volume, sizes );
    get_volume_separations( volume, separations );

    /* Does time ever have a negative step size? I guess it's
     * not impossible...
     */
    if( separations[VIO_T] < 0.0 )
        delta = -delta;

    get_current_voxel( slice_window, volume_index, voxel );

    new_position = VIO_ROUND( voxel[VIO_T] + delta );

    /* Check that the time position stays in range.
     */
    if( new_position < 0.0 )
        new_position = 0.0;

    if( new_position > sizes[VIO_T] - 1.0 )
        new_position = sizes[VIO_T] - 1.0;

    if (new_position != voxel[VIO_T])
    {
        voxel[VIO_T] = new_position;
        if( set_current_voxel( slice_window, volume_index, voxel ))
            update_cursor_from_voxel( slice_window );
    }
}

/**
 * \brief Command to move forward one position along the time axis.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(move_time_plus)
{
    change_current_time_by_one( display, 1 );

    return( VIO_OK );
}

DEF_MENU_UPDATE(move_time_plus)
{
    display_struct *slice_window;
    VIO_Volume volume;

    if( !get_slice_window( display, &slice_window ))
        return FALSE;

    if ((volume = get_volume(slice_window)) == NULL)
        return FALSE;

    return( get_volume_n_dimensions(volume) > 3 );
}

/**
 * \brief Command to move backward one position along the time axis.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(move_time_minus)
{
    change_current_time_by_one( display, -1 );

    return( VIO_OK );
}

DEF_MENU_UPDATE(move_time_minus)
{
    display_struct *slice_window;
    VIO_Volume volume;

    if( !get_slice_window( display, &slice_window ))
        return FALSE;

    if ((volume = get_volume(slice_window)) == NULL)
        return FALSE;

    return( get_volume_n_dimensions(volume) > 3 );
}

/**
 * Helper function to change the slice coordinate by a given value.
 * \param display A pointer to a top-level window.
 * \param delta The amount (positive or negative) by which to move
 * the cursor along the slice axis.
 */
static void
change_current_slice_by_delta( display_struct *display, int delta )
{
    display_struct   *slice_window;
    VIO_Volume           volume;
    VIO_Real         voxel[VIO_MAX_DIMENSIONS];
    VIO_Real         separations[VIO_MAX_DIMENSIONS];
    int              sizes[VIO_MAX_DIMENSIONS], axis_index, volume_index;

    if( get_slice_window( display, &slice_window ) &&
        get_axis_index_under_mouse( display, &volume_index, &axis_index ) )
    {
        volume = get_nth_volume( slice_window, volume_index );
        get_volume_sizes( volume, sizes );
        get_volume_separations( volume, separations );

        if( separations[axis_index] < 0.0 )
            delta = -delta;

        get_current_voxel( slice_window, volume_index, voxel );

        voxel[axis_index] = (VIO_Real) VIO_ROUND( voxel[axis_index] + (VIO_Real) delta );

        if( voxel[axis_index] < 0.0 )
            voxel[axis_index] = 0.0;
        else if( voxel[axis_index] > (VIO_Real) sizes[axis_index]-1.0 )
            voxel[axis_index] = (VIO_Real) sizes[axis_index] - 1.0;

        if( set_current_voxel( slice_window, volume_index, voxel ))
        {
            if( update_cursor_from_voxel( slice_window ) )
            {
                set_update_required( get_three_d_window(slice_window),
                                     get_cursor_bitplanes() );
            }
        }
    }
}

/**
 * \brief Command to move one positive step along the slice dimension.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(move_slice_plus)
{
    change_current_slice_by_delta( display, Slice_change_step );
    return( VIO_OK );
}

DEF_MENU_UPDATE(move_slice_plus )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to move ten positive steps along the slice dimension.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(fast_forward_slice)
{
    change_current_slice_by_delta( display, Slice_change_step * Slice_change_fast );
    return( VIO_OK );
}

DEF_MENU_UPDATE(fast_forward_slice)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to move one negative step along the slice dimension.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(move_slice_minus)
{
    change_current_slice_by_delta( display, -Slice_change_step );
    return( VIO_OK );
}

DEF_MENU_UPDATE(move_slice_minus )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to move ten negative steps along the slice dimension.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(fast_rewind_slice)
{
    change_current_slice_by_delta( display, -Slice_change_step * Slice_change_fast );
    return( VIO_OK );
}

DEF_MENU_UPDATE(fast_rewind_slice)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to toggle the visibility of a slice view.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(toggle_slice_visibility)
{
    int              view_index, volume_index;
    display_struct   *slice_window;
    int              n_volumes;

    if( get_slice_window( display, &slice_window ) &&
        ( n_volumes = get_n_volumes( slice_window ) ) > 0 &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) )
    {
        if( get_slice_visibility( slice_window, -1, view_index ) )
        {
            /* If any volume is visible in this slace, make them all
             * invisible.
             */
            for_less( volume_index, 0, n_volumes )
            {
                set_slice_visibility( slice_window, volume_index,
                                      view_index, FALSE );
            }
        }
        else
        {
            /* Show the volumes that are visible in the other slice views.
             */
            int n_visible = 0;
            for_less( volume_index, 0, n_volumes )
            {
                if( get_volume_visibility( slice_window, volume_index ) )
                {
                    set_slice_visibility( slice_window, volume_index, 
                                          view_index, TRUE );

                    n_visible++;
                }
            }
            /* If _no_ volumes are currently visible in any view, then
             *  make them all visible in this view.
             */
            if (n_visible == 0)
            {
                for_less( volume_index, 0, n_volumes )
                {
                    set_slice_visibility( slice_window, volume_index, 
                                          view_index, TRUE );

                }
            }
        }
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE(toggle_slice_visibility )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Toggles the visibility of the oblique cross-section plane in
 * the 3D view window.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */

DEF_MENU_FUNCTION(toggle_cross_section_visibility)
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        set_volume_cross_section_visibility( display,
                                !get_volume_cross_section_visibility(display) );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(toggle_cross_section_visibility )
{
    return( slice_window_exists(display) );
}

/**
 * \brief Resets the translation and zoom of the slice view under the
 * mouse cursor, if any.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(reset_current_slice_view)
{
    int              view_index;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) )
    {
        reset_slice_view( slice_window, view_index );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(reset_current_slice_view )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * Helper function to copy the colours from the slice view to the current
 * 3D object.
 * \param display A pointer to a top-level window.
 * \param labels_only If TRUE, only the colours of the labels will be copied
 * to the object. If FALSE, the composite of all visible volumes and labels
 * will be copied.
 */
static void
copy_colours_to_object( display_struct *display, VIO_BOOL labels_only )
{
    object_struct           *object, *current_object;
    VIO_Volume              volume;
    object_traverse_struct  object_traverse;

    if( get_current_object( display, &current_object ) &&
        get_slice_window_volume( display, &volume ) )
    {
        initialize_object_traverse( &object_traverse, FALSE, 1,
                                    &current_object );

        while( get_next_object_traverse( &object_traverse, &object ) )
        {
            colour_code_an_object( display, object, labels_only );
        }
        set_update_required( display, NORMAL_PLANES );
    }
}

/**
 * \brief Colourize the currently selected 3D object using the label colours
 * only.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION( label_objects )
{
    copy_colours_to_object( display, TRUE );
    return( VIO_OK );
}

DEF_MENU_UPDATE( label_objects )
{
    return( get_n_volumes( display ) > 0 && current_object_exists( display ) );
}

/**
 * \brief Colourize the currently selected 3D object using the label and
 * volume colours together.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION( colour_code_objects )
{
    copy_colours_to_object( display, FALSE );
    return( VIO_OK );
}

DEF_MENU_UPDATE( colour_code_objects )
{
    return( get_n_volumes( display ) > 0 && current_object_exists( display ) );
}

/**
 * This function does several interesting things:
 * 1. If scale_slice_flag is FALSE and the mouse cursor is over one of the
 * three perpendicular slice views, it creates a 3D colourized quadmesh of the
 * current slice and displays this quadmesh in the 3D window. This gives you
 * a single slice that can be superimposed on other 3D objects.
 * 2. If slice_scale_flag is TRUE and the mouse cursor is over one of the
 * three perpendicular slice views, it creates a 3D monochrome quadmesh whose
 * height along the perpendicular axis reflects the intensity of the voxel
 * at each point.
 * 3. If the mouse cursor is over the oblique slice, it just creates a
 * monochrome polygon. Not sure what the point of this is!!
 * \param display A pointer to a top-level window.
 * \param scale_slice_flag TRUE if the quadmesh height should be proportional
 * to the intensity of the slice.
 */
static void
create_scaled_slice( display_struct *display, VIO_BOOL scale_slice_flag )
{
    display_struct   *slice_window;
    int              x_index, y_index, axis_index, view_index;
    VIO_Real         current_voxel[VIO_MAX_DIMENSIONS];
    VIO_Real         perp_axis[VIO_MAX_DIMENSIONS];
    VIO_Real         value, min_value, xw, yw, zw;
    VIO_Real         scaling;
    VIO_Vector       normal;
    object_struct    *object;
    quadmesh_struct  *quadmesh;
    VIO_Point        point;
    int              m, n, i, j;
    int              volume_index;
    VIO_Volume       volume;

    if( !get_slice_window( display, &slice_window ) ||
        !get_n_volumes(slice_window) > 0 ||
        !get_slice_view_index_under_mouse( slice_window, &view_index ))
    {
        return;                 /* give up! */
    }

    volume_index = get_current_volume_index( slice_window );
    volume = get_volume( display );

    if (slice_has_ortho_axes( slice_window, volume_index, view_index,
                              &x_index, &y_index, &axis_index ) )

    {
        if( scale_slice_flag )
        {
            if (get_user_input("Enter scaling: ", "r", &scaling) != VIO_OK)
                return;
        }

        get_current_voxel( slice_window, volume_index, current_voxel );
        object = create_3d_slice_quadmesh( volume, axis_index,
                                           current_voxel[axis_index] );

        if( scale_slice_flag )
        {
            quadmesh = get_quadmesh_ptr( object );
            m = quadmesh->m;
            n = quadmesh->n;
            min_value = get_volume_real_min( volume );
            colour_code_an_object( display, object, FALSE );
            for_less( i, 0, m )
            {
                for_less( j, 0, n )
                {
                    get_quadmesh_point( quadmesh, i, j, &point );
                    evaluate_volume_in_world( volume,
                                              Point_x(point),
                                              Point_y(point),
                                              Point_z(point), 0, FALSE,
                                              min_value, &value,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL, NULL, NULL, NULL );
                    Point_coord(point, axis_index) += (scaling *
                                                       (value - min_value));
                    set_quadmesh_point( quadmesh, i, j, &point, NULL );
                }
            }
            compute_quadmesh_normals( quadmesh );
        }
        else
        {
            colour_code_an_object( display, object, FALSE );
        }

        add_object_to_current_model( display, object );
    }
    else
    {
        object = create_object( POLYGONS );

        get_slice_perp_axis( slice_window, volume_index,
                             view_index, perp_axis );
        convert_voxel_vector_to_world( volume, perp_axis, &xw, &yw, &zw );
        fill_Vector( normal, xw, yw, zw );
        NORMALIZE_VECTOR( normal, normal );

        get_cursor_origin( display, &point );
        create_slice_3d( volume, &point, &normal, get_polygons_ptr( object ) );

        add_object_to_current_model( display, object );
    }

    show_three_d_window( get_three_d_window( display ),
                         get_display_by_type( MARKER_WINDOW ) );
}

/**
 * \brief Create a flat quadmesh colourized as the current slice, to be
 * displayed in the 3D window.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(create_3d_slice)
{
    create_scaled_slice( display, FALSE );
    return( VIO_OK );
}

DEF_MENU_UPDATE(create_3d_slice)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * Command to create a 3D object that is a quadmesh where the height is
 * proportional to the intensity of the image.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(create_3d_slice_profile)
{
    create_scaled_slice( display, TRUE );
    return( VIO_OK );
}

DEF_MENU_UPDATE(create_3d_slice_profile)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to resample a volume to a new size.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(resample_slice_window_volume)
{
    int              sizes[VIO_MAX_DIMENSIONS];
    int              new_nx, new_ny, new_nz;
    char             label[VIO_EXTREMELY_LARGE_STRING_SIZE];
    display_struct   *slice_window;
    VIO_Volume           volume, resampled_volume;

    if( get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        char prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

        get_volume_sizes( volume, sizes );

        sprintf(prompt, "The original volume is %d by %d by %d.\n"
                "Enter desired resampled size: ",
                sizes[VIO_X], sizes[VIO_Y], sizes[VIO_Z] );
        if (get_user_input( prompt, "ddd", &new_nx, &new_ny, &new_nz) == VIO_OK &&
            (new_nx > 0 || new_ny > 0 || new_nz > 0) )
        {
            resampled_volume = smooth_resample_volume(
                                        volume, new_nx, new_ny, new_nz );

            (void) sprintf( label, "Subsampled to %d,%d,%d: %s",
                            new_nx, new_ny, new_nz,
                            get_volume_filename(slice_window,
                                   get_current_volume_index(slice_window) ) );

            add_slice_window_volume( slice_window, "", label, resampled_volume );
        }
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(resample_slice_window_volume)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to apply a box filter to the current volume.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(box_filter_slice_window_volume)
{
    char             ch;
    VIO_Real             x_width, y_width, z_width;
    VIO_Real             separations[VIO_MAX_DIMENSIONS];
    char             label[VIO_EXTREMELY_LARGE_STRING_SIZE];
    display_struct   *slice_window;
    VIO_Volume           volume, resampled_volume;

    if( get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        get_volume_separations( volume, separations );

        if (get_user_input( "Enter box filter x_width, y_width, z_width, v/w: " ,
                            "rrrc", &x_width, &y_width, &z_width, &ch) == VIO_OK &&

            (ch == 'w' ||
             x_width > 1.0 || y_width > 1.0 || z_width > 1.0) )
        {
            (void) sprintf( label, "Box Filtered at %g,%g,%g,%c: %s",
                            x_width, y_width, z_width, ch,
                            get_volume_filename(slice_window,
                                   get_current_volume_index(slice_window) ) );

            if( ch == 'w' )
            {
                x_width /= VIO_FABS( separations[VIO_X] );
                y_width /= VIO_FABS( separations[VIO_Y] );
                z_width /= VIO_FABS( separations[VIO_Z] );
            }

            resampled_volume = create_box_filtered_volume( volume,
                                        NC_BYTE, FALSE, 0.0, 0.0,
                                        x_width, y_width, z_width );

            add_slice_window_volume( slice_window, "", label, resampled_volume );
        }
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(box_filter_slice_window_volume)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to select the angle of the oblique slice.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(pick_slice_angle_point)
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        start_picking_slice_angle( slice_window );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(pick_slice_angle_point)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to start the rotation of the oblique slice plane.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION( rotate_slice_axes )
{
    initialize_rotating_slice( display );

    return( VIO_OK );
}

DEF_MENU_UPDATE(rotate_slice_axes )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to reset the position of the crop box.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(reset_slice_crop)
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        reset_crop_box_position( slice_window );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(reset_slice_crop)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to toggle the visibility of the crop box.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(toggle_slice_crop_visibility)
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        toggle_slice_crop_box_visibility( slice_window );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(toggle_slice_crop_visibility)
{
    display_struct   *slice_window;
    VIO_BOOL          visible;

    if( get_slice_window( display, &slice_window ) )
        visible = slice_window->slice.crop.crop_visible;
    else
        visible = FALSE;

    set_menu_text_on_off( menu_window, menu_entry, visible );

    return( slice_window_exists(display) );
}

/**
 * \brief Command to select an edge of the crop box.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(pick_crop_box_edge)
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        start_picking_crop_box( slice_window );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(pick_crop_box_edge)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to set the name of the file to be cropped.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(set_crop_box_filename)
{
    display_struct   *slice_window;
    VIO_STR           filename;

    if( get_slice_window( display, &slice_window ) )
    {
      if (get_user_file( "Enter crop filename: ", FALSE, NULL,
                         &filename ) == VIO_OK)
      {
        set_crop_filename( slice_window, filename );
        delete_string( filename );
      }
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(set_crop_box_filename)
{
    return( TRUE );
}

/**
 * \brief Command to crop and then load the cropped volume.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(load_cropped_volume)
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        crop_and_load_volume( slice_window );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(load_cropped_volume)
{
    return( slice_window_exists(display) );
}

/**
 * \brief Command to crop the volume to a given file.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(crop_volume_to_file)
{
    VIO_STR           filename;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
      if (get_user_file( "Enter filename to create: ", TRUE, NULL,
                         &filename ) == VIO_OK)
        {
            if( create_cropped_volume_to_file( slice_window, filename ) == VIO_OK )
                print( "Created %s.\n", filename );

            delete_string( filename );
        }
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(crop_volume_to_file)
{
    return( slice_window_exists(display) );
}

/**
 * Helper function to create the optional histogram that appears alongside
 * the colour bar in the slice window.
 *
 * \param display A pointer to a top-level window.
 * \param labeled True if the histogram should consider only labeled voxels.
 */
static  void  do_histogram(
    display_struct   *display,
    VIO_BOOL          labeled )
{
    int              x_index, y_index, view_index, axis_index;
    VIO_Real             voxel[VIO_MAX_DIMENSIONS], slice;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        if( get_slice_view_index_under_mouse( slice_window, &view_index ) &&
            slice_has_ortho_axes( slice_window,
                       get_current_volume_index( slice_window ), view_index,
                       &x_index, &y_index, &axis_index ) )
        {
            get_current_voxel( slice_window,
                               get_current_volume_index(slice_window), voxel );
            slice = voxel[axis_index];
        }
        else
        {
            axis_index = -1;
            slice = 0.0;
        }

        compute_histogram( slice_window, axis_index, VIO_ROUND(slice), labeled );

        set_slice_viewport_update( slice_window, COLOUR_BAR_MODEL );
    }
}

/**
 * \brief Command to recalculate the voxel intensity histogram.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(redo_histogram)
{
    do_histogram( display, FALSE );

    return( VIO_OK );
}

DEF_MENU_UPDATE(redo_histogram)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to recalculate the intensity histogram for labeled
 * voxels only.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(redo_histogram_labeled)
{
    do_histogram( display, TRUE );

    return( VIO_OK );
}

DEF_MENU_UPDATE(redo_histogram_labeled)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to print the current cursor position in both voxel and world
 * coordinates.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(print_voxel_origin)
{
    VIO_Real             voxel[VIO_MAX_DIMENSIONS], xw, yw, zw;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        get_current_voxel( slice_window,
                      get_current_volume_index(slice_window), voxel );

        convert_voxel_to_world( get_volume(slice_window), voxel,
                                &xw, &yw, &zw );

        print( "Current voxel origin: %g %g %g\n", voxel[0], voxel[1],voxel[2]);
        print( "Current world origin: %g %g %g\n", xw, yw, zw );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(print_voxel_origin)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to print the current plane orientation for the slice
 * view under the mouse.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(print_slice_plane)
{
    display_struct   *slice_window;
    VIO_Vector           normal;
    int              view_index;
    VIO_Real             perp_axis[VIO_MAX_DIMENSIONS], xw, yw, zw;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) )
    {
        get_slice_perp_axis( slice_window,
                get_current_volume_index(slice_window), view_index, perp_axis );
        convert_voxel_vector_to_world( get_volume(slice_window),
                                       perp_axis, &xw, &yw, &zw );

        fill_Vector( normal, perp_axis[0], perp_axis[1], perp_axis[2] );
        NORMALIZE_VECTOR( normal, normal );

        print( "View: %2d    Voxel Perpendicular: %g %g %g\n",
               view_index,
               Vector_x(normal), Vector_y(normal), Vector_z(normal) );

        fill_Vector( normal, xw, yw, zw );
        NORMALIZE_VECTOR( normal, normal );

        print( "            World Perpendicular: %g %g %g\n",
               Vector_x(normal), Vector_y(normal), Vector_z(normal) );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(print_slice_plane)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to allow the user to enter a specific position for
 * the cursor.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(type_in_voxel_origin)
{
    char             type;
    VIO_Real         voxel[VIO_MAX_DIMENSIONS], xw, yw, zw;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
      if (get_user_input("Enter x y z world coordinate and v|w: ",
                         "rrrc", &xw, &yw, &zw, &type) == VIO_OK)
      {
            if( type == 'w' )
            {
                convert_world_to_voxel( get_volume(slice_window), xw, yw, zw,
                                        voxel );
            }
            else
            {
                voxel[0] = xw;
                voxel[1] = yw;
                voxel[2] = zw;
            }

            if( set_current_voxel( slice_window,
                        get_current_volume_index(slice_window), voxel ) )
            {
                if( update_cursor_from_voxel( slice_window ) )
                    set_update_required( display, NORMAL_PLANES );
            }
        }
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(type_in_voxel_origin)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to allow the user to enter a specific plane orientation for
 * the slice view.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(type_in_slice_plane)
{
    int              view_index;
    VIO_Real             perp_axis[VIO_MAX_DIMENSIONS], xw, yw, zw;
    char             type;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) )
    {
        char prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];
        sprintf(prompt,
                "View %d: enter x y z plane normal in world coordinate\n"
                "and v or w for voxel or world: ", view_index );
        if (get_user_input(prompt, "rrrc", &xw, &yw, &zw, &type) == VIO_OK)
        {
            if( type == 'w' )
            {
                convert_world_vector_to_voxel( get_volume(slice_window),
                                               xw, yw, zw, perp_axis );
            }
            else
            {
                perp_axis[0] = xw;
                perp_axis[1] = yw;
                perp_axis[2] = zw;
            }

            set_slice_plane_perp_axis( slice_window,
                                       get_current_volume_index(slice_window),
                                       view_index, perp_axis);
            reset_slice_view( slice_window, view_index );
        }
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(type_in_slice_plane)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to toggle the projection of the oblique plane in the
 * other three slice views.
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(toggle_slice_cross_section_visibility)
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        slice_window->slice.cross_section_visibility =
                             !slice_window->slice.cross_section_visibility;
        set_slice_cross_section_update( slice_window, -1 );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(toggle_slice_cross_section_visibility)
{
    return( slice_window_exists(display) );
}

/**
 * \brief Command to set which slice view shows the oblique plane.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(set_current_arbitrary_view)
{
    int              view_index;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) )
    {
        print( "Current arbitrary view is now: %d\n", view_index );

        slice_window->slice.cross_section_index = view_index;
        set_slice_cross_section_update( slice_window, -1 );
        rebuild_volume_cross_section( slice_window );
        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(set_current_arbitrary_view)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to toggle the state of the slice anchor mode.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(toggle_slice_anchor)
{
    int              c, view_index;
    VIO_Vector           axis;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        if( slice_window->slice.cross_section_vector_present ||
            !get_slice_view_index_under_mouse( slice_window, &view_index ) ||
            view_index == get_arbitrary_view_index( slice_window ) )
        {
            slice_window->slice.cross_section_vector_present = FALSE;
            print( "Slice anchor turned off\n" );
        }
        else
        {
            slice_window->slice.cross_section_vector_present = TRUE;
            print( "Setting slice anchor to view index: %d\n", view_index );

            get_slice_cross_section_direction( slice_window, view_index,
                           get_arbitrary_view_index( slice_window ),
                           &axis );

            for_less( c, 0, VIO_N_DIMENSIONS )
                slice_window->slice.cross_section_vector[c] =
                                                   (VIO_Real) Vector_coord(axis,c);
        }
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(toggle_slice_anchor)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to remove the current volume from the display.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(delete_current_volume)
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        delete_slice_window_volume( slice_window,
                                    get_current_volume_index(slice_window) );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(delete_current_volume)
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to set the current volume to be the "next" volume in
 * the list.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(toggle_current_volume)
{
    display_struct   *slice_window;
    int              n_volumes;

    if( get_slice_window( display, &slice_window ) &&
        ( n_volumes = get_n_volumes( slice_window ) ) > 1 )
    {
        int current_index = get_current_volume_index( slice_window );
        current_index = (current_index + 1) % n_volumes;
        set_current_volume_index( slice_window, current_index );
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE(toggle_current_volume)
{
    int              current_index;

    if( get_n_volumes(display) > 1 )
        current_index = get_current_volume_index( display ) + 1;
    else
        current_index = 1;

    set_menu_text_int( menu_window, menu_entry, current_index );

    return( get_n_volumes(display) > 1 );
}

/**
 * \brief Command to set the current volume to be the "previous" volume in
 * the list.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(prev_current_volume)
{
    display_struct   *slice_window;
    int              n_volumes;

    if( get_slice_window( display, &slice_window ) &&
        ( n_volumes = get_n_volumes(slice_window) ) > 1 )
    {
        int current_index = get_current_volume_index( slice_window );
        current_index = (current_index - 1 + n_volumes) % n_volumes;
        set_current_volume_index( slice_window, current_index );
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE(prev_current_volume)
{
    return( get_n_volumes(display) > 1 );
}

/**
 * \brief Command to set the opacity (alpha) of the current volume.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(set_current_volume_opacity)
{
    int              current;
    VIO_Real         opacity;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        current = get_current_volume_index( slice_window );

        if( current >= 0 )
        {
            if (get_user_input("Enter volume opacity ( 0.0 <= o <= 1.0 ): ",
                               "r", &opacity) == VIO_OK && opacity >= 0.0)
            {
                set_volume_opacity( slice_window, current, opacity );
            }
        }
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(set_current_volume_opacity)
{
    VIO_Real         opacity;
    int              current_index;
    display_struct   *slice_window;

    current_index = get_current_volume_index( display );

    if( current_index >= 0 && get_slice_window( display, &slice_window ) )
        opacity = slice_window->slice.volumes[current_index].opacity;
    else
        opacity = 1.0;

    set_menu_text_real( menu_window, menu_entry, opacity );

    return( get_n_volumes(display) > 1 );
}

/**
 * Helper function to return the first visible volume, if any.
 *
 * \param display A pointer to a top-level window.
 * \returns The zero-based index of the _first_ visible volume.
 */
static int
get_current_visible_volume( display_struct *display )
{
    int              current_visible, volume_index, view;
    display_struct   *slice_window;
    int              n_volumes;

    current_visible = -1;
    if( get_slice_window( display, &slice_window ) &&
        ( n_volumes = get_n_volumes( slice_window ) ) > 0 )
    {
        for_less( view, 0, N_SLICE_VIEWS )
        {
            for_less( volume_index, 0, n_volumes )
            {
                if( get_slice_visibility( slice_window, volume_index, view ) )
                {
                    current_visible = volume_index;
                    break;
                }
            }
            if( current_visible >= 0 )
                break;
        }

        if( volume_index < n_volumes )
            current_visible = volume_index;
    }

    return( current_visible );
}

/**
 * Change which volume is visible, by advancing the current volume by
 * "increment" and making that volume visible.
 * \param display A pointer to a top-level window.
 * \param increment The amount by which to increment the current visible
 * volume.
 */
static void
change_visible_volume( display_struct *display, int increment )
{
    int              current, view, volume_index;
    display_struct   *slice_window;
    VIO_BOOL         all_invisible;
    int              n_volumes;

    if( get_slice_window( display, &slice_window ) &&
        ( n_volumes = get_n_volumes(slice_window) ) > 0 )
    {
        current = get_current_visible_volume( slice_window );
        current = (current + increment + n_volumes) % n_volumes;

        for_less( view, 0, N_SLICE_VIEWS )
        {
            all_invisible = TRUE;
            for_less( volume_index, 0, n_volumes )
            {
                if( get_slice_visibility( slice_window, volume_index, view ) )
                    all_invisible = FALSE;
                set_slice_visibility( slice_window, volume_index, view, FALSE );
            }

            if( !all_invisible || n_volumes == 1 )
                set_slice_visibility( slice_window, current, view, TRUE );
        }

        set_current_volume_index( slice_window, current );
    }
}

/**
 * \brief Command to advance to the next volume and make it visible.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(next_volume_visible)
{
    change_visible_volume( display, 1 );

    return( VIO_OK );
}

DEF_MENU_UPDATE(next_volume_visible)
{
    set_menu_text_int( menu_window, menu_entry,
                       get_current_visible_volume(display) + 1 );

    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to back up to the previous volume and make it visible.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(prev_volume_visible)
{
    change_visible_volume( display, -1 );

    return( VIO_OK );
}

DEF_MENU_UPDATE(prev_volume_visible)
{
    set_menu_text_int( menu_window, menu_entry,
                       get_current_visible_volume(display) + 1 );

    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to cycle through the supported interpolation modes.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(toggle_slice_interpolation)
{
    int              continuity;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        continuity = slice_window->slice.degrees_continuity;
        if( continuity == -1 )
            continuity = 0;     /* 0 -> trilinear */
        else if( continuity == 0 )
            continuity = 2;     /* 2 -> tricubic */
        else if( continuity == 2 )
            continuity = -1;    /* -1 -> nearest neighbour */

        slice_window->slice.degrees_continuity = continuity;

        set_slice_window_all_update( slice_window, -1, UPDATE_SLICE );
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE(toggle_slice_interpolation )
{
    int              continuity;
    display_struct   *slice_window;
    VIO_BOOL          active;
    VIO_STR           name;

    active = get_slice_window( display, &slice_window );

    if( active )
        continuity = slice_window->slice.degrees_continuity;
    else
        continuity = Initial_slice_continuity;

    switch( continuity )
    {
    case 0:
        name = "trilinear";
        break;
    case 2:
        name = "tricubic";
        break;
    default:
        name = "near neigh";
        break;
    }
    set_menu_text_string( menu_window, menu_entry, name );
    return( active );
}

/**
 * \brief Command to save an image of the slice view under the mouse.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION( save_slice_image )
{
    display_struct    *slice_window;
    VIO_Status        status;
    int               view_index, x_min, x_max, y_min, y_max;
    VIO_STR           filename;

    status = VIO_OK;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) )
    {
        if( get_user_file("Enter filename: " , TRUE, NULL, &filename) == VIO_OK)
        {
            get_slice_viewport( slice_window, view_index,
                                &x_min, &x_max, &y_min, &y_max );

            status = save_window_to_file( slice_window, filename,
                                          x_min, x_max, y_min, y_max );

            print( "Done saving slice image to %s.\n", filename );

            delete_string( filename );
        }
    }

    return( status );
}

DEF_MENU_UPDATE(save_slice_image )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Command to save an image of the entire slice view.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION( save_slice_window )
{
    display_struct    *slice_window;
    VIO_Status        status;
    int               x_size, y_size;
    VIO_STR           filename;

    status = VIO_OK;

    if( get_slice_window( display, &slice_window ) )
    {
        if (get_user_file( "Enter filename: " , TRUE, NULL,
                           &filename) == VIO_OK)
        {
            G_get_window_size( slice_window->window, &x_size, &y_size );

            status = save_window_to_file( slice_window, filename,
                                          0, x_size-1, 0, y_size-1 );

            print( "Done saving slice window to %s.\n", filename );

            delete_string( filename );
        }
    }

    return( status );
}

DEF_MENU_UPDATE(save_slice_window )
{
    return( get_n_volumes(display) > 0 );
}


/**
 * \brief Command to toggle incremental slice update.
 *
 * Incremental slice update allows for partial redrawing of the slice
 * window when, for example, the interpolation takes a long time (perhaps
 * because tricubic interpolation is selected, e.g.). It is not mostly
 * obsolete and may be removed.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(toggle_incremental_slice_update)
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        slice_window->slice.incremental_update_allowed =
                       !slice_window->slice.incremental_update_allowed;
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(toggle_incremental_slice_update)
{
    display_struct   *slice_window;
    VIO_BOOL          state;

    if( get_slice_window( display, &slice_window ) )
        state = slice_window->slice.incremental_update_allowed;
    else
        state = Initial_incremental_update;

    set_menu_text_on_off( menu_window, menu_entry, state );

    return( slice_window_exists(display) );
}

/**
 * \brief Command to toggle the visibility of the slice view crosshair cursor.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(toggle_cursor_visibility)
{
    int              view;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        slice_window->slice.cursor_visibility =
                                       !slice_window->slice.cursor_visibility;
        for_less( view, 0, N_SLICE_VIEWS )
            slice_window->slice.slice_views[view].update_cursor_flag = TRUE;
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(toggle_cursor_visibility )
{
    VIO_BOOL          state, visible;
    display_struct   *slice_window;

    state = get_slice_window(display,&slice_window);

    if( state )
        visible = slice_window->slice.cursor_visibility;
    else
        visible = TRUE;

    set_menu_text_on_off( menu_window, menu_entry, visible );
    return( state );
}

/**
 * \brief Command to reassign a loaded volume as labels.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(insert_volume_as_labels)
{
    int              src_index, rnd;
    char             filename[VIO_EXTREMELY_LARGE_STRING_SIZE];
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
      if (get_user_input( "Enter the index of the volume which represents the labels: ", "d", &src_index ) != VIO_OK ||
          src_index < 1 ||
          src_index > get_n_volumes(display) )
        {
            print_error( "Index out of range, operation cancelled.\n" );
            return( VIO_ERROR );
        }

        --src_index;

        rnd = get_random_int( 1000000000 );
        (void) sprintf( filename, "/tmp/tmp_labels_%d.mnc", rnd );

        if( output_volume( filename, NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                           get_nth_volume(slice_window,src_index),
                           "Label volume\n", NULL ) != VIO_OK )
            return( VIO_ERROR );

        input_label_volume_file( display, filename );

        remove_file( filename );
    }

    return( VIO_OK );
}

DEF_MENU_UPDATE(insert_volume_as_labels )
{
    return( get_n_volumes(display) >= 2 );
}

/**
 * \brief Command to terminate existing interactions and reset the UI.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(reset_interactions)
{
    terminate_any_interactions(display);
    if (display->window_type == THREE_D_WINDOW)
    {
        initialize_virtual_spaceball(display);
    }
    return VIO_OK;
}

DEF_MENU_UPDATE(reset_interactions)
{
    return TRUE;
}

/**
 * \brief Command to toggle the visibility of the slice rulers.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION(slice_rulers_toggle)
{
  display_struct *slice_window;

  if (get_slice_window( display, &slice_window ))
  {
    int view_index;
    for (view_index = 0; view_index < N_SLICE_VIEWS; view_index++)
    {
      VIO_BOOL state = get_slice_rulers_visibility( slice_window, view_index );
      set_slice_rulers_visibility( slice_window, view_index, !state );
    }
    /* This is a bit hacky, it relies on knowing that the rulers are
     * drawn when the update_cursor_flag is set.
     */
    set_slice_cursor_update( slice_window, -1 );
  }
  return VIO_OK;
}

DEF_MENU_UPDATE(slice_rulers_toggle)
{
    return TRUE;
}

/**
 * \brief Command to make all volumes visible (again).
 *
 * This will not change the visibility of any single slice view. So if the
 * sagittal view has been hidden, the volumes will appear only in the other
 * views.
 *
 * \param display A pointer to a top-level window.
 * \param menu_window A pointer to the menu window.
 * \param menu_entry A pointer to the menu entry for this command.
 * \returns VIO_OK for normal operation.
 */
DEF_MENU_FUNCTION( make_all_volumes_visible )
{
  display_struct *slice_window;
  int            view_index;
  int            volume_index;
  int            is_vis;
  int            n_volumes;

  if( get_slice_window( display, &slice_window ) &&
      ( n_volumes = get_n_volumes(slice_window) ) > 0 )
  {
    for_less( view_index, 0, N_SLICE_VIEWS )
    {
      is_vis = FALSE;

      /* See if any of the volumes are visible in this slice view.
       */
      for_less( volume_index, 0, n_volumes )
        if (get_slice_visibility( slice_window, volume_index, view_index ))
          is_vis = TRUE;

      /* Now make all of the volumes visible if any of them were visible.
       */
      for_less( volume_index, 0, n_volumes )
        set_slice_visibility( slice_window, volume_index, view_index, is_vis );
    }
  }
  return VIO_OK;
}

DEF_MENU_UPDATE( make_all_volumes_visible )
{
  display_struct *slice_window;

  return ( get_slice_window( display, &slice_window ) &&
           get_n_volumes(slice_window) > 0 );
}
