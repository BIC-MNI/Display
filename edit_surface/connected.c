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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/edit_surface/connected.c,v 1.13 1995-10-19 15:51:12 david Exp $";
#endif


#include  <display.h>

private    DEF_EVENT_FUNCTION( pick_start_point );
private    DEF_EVENT_FUNCTION( terminate_connected );
private  void  make_connected_invisible(
    polygons_struct   *polygons,
    int               poly_index );

public  void  turn_off_connected_polygons(
    display_struct   *display )
{
    polygons_struct      *edit_polygons;

    if( get_edited_polygons( &display->three_d.surface_edit, &edit_polygons ) )
    {
        push_action_table( &display->action_table, MIDDLE_MOUSE_DOWN_EVENT );

        add_action_table_function( &display->action_table,
                                   MIDDLE_MOUSE_DOWN_EVENT,
                                   pick_start_point );

        add_action_table_function( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   terminate_connected );
    }
}

private  void  remove_events(
    action_table_struct   *action_table )
{
    pop_action_table( action_table, MIDDLE_MOUSE_DOWN_EVENT );

    remove_action_table_function( action_table, TERMINATE_INTERACTION_EVENT,
                                  terminate_connected );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( terminate_connected )
{
    remove_events( &display->action_table );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( pick_start_point )
{
    int               poly_index;
    polygons_struct   *polygons, *edit_polygons;
    Point             point;

    remove_events( &display->action_table );

    if( get_edited_polygons( &display->three_d.surface_edit, &edit_polygons )&&
        get_polygon_under_mouse( display, &polygons, &poly_index, &point ) &&
        edit_polygons == polygons )
    {
        check_polygons_neighbours_computed( polygons );

        make_connected_invisible( polygons, poly_index );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

private  void  make_connected_invisible(
    polygons_struct   *polygons,
    int               poly_index )
{
    int      n, start_index, end_index, neighbour, n_queued, n_alloced, *queue;

    n_queued = 0;
    n_alloced = 0;

    ADD_ELEMENT_TO_ARRAY_WITH_SIZE( queue, n_alloced, n_queued,
                                    poly_index, DEFAULT_CHUNK_SIZE );

    while( n_queued > 0 )
    {
        --n_queued;
        poly_index = queue[n_queued];

        start_index = START_INDEX( polygons->end_indices, poly_index );
        end_index = polygons->end_indices[poly_index];

        for_less( n, start_index, end_index )
        {
            neighbour = polygons->neighbours[n];

            if( neighbour >= polygons->n_items )
            {
                HANDLE_INTERNAL_ERROR( "make_connected_invisible" );
            }

            if( neighbour >= 0 && polygons->visibilities[neighbour] )
            {
                polygons->visibilities[neighbour] = FALSE;

                ADD_ELEMENT_TO_ARRAY_WITH_SIZE( queue, n_alloced, n_queued,
                                    neighbour, DEFAULT_CHUNK_SIZE );
            }
        }
    }

    if( n_alloced > 0 )
        FREE( queue );
}
