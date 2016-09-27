/**
 * \file regions.c
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

/* ARGSUSED */

DEF_MENU_FUNCTION( set_paint_xy_brush_radius )
{
    VIO_Real        xy_brush_radius;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        if ( get_user_input( "Enter xy brush size: ", "r", 
                             &xy_brush_radius ) == VIO_OK &&
             xy_brush_radius >= 0.0 )
        {
            slice_window->slice.x_brush_radius = xy_brush_radius;
            slice_window->slice.y_brush_radius = xy_brush_radius;
        }
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_paint_xy_brush_radius )
{
    VIO_BOOL         state;
    VIO_Real         x_brush_radius;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );
    if( state )
        x_brush_radius = slice_window->slice.x_brush_radius;
    else
        x_brush_radius = Default_x_brush_radius;

    set_menu_text_real( menu_window, menu_entry, x_brush_radius );

    return( state );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_paint_z_brush_radius )
{
    VIO_Real        z_brush_radius;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        if (get_user_input( "Enter out of plane brush size: ", "r",
                            &z_brush_radius) == VIO_OK &&
            z_brush_radius >= 0.0 )
            slice_window->slice.z_brush_radius = z_brush_radius;
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_paint_z_brush_radius )
{
    VIO_BOOL         state;
    VIO_Real         z_brush_radius;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );
    if( state )
        z_brush_radius = slice_window->slice.z_brush_radius;
    else
        z_brush_radius = Default_z_brush_radius;

    set_menu_text_real( menu_window, menu_entry, z_brush_radius );

    return( state );
}


/* ARGSUSED */

  DEF_MENU_FUNCTION( set_current_paint_label )
{
    int             label, view_index, volume_index;
    VIO_Real        voxel[VIO_MAX_DIMENSIONS];
    int             int_voxel[VIO_N_DIMENSIONS];
    display_struct  *slice_window;
    VIO_BOOL         done;

    if( get_slice_window( display, &slice_window ) )
    {
        done = FALSE;

        if( G_is_mouse_in_window( slice_window->window ) &&
            get_voxel_in_slice_window( slice_window, voxel,
                                       &volume_index, &view_index ) )
        {
            convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, int_voxel );
            label = get_voxel_label( slice_window, volume_index,
                                     int_voxel[VIO_X], int_voxel[VIO_Y], int_voxel[VIO_Z] );
            if( label != 0 )
                done = TRUE;
        }

        if( !done )
        {
            if (get_user_input( "Enter current paint label: ", "d",
                                &label ) == VIO_OK &&
                label >= 0 && label < get_num_labels(slice_window,
                                      get_current_volume_index(slice_window)) )
                done = TRUE;
        }

        if( done )
        {
            slice_window->slice.current_paint_label = label;

            print( "Paint label set to: %d\n",
                   slice_window->slice.current_paint_label );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_current_paint_label )
{
    VIO_BOOL          state;
    int              current_label;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );

    if( state )
        current_label = slice_window->slice.current_paint_label;
    else
        current_label = Default_paint_label;

    set_menu_text_int( menu_window, menu_entry, current_label );

    return( state );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_current_erase_label )
{
    int             label, axis_index, volume_index;
    VIO_Real        voxel[VIO_MAX_DIMENSIONS];
    int             int_voxel[VIO_N_DIMENSIONS];
    display_struct  *slice_window;
    VIO_BOOL         done;

    if( get_slice_window( display, &slice_window ) )
    {
        done = FALSE;

        if( get_voxel_under_mouse( display, &volume_index, &axis_index, voxel ))
        {
            convert_real_to_int_voxel( VIO_N_DIMENSIONS, voxel, int_voxel );
            label = get_voxel_label( slice_window, volume_index,
                                     int_voxel[VIO_X], int_voxel[VIO_Y], int_voxel[VIO_Z] );
            done = TRUE;
        }

        if( !done )
        {
            if (get_user_input( "Enter current erase label: ", "d", 
                                &label ) == VIO_OK &&
                label >= 0 && label < get_num_labels(slice_window,
                                      get_current_volume_index(slice_window)) )
                done = TRUE;
        }

        if( done )
        {
            slice_window->slice.current_erase_label = label;

            print( "Erase label set to: %d\n",
                   slice_window->slice.current_erase_label );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_current_erase_label )
{
    VIO_BOOL         state;
    int              current_label;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );

    if( state )
        current_label = slice_window->slice.current_erase_label;
    else
        current_label = 0;

    set_menu_text_int( menu_window, menu_entry, current_label );

    return( state );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_label_colour )
{
    display_struct   *slice_window;
    int              label;
    VIO_STR          line;
    VIO_Colour       col;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        int volume_index = get_current_volume_index(slice_window);
        if (get_user_input("Enter the label number and colour for this label: ",
                           "ds", &label, &line) == VIO_OK &&
            label >= 1 && label < get_num_labels(slice_window, volume_index))
        {
            if ( string_to_colour( line, &col ) == VIO_OK )
            {
                set_colour_of_label( slice_window, volume_index, label, col );

                set_slice_window_all_update( slice_window, volume_index, 
                                             UPDATE_LABELS );
                delete_string( line );
            }
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_label_colour )
{
    return( get_n_volumes(display) > 0 );
}

static  void  copy_labels_from_adjacent_slice(
    display_struct   *display,
    int              src_offset )
{
    VIO_Real         real_dest_index[VIO_MAX_DIMENSIONS];
    int              src_index[VIO_N_DIMENSIONS], dest_index[VIO_MAX_DIMENSIONS];
    int              view_index, x_index, y_index, axis_index;
    int              volume_index;
    display_struct   *slice_window;
    VIO_Volume       volume;

    if( get_slice_window( display, &slice_window ) &&
        get_voxel_under_mouse( display, &volume_index,
                               &view_index, real_dest_index ) &&
        slice_has_ortho_axes( slice_window, volume_index, view_index,
                              &x_index, &y_index, &axis_index ) )
    {
        volume = get_nth_volume( slice_window, volume_index );

        convert_real_to_int_voxel( VIO_N_DIMENSIONS, real_dest_index, dest_index );

        src_index[VIO_X] = 0;
        src_index[VIO_Y] = 0;
        src_index[VIO_Z] = 0;
        src_index[axis_index] = dest_index[axis_index] + src_offset;

        if( int_voxel_is_within_volume( volume, src_index ) )
        {
            undo_start(slice_window, volume_index);

            copy_labels_slice_to_slice(
                         slice_window, volume_index,
                         axis_index,
                         src_index[axis_index],
                         dest_index[axis_index],
                         slice_window->slice.segmenting.min_threshold,
                         slice_window->slice.segmenting.max_threshold );

            undo_finish(slice_window, volume_index);

            set_slice_window_all_update( slice_window, volume_index,
                                         UPDATE_LABELS );
        }
    }
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( copy_labels_from_lower_slice )
{
    copy_labels_from_adjacent_slice( display, -1 );

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(copy_labels_from_lower_slice )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( copy_labels_from_higher_slice )
{
    copy_labels_from_adjacent_slice( display, 1 );

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(copy_labels_from_higher_slice )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( toggle_display_labels )
{
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        slice_window->slice.volumes[get_current_volume_index(slice_window)].
                        display_labels =
           !slice_window->slice.volumes[get_current_volume_index(slice_window)].
                           display_labels;
        set_slice_window_all_update( slice_window,
                        get_current_volume_index(slice_window), UPDATE_LABELS );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(toggle_display_labels )
{
    VIO_BOOL          display_labels, state;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window ) &&
            get_n_volumes(slice_window) > 0;

    if( state )
        display_labels = slice_window->slice.
                volumes[get_current_volume_index(slice_window)].display_labels;
    else
        display_labels = Initial_display_labels;

    set_menu_text_on_off( menu_window, menu_entry, display_labels );

    return( state );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( change_labels_in_range )
{
    display_struct  *slice_window;
    VIO_Status          status;
    int             src_min, src_max, dest_label;
    int             range_changed[2][VIO_N_DIMENSIONS];
    VIO_Real            min_threshold, max_threshold;
    VIO_STR          line;
    VIO_Volume          volume;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        status = VIO_OK;

        status = get_user_input( "Label or range to change from: ", "s",
                                 &line);
        if( status == VIO_OK )
        {
            if( sscanf( line, "%d %d", &src_min, &src_max ) != 2 )
            {
                if( sscanf( line, "%d", &src_min ) == 1 )
                    src_max = src_min;
                else
                    status = VIO_ERROR;
            }

            delete_string( line );
        }

        if( status == VIO_OK )
        {
            status = get_user_input( "Label to change to: ", "d", &dest_label);
        }

        if( status == VIO_OK )
        {
            status = get_user_input( "Min and max of value range: ", "rr",
                                     &min_threshold, &max_threshold);
        }

        if( status == VIO_OK )
        {
            modify_labels_in_range( volume, get_label_volume(slice_window),
                                    src_min, src_max, dest_label,
                                    min_threshold, max_threshold,
                                    range_changed );
            delete_slice_undo( slice_window,
                               get_current_volume_index(slice_window) );
            set_slice_window_all_update( slice_window,
                       get_current_volume_index(slice_window), UPDATE_LABELS );
            tell_surface_extraction_range_of_labels_changed( display,
                       get_current_volume_index(slice_window), range_changed );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(change_labels_in_range )
{
    return( get_n_volumes(display) > 0 );
}

static  void  calculate_label_volume(
    display_struct  *slice_window,
    int             volume_index,
    int             label,
    int             *n_voxels,
    VIO_Real            *cubic_mm )
{
    int     x, y, z, sizes[VIO_MAX_DIMENSIONS];
    int     n_vox;
    VIO_Real    separations[VIO_MAX_DIMENSIONS];
    VIO_Volume  label_volume;

    label_volume = get_nth_label_volume(slice_window,volume_index);

    get_volume_sizes( label_volume, sizes );
    n_vox = 0;

    for_less( x, 0, sizes[VIO_X] )
    {
        for_less( y, 0, sizes[VIO_Y] )
        {
            for_less( z, 0, sizes[VIO_Z] )
            {
                if( get_voxel_label( slice_window, volume_index,
                                     x, y, z ) == label )
                    ++n_vox;
            }
        }
    }

    get_volume_separations( label_volume, separations );

    *n_voxels = n_vox;
    *cubic_mm = (VIO_Real) n_vox * separations[VIO_X] * separations[VIO_Y] * separations[VIO_Z];
    *cubic_mm = VIO_FABS( *cubic_mm );

    print( "Voxel size: %g mm by %g mm by %g mm\n",
           VIO_FABS( separations[VIO_X] ),
           VIO_FABS( separations[VIO_Y] ),
           VIO_FABS( separations[VIO_Z] ) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( calculate_volume )
{
    display_struct  *slice_window;
    int             n_voxels;
    VIO_Real            cubic_millimetres;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        calculate_label_volume( slice_window,
                                get_current_volume_index(slice_window),
                                slice_window->slice.current_paint_label,
                                &n_voxels, &cubic_millimetres );

        print( "Region %d:    number of voxels: %d\n",
                  slice_window->slice.current_paint_label, n_voxels );
        print( "             cubic millimetres: %g\n", cubic_millimetres );
        print( "             cubic centimetres: %g\n",
               cubic_millimetres / 1000.0 );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(calculate_volume )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( flip_labels_in_x )
{
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        flip_labels_around_zero( slice_window );
        delete_slice_undo( slice_window,
                           get_current_volume_index(slice_window) );

        set_slice_window_all_update( slice_window,
                        get_current_volume_index(slice_window), UPDATE_LABELS );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(flip_labels_in_x )
{
    return( get_n_volumes(display) > 0 );
}

static  void  translate_labels_callback(
    display_struct   *display,
    int              x_delta,
    int              y_delta )
{
    display_struct  *slice_window;
    int             view_index, axis_index, x_index, y_index;
    int             delta[VIO_N_DIMENSIONS];

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 &&
        get_slice_view_index_under_mouse( display, &view_index ) &&
        slice_has_ortho_axes( slice_window,
                       get_current_volume_index( slice_window ), view_index,
                       &x_index, &y_index, &axis_index ) )
    {
        delta[VIO_X] = 0;
        delta[VIO_Y] = 0;
        delta[VIO_Z] = 0;

        delta[x_index] = x_delta;
        delta[y_index] = y_delta;

        translate_labels( slice_window, get_current_volume_index(slice_window),
                          delta );
        delete_slice_undo( slice_window,
                           get_current_volume_index(slice_window) );

        set_slice_window_all_update( slice_window,
                        get_current_volume_index(slice_window), UPDATE_LABELS);
    }
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( translate_labels_up )
{
    translate_labels_callback( display, 0, 1 );

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(translate_labels_up )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( translate_labels_down )
{
    translate_labels_callback( display, 0, -1 );

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(translate_labels_down )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( translate_labels_left )
{
    translate_labels_callback( display, -1, 0 );

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(translate_labels_left )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( translate_labels_right )
{
    translate_labels_callback( display, 1, 0 );

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(translate_labels_right )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( undo_slice_labels )
{
    int   volume_index;

    volume_index = undo_slice_labels_if_any( display );

    if( volume_index >= 0 )
    {
        set_slice_window_all_update( get_display_by_type( SLICE_WINDOW ),
                                     volume_index, UPDATE_LABELS );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(undo_slice_labels )
{
    return slice_labels_to_undo(display);
}

/* ARGSUSED */

DEF_MENU_FUNCTION( translate_labels_arbitrary )
{
    int              delta[VIO_MAX_DIMENSIONS];
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        if (get_user_input( "Enter offset to translate for x, y and z: " , 
                            "ddd", 
                            &delta[VIO_X], 
                            &delta[VIO_Y], 
                            &delta[VIO_Z] ) == VIO_OK)
        {
            translate_labels( slice_window,
                              get_current_volume_index(slice_window), delta );
            set_slice_window_all_update( slice_window,
                        get_current_volume_index(slice_window), UPDATE_LABELS );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(translate_labels_arbitrary )
{
    return( get_n_volumes(display) > 0 );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_fast_update )
{
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        slice_window->slice.segmenting.fast_updating_allowed =
                       !slice_window->slice.segmenting.fast_updating_allowed;
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_fast_update )
{
    VIO_BOOL          fast_flag, state;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );

    if( state )
        fast_flag = slice_window->slice.segmenting.fast_updating_allowed;
    else
        fast_flag = Default_fast_painting_flag;

    set_menu_text_on_off( menu_window, menu_entry, fast_flag );

    return( state );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_cursor_follows_paintbrush )
{
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        slice_window->slice.segmenting.cursor_follows_paintbrush =
                     !slice_window->slice.segmenting.cursor_follows_paintbrush;
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_cursor_follows_paintbrush )
{
    VIO_BOOL          follow_flag, state;
    display_struct   *slice_window;

    state = get_slice_window( display, &slice_window );

    if( state )
        follow_flag = slice_window->slice.segmenting.cursor_follows_paintbrush;
    else
        follow_flag = Default_cursor_follows_paintbrush_flag;

    set_menu_text_on_off( menu_window, menu_entry, follow_flag );

    return( state );
}


/*
 * The following two functions are added in order to
 * implement the freestyle/straightline mode option.
 * Rene Mandl june, 17 1998
 */

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_freestyle_painting )
{
  set_painting_mode( display, !get_painting_mode( display ) );
  return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_freestyle_painting )
{
 /*
  * This funtion determines whether or not the corresponding
  * menu item should be enabled/disabled.
  * This decision is based on the presence of a data volume.
  * If a volume is present the button is enabled.
  */

 VIO_BOOL          state;
 display_struct   *slice_window;

 state = get_slice_window( display, &slice_window ) &&
   get_n_volumes(slice_window) > 0;

 if( state )
 {
   set_menu_text_on_off( menu_window, menu_entry, get_painting_mode( display ));
 }
 return( state );
}
