
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

private  void  set_slice_angle(
    display_struct    *slice_window,
    int               x_pixel,
    int               y_pixel )
{
    int        c, view_index;
    Real       origin_voxel[MAX_DIMENSIONS], voxel[MAX_DIMENSIONS];
    Real       perp_axis[MAX_DIMENSIONS], view_perp_axis[MAX_DIMENSIONS];
    Real       separations[MAX_DIMENSIONS], factor, mag, scale;
    Point      origin, in_plane_point;
    Vector     current_normal, plane_normal, x_axis, y_axis, t;
    Vector     in_plane_normal, offset, new_normal;
    Real       x_voxel_axis[MAX_DIMENSIONS], y_voxel_axis[MAX_DIMENSIONS];

    if( !convert_pixel_to_voxel( slice_window, x_pixel, y_pixel, voxel,
                                 &view_index ) )
    {
        return;
    }

    /*--- get the information in voxel coordinates */

    get_current_voxel( slice_window, origin_voxel );
    get_volume_separations( get_volume(slice_window), separations );
    get_slice_perp_axis( slice_window, view_index, view_perp_axis );
    get_slice_perp_axis( slice_window, OBLIQUE_VIEW_INDEX, perp_axis );

    /*--- convert the info to points and vectors in pseudo-world space */

    for_less( c, 0, N_DIMENSIONS )
    {
        separations[c] = ABS( separations[c] );
        Point_coord( origin, c ) = origin_voxel[c] * separations[c];
        Point_coord( in_plane_point, c ) = voxel[c] * separations[c];
        Vector_coord( current_normal, c ) = perp_axis[c] * separations[c];
        Vector_coord( plane_normal, c ) = view_perp_axis[c] * separations[c];
    }

    /*--- check for degenerate conditions */

    if( EQUAL_POINTS( origin, in_plane_point ) )
        return;
    if( null_Vector(&plane_normal) )
        return;

    /*--- find the x_axis and the projection of the new normal into the plane */

    SUB_POINTS( x_axis, in_plane_point, origin );
    NORMALIZE_VECTOR( x_axis, x_axis );
    CROSS_VECTORS( in_plane_normal, x_axis, plane_normal );
    factor = DOT_VECTORS( current_normal, plane_normal ) /
             DOT_VECTORS( plane_normal, plane_normal );

    /*--- add the plane normal component of the oblique plane normal */

    SCALE_VECTOR( offset, plane_normal, factor );
    SUB_VECTORS( t, current_normal, offset );
    mag = MAGNITUDE( t );
    if( mag == 0.0 )
        return;

    scale = MAGNITUDE( in_plane_normal ) / mag;
    SCALE_VECTOR( offset, offset, scale );

    ADD_VECTORS( new_normal, in_plane_normal, offset );

    CROSS_VECTORS( y_axis, new_normal, x_axis );

    NORMALIZE_VECTOR( x_axis, x_axis );
    NORMALIZE_VECTOR( y_axis, y_axis );

    /*--- convert back to voxel coordinates */

    for_less( c, 0, N_DIMENSIONS )
    {
        x_voxel_axis[c] = Vector_coord( x_axis, c ) / separations[c];
        y_voxel_axis[c] = Vector_coord( y_axis, c ) / separations[c];
    }

    set_slice_plane( slice_window, OBLIQUE_VIEW_INDEX,
                     x_voxel_axis, y_voxel_axis );
    reset_slice_view( slice_window, OBLIQUE_VIEW_INDEX );
    set_slice_window_update( slice_window, OBLIQUE_VIEW_INDEX );
}
