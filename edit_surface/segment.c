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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/edit_surface/segment.c,v 1.14 1995-10-19 15:51:13 david Exp $";
#endif


#include  <display.h>

private    DEF_EVENT_FUNCTION( start_segmenting );
private    DEF_EVENT_FUNCTION( terminate_segmenting );
private    DEF_EVENT_FUNCTION( pick_surface_polygon );
private    DEF_EVENT_FUNCTION( end_segmenting );
private  void   add_polygon_under_mouse(
    display_struct    *display );
private  void  segment_polygons(
    display_struct    *display,
    BOOLEAN           *segmented );
private  void   create_complete_path(
    int                n_vertices,
    int                vertices[],
    polygons_struct    *polygons,
    BOOLEAN            *path_exists,
    int                *path_length,
    int                *path[] );

public  void  start_segmenting_polygons(
    display_struct   *display )
{
    polygons_struct      *edit_polygons;

    if( get_edited_polygons( &display->three_d.surface_edit, &edit_polygons ) )
    {
        push_action_table( &display->action_table, MIDDLE_MOUSE_DOWN_EVENT );

        add_action_table_function( &display->action_table,
                                   MIDDLE_MOUSE_DOWN_EVENT,
                                   start_segmenting );

        add_action_table_function( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   terminate_segmenting );
    }
}

private  void  turn_off_segmenting(
    action_table_struct   *action_table )
{
    pop_action_table( action_table, MIDDLE_MOUSE_DOWN_EVENT );

    remove_action_table_function( action_table, TERMINATE_INTERACTION_EVENT,
                                  terminate_segmenting );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( terminate_segmenting )
{
    turn_off_segmenting( &display->action_table );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( start_segmenting )
{
    push_action_table( &display->action_table, MIDDLE_MOUSE_UP_EVENT );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               end_segmenting );

    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               pick_surface_polygon );

    fill_Point( display->prev_mouse_position, -1.0, -1.0, -1.0 );

    return( OK );
}

private  void  remove_events(
    action_table_struct  *action_table )
{
    pop_action_table( action_table, MIDDLE_MOUSE_UP_EVENT );
    remove_action_table_function( action_table, NO_EVENT,
                                  pick_surface_polygon );

    turn_off_segmenting( action_table );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( end_segmenting )
{
    BOOLEAN  segmented;

    remove_events( &display->action_table );

    add_polygon_under_mouse( display );

    segment_polygons( display, &segmented );

    display->three_d.surface_edit.n_vertices = 0;

    if( segmented )
    {
        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

private  void   add_polygon_under_mouse(
    display_struct    *display )
{
    int                  poly_index;
    Real                 x, y, x_prev, y_prev;
    Point                point;
    polygons_struct      *edit_polygons, *polygons;
    surface_edit_struct  *surface_edit;

    surface_edit = &display->three_d.surface_edit;

    if( mouse_moved( display, &x, &y, &x_prev, &y_prev ) &&
        get_edited_polygons( surface_edit, &edit_polygons ) &&
        get_polygon_under_mouse( display, &polygons, &poly_index, &point ) &&
        edit_polygons == polygons )
    {
        if( surface_edit->n_vertices == 0 ||
            surface_edit->vertices[surface_edit->n_vertices-1] != poly_index )
        {
            ADD_ELEMENT_TO_ARRAY_WITH_SIZE( surface_edit->vertices,
                                            surface_edit->n_vertices_alloced,
                                            surface_edit->n_vertices,
                                            poly_index, DEFAULT_CHUNK_SIZE );
        }
    }
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( pick_surface_polygon )
{
    add_polygon_under_mouse( display );

    return( OK );
}

private  void  segment_polygons(
    display_struct    *display,
    BOOLEAN           *segmented )
{
    int                  i, *path, path_length;
    BOOLEAN              path_exists;
    surface_edit_struct  *surface_edit;

    surface_edit = &display->three_d.surface_edit;

    create_complete_path( surface_edit->n_vertices, surface_edit->vertices,
                          surface_edit->polygons,
                          &path_exists, &path_length, &path );

    if( path_exists )
    {
#ifdef DEBUG
        display_path( display, surface_edit->polygons, path_length, path );
#else
        for_less( i, 0, path_length )
            surface_edit->polygons->visibilities[path[i]] = FALSE;
#endif
        *segmented = TRUE;
    }
    else
    {
        *segmented = FALSE;
    }

    if( path_length > 0 )
        FREE( path );
}

private  void   create_complete_path(
    int                n_vertices,
    int                vertices[],
    polygons_struct    *polygons,
    BOOLEAN            *path_exists,
    int                *path_length,
    int                *path[] )
{
    BOOLEAN   exists;
    int       i, next_i, p, len, *poly_path;

    *path_length = 0;

    *path_exists = TRUE;

    for_less( i, 0, n_vertices )
    {
        next_i = (i + 1) % n_vertices;

        find_path_between_polygons( vertices[i], vertices[next_i],
                                    polygons->n_items, polygons->end_indices,
                                    polygons->visibilities,
                                    polygons->neighbours, &exists, &len,
                                    &poly_path );

        if( !exists )
        {
            *path_exists = FALSE;
            break;
        }

        for_less( p, 0, len-1 )
        {
            ADD_ELEMENT_TO_ARRAY( *path, *path_length, poly_path[p],
                                  DEFAULT_CHUNK_SIZE );
        }

        if( len > 0 )
        {
            FREE( poly_path );
        }
    }
}

#ifdef  DEBUG
private  void  display_path(
    display_struct    *display,
    polygons_struct   *polygons,
    int               path_length,
    int               path[] )
{
    int     i, p, start_index, end_index;
    char    number[EXTREMELY_LARGE_STRING_SIZE];
    Point   centroid;

    for_less( i, 0, path_length )
    {
        start_index = START_INDEX( polygons->end_indices, path[i] );
        end_index = polygons->end_indices[path[i]];

        fill_Point( centroid, 0.0, 0.0, 0.0 );
        for_less( p, start_index, end_index )
        {
            Point_x(centroid) += Point_x(polygons->points[polygons->indices[p]]);
            Point_y(centroid) += Point_y(polygons->points[polygons->indices[p]]);
            Point_z(centroid) += Point_z(polygons->points[polygons->indices[p]]);
        }

        Point_x(centroid) /= (Real) (end_index - start_index);
        Point_y(centroid) /= (Real) (end_index - start_index);
        Point_z(centroid) /= (Real) (end_index - start_index);

        (void) sprintf( number, "%d", i+1 );

        draw_text_3d( display, &centroid, &GREEN, number );
    }
}
#endif
