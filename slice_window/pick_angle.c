
#include  <display.h>

private    DEF_EVENT_FUNCTION( start_picking_angle );
private    DEF_EVENT_FUNCTION( terminate_picking_angle );
private    DEF_EVENT_FUNCTION( handle_update_picking_angle );

private  void  set_slice_angle(
    display_struct    *slice_window,
    int               x_pixel,
    int               y_pixel );
private  void  update_picking_angle(
    display_struct    *slice_window );

public  void  start_picking_slice_angle(
    display_struct    *slice_window )
{
    push_action_table( &slice_window->action_table, NO_EVENT );
    push_action_table( &slice_window->action_table,
                       TERMINATE_INTERACTION_EVENT);
    push_action_table( &slice_window->action_table,
                       LEFT_MOUSE_DOWN_EVENT );
    push_action_table( &slice_window->action_table,
                       LEFT_MOUSE_UP_EVENT );

    add_action_table_function( &slice_window->action_table,
                               LEFT_MOUSE_DOWN_EVENT, start_picking_angle );
}

private  void  terminate_event(
    display_struct   *display )
{
    pop_action_table( &display->action_table, NO_EVENT );
    pop_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );
    pop_action_table( &display->action_table, LEFT_MOUSE_DOWN_EVENT );
    pop_action_table( &display->action_table, LEFT_MOUSE_UP_EVENT );
}

private  DEF_EVENT_FUNCTION( start_picking_angle )    /* ARGSUSED */
{
    int          view_index;

    if( get_slice_view_index_under_mouse( display, &view_index ) )
    {
        add_action_table_function( &display->action_table,
                                   NO_EVENT, handle_update_picking_angle );

        add_action_table_function( &display->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   terminate_picking_angle );

        add_action_table_function( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   terminate_picking_angle );

        fill_Point( display->prev_mouse_position, 0.0, 0.0, 0.0 );
        update_picking_angle( display );

        if( view_index == OBLIQUE_VIEW_INDEX )
            terminate_event( display ); 
    }

    return( OK );
}

private  void  update_picking_angle(
    display_struct    *slice_window )
{
    int    x, y, x_prev, y_prev;

    if( pixel_mouse_moved( slice_window, &x, &y, &x_prev, &y_prev ) )
        set_slice_angle( slice_window, x, y );
}

private  DEF_EVENT_FUNCTION( terminate_picking_angle )     /* ARGSUSED */
{
    update_picking_angle( display );

    terminate_event( display );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_update_picking_angle )     /* ARGSUSED */
{
    update_picking_angle( display );

    return( OK );
}

private  void  create_slice_axes(
    int    view_index,
    Real   axis[],
    Real   perp_axis[],
    Real   x_axis[],
    Real   y_axis[] )
{
    switch( view_index )
    {
    case  0:
        y_axis[X] = axis[X];
        y_axis[Y] = axis[Y];
        y_axis[Z] = axis[Z];
        x_axis[X] = 1.0;
        x_axis[Y] = 0.0;
        x_axis[Z] = 0.0;
        break;

    case  1:
        y_axis[X] = 0.0;
        y_axis[Y] = 1.0;
        y_axis[Z] = 0.0;
        x_axis[X] = axis[X];
        x_axis[Y] = axis[Y];
        x_axis[Z] = axis[Z];
        break;

    case  2:
        y_axis[X] = 0.0;
        y_axis[Y] = 0.0;
        y_axis[Z] = 1.0;
        x_axis[X] = axis[X];
        x_axis[Y] = axis[Y];
        x_axis[Z] = axis[Z];
        break;

     case OBLIQUE_VIEW_INDEX:
        x_axis[X] = perp_axis[X];
        x_axis[Y] = perp_axis[Y];
        x_axis[Z] = perp_axis[Z];
        y_axis[X] = axis[X];
        y_axis[Y] = axis[Y];
        y_axis[Z] = axis[Z];
        break;
    }
}

private  void  set_slice_angle(
    display_struct    *slice_window,
    int               x_pixel,
    int               y_pixel )
{
    int        c, view_index;
    Real       origin_voxel[MAX_DIMENSIONS], voxel[MAX_DIMENSIONS];
    Real       axis[MAX_DIMENSIONS];
    Real       x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];
    Real       perp_axis[MAX_DIMENSIONS];
    Real       len;

    if( !convert_pixel_to_voxel( slice_window, x_pixel, y_pixel, voxel,
                                 &view_index ) )
    {
        return;
    }

    get_current_voxel( slice_window, origin_voxel );

    len = 0.0;
    for_less( c, 0, N_DIMENSIONS )
    {
        axis[c] = voxel[c] - origin_voxel[c];
        len += axis[c] * axis[c];
    }

    if( len == 0.0 )
        return;

    len = sqrt( len );

    for_less( c, 0, N_DIMENSIONS )
        axis[c] /= len;

    get_slice_perp_axis( slice_window, view_index, perp_axis );
    create_slice_axes( view_index, axis, perp_axis, x_axis, y_axis );

    set_slice_plane( slice_window, OBLIQUE_VIEW_INDEX, perp_axis, axis );
    reset_slice_view( slice_window, OBLIQUE_VIEW_INDEX );
    set_slice_window_update( slice_window, OBLIQUE_VIEW_INDEX );
}
