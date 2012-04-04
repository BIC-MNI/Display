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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/slice_window/pick_angle.c,v 1.15 2001/05/27 00:19:54 stever Exp $";
#endif


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
    terminate_any_interactions( slice_window );

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

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( start_picking_angle )
{
    int          view_index;

    if( get_n_volumes(display) > 0 &&
        get_slice_view_index_under_mouse( display, &view_index ) )
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

        if( view_index == get_arbitrary_view_index(display) )
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

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( terminate_picking_angle )
{
    update_picking_angle( display );

    terminate_event( display );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_update_picking_angle )
{
    update_picking_angle( display );

    return( OK );
}

private  void  set_slice_angle(
    display_struct    *slice_window,
    int               x_pixel,
    int               y_pixel )
{
    int        c, view_index, volume_index;
    Real       origin_voxel[MAX_DIMENSIONS], voxel[MAX_DIMENSIONS];
    Real       perp_axis[MAX_DIMENSIONS], view_perp_axis[MAX_DIMENSIONS];
    Real       separations[MAX_DIMENSIONS], factor, mag, scale;
    Real       new_axis[MAX_DIMENSIONS];
    Point      origin, in_plane_point;
    Vector     current_normal, plane_normal, x_axis, current_in_plane;
    Vector     offset, new_normal, in_plane;

    volume_index = get_current_volume_index( slice_window );
    if( !convert_pixel_to_voxel( slice_window, volume_index,
                                 x_pixel, y_pixel, voxel, &view_index ) )
    {
        return;
    }

    /*--- get the information in voxel coordinates */

    get_current_voxel( slice_window, volume_index, origin_voxel );
    get_volume_separations( get_nth_volume(slice_window,volume_index),
                            separations );
    get_slice_perp_axis( slice_window, volume_index, view_index,
                         view_perp_axis );
    get_slice_perp_axis( slice_window, volume_index,
                         get_arbitrary_view_index(slice_window), perp_axis );

    /*--- convert the info to points and vectors in pseudo-world space */

    for_less( c, 0, N_DIMENSIONS )
    {
        separations[c] = FABS( separations[c] );
        Point_coord( origin, c ) = (Point_coord_type)
                                      (origin_voxel[c] * separations[c]);
        Point_coord( in_plane_point, c ) = (Point_coord_type)
                                      (voxel[c] * separations[c]);
        Vector_coord( current_normal, c ) = (Point_coord_type)
                                      (perp_axis[c] * separations[c]);
        Vector_coord( plane_normal, c ) = (Point_coord_type)
                                      (view_perp_axis[c] * separations[c]);
    }

    /*--- check for degenerate conditions */

    if( EQUAL_POINTS( origin, in_plane_point ) )
        return;
    if( null_Vector(&plane_normal) )
        return;

    /*--- find the axis in the plane */

    SUB_POINTS( x_axis, in_plane_point, origin );

    if( slice_window->slice.cross_section_vector_present )
    {
        Vector  axis1;

        for_less( c, 0, N_DIMENSIONS )
        {
            Vector_coord( axis1, c ) = (Point_coord_type)
              (slice_window->slice.cross_section_vector[c] * separations[c]);
        }
        CROSS_VECTORS( new_normal, axis1, x_axis );
    }
    else
    {
        /*--- find the projection of the new normal into the plane */

        NORMALIZE_VECTOR( x_axis, x_axis );
        CROSS_VECTORS( in_plane, x_axis, plane_normal );

        factor = DOT_VECTORS( current_normal, plane_normal ) /
                 DOT_VECTORS( plane_normal, plane_normal );

        /*--- add the plane normal component of the oblique plane normal */

        SCALE_VECTOR( offset, plane_normal, factor );
        SUB_VECTORS( current_in_plane, current_normal, offset );
        mag = MAGNITUDE( current_in_plane );
        if( mag == 0.0 )
            return;

        if( DOT_VECTORS( in_plane, current_in_plane ) < 0.0 )
            SCALE_VECTOR( in_plane, in_plane, -1.0 );

        scale = MAGNITUDE( in_plane ) / mag;
        SCALE_VECTOR( offset, offset, scale );

        ADD_VECTORS( new_normal, in_plane, offset );
    }

    if( null_Vector( &new_normal ) )
        return;

    for_less( c, 0, N_DIMENSIONS )
        new_axis[c] = (Real) Vector_coord( new_normal, c ) / separations[c];

    set_slice_plane_perp_axis( slice_window, volume_index,
                               get_arbitrary_view_index(slice_window),
                               new_axis );
    reset_slice_view( slice_window, get_arbitrary_view_index(slice_window) );
}
