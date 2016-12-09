/**
 * \file crop.c
 * \brief Functions for implementing the cropping operation in the slice window.
 *
 * Cropping operations are a bit strange in Display. The program
 * does not directly crop a volume. Instead, it allows the user to
 * select a crop box based on the currently loaded volume(s), then
 * the user can _apply_ that crop box to a separate input file, the
 * name of the input file must be specified by the user.
 *
 * An external program (mincreshape) is invoked to crop the volume.
 * The output of the cropping operation can be stored in a temporary file
 * and immediately loaded, or it can be saved to a given file.
 *
 * There is no way to simply crop the currently loaded volume.
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
#include  <unistd.h>

static  void  set_slice_crop_position(
    display_struct    *slice_window,
    int               x_pixel,
    int               y_pixel );

void  initialize_crop_box(
    display_struct   *slice_window )
{
    int  c;

    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        slice_window->slice.crop.limits[0][c] = 0.0;
        slice_window->slice.crop.limits[1][c] = -1.0;
    }

    slice_window->slice.crop.filename = create_string( NULL );
    slice_window->slice.crop.crop_visible = FALSE;
}

void  delete_crop_box(
    display_struct   *slice_window )
{
    delete_string( slice_window->slice.crop.filename );
}

void  set_crop_filename(
    display_struct   *slice_window,
    VIO_STR           filename )
{
    delete_string( slice_window->slice.crop.filename );
    slice_window->slice.crop.filename = create_string( filename );
}

VIO_Status  create_cropped_volume_to_file(
    display_struct   *slice_window,
    VIO_STR           cropped_filename )
{
    char                  command[VIO_EXTREMELY_LARGE_STRING_SIZE];
    VIO_Volume                file_volume, volume;
    volume_input_struct   volume_input;
    VIO_Real                  xw, yw, zw;
    VIO_Real                  voxel[VIO_MAX_DIMENSIONS];
    VIO_Real                  min_voxel[VIO_MAX_DIMENSIONS];
    VIO_Real                  max_voxel[VIO_MAX_DIMENSIONS];
    VIO_BOOL               first;
    int                   c, limit1, limit2, limit3;
    int                   sizes[VIO_MAX_DIMENSIONS];
    int                   int_min_voxel[VIO_MAX_DIMENSIONS];
    int                   int_max_voxel[VIO_MAX_DIMENSIONS];

    if( string_length( slice_window->slice.crop.filename ) == 0 )
    {
        print( "You have not set the crop filename yet.\n" );
        return( VIO_ERROR );
    }

    if( start_volume_input( slice_window->slice.crop.filename, 3,
                            File_order_dimension_names,
                            NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                            TRUE, &file_volume,
                            (minc_input_options *) NULL,
                            &volume_input ) != VIO_OK )
    {
        print( "Error in cropping MINC file: %s\n",
               slice_window->slice.crop.filename );

        return( VIO_ERROR );
    }

    volume = get_volume( slice_window );
    first = TRUE;

    for_less( limit1, 0, 2 )
    {
        for_less( limit2, 0, 2 )
        {
            for_less( limit3, 0, 2 )
            {
                voxel[0] = slice_window->slice.crop.limits[limit1][0];
                voxel[1] = slice_window->slice.crop.limits[limit2][1];
                voxel[2] = slice_window->slice.crop.limits[limit3][2];

                convert_voxel_to_world( volume, voxel, &xw, &yw, &zw );
                convert_world_to_voxel( file_volume, xw, yw, zw, voxel );
                if( first )
                {
                    for_less( c, 0, VIO_N_DIMENSIONS )
                    {
                        min_voxel[c] = voxel[c];
                        max_voxel[c] = voxel[c];
                    }
                    first = FALSE;
                }
                else
                {
                    for_less( c, 0, VIO_N_DIMENSIONS )
                    {
                        if( voxel[c] < min_voxel[c] )
                            min_voxel[c] = voxel[c];
                        else if( voxel[c] > max_voxel[c] )
                            max_voxel[c] = voxel[c];
                    }
                }
            }
        }
    }

    get_volume_sizes( file_volume, sizes );
    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        int_min_voxel[c] = MAX( 0, VIO_ROUND(min_voxel[c]) );
        int_max_voxel[c] = MIN( sizes[c], VIO_ROUND(max_voxel[c]) );
    }

    delete_volume_input( &volume_input );
    delete_volume( file_volume );

    snprintf( command, sizeof(command), Crop_volume_command,
              slice_window->slice.crop.filename,
              cropped_filename,
              int_min_voxel[VIO_X], int_min_voxel[VIO_Y], int_min_voxel[VIO_Z],
              int_max_voxel[VIO_X] - int_min_voxel[VIO_X] + 1,
              int_max_voxel[VIO_Y] - int_min_voxel[VIO_Y] + 1,
              int_max_voxel[VIO_Z] - int_min_voxel[VIO_Z] + 1 );

    print( "Issuing system command:\n\t%s\n", command );

    if( system( command ) != 0 )
    {
        print( "Error cropping volume: %s\n",
               slice_window->slice.crop.filename );
        return( VIO_ERROR );
    }

    return( VIO_OK );
}

void  crop_and_load_volume(
    display_struct   *slice_window )
{
    char     tmp_name[] = { "mni-displayXXXXXX" };
    VIO_STR  cropped_filename;
    int      fd = mkstemp( tmp_name );

    cropped_filename = create_string( tmp_name );

    if( create_cropped_volume_to_file( slice_window, cropped_filename ) == VIO_OK )
    {
        (void) load_graphics_file( get_three_d_window(slice_window),
                                   cropped_filename, FALSE );

        remove_file( cropped_filename );

        close( fd );

        slice_window->slice.crop.crop_visible = FALSE;

        set_crop_box_update( slice_window, -1 );
    }

    delete_string( cropped_filename );
}

void  toggle_slice_crop_box_visibility(
    display_struct   *slice_window )
{
    slice_window->slice.crop.crop_visible =
                   !slice_window->slice.crop.crop_visible;

    set_crop_box_update( slice_window, -1 );
}

void  reset_crop_box_position(
    display_struct   *display )
{
    VIO_Volume           volume;
    int              c, sizes[VIO_MAX_DIMENSIONS];
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( slice_window, &volume ) )
    {
        get_volume_sizes( volume, sizes );
        for_less( c, 0, VIO_N_DIMENSIONS )
        {
            slice_window->slice.crop.limits[0][c] = 0.0;
            slice_window->slice.crop.limits[1][c] = (VIO_Real) sizes[c] - 1.0;
        }
    }
    else
    {
        for_less( c, 0, VIO_N_DIMENSIONS )
        {
            slice_window->slice.crop.limits[0][c] = 0.0;
            slice_window->slice.crop.limits[1][c] = -1.0;
        }
    }

    set_crop_box_update( slice_window, -1 );
}
    
static    DEF_EVENT_FUNCTION( start_picking_box );
static    DEF_EVENT_FUNCTION( terminate_picking_box );
static    DEF_EVENT_FUNCTION( handle_update_picking_box );

static  void  update_picking_box(
    display_struct    *slice_window );

  void  start_picking_crop_box(
    display_struct    *slice_window )
{
    terminate_any_interactions( slice_window );

    push_action_table( &slice_window->action_table, NO_EVENT );
    push_action_table( &slice_window->action_table,
                       TERMINATE_INTERACTION_EVENT);
    push_action_table( &slice_window->action_table,
                       LEFT_MOUSE_DOWN_EVENT );
    push_action_table( &slice_window->action_table,
                       LEFT_MOUSE_UP_EVENT );

    add_action_table_function( &slice_window->action_table,
                               LEFT_MOUSE_DOWN_EVENT, start_picking_box );

    if( slice_window->slice.crop.limits[0][0] >
        slice_window->slice.crop.limits[1][0] )
    {
        reset_crop_box_position( slice_window );
    }

    slice_window->slice.crop.crop_visible = TRUE;
    set_crop_box_update( slice_window, -1 );
}

static  void  terminate_event(
    display_struct   *display )
{
    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );
    pop_action_table( &display->action_table, LEFT_MOUSE_DOWN_EVENT );
    pop_action_table( &display->action_table, LEFT_MOUSE_UP_EVENT );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( start_picking_box )
{
    int             volume_index;
    int             view_index, x_index, y_index, axis, x_mouse, y_mouse;
    int             limit_being_moved;
    int             x_min, x_max, y_min, y_max;
    VIO_Real            x_low, y_low, x_high, y_high;
    VIO_Real            best_dist, dist_low, dist_high;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 &&
        get_slice_view_index_under_mouse( slice_window, &view_index ) &&
        slice_has_ortho_axes( slice_window,
                              get_current_volume_index(slice_window),
                              view_index, &x_index, &y_index, &axis ) )
    {
        (void) G_get_mouse_position( slice_window->window, &x_mouse, &y_mouse );

        get_slice_viewport( slice_window, view_index,
                            &x_min, &x_max, &y_min, &y_max );

        x_mouse -= x_min;
        y_mouse -= y_min;

        volume_index = get_current_volume_index(slice_window);
        convert_voxel_to_pixel( slice_window, volume_index, view_index,
                                slice_window->slice.crop.limits[0],
                                &x_low, &y_low );
        convert_voxel_to_pixel( slice_window, volume_index, view_index,
                                slice_window->slice.crop.limits[1],
                                &x_high, &y_high );

        slice_window->slice.crop.limit_being_moved[0] = -1;
        slice_window->slice.crop.limit_being_moved[1] = -1;
        slice_window->slice.crop.axis_being_moved[0] = -1;
        slice_window->slice.crop.axis_being_moved[1] = -1;

        dist_low = VIO_FABS( x_low - (VIO_Real) x_mouse );
        dist_high = VIO_FABS( x_high - (VIO_Real) x_mouse );

        if( dist_low <= dist_high )
        {
            best_dist = dist_low;
            limit_being_moved = 0;
        }
        else
        {
            best_dist = dist_high;
            limit_being_moved = 1;
        }

        if( best_dist <= Slice_crop_pick_distance )
        {
            slice_window->slice.crop.limit_being_moved[0] = limit_being_moved;
            slice_window->slice.crop.axis_being_moved[0] = x_index;
        }

        dist_low = VIO_FABS( y_low - (VIO_Real) y_mouse );
        dist_high = VIO_FABS( y_high - (VIO_Real) y_mouse );

        if( dist_low <= dist_high )
        {
            best_dist = dist_low;
            limit_being_moved = 0;
        }
        else
        {
            best_dist = dist_high;
            limit_being_moved = 1;
        }

        if( best_dist <= Slice_crop_pick_distance )
        {
            slice_window->slice.crop.limit_being_moved[1] = limit_being_moved;
            slice_window->slice.crop.axis_being_moved[1] = y_index;
        }

        /*--- not close enough to any edge, translate entire box */

        if( slice_window->slice.crop.axis_being_moved[0] < 0 &&
            slice_window->slice.crop.axis_being_moved[1] < 0 )
        {
            (void) get_voxel_in_slice_window( slice_window,
                                         slice_window->slice.crop.start_voxel,
                                         &volume_index, &view_index );
        }

        slice_window->slice.crop.view_index = view_index;

        add_action_table_function( &slice_window->action_table,
                                   NO_EVENT, handle_update_picking_box );

        add_action_table_function( &slice_window->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   terminate_picking_box );

        add_action_table_function( &slice_window->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   terminate_picking_box );

        fill_Point( slice_window->prev_mouse_position, 0.0, 0.0, 0.0 );
        update_picking_box( slice_window );
    }

    return( VIO_OK );
}

static  void  update_picking_box(
    display_struct    *slice_window )
{
    int    x, y, x_prev, y_prev;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) )
        set_slice_crop_position( slice_window, x, y );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( terminate_picking_box )
{
    update_picking_box( display );

    terminate_event( display );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_update_picking_box )
{
    update_picking_box( display );

    return( VIO_OK );
}

static  void  set_slice_crop_position(
    display_struct    *slice_window,
    int               x_pixel,
    int               y_pixel )
{
    int        view_index, volume_index, dim, limit, axis, a;
    int        x_min, x_max, y_min, y_max;
    VIO_Real   voxel[VIO_MAX_DIMENSIONS], origin[VIO_MAX_DIMENSIONS];
    VIO_Real   x_axis[VIO_MAX_DIMENSIONS], y_axis[VIO_MAX_DIMENSIONS];
    VIO_Real   delta;
    VIO_BOOL   changed;

    volume_index = get_current_volume_index( slice_window );
    view_index = slice_window->slice.crop.view_index;

    get_slice_viewport( slice_window, view_index,
                        &x_min, &x_max, &y_min, &y_max );
    get_slice_plane( slice_window, volume_index,
                     view_index, origin, x_axis, y_axis );

    x_pixel -= x_min;
    y_pixel -= y_min;

    (void) convert_slice_pixel_to_voxel( get_nth_volume(slice_window,
                                volume_index), (VIO_Real) x_pixel, (VIO_Real) y_pixel,
         origin, x_axis, y_axis,
         slice_window->slice.volumes[volume_index].views[view_index].x_trans,
         slice_window->slice.volumes[volume_index].views[view_index].y_trans,
         slice_window->slice.volumes[volume_index].views[view_index].x_scaling,
         slice_window->slice.volumes[volume_index].views[view_index].y_scaling,
         voxel );

    changed = FALSE;

    if( slice_window->slice.crop.axis_being_moved[0] < 0 &&
        slice_window->slice.crop.axis_being_moved[1] < 0 )
    {
        for_less( dim, 0, VIO_N_DIMENSIONS )
        {
            delta = voxel[dim] - slice_window->slice.crop.start_voxel[dim];
            if( delta != 0.0 )
                changed = TRUE;
            slice_window->slice.crop.start_voxel[dim] = voxel[dim];
            slice_window->slice.crop.limits[0][dim] += delta;
            slice_window->slice.crop.limits[1][dim] += delta;
        }
    }
    else
    {
        for_less( a, 0, 2 )
        {
            limit = slice_window->slice.crop.limit_being_moved[a];
            if( limit >= 0 )
            {
                axis = slice_window->slice.crop.axis_being_moved[a];
                if( (limit == 0 &&
                     voxel[axis] < slice_window->slice.crop.limits[1][axis]) ||
                    (limit == 1 &&
                     voxel[axis] > slice_window->slice.crop.limits[0][axis]) )
                {
                    slice_window->slice.crop.limits[limit][axis] = voxel[axis];
                    changed = TRUE;
                }
            }
        }
    }

    if( changed )
        set_crop_box_update( slice_window, -1 );
}

  void  get_volume_crop_limits(
    display_struct    *display,
    int               min_voxel[],
    int               max_voxel[] )
{
    int               dim, sizes[VIO_MAX_DIMENSIONS];
    display_struct    *slice_window;

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes(slice_window) > 0 )
    {
        get_volume_sizes( get_volume(slice_window), sizes );

        for_less( dim, 0, VIO_N_DIMENSIONS )
        {
            if( slice_window->slice.crop.limits[0][dim] >
                slice_window->slice.crop.limits[1][dim] )
            {
                min_voxel[dim] = 0;
                max_voxel[dim] = sizes[dim]-1;
            }
            else
            {
                min_voxel[dim] = VIO_ROUND(slice_window->slice.crop.limits[0][dim]);
                if( min_voxel[dim] < 0 )
                    min_voxel[dim] = 0;
                max_voxel[dim] = VIO_ROUND(slice_window->slice.crop.limits[1][dim]);
                if( max_voxel[dim] >= sizes[dim] )
                    max_voxel[dim] = sizes[dim]-1;
            }
        }
    }
    else
    {
        for_less( dim, 0, VIO_N_DIMENSIONS )
        {
            min_voxel[dim] = 0;
            max_voxel[dim] = 0;
        }
    }
}
