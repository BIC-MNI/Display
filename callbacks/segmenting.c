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
#include "stack.h"

static  void  set_slice_labels(
    display_struct     *display,
    int                label );
static  void   set_connected_labels(
    display_struct   *display,
    int              label,
    VIO_BOOL          use_threshold );

/* ARGSUSED */

  DEF_MENU_FUNCTION( toggle_undo_feature )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window) && 
            ! (slice_window->slice.toggle_undo_feature =
                !slice_window->slice.toggle_undo_feature) )
    {
        delete_slice_undo( &slice_window->slice.undo, -1 );
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

/* ARGSUSED */

  DEF_MENU_FUNCTION( label_voxel )
{
    VIO_Real           voxel[VIO_MAX_DIMENSIONS];
    int            view_index, int_voxel[VIO_MAX_DIMENSIONS], volume_index;
    display_struct *slice_window;
    int 		   value;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index, &view_index, voxel))
    {
        if ( slice_window->slice.toggle_undo_feature ){
            record_slice_under_mouse( slice_window, volume_index );
        }
        convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, int_voxel );
        value = get_current_paint_label(slice_window);
        set_voxel_label( slice_window, volume_index,
                         int_voxel[VIO_X],
                         int_voxel[VIO_Y],
                         int_voxel[VIO_Z], value );
        set_slice_window_all_update( slice_window, volume_index,
                                     UPDATE_LABELS );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(label_voxel )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( clear_voxel )
{
    VIO_Real           voxel[VIO_MAX_DIMENSIONS];
    display_struct *slice_window;
    int            view_index, int_voxel[VIO_MAX_DIMENSIONS], volume_index;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index, &view_index,voxel))
    {
        if ( slice_window->slice.toggle_undo_feature ){
            record_slice_under_mouse( slice_window, volume_index );
        }
        convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, int_voxel );
        set_voxel_label( slice_window, volume_index,
                         int_voxel[VIO_X], int_voxel[VIO_Y], int_voxel[VIO_Z], 0 );
        set_slice_window_all_update( slice_window, volume_index,
                                     UPDATE_LABELS );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(clear_voxel )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( reset_segmenting )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        clear_all_labels( slice_window );
        delete_slice_undo( &slice_window->slice.undo,
                           get_current_volume_index(slice_window) );
        set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );

        pop_menu_one_level( display->associated[MENU_WINDOW] );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(reset_segmenting )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( set_segmenting_threshold )
{
    display_struct   *slice_window;
    VIO_Real             min, max;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter min and max threshold: " );

        if( input_real( stdin, &min ) == VIO_OK && input_real( stdin, &max ) == VIO_OK )
        {
            slice_window->slice.segmenting.min_threshold = min;
            slice_window->slice.segmenting.max_threshold = max;
        }

        (void) input_newline( stdin );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(set_segmenting_threshold )
{
    return( slice_window_exists(display) );
}

  VIO_Status  input_label_volume_file(
    display_struct   *display,
    VIO_STR           filename )
{
    int              range[2][VIO_N_DIMENSIONS];
    VIO_Status           status;
    display_struct   *slice_window;

    status = VIO_OK;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        status = load_label_volume( filename,
                                    get_label_volume(slice_window) );

        if( status == VIO_OK )
        {
            replace_string( &slice_window->slice.volumes[
                      get_current_volume_index(slice_window)].labels_filename,
                      create_string(filename) );
        }

        delete_slice_undo( &slice_window->slice.undo,
                           get_current_volume_index(slice_window) );

        set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );

        range[0][VIO_X] = 0;
        range[0][VIO_Y] = 0;
        range[0][VIO_Z] = 0;
        get_volume_sizes( get_volume(slice_window), range[1] );
        tell_surface_extraction_range_of_labels_changed( display, 
                              get_current_volume_index(slice_window),
                              range );
    }

    return( status );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION(load_label_data)
{
    VIO_Status           status;
    VIO_STR           filename;

    status = VIO_OK;

    if( get_n_volumes(display) > 0 )
    {
        print( "Enter filename to load: " );

        status = input_string( stdin, &filename, ' ' );

        (void) input_newline( stdin );

        status = input_label_volume_file( display, filename );

        delete_string( filename );

        print( "Done\n" );
    }

    return( status );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(load_label_data )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION(save_label_data)
{
    VIO_Status           status;
    VIO_STR           filename, backup_filename;
    display_struct   *slice_window;
    VIO_Real             crop_threshold;

    status = VIO_OK;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {

    	if( string_length(Output_label_filename) )
    		filename = Output_label_filename;
    	else
    	{
    		print( "Enter filename to save: " );

			status = input_string( stdin, &filename, ' ' );

			(void) input_newline( stdin );
    	}

        if( status == VIO_OK && check_clobber_file( filename ) )
        {
            if( !slice_window->slice.crop_labels_on_output_flag )
                crop_threshold = 0.0;
            else
                crop_threshold = Crop_label_volumes_threshold;

            status = make_backup_file( filename, &backup_filename );

            if( status == VIO_OK )
            {
                status = save_label_volume( filename,
                          backup_filename,
                          get_label_volume(slice_window), crop_threshold );

                cleanup_backup_file( filename, backup_filename, status );
            }

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
        }

        delete_string( filename );
    }

    return( status );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(save_label_data )
{
    return( get_n_volumes(display) > 0 );
}

  VIO_Status input_tag_label_file(
    display_struct   *display,
    VIO_STR           filename )
{
    VIO_Status         status;
    VIO_BOOL        landmark_format;
    FILE           *file;
    display_struct *slice_window;
    VIO_Volume         volume;

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

        delete_slice_undo( &slice_window->slice.undo,
                           get_current_volume_index(slice_window) );

        set_slice_window_all_update( slice_window,
                 get_current_volume_index(slice_window), UPDATE_LABELS  );
    }

    return( status );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( load_labels )
{
    VIO_STR         filename;

    if( get_n_volumes(display) > 0 )
    {
        print( "Enter filename: " );
        if( input_string( stdin, &filename, ' ' ) == VIO_OK )
        {
            (void) input_tag_label_file( display, filename );

            print( "Done loading.\n" );
        }

        (void) input_newline( stdin );

        delete_string( filename );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(load_labels )
{
    return( get_n_volumes(display) > 0 );
}

static  void   save_labels_as_tags(
    display_struct  *display,
    display_struct  *slice_window,
    int             desired_label )
{
    VIO_Status         status;
    FILE           *file;
    VIO_STR         filename;

    print( "Enter filename to save: " );
    status = input_string( stdin, &filename, ' ' );
    (void) input_newline( stdin );

    if( status == VIO_OK && check_clobber_file_default_suffix( filename,
                                            get_default_tag_file_suffix() ) )
    {
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
    }

    delete_string( filename );
}

/*
 * Create tags from a label image
 */
 VIO_Status tags_from_label(
	display_struct *display,
    int       *n_tag_points,
    VIO_Real      ***tags_volume1,
    VIO_Real      ***tags_volume2,
    VIO_Real      **weights,
    int       **structure_ids,
    int       **patient_ids,
    VIO_STR    *labels[] )
{
	display_struct 	*marker_window;
	display_struct 	*slice_window;
	VIO_Status 			status;
	VIO_Volume 			volume;
	VIO_Volume 			label_volume;
	int 			sizes[VIO_MAX_DIMENSIONS];
	int  			ind[VIO_N_DIMENSIONS];
	VIO_Real 			real_ind[VIO_N_DIMENSIONS];
	VIO_Real 			tags[VIO_N_DIMENSIONS];
	int 			value;
	int 			structure_id, patient_id;
	VIO_Real 			weight;
	VIO_STR 			label;
	VIO_Real 			*coords;
	int				i;

	status = VIO_OK;
	slice_window = display->associated[SLICE_WINDOW];
	marker_window = display->associated[MARKER_WINDOW];
	volume = get_volume(slice_window);
	label_volume = get_label_volume(slice_window);
	get_volume_sizes( label_volume, sizes );
    *n_tag_points = 0;
    label = NULL;
    weight = 0.0;
    structure_id = -1;
    patient_id = -1;

    SET_ARRAY_SIZE( marker_window->label_stack, 0,
    		Initial_num_labels, DEFAULT_CHUNK_SIZE);
    for (i=0; i<Initial_num_labels; ++i)
    	marker_window->label_stack[i] = NULL;


    for_less (ind[VIO_X], 0, sizes[VIO_X])
	{
		real_ind[VIO_X] = (VIO_Real) ind[VIO_X];
		for_less (ind[VIO_Y], 0, sizes[VIO_Y])
		{
			real_ind[VIO_Y] = (VIO_Real) ind[VIO_Y];
			for_less (ind[VIO_Z], 0, sizes[VIO_Z])
			{
				real_ind[VIO_Z] = (VIO_Real) ind[VIO_Z];
				value = get_volume_label_data( label_volume, ind );
				if (!value)
					continue;

				convert_voxel_to_world( volume, real_ind,
						&tags[VIO_X], &tags[VIO_Y], &tags[VIO_Z] );

			    ALLOC( coords, VIO_MAX_DIMENSIONS );
			    coords[VIO_X] = tags[VIO_X];
			    coords[VIO_Y] = tags[VIO_Y];
			    coords[VIO_Z] = tags[VIO_Z];

				if (marker_window->label_stack[value] != NULL)
				{
				    marker_window->label_stack[value] = push(marker_window->label_stack[value], coords);
				}
				else
				{
					marker_window->label_stack[value] = stack_new();
				    marker_window->label_stack[value] = push(marker_window->label_stack[value], coords);


					SET_ARRAY_SIZE( *tags_volume1, *n_tag_points, *n_tag_points+1,
		                            DEFAULT_CHUNK_SIZE );
		            ALLOC( (*tags_volume1)[*n_tag_points], 3 );
		            (*tags_volume1)[*n_tag_points][VIO_X] = tags[VIO_X];
		            (*tags_volume1)[*n_tag_points][VIO_Y] = tags[VIO_Y];
		            (*tags_volume1)[*n_tag_points][VIO_Z] = tags[VIO_Z];

		            if (weights != NULL)
					{
						SET_ARRAY_SIZE( *weights, *n_tag_points, *n_tag_points+1,
								DEFAULT_CHUNK_SIZE);
						(*weights)[*n_tag_points] = weight;
					}

					if (structure_ids != NULL)
					{
						SET_ARRAY_SIZE( *structure_ids, *n_tag_points, *n_tag_points+1,
								DEFAULT_CHUNK_SIZE);
						(*structure_ids)[*n_tag_points] = structure_id;
					}

					if (patient_ids != NULL)
					{
						SET_ARRAY_SIZE( *patient_ids, *n_tag_points, *n_tag_points+1,
								DEFAULT_CHUNK_SIZE);
						(*patient_ids)[*n_tag_points] = patient_id;
					}

					if (labels != NULL)
					{
						SET_ARRAY_SIZE( *labels, *n_tag_points, *n_tag_points+1,
								DEFAULT_CHUNK_SIZE);
						(*labels)[*n_tag_points] = label;
					}
					else
						delete_string(label);

		            ++(*n_tag_points);

				}


			}
		}
	}
	return (status);
}

/*
 * Mimic the function input_tag_objects_file, but for a label image
 */

  VIO_Status   input_tag_objects_label(
    display_struct* display,
    VIO_Colour         marker_colour,
    VIO_Real           default_size,
    Marker_types   default_type,
    int            *n_objects,
    object_struct  **object_list[])
{
    VIO_Status             status;
    object_struct      *object;
    marker_struct      *marker;
    int                i, n_volumes, n_tag_points, *structure_ids, *patient_ids;
    VIO_STR             *labels;
    double             *weights;
    double             **tags1, **tags2;

    *n_objects = 0;

    n_volumes = 1;
    status = tags_from_label ( display, &n_tag_points,
                             &tags1, &tags2, &weights,
                             &structure_ids, &patient_ids, &labels );

    if( status == VIO_OK )
    {
        for_less( i, 0, n_tag_points )
        {
            object = create_object( MARKER );
            marker = get_marker_ptr( object );
            fill_Point( marker->position, tags1[i][VIO_X], tags1[i][VIO_Y],tags1[i][VIO_Z]);
            marker->label = create_string( labels[i] );

            if( structure_ids[i] >= 0 )
                marker->structure_id = structure_ids[i];
            else
                marker->structure_id = -1;

            if( patient_ids[i] >= 0 )
                marker->patient_id = patient_ids[i];
            else
                marker->patient_id = -1;

            if( weights[i] > 0.0 )
                marker->size = weights[i];
            else
                marker->size = default_size;

            marker->colour = marker_colour;
            marker->type = default_type;

            add_object_to_list( n_objects, object_list, object );
        }

        free_tag_points( n_volumes, n_tag_points, tags1, tags2, weights,
                         structure_ids, patient_ids, labels );
    }

    return( status );

}


/* ARGSUSED */

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

  DEF_MENU_FUNCTION( save_current_label )
{
    display_struct *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        if( slice_window->slice.current_paint_label > 0 )
        {
            save_labels_as_tags( display, slice_window,
                                 slice_window->slice.current_paint_label );
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

  DEF_MENU_FUNCTION(clear_slice)
{
    set_slice_labels( display, 0 );

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(clear_slice )
{
    return( get_n_volumes(display) > 0 );
}

static  void  set_slice_labels(
    display_struct     *display,
    int                label )
{
    VIO_Real             voxel[VIO_MAX_DIMENSIONS];
    int              view_index, int_voxel[VIO_MAX_DIMENSIONS], volume_index;
    int              x_index, y_index, axis_index;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index, &view_index,
                               voxel ) &&
        slice_has_ortho_axes( slice_window, volume_index, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        if ( slice_window->slice.toggle_undo_feature ){
            record_slice_under_mouse( display, volume_index );
        }

        convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, int_voxel );
        set_labels_on_slice( slice_window, volume_index,
                             axis_index, int_voxel[axis_index],
                             label );

        set_slice_window_all_update( slice_window, volume_index,
                                     UPDATE_LABELS );
    }
}

/* ARGSUSED */

  DEF_MENU_FUNCTION(clear_connected)
{
    set_connected_labels( display, 0, TRUE );

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(clear_connected )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

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

static  void   set_connected_labels(
    display_struct   *display,
    int              desired_label,
    VIO_BOOL          use_threshold )
{
    VIO_Real             voxel[VIO_MAX_DIMENSIONS], min_threshold, max_threshold;
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
        if ( slice_window->slice.toggle_undo_feature ){
            record_slice_under_mouse( display, volume_index );
        }

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

        set_slice_window_all_update( slice_window, volume_index, UPDATE_LABELS);
    }
}

/* ARGSUSED */

  DEF_MENU_FUNCTION(label_connected_3d)
{
    VIO_Real             voxel[VIO_MAX_DIMENSIONS];
    int              range_changed[2][VIO_N_DIMENSIONS];
    int              view_index, int_voxel[VIO_MAX_DIMENSIONS];
    int              label_under_mouse, desired_label, volume_index;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( slice_window, &volume_index, &view_index, voxel))
    {
        convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, int_voxel );

        label_under_mouse = get_voxel_label( slice_window, volume_index,
                                             int_voxel[VIO_X],
                                             int_voxel[VIO_Y],
                                             int_voxel[VIO_Z] );

        desired_label = get_current_paint_label( slice_window );

        print( "Filling 3d from %d %d %d, label %d becomes %d\n",
               int_voxel[VIO_X], int_voxel[VIO_Y], int_voxel[VIO_Z],
               label_under_mouse, desired_label );

        (void) fill_connected_voxels( get_nth_volume(slice_window,volume_index),
                               get_nth_label_volume(slice_window,volume_index),
                               slice_window->slice.segmenting.connectivity,
                               int_voxel,
                               label_under_mouse, label_under_mouse,
                               desired_label,
                               slice_window->slice.segmenting.min_threshold,
                               slice_window->slice.segmenting.max_threshold,
                               range_changed );

        delete_slice_undo( &slice_window->slice.undo, volume_index );

        print( "Done\n" );

        set_slice_window_all_update( slice_window, volume_index, UPDATE_LABELS);

        tell_surface_extraction_range_of_labels_changed( display,
                                               volume_index, range_changed );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(label_connected_3d )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION(dilate_labels)
{
    int              min_outside_label, max_outside_label;
    int              range_changed[2][VIO_N_DIMENSIONS];
    VIO_Volume           volume;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( slice_window, &volume) )
    {
        print( "Enter min and max outside label: " );

        if( input_int( stdin, &min_outside_label ) == VIO_OK &&
            input_int( stdin, &max_outside_label ) == VIO_OK )
        {
            (void) dilate_voxels_3d( get_volume(display),
                                  get_label_volume(display),
                                  (VIO_Real) get_current_paint_label(display),
                                  (VIO_Real) get_current_paint_label(display),
                                  0.0, -1.0,
                                  (VIO_Real) min_outside_label,
                                  (VIO_Real) max_outside_label,
                                  slice_window->slice.segmenting.min_threshold,
                                  slice_window->slice.segmenting.max_threshold,
                                  (VIO_Real) get_current_paint_label(display),
                                  slice_window->slice.segmenting.connectivity,
                                  range_changed );

            delete_slice_undo( &slice_window->slice.undo,
                               get_current_volume_index(slice_window) );

            print( "Done\n" );

            set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );

            tell_surface_extraction_range_of_labels_changed( display,
                     get_current_volume_index(slice_window), range_changed );
        }

        (void) input_newline( stdin );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(dilate_labels )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION(erode_labels)
{
    int              min_outside_label, max_outside_label, set_value;
    int              range_changed[2][VIO_N_DIMENSIONS];
    VIO_Volume           volume;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( slice_window, &volume) )
    {
        print( "Enter min and max outside label: " );

        if( input_int( stdin, &min_outside_label ) == VIO_OK &&
            input_int( stdin, &max_outside_label ) == VIO_OK )
        {
            if( min_outside_label <= max_outside_label )
                set_value = MAX( min_outside_label, 0 );
            (void) dilate_voxels_3d( get_volume(display),
                                  get_label_volume(display),
                                  (VIO_Real) min_outside_label,
                                  (VIO_Real) max_outside_label,
                                  0.0, -1.0,
                                  (VIO_Real) get_current_paint_label(display),
                                  (VIO_Real) get_current_paint_label(display),
                                  slice_window->slice.segmenting.min_threshold,
                                  slice_window->slice.segmenting.max_threshold,
                                  (VIO_Real) set_value,
                                  slice_window->slice.segmenting.connectivity,
                                  range_changed );

            delete_slice_undo( &slice_window->slice.undo,
                               get_current_volume_index(slice_window) );

            print( "Done\n" );

            set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );

            tell_surface_extraction_range_of_labels_changed( display,
                     get_current_volume_index(slice_window), range_changed );
        }

        (void) input_newline( stdin );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(erode_labels )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

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

  DEF_MENU_FUNCTION(clear_label_connected_3d)
{
    VIO_Real             voxel[VIO_MAX_DIMENSIONS];
    int              range_changed[2][VIO_N_DIMENSIONS];
    int              view_index, int_voxel[VIO_MAX_DIMENSIONS];
    int              label_under_mouse, desired_label, volume_index;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
//        get_voxel_under_mouse( slice_window, &volume_index, &view_index, voxel))
    {
    	volume_index = get_current_volume_index( slice_window );
    	get_current_voxel( display, volume_index, voxel);

        convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, int_voxel );

        label_under_mouse = get_voxel_label( slice_window, volume_index,
                                             int_voxel[VIO_X],
                                             int_voxel[VIO_Y],
                                             int_voxel[VIO_Z] );

        /* desired_label = get_current_paint_label( slice_window ); */
		desired_label = 0;

        print( "Clear 3d from %d %d %d, label %d becomes %d\n",
               int_voxel[VIO_X], int_voxel[VIO_Y], int_voxel[VIO_Z],
               label_under_mouse, desired_label );

        (void) fill_connected_voxels( get_nth_volume(slice_window,volume_index),
                               get_nth_label_volume(slice_window,volume_index),
                               slice_window->slice.segmenting.connectivity,
                               int_voxel,
                               label_under_mouse, label_under_mouse,
                               desired_label,
                               slice_window->slice.segmenting.min_threshold,
                               slice_window->slice.segmenting.max_threshold,
                               range_changed );

        delete_slice_undo( &slice_window->slice.undo, volume_index );

        print( "Done\n" );

        set_slice_window_all_update( slice_window, volume_index, UPDATE_LABELS);

        tell_surface_extraction_range_of_labels_changed( display,
                                               volume_index, range_changed );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(clear_label_connected_3d )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */
