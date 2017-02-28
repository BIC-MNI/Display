/**
 * \file callbacks/segmenting.c
 * \brief Menu commands to control painting (segmenting) in the slice view.
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
#include <multidim_x.h>

static  void  set_slice_labels(
    display_struct     *display,
    int                label );
static  void   set_connected_labels(
    display_struct   *display,
    int              label,
    VIO_BOOL          use_threshold );

/* ARGSUSED */

/**
 * Menu command that toggles whether undo is enabled for painting
 * operations in the slice window. This is global to the slice
 * window, it applies to all loaded volumes.
 */
DEF_MENU_FUNCTION( toggle_undo_feature )
{
    display_struct   *slice_window;

    if ( get_slice_window( display, &slice_window))
    {
        slice_window->slice.toggle_undo_feature =
            !slice_window->slice.toggle_undo_feature;

        delete_slice_undo( slice_window, -1 );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE( toggle_undo_feature )
{
    VIO_BOOL          state, set;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );

    if( state )
        set = slice_window->slice.toggle_undo_feature;
    else
        set = Initial_undo_feature;

    set_menu_text_on_off( menu_window, menu_entry, set );

    return( state );
}


/**
 * Helper function to implement labeling or clearing a single
 * voxel, if the voxel is not already in the desired state.
 * \param slice_window A pointer to slice window's display_struct
 * \param volume_index The index of the selected volume.
 * \param voxel The voxel address to change.
 * \param value The new label value desired.
 */
static void
maybe_set_single_voxel(display_struct *slice_window,
                       int volume_index,
                       VIO_Real voxel[],
                       int value)
{
    int vprev;
    int int_voxel[VIO_N_DIMENSIONS];

    convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, int_voxel );
    vprev = get_voxel_label( slice_window, volume_index,
                             int_voxel[VIO_X],
                             int_voxel[VIO_Y],
                             int_voxel[VIO_Z]);
    if (value == vprev)
    {
        return;
    }

    undo_start(slice_window, volume_index);
    make_current_label_visible( slice_window, volume_index );

    set_voxel_label( slice_window, volume_index,
                     int_voxel[VIO_X], int_voxel[VIO_Y], int_voxel[VIO_Z],
                     value );

    undo_save( slice_window, volume_index, int_voxel, vprev );

    undo_finish( slice_window, volume_index );

    set_slice_window_all_update( slice_window, volume_index, UPDATE_LABELS );
}

/* ARGSUSED */

/**
 * Menu command that sets the single voxel under the mouse to the current
 * paint label.
 *
 * Can be undone.
 */
DEF_MENU_FUNCTION( label_voxel )
{
    display_struct *slice_window;
    int            view_index, volume_index;
    VIO_Real       voxel[VIO_MAX_DIMENSIONS];

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index, &view_index, voxel))
    {
        maybe_set_single_voxel( slice_window, volume_index, voxel,
                                get_current_paint_label( slice_window ) );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(label_voxel )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

/**
 * Menu command that sets the single voxel under the mouse to the current
 * erase label.
 *
 * Can be undone.
 */
DEF_MENU_FUNCTION( clear_voxel )
{
    display_struct *slice_window;
    int            view_index, volume_index;
    VIO_Real       voxel[VIO_MAX_DIMENSIONS];

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index, &view_index, voxel))
    {
        maybe_set_single_voxel( slice_window, volume_index, voxel,
                                get_current_erase_label( slice_window ) );
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(clear_voxel )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

/**
 * Menu command that clears all of the labels for a given volume. Must
 * pop the menu because it is generally called from a submenu, in order
 * to confirm the destructive operation.
 *
 * Cannot be undone!
 */
DEF_MENU_FUNCTION( reset_segmenting )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        int volume_index = get_current_volume_index( slice_window );
        clear_all_labels( slice_window );
        delete_slice_undo( slice_window, volume_index );
        set_slice_window_all_update( slice_window, volume_index,
                                     UPDATE_LABELS );

        /* FIXME: Use of these functions assumes that we know the menu
         * structure.  That's really pretty evil, as it breaks the
         * menu configurability.
         */
        pop_menu_one_level( get_display_by_type( MENU_WINDOW ) );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(reset_segmenting )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */
/**
 * Menu command to set the minimum and maximum thresholds used to
 * limit voxel painting operations.
 */
DEF_MENU_FUNCTION( set_segmenting_threshold )
{
    display_struct   *slice_window;
    VIO_Real         min, max;

    if( get_slice_window( display, &slice_window ) )
    {
        if (get_user_input( "Enter min and max threshold: ", "rr",
                            &min, &max) == VIO_OK)
        {
            slice_window->slice.segmenting.min_threshold = min;
            slice_window->slice.segmenting.max_threshold = max;
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_segmenting_threshold )
{
    return( slice_window_exists(display) );
}

/**
 * Opens the volume file of the given name and checks the
 * data type, voxel range, and real range. This is used to
 * set up the existing label volume to receive the new data.
 * \param filename The path to the volume file.
 * \param data_type A pointer to hold the file's voxel data type.
 * \param voxel_range A 2-element array to hold the file's voxel data
 * range. These are the values actually recorded in the voxels.
 * \param real_range A 2-element array to hold the file's real data
 * range. These are the scaled values that represent the "true" values
 * associated with a volume.
 */
VIO_Status get_type_and_range_of_volume( VIO_STR filename,
                                         VIO_Data_types *data_type,
                                         VIO_Real voxel_range[],
                                         VIO_Real real_range[])
{
    VIO_Volume volume;

    if( input_volume_header_only( filename, VIO_N_DIMENSIONS,
                                  NULL,
                                  &volume, NULL ) != VIO_OK )
    {
        return( VIO_ERROR );
    }

    if (data_type != NULL)
    {
        *data_type = get_volume_data_type( volume );
    }
    if (voxel_range != NULL)
    {
        get_volume_voxel_range( volume, &voxel_range[0], &voxel_range[1]);
    }
    if (real_range != NULL)
    {
        get_volume_real_range( volume, &real_range[0], &real_range[1]);
    }
    delete_volume( volume );
    return VIO_OK;
}

/**
 * Read voxels from a MINC file and incorporate the newly-read values
 * into the selected label volume.
 *
 * If the range of labels has increased, we may have to increase the
 * number of labels, and therefore widen the data type of
 * the label volume.
 *
 * This function is used to load labels from either the command line or
 * the menu.
 *
 * \param display The display_struct of the event window.
 * \param filename The path to the label file to read.
 * \returns VIO_OK if the operation is a success.
 */
VIO_Status
input_label_volume_file(display_struct *display,
                        VIO_STR        filename )
{
    int              range[2][VIO_N_DIMENSIONS];
    int              sizes[VIO_MAX_DIMENSIONS];
    VIO_Status       status;
    display_struct   *slice_window;
    int              volume_index;
    VIO_Data_types   data_type;
    VIO_Real         voxel_range[2];
    VIO_Real         real_range[2];
    int              i;

    status = VIO_OK;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        slice_window_struct *slice = &slice_window->slice;

        volume_index = get_current_volume_index( slice_window );
        get_type_and_range_of_volume( filename, &data_type, voxel_range,
                                      real_range );

        if (real_range[0] == 0 &&
            real_range[1] > get_num_labels( slice_window, volume_index ))
        {
            set_slice_window_number_labels( slice_window, volume_index,
                                            (int) VIO_ROUND(real_range[1]) + 1);
        }

        status = load_label_volume( filename, get_label_volume(slice_window) );

        if( status == VIO_OK )
        {
            replace_string( &slice->volumes[volume_index].labels_filename,
                            create_string(filename) );
        }

        delete_slice_undo( slice_window, volume_index );

        set_slice_window_all_update( slice_window, volume_index,
                                     UPDATE_LABELS );

        range[0][VIO_X] = 0;
        range[0][VIO_Y] = 0;
        range[0][VIO_Z] = 0;

        /* Get sizes through an intermediate variable, since
         * there can be more than three dimensions in the file
         * itself.
         */
        get_volume_sizes( get_volume(slice_window), sizes );
        for (i = 0; i < VIO_N_DIMENSIONS; i++)
        {
            range[1][i] = sizes[i];
        }
        tell_surface_extraction_range_of_labels_changed( display,
                                                         volume_index,
                                                         range );
    }

    return( status );
}

/* ARGSUSED */

/**
 * Menu command to load labels from a volumetric (e.g. MINC) file.
 */
DEF_MENU_FUNCTION(load_label_data)
{
    VIO_Status           status;
    VIO_STR           filename;

    status = VIO_OK;

    if( get_n_volumes(display) > 0 )
    {
        status = get_user_file( "Enter filename to load: ", FALSE, NULL,
                                &filename );
        if (status == VIO_OK)
        {
            status = input_label_volume_file( display, filename );

            delete_string( filename );

            print( "Done\n" );
        }
    }

    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(load_label_data )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * \brief Scan a volume and set the real and voxel ranges to their
 * true values.
 *
 * Normally the label volume is set to have the a voxel minimum of
 * zero, and a voxel maximum of the currently set largest permitted
 * label value (255 by default). However, when saving the volume, this
 * is awkward or even incorrect because label volumes might have only
 * a few label values set, or in the extreme case of a mask volume,
 * might just have two possible values.
 *
 * To work around this, we save the original settings for the ranges, then
 * scan the volume to determine the "true" minimum and maximum. We then
 * temporarily set the voxel and real ranges to the true scanned values.
 * We will restore it to normal after the save completes.
 *
 * \param label_volume The label volume to scan.
 * \param real_range Will receive the current real range of the volume.
 * \param voxel_range Will receive the current voxel range of the volume.
 */

static void
temporarily_fix_range( VIO_Volume label_volume,
                       VIO_Real real_range[],
                       VIO_Real voxel_range[] )
{
  VIO_Real voxel_min;
  VIO_Real voxel_max;

  get_volume_real_range( label_volume, &real_range[0], &real_range[1] );
  get_volume_voxel_range( label_volume, &voxel_range[0], &voxel_range[1] );

  multidim_scan_range( &label_volume->array, &voxel_min, &voxel_max );

  /* Temporarily reconfigure the volume for saving. */
  set_volume_voxel_range( label_volume, voxel_min, voxel_max );
  set_volume_real_range( label_volume, voxel_min, voxel_max );
}

/**
 * Menu command to save labels to a volumetric (e.g. MINC) file.
 */
DEF_MENU_FUNCTION(save_label_data)
{
    VIO_Status       status;
    VIO_STR          filename, backup_filename;
    display_struct   *slice_window;
    VIO_Real         crop_threshold;
    VIO_Real         old_real_range[2];
    VIO_Real         old_voxel_range[2];
    VIO_Volume       label_volume;

    status = VIO_OK;

    /* This command can only function if there is an active slice
     * window with a loaded volume.
     */
    if( !get_slice_window( display, &slice_window ) ||
        get_n_volumes(slice_window) <= 0 )
    {
        return VIO_ERROR;
    }

    /* Use the filename given on the command line via the
     * -output-label option, if any.
     */
    if( string_length(Output_label_filename) )
        filename = create_string(Output_label_filename);
    else
    {
        /* Prompt the user for a filename. */
        if ( get_user_file( "Enter filename to save: ", TRUE, NULL,
                            &filename) != VIO_OK )
        {
            return VIO_ERROR;
        }
    }

    if( !slice_window->slice.crop_labels_on_output_flag )
        crop_threshold = 0.0;
    else
        crop_threshold = Crop_label_volumes_threshold;

    label_volume = get_label_volume( slice_window );

    temporarily_fix_range( label_volume, old_real_range, old_voxel_range );

    status = make_backup_file( filename, &backup_filename );
    if( status == VIO_OK )
    {
        VIO_BOOL made_backup = backup_filename != NULL;
        if ( !made_backup )
        {
            /* If NOT replacing an existing file, THEN treat the
             * original volume as the template from which to copy
             * volume ordering, etc.
             */
            int volume_index = get_current_volume_index( slice_window );
            backup_filename = get_volume_filename( slice_window, volume_index );
        }

        status = save_label_volume( filename,
                                    backup_filename,
                                    label_volume,
                                    crop_threshold );
        if ( made_backup )
        {
            /* We made a backup, so clean it up now.
             */
            cleanup_backup_file( filename, backup_filename, status );

            delete_string( backup_filename );
        }
    }

    /* Restore the original voxel and real ranges.
     */
    set_volume_voxel_range( label_volume,
                            old_voxel_range[0], old_voxel_range[1] );
    set_volume_real_range( label_volume,
                           old_real_range[0], old_real_range[1] );

    if( status == VIO_OK )
        print( "Label saved to %s\n", filename );
    else
    {
        print( "\n" );
        print( "###############################################\n" );
        print( "#                                             #\n" );
        print( "#  Error:  Labels were NOT saved.             #\n" );
        print( "#                                             #\n" );
        print( "###############################################\n" );
        print( "\n" );
    }

    delete_string( filename );
    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(save_label_data )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * Read tag points from a file and incorporate the newly-read values
 * into the selected label volume.
 *
 * This function is used to load labels from either the command line or
 * the menu.
 *
 * \param display The display_struct of the event window.
 * \param filename The path to the landmark or tag label file to read.
 * \returns VIO_OK if the operation is a success.
 */

VIO_Status
input_tag_label_file(display_struct *display,
                     VIO_STR        filename )
{
    VIO_Status     status;
    VIO_BOOL       landmark_format;
    FILE           *file;
    display_struct *slice_window;
    VIO_Volume     volume;

    status = VIO_OK;

    if( get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        landmark_format = filename_extension_matches( filename,
                             get_default_landmark_file_suffix() );

        status = open_file_with_default_suffix( filename,
                            get_default_tag_file_suffix(),
                            READ_FILE, ASCII_FORMAT, &file );

        if( status == VIO_OK )
        {
            if( landmark_format )
                status = input_landmarks_as_labels( file, volume,
                                        get_label_volume(slice_window) );
            else
                status = input_tags_as_labels( file, volume,
                                        get_label_volume(slice_window) );
        }

        if( status == VIO_OK )
            status = close_file( file );

        delete_slice_undo( slice_window,
                           get_current_volume_index(slice_window) );

        set_slice_window_all_update( slice_window,
                 get_current_volume_index(slice_window), UPDATE_LABELS  );
    }

    return( status );
}

/* ARGSUSED */

/**
 * Menu command to load labels from a landmark or tag point file.
 */
DEF_MENU_FUNCTION( load_labels )
{
    VIO_STR         filename;

    if( get_n_volumes(display) > 0 )
    {
        if (get_user_file("Enter filename: ", FALSE, NULL, &filename) == VIO_OK)
        {
            (void) input_tag_label_file( display, filename );

            print( "Done loading.\n" );

            delete_string( filename );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(load_labels )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * Save volume labels as a tag point file.
 * \param display The display_struct of the 3D window.
 * \param slice_window The display_struct of the slice window.
 * \param desired_label The label to save. All non-zero labels will be saved if
 * this value is negative.
 */
static  void   save_labels_as_tags(
    display_struct  *display,
    display_struct  *slice_window,
    int             desired_label )
{
    VIO_Status     status;
    FILE           *file;
    VIO_STR        filename;

    status = get_user_file( "Enter filename to save: ", TRUE,
                            get_default_tag_file_suffix(), &filename);
    if (status != VIO_OK)
    {
        return;
    }

    status = open_file_with_default_suffix( filename,
                     get_default_tag_file_suffix(),
                     WRITE_FILE, ASCII_FORMAT, &file );

    if( status == VIO_OK )
        status = output_labels_as_tags( file,
                  get_volume(slice_window),
                  get_label_volume(slice_window),
                  desired_label,
                  display->three_d.default_marker_size,
                  display->three_d.default_marker_patient_id );

    if( status == VIO_OK )
        status = close_file( file );

    print( "Done saving.\n" );

    delete_string( filename );
}

/* ARGSUSED */

/**
 * Menu command to save the current volume labels as tags.
 */
DEF_MENU_FUNCTION( save_labels )
{
    display_struct *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        save_labels_as_tags( display, slice_window, -1 );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(save_labels )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

/**
 * Menu command to save all labels with the value of the current paint
 * label as tags.
 *
 * That is, this command generates and saves a list of tag points
 * corresponding to every voxel in the current volume that has the
 * currently selected painting label.
 */
DEF_MENU_FUNCTION( save_current_label )
{
    display_struct *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        int label = get_current_paint_label( display );
        if( label > 0 )
        {
            save_labels_as_tags( display, slice_window, label );
        }
        else
            print( "You first have to set the current label > 0.\n" );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(save_current_label )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

/**
 * Menu command to set the labels for every voxel in the current slice.
 *
 * Can be undone.
 */
DEF_MENU_FUNCTION(label_slice)
{
    set_slice_labels( display, get_current_paint_label(display) );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(label_slice )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */
/**
 * Menu command to erase all of the labels on the current slice.
 *
 * Can be undone.
 */
DEF_MENU_FUNCTION(clear_slice)
{
    set_slice_labels( display, get_current_erase_label(display) );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(clear_slice )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * Helper function for clear_slice() and label_slice().
 */
static  void  set_slice_labels(
    display_struct     *display,
    int                label )
{
    VIO_Real         voxel[VIO_MAX_DIMENSIONS];
    int              view_index, int_voxel[VIO_MAX_DIMENSIONS], volume_index;
    int              x_index, y_index, axis_index;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index, &view_index,
                               voxel ) &&
        slice_has_ortho_axes( slice_window, volume_index, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        undo_start( slice_window, volume_index );
        make_current_label_visible( slice_window, volume_index );

        convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, int_voxel );
        set_labels_on_slice( slice_window, volume_index,
                             axis_index, int_voxel[axis_index],
                             label );

        undo_finish( slice_window, volume_index );

        set_slice_window_all_update( slice_window, volume_index,
                                     UPDATE_LABELS );
    }
}

/* ARGSUSED */


/**
 * Menu command to erase all of the voxels connected to the current
 * voxel. This implements a flood fill which labels voxels up to some
 * boundary of previously labeled voxels. Respects any thresholds that
 * have been set.
 *
 * "Connected" voxels can be either 4-connected or 8-connected
 * depending on the program settings.
 *
 * Can be undone.
 */
DEF_MENU_FUNCTION(clear_connected)
{
    set_connected_labels( display, get_current_erase_label(display), TRUE );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(clear_connected )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

/**
 * Menu command to label all of the voxels connected to the current
 * voxel. This implements a flood fill which labels voxels up to some
 * boundary of previously labeled voxels. Respects any thresholds that
 * have been set.
 *
 * "Connected" voxels can be either 4-connected or 8-connected
 * depending on the program settings.
 *
 * Can be undone.
 */
DEF_MENU_FUNCTION(label_connected)
{
    set_connected_labels( display, get_current_paint_label(display), TRUE );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(label_connected )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

/**
 * Menu command to label all of the voxels connected to the current
 * voxel. This implements a flood fill which labels voxels up to some
 * boundary of previously labeled voxels. Ignores any thresholds that
 * have been set.
 *
 * "Connected" voxels can be either 4-connected or 8-connected
 * depending on the program settings.
 *
 * Can be undone.
 */
DEF_MENU_FUNCTION(label_connected_no_threshold)
{
    set_connected_labels( display, get_current_paint_label(display), FALSE );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(label_connected_no_threshold )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * Helper function for label_connected(), clear_connected(), and
 * label_connected_no_threshold().
 */
static  void   set_connected_labels(
    display_struct   *display,
    int              desired_label,
    VIO_BOOL         use_threshold )
{
    VIO_Real         voxel[VIO_MAX_DIMENSIONS], min_threshold, max_threshold;
    int              view_index, int_voxel[VIO_MAX_DIMENSIONS];
    int              label_under_mouse, volume_index;
    int              x_index, y_index, axis_index;
    int              min_label_threshold, max_label_threshold;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index,
                               &view_index, voxel ) &&
        slice_has_ortho_axes( slice_window, volume_index, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        undo_start(slice_window, volume_index);
        make_current_label_visible( slice_window, volume_index );

        if( use_threshold )
        {
            min_threshold = slice_window->slice.segmenting.min_threshold;
            max_threshold = slice_window->slice.segmenting.max_threshold;
        }
        else
        {
            min_threshold = 1.0;
            max_threshold = 0.0;
        }

        convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, int_voxel );

        label_under_mouse = get_voxel_label( slice_window, volume_index,
                                             int_voxel[VIO_X],
                                             int_voxel[VIO_Y],
                                             int_voxel[VIO_Z] );

        min_label_threshold = label_under_mouse;
        max_label_threshold = label_under_mouse;

        set_connected_voxels_labels( slice_window, volume_index,
                          axis_index, int_voxel,
                          min_threshold, max_threshold,
                          min_label_threshold, max_label_threshold,
                          slice_window->slice.segmenting.connectivity,
                          desired_label );

        undo_finish(slice_window, volume_index);

        set_slice_window_all_update( slice_window, volume_index, UPDATE_LABELS);
    }
}

/** Structure used to pass parameters into the voxel setting callback. */
typedef struct
{
  /** A pointer to the slice window. */
  display_struct *slice_window;
  /** The index of the volume to modify. */
  int volume_index;
} callback_data;

/**
 * This callback is used to actually set the label voxel during 3D dilation
 * and fill operations. It therefore has to have the standard parameter
 * types.
 * \param volume The label volume to modify.
 * \param x The X voxel coordinate to set.
 * \param y The Y voxel coordinate to set.
 * \param z The Z voxel coordinate to set.
 * \param label The label value for this position.
 * \param data A pointer to an struct of type callback_data.
 */
static void
set_label_callback( VIO_Volume volume, int x, int y, int z,
                    int label, void *data)
{
  callback_data *ptr = (callback_data *) data;
  int voxel[VIO_MAX_DIMENSIONS];
  voxel[VIO_X] = x;
  voxel[VIO_Y] = y;
  voxel[VIO_Z] = z;
  voxel[VIO_T] = 0;
  voxel[VIO_V] = 0;
  set_voxel_label_with_undo( ptr->slice_window, ptr->volume_index, voxel,
                             label );
}

/**
 * Helper function for clear_label_connected_3d() and label_connected_3d().
 * \param display A pointer to a slice window.
 * \param is_erase True if we are clearing labels.
 */
static void
do_fill_connected_3d( display_struct *display, VIO_BOOL is_erase )
{
    VIO_Real         voxel[VIO_MAX_DIMENSIONS];
    int              range_changed[2][VIO_N_DIMENSIONS];
    int              view_index, int_voxel[VIO_MAX_DIMENSIONS];
    int              label_under_mouse, desired_label, volume_index;
    display_struct   *slice_window;
    callback_data    data;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index, &view_index, voxel))
    {
        convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, int_voxel );

        label_under_mouse = get_voxel_label( slice_window, volume_index,
                                             int_voxel[VIO_X],
                                             int_voxel[VIO_Y],
                                             int_voxel[VIO_Z] );
        if (is_erase)
          desired_label = get_current_erase_label( slice_window );
        else
          desired_label = get_current_paint_label( slice_window );

        print( "%s 3d from %d %d %d, label %d becomes %d\n",
               (is_erase) ? "Clear" : "Filling",
               int_voxel[VIO_X], int_voxel[VIO_Y], int_voxel[VIO_Z],
               label_under_mouse, desired_label );

        data.slice_window = slice_window;
        data.volume_index = volume_index;

        fill_connected_voxels_callback( get_nth_volume(slice_window,volume_index),
                                        get_nth_label_volume(slice_window,volume_index),
                                        slice_window->slice.segmenting.connectivity,
                                        int_voxel,
                                        label_under_mouse, label_under_mouse,
                                        desired_label,
                                        slice_window->slice.segmenting.min_threshold,
                                        slice_window->slice.segmenting.max_threshold,
                                        range_changed,
                                        set_label_callback, &data );

        delete_slice_undo( slice_window, volume_index );

        print( "Done\n" );

        set_slice_window_all_update( slice_window, volume_index, UPDATE_LABELS);

        tell_surface_extraction_range_of_labels_changed( display, volume_index,
                                                         range_changed );
    }
}

/* ARGSUSED */

/**
 * Menu command to flood fill a region in three dimensions.
 *
 * Because of the possible size of the region, this cannot be undone.
 */

DEF_MENU_FUNCTION(label_connected_3d)
{
    do_fill_connected_3d( display, FALSE );
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(label_connected_3d )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * Helper function for 3D dilation or erosion.
 * \param display A pointer to a window structure.
 * \param do_erosion TRUE if erosion, FALSE if dilation.
 */
static void
dilation_or_erosion_command(display_struct *display, VIO_BOOL do_erosion )
{
  int              min_inside_label, max_inside_label;
  int              min_outside_label, max_outside_label;
  int              min_user_label, max_user_label;
  int              set_label;
  int              range_changed[2][VIO_N_DIMENSIONS];
  int              volume_index;
  display_struct   *slice_window;
  callback_data    data;

  if( !get_slice_window( display, &slice_window ) )
    return;

  volume_index = get_current_volume_index( slice_window );
  if ( volume_index < 0 )
    return;

  if (get_user_input( "Enter min and max outside label: ", "dd",
                      &min_user_label, &max_user_label ) != VIO_OK )
  {
    print( "Error in input label range.\n");
    return;
  }

  if (do_erosion)
  {
    set_label = get_current_erase_label( display );
    if( min_user_label <= max_user_label )
    {
      set_label = MAX( min_user_label, set_label );
    }
    min_inside_label = min_user_label;
    max_inside_label = max_user_label;
    min_outside_label = max_outside_label = get_current_paint_label( display );
  }
  else
  {
    set_label = get_current_paint_label( display );
    min_inside_label = max_inside_label = get_current_paint_label( display );
    min_outside_label = min_user_label;
    max_outside_label = max_user_label;
  }

  data.slice_window = slice_window;
  data.volume_index = volume_index;

  undo_start( slice_window, volume_index );
  dilate_voxels_callback( get_volume( display ),
                          get_label_volume( display ),
                          min_inside_label,
                          max_inside_label,
                          0.0, -1.0,  /* ignore inside voxel range. */
                          min_outside_label,
                          max_outside_label,
                          slice_window->slice.segmenting.min_threshold,
                          slice_window->slice.segmenting.max_threshold,
                          set_label,
                          slice_window->slice.segmenting.connectivity,
                          range_changed,
                          set_label_callback, &data );

  undo_finish( slice_window, volume_index );

  set_slice_window_all_update( slice_window, volume_index, UPDATE_LABELS );

  tell_surface_extraction_range_of_labels_changed( display, volume_index,
                                                   range_changed );
  print( "Done\n" );
}

/**
 * Menu command to dilate a labeled region in 3D.
 *
 * Because this can change a very large number of voxels, it cannot be undone.
 */
DEF_MENU_FUNCTION(dilate_labels)
{
  dilation_or_erosion_command( display, FALSE );
  return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(dilate_labels )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

/**
 * Menu command to erode a labeled region in 3D.
 *
 * Because this can change a very large number of voxels, it cannot be undone.
 */
DEF_MENU_FUNCTION(erode_labels)
{
  dilation_or_erosion_command( display, TRUE );
  return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(erode_labels )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

/**
 * Menu command to switch between 4-neighbour and 8-neighbor connectivity.
 * This affects the behavior of fill operations.
 */
DEF_MENU_FUNCTION(toggle_connectivity)
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window) )
    {
        if( slice_window->slice.segmenting.connectivity == FOUR_NEIGHBOURS )
            slice_window->slice.segmenting.connectivity = EIGHT_NEIGHBOURS;
        else
            slice_window->slice.segmenting.connectivity = FOUR_NEIGHBOURS;
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_connectivity )
{
    VIO_BOOL          state;
    display_struct   *slice_window;
    Neighbour_types  connectivity;
    int              n_neigh;

    state = get_slice_window( display, &slice_window );

    if( state )
        connectivity = slice_window->slice.segmenting.connectivity;
    else
        connectivity = (Neighbour_types) Segmenting_connectivity;

    switch( connectivity )
    {
    case  FOUR_NEIGHBOURS:
        n_neigh = 4;
        break;
    case  EIGHT_NEIGHBOURS:
        n_neigh = 8;
        break;
    }

    set_menu_text_int( menu_window, menu_entry, n_neigh );

    return( state );
}

/* ARGSUSED */

/**
 * Menu command to toggle the cropping of labels on output. If the
 * flag is set, label volumes will be cropped to remove as much
 * unlabeled background as possible, subject to the limit chosen in
 * Crop_label_volumes_threshold.
 */
DEF_MENU_FUNCTION(toggle_crop_labels_on_output)
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window) )
    {
        slice_window->slice.crop_labels_on_output_flag =
                            !slice_window->slice.crop_labels_on_output_flag;
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_crop_labels_on_output)
{
    VIO_BOOL          state, set;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );

    if( state )
        set = slice_window->slice.crop_labels_on_output_flag;
    else
        set = Initial_crop_labels_on_output;

    set_menu_text_on_off( menu_window, menu_entry, set );

    return( state );
}

/* ARGSUSED */

/**
 * Flood erase a region in three dimensions.
 *
 * Because of the possible size of the region, this cannot be undone.
 */
DEF_MENU_FUNCTION(clear_label_connected_3d)
{
    do_fill_connected_3d( display, TRUE );
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(clear_label_connected_3d )
{
    return( get_n_volumes(display) > 0 );
}

/**
 * Helper function to copy the current brush state into the selected
 * brush structure.
 */
static void
save_brush( display_struct *slice_window, int index )
{
    brush_struct *cur_br = &slice_window->slice.segmenting.brush[index];
    /* Save the current values in the current brush. */
    cur_br->radius[VIO_X] = slice_window->slice.x_brush_radius;
    cur_br->radius[VIO_Y] = slice_window->slice.y_brush_radius;
    cur_br->radius[VIO_Z] = slice_window->slice.z_brush_radius;
}

/**
 * Helper function to load the current brush state from the selected
 * brush structure.
 */
static void
load_brush( display_struct *slice_window, int index )
{
    brush_struct *cur_br = &slice_window->slice.segmenting.brush[index];
    /* Save the current values in the current brush. */
    slice_window->slice.x_brush_radius = cur_br->radius[VIO_X];
    slice_window->slice.y_brush_radius = cur_br->radius[VIO_Y];
    slice_window->slice.z_brush_radius = cur_br->radius[VIO_Z];
}

/**
 * Toggle the current brush. The state of the current brush is saved, and
 * a new brush is loaded.
 */
/* ARGSUSED */
DEF_MENU_FUNCTION(toggle_secondary_brush)
{
    display_struct *slice_window;

    if ( get_slice_window( display, &slice_window ) )
    {
      int index = slice_window->slice.segmenting.brush_index;

      save_brush( slice_window, index );

      /* Increment the brush index, wrapping if necessary. */
      if (++index >= N_BRUSHES)
        index = 0;

      load_brush( slice_window, index );

      slice_window->slice.segmenting.brush_index = index;
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_secondary_brush)
{
    return( TRUE );
}
