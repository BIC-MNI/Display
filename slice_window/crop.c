#include  <display.h>

public  void  initialize_crop_box(
    display_struct   *slice_window )
{
    int  c;

    for_less( c, 0, N_DIMENSIONS )
    {
        slice_window->slice.crop.limits[0][c] = 0.0;
        slice_window->slice.crop.limits[1][c] = 0.0;
    }

    slice_window->slice.crop.filename[0] = (char) 0;
    slice_window->slice.crop.crop_visible = FALSE;
}

public  void  set_crop_filename(
    display_struct   *slice_window,
    char             filename[] )
{
    (void) strcpy( slice_window->slice.crop.filename, filename );
}

public  void  crop_and_load_volume(
    display_struct   *slice_window )
{
    Volume                file_volume, volume;
    volume_input_struct   volume_input;
    Real                  xw, yw, zw;
    Real                  voxel[MAX_DIMENSIONS];
    Real                  min_voxel[MAX_DIMENSIONS];
    Real                  max_voxel[MAX_DIMENSIONS];
    Real                  limits[2][MAX_DIMENSIONS];
    BOOLEAN               first;
    int                   c, limit1, limit2, limit3;
    int                   sizes[MAX_DIMENSIONS];
    int                   int_min_voxel[MAX_DIMENSIONS];
    int                   int_max_voxel[MAX_DIMENSIONS];

    if( strlen( slice_window->slice.crop.filename ) == 0 )
    {
        print( "You have not set the crop filename yet.\n" );
        return;
    }

    if( start_volume_input( slice_window->slice.crop.filename, 3,
                            XYZ_dimension_names,
                            NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                            TRUE, &file_volume,
                            (minc_input_options *) NULL,
                            &volume_input ) != OK )
    {
        print( "Error in cropping MINC file: %s\n",
               slice_window->slice.crop.filename );

        return;
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
                    for_less( c, 0, N_DIMENSIONS )
                    {
                        min_voxel[c] = voxel[c];
                        max_voxel[c] = voxel[c];
                    }
                    first = FALSE;
                }
                else
                {
                    for_less( c, 0, N_DIMENSIONS )
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
    for_less( c, 0, N_DIMENSIONS )
    {
        int_min_voxel[c] = MAX( 0, ROUND(min_voxel[c]) );
        int_max_voxel[c] = MIN( sizes[c], ROUND(max_voxel[c]) );
    }

    delete_volume_input( &volume_input );
    delete_volume( file_volume );
}

public  void  toggle_slice_crop_box_visibility(
    display_struct   *slice_window )
{
    slice_window->slice.crop.crop_visible =
                   !slice_window->slice.crop.crop_visible;

    rebuild_slice_crop_boxes( slice_window );
}
    
private    DEF_EVENT_FUNCTION( start_picking_box );
private    DEF_EVENT_FUNCTION( terminate_picking_box );
private    DEF_EVENT_FUNCTION( handle_update_picking_box );

private  void  update_picking_box(
    display_struct    *slice_window );

public  void  start_picking_slice_box(
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

    slice_window->slice.crop.crop_visible = TRUE;
}

private  void  terminate_event(
    display_struct   *display )
{
    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );
    pop_action_table( &display->action_table, LEFT_MOUSE_DOWN_EVENT );
    pop_action_table( &display->action_table, LEFT_MOUSE_UP_EVENT );
}

private  DEF_EVENT_FUNCTION( start_picking_box )    /* ARGSUSED */
{
    int          volume_index;
    int          view_index, x_index, y_index, axis, x_mouse, y_mouse;
    int          limit_being_moved, axis_being_moved;
    int          x_min, x_max, y_min, y_max;
    Real         x_low, y_low, x_high, y_high, dist, best_dist;
    Real         limits[2][MAX_DIMENSIONS];

    if( get_n_volumes(display) > 0 &&
        get_slice_view_index_under_mouse( display, &view_index ) &&
        slice_has_ortho_axes( display, get_current_volume_index(display),
                              view_index, &x_index, &y_index, &axis ) )
    {
        (void) G_get_mouse_position( display->window, &x_mouse, &y_mouse );

        get_slice_viewport( display, view_index,
                            &x_min, &x_max, &y_min, &y_max );

        x_mouse -= x_min;
        y_mouse -= y_min;

        volume_index = get_current_volume_index(display);
        (void) convert_voxel_to_pixel( display, volume_index, view_index,
                                       limits[0], &x_low, &y_low );
        (void) convert_voxel_to_pixel( display, volume_index, view_index,
                                       limits[1], &x_high, &y_high );

        limit_being_moved = 0;
        axis_being_moved = x_index;
        best_dist = ABS( x_low - x_mouse );

        dist = ABS( y_low - y_mouse );
        if( dist < best_dist )
        {
            limit_being_moved = 0;
            axis_being_moved = y_index;
            best_dist = dist;
        }

        dist = ABS( x_high - y_mouse );
        if( dist < best_dist )
        {
            limit_being_moved = 1;
            axis_being_moved = x_index;
            best_dist = dist;
        }

        dist = ABS( y_high - y_mouse );
        if( dist < best_dist )
        {
            limit_being_moved = 1;
            axis_being_moved = y_index;
            best_dist = dist;
        }

        display->slice.crop.limit_being_moved = limit_being_moved;
        display->slice.crop.axis_being_moved = axis_being_moved;
        display->slice.crop.view_index = view_index;

        add_action_table_function( &display->action_table,
                                   NO_EVENT, handle_update_picking_box );

        add_action_table_function( &display->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   terminate_picking_box );

        add_action_table_function( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   terminate_picking_box );

        fill_Point( display->prev_mouse_position, 0.0, 0.0, 0.0 );
        update_picking_box( display );
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

private  DEF_EVENT_FUNCTION( terminate_picking_box )     /* ARGSUSED */
{
    update_picking_box( display );

    terminate_event( display );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_picking_box )     /* ARGSUSED */
{
    update_picking_box( display );

    return( OK );
}

private  void  set_slice_crop_position(
    display_struct    *slice_window,
    int               x_pixel,
    int               y_pixel )
{
    int        c, view_index, volume_index;
    int        limit_being_moved, axis_being_moved;
    int        x_min, x_max, y_min, y_max;
    Real       voxel[MAX_DIMENSIONS], origin[MAX_DIMENSIONS];
    Real       x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];

    volume_index = get_current_volume_index( slice_window );
    view_index = slice_window->slice.crop.view_index;
    limit_being_moved = slice_window->slice.crop.limit_being_moved;
    axis_being_moved = slice_window->slice.crop.axis_being_moved;

    get_slice_viewport( slice_window, view_index,
                        &x_min, &x_max, &y_min, &y_max );
    get_slice_plane( slice_window, volume_index,
                     view_index, origin, x_axis, y_axis );

    x_pixel -= x_min;
    y_pixel -= y_min;

    (void) convert_slice_pixel_to_voxel( get_nth_volume(slice_window,
                                volume_index), x_pixel, y_pixel,
         origin, x_axis, y_axis,
         slice_window->slice.volumes[volume_index].views[view_index].x_trans,
         slice_window->slice.volumes[volume_index].views[view_index].y_trans,
         slice_window->slice.volumes[volume_index].views[view_index].x_scaling,
         slice_window->slice.volumes[volume_index].views[view_index].y_scaling,
         voxel );

    if( slice_window->slice.crop.limits[limit_being_moved][axis_being_moved] !=
        voxel[axis_being_moved] )
    {
        slice_window->slice.crop.limits[limit_being_moved][axis_being_moved] =
                                              voxel[axis_being_moved];
    }

    rebuild_slice_crop_boxes( slice_window );
}
