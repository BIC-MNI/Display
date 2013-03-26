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

private  void  set_slice_crop_position(
    display_struct    *slice_window,
    int               x_pixel,
    int               y_pixel );

public  void  initialize_crop_box(
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

public  void  delete_crop_box(
    display_struct   *slice_window )
{
    delete_string( slice_window->slice.crop.filename );
}

public  void  set_crop_filename(
    display_struct   *slice_window,
    VIO_STR           filename )
{
    delete_string( slice_window->slice.crop.filename );
    slice_window->slice.crop.filename = create_string( filename );
}

public  VIO_Status  create_cropped_volume_to_file(
    display_struct   *slice_window,
    VIO_STR           cropped_filename )
{
    char                  command[EXTREMELY_LARGE_STRING_SIZE];
    VIO_Volume                file_volume, volume;
    volume_input_struct   volume_input;
    Real                  xw, yw, zw;
    Real                  voxel[VIO_MAX_DIMENSIONS];
    Real                  min_voxel[VIO_MAX_DIMENSIONS];
    Real                  max_voxel[VIO_MAX_DIMENSIONS];
    VIO_BOOL               first;
    int                   c, limit1, limit2, limit3;
    int                   sizes[VIO_MAX_DIMENSIONS];
    int                   int_min_voxel[VIO_MAX_DIMENSIONS];
    int                   int_max_voxel[VIO_MAX_DIMENSIONS];

    if( string_length( slice_window->slice.crop.filename ) == 0 )
    {
        print( "You have not set the crop filename yet.\n" );
        return( ERROR );
    }

    if( start_volume_input( slice_window->slice.crop.filename, 3,
                            File_order_dimension_names,
                            NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                            TRUE, &file_volume,
                            (minc_input_options *) NULL,
                            &volume_input ) != OK )
    {
        print( "Error in cropping MINC file: %s\n",
               slice_window->slice.crop.filename );

        return( ERROR );
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
        int_min_voxel[c] = MAX( 0, ROUND(min_voxel[c]) );
        int_max_voxel[c] = MIN( sizes[c], ROUND(max_voxel[c]) );
    }

    delete_volume_input( &volume_input );
    delete_volume( file_volume );

    (void) sprintf( command, Crop_volume_command,
                    slice_window->slice.crop.filename,
                    cropped_filename,
                    int_min_voxel[X], int_min_voxel[Y], int_min_voxel[Z],
                    int_max_voxel[X] - int_min_voxel[X] + 1,
                    int_max_voxel[Y] - int_min_voxel[Y] + 1,
                    int_max_voxel[Z] - int_min_voxel[Z] + 1 );

    print( "Issuing system command:\n\t%s\n", command );

    if( system( command ) != 0 )
    {
        print( "Error cropping volume: %s\n",
               slice_window->slice.crop.filename );
        return( ERROR );
    }

    return( OK );
}

public  void  crop_and_load_volume(
    display_struct   *slice_window )
{
    char        tmp_name[L_tmpnam];
    VIO_STR      cropped_filename;

    (void) tmpnam( tmp_name );

    cropped_filename = concat_strings( tmp_name, ".mnc" );

    if( create_cropped_volume_to_file( slice_window, cropped_filename ) == OK )
    {
        (void) load_graphics_file( get_three_d_window(slice_window),
                                   cropped_filename, FALSE );

        remove_file( cropped_filename );

        slice_window->slice.crop.crop_visible = FALSE;

        set_crop_box_update( slice_window, -1 );
    }

    delete_string( cropped_filename );
}

public  void  toggle_slice_crop_box_visibility(
    display_struct   *slice_window )
{
    slice_window->slice.crop.crop_visible =
                   !slice_window->slice.crop.crop_visible;

    set_crop_box_update( slice_window, -1 );
}

public  void  reset_crop_box_position(
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
            slice_window->slice.crop.limits[1][c] = (Real) sizes[c] - 1.0;
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
    
private    DEF_EVENT_FUNCTION( start_picking_box );
private    DEF_EVENT_FUNCTION( terminate_picking_box );
private    DEF_EVENT_FUNCTION( handle_update_picking_box );

private  void  update_picking_box(
    display_struct    *slice_window );

public  void  start_picking_crop_box(
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

private  void  terminate_event(
    display_struct   *display )
{
    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );
    pop_action_table( &display->action_table, LEFT_MOUSE_DOWN_EVENT );
    pop_action_table( &display->action_table, LEFT_MOUSE_UP_EVENT );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( start_picking_box )
{
    int             volume_index;
    int             view_index, x_index, y_index, axis, x_mouse, y_mouse;
    int             limit_being_moved;
    int             x_min, x_max, y_min, y_max;
    Real            x_low, y_low, x_high, y_high;
    Real            best_dist, dist_low, dist_high;
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

        dist_low = FABS( x_low - (Real) x_mouse );
        dist_high = FABS( x_high - (Real) x_mouse );

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

        dist_low = FABS( y_low - (Real) y_mouse );
        dist_high = FABS( y_high - (Real) y_mouse );

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

    return( OK );
}

private  void  update_picking_box(
    display_struct    *slice_window )
{
    int    x, y, x_prev, y_prev;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) )
        set_slice_crop_position( slice_window, x, y );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( terminate_picking_box )
{
    update_picking_box( display );

    terminate_event( display );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_update_picking_box )
{
    update_picking_box( display );

    return( OK );
}

private  void  set_slice_crop_position(
    display_struct    *slice_window,
    int               x_pixel,
    int               y_pixel )
{
    int        view_index, volume_index, dim, limit, axis, a;
    int        x_min, x_max, y_min, y_max;
    Real       voxel[VIO_MAX_DIMENSIONS], origin[VIO_MAX_DIMENSIONS];
    Real       x_axis[VIO_MAX_DIMENSIONS], y_axis[VIO_MAX_DIMENSIONS];
    Real       delta;
    VIO_BOOL    changed;

    volume_index = get_current_volume_index( slice_window );
    view_index = slice_window->slice.crop.view_index;

    get_slice_viewport( slice_window, view_index,
                        &x_min, &x_max, &y_min, &y_max );
    get_slice_plane( slice_window, volume_index,
                     view_index, origin, x_axis, y_axis );

    x_pixel -= x_min;
    y_pixel -= y_min;

    (void) convert_slice_pixel_to_voxel( get_nth_volume(slice_window,
                                volume_index), (Real) x_pixel, (Real) y_pixel,
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
                if( limit == 0 &&
                    voxel[axis] < slice_window->slice.crop.limits[1][axis] ||
                    limit == 1 &&
                    voxel[axis] > slice_window->slice.crop.limits[0][axis] )
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

public  void  get_volume_crop_limits(
    display_struct    *display,
    int               min_voxel[],
    int               max_voxel[] )
{
    int               dim, sizes[VIO_N_DIMENSIONS];
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
                min_voxel[dim] = ROUND(slice_window->slice.crop.limits[0][dim]);
                if( min_voxel[dim] < 0 )
                    min_voxel[dim] = 0;
                max_voxel[dim] = ROUND(slice_window->slice.crop.limits[1][dim]);
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
