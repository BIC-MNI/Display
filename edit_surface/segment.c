
#include  <def_graphics.h>
#include  <def_stdio.h>
#include  <def_globals.h>
#include  <def_colours.h>

static    DECL_EVENT_FUNCTION( start_segmenting );
static    DECL_EVENT_FUNCTION( terminate_segmenting );
static    DECL_EVENT_FUNCTION( pick_surface_polygon );
static    DECL_EVENT_FUNCTION( end_segmenting );
static    Status   segment_polygons();
static    Status   add_polygon_under_mouse();
static    Status   create_complete_path();

public  void  start_segmenting_polygons( graphics )
    graphics_struct  *graphics;
{
    void                 push_action_table();
    void                 add_action_table_function();
    polygons_struct      *edit_polygons;
    Boolean              get_edited_polygons();

    if( get_edited_polygons( &graphics->three_d.surface_edit, &edit_polygons ) )
    {
        push_action_table( &graphics->action_table, MIDDLE_MOUSE_DOWN_EVENT );

        add_action_table_function( &graphics->action_table,
                                   MIDDLE_MOUSE_DOWN_EVENT,
                                   start_segmenting );

        add_action_table_function( &graphics->action_table,
                                   TERMINATE_EVENT,
                                   terminate_segmenting );
    }
}

private  void  turn_off_segmenting( action_table )
    action_table_struct   *action_table;
{
    void   remove_action_table_function();
    void   pop_action_table();

    pop_action_table( action_table, MIDDLE_MOUSE_DOWN_EVENT );

    remove_action_table_function( action_table, TERMINATE_EVENT,
                                  terminate_segmenting );
}

private  DEF_EVENT_FUNCTION( terminate_segmenting )
    /* ARGSUSED */
{
    turn_off_segmenting( &graphics->action_table );

    return( OK );
}

private  DEF_EVENT_FUNCTION( start_segmenting )
    /* ARGSUSED */
{
    void                 push_action_table();
    void                 add_action_table_function();

    push_action_table( &graphics->action_table, MIDDLE_MOUSE_UP_EVENT );

    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               end_segmenting );

    add_action_table_function( &graphics->action_table,
                               NO_EVENT,
                               pick_surface_polygon );

    fill_Point( graphics->prev_mouse_position, -1.0, -1.0, -1.0 );
}

private  void  remove_events( action_table )
    action_table_struct  *action_table;
{
    void   remove_action_table_function();
    void   pop_action_table();

    pop_action_table( action_table, MIDDLE_MOUSE_UP_EVENT );
    remove_action_table_function( action_table, NO_EVENT,
                                  pick_surface_polygon );

    turn_off_segmenting( action_table );
}

private  DEF_EVENT_FUNCTION( end_segmenting )
    /* ARGSUSED */
{
    Status   status;
    Boolean  segmented;
    void     remove_events();
    void     set_update_required();

    remove_events( &graphics->action_table );

    status = add_polygon_under_mouse( graphics );

    if( status == OK )
    {
        status = segment_polygons( graphics, &segmented );
    }

    graphics->three_d.surface_edit.n_vertices = 0;

    if( segmented )
    {
        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

private  Status   add_polygon_under_mouse( graphics )
    graphics_struct   *graphics;
{
    Status               status;
    int                  poly_index;
    Point                intersection_point;
    polygons_struct      *edit_polygons, *polygons;
    surface_edit_struct  *surface_edit;
    Boolean              get_edited_polygons();

    surface_edit = &graphics->three_d.surface_edit;

    status = OK;

    if( mouse_moved( graphics) &&
        get_edited_polygons( surface_edit, &edit_polygons ) &&
        get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &intersection_point ) &&
        edit_polygons == polygons )
    {
        if( surface_edit->n_vertices == 0 ||
            surface_edit->vertices[surface_edit->n_vertices-1] != poly_index )
        {
            ADD_ELEMENT_TO_ARRAY_WITH_SIZE( status,
                                            surface_edit->n_vertices_alloced,
                                            surface_edit->n_vertices,
                                            surface_edit->vertices, poly_index,
                                            int, DEFAULT_CHUNK_SIZE );
        }
    }

    return( status );
}

private  DEF_EVENT_FUNCTION( pick_surface_polygon )
    /* ARGSUSED */
{
    Status               status;

    status = add_polygon_under_mouse( graphics );

    return( status );
}

private  Status  segment_polygons( graphics, segmented )
    graphics_struct   *graphics;
    Boolean           *segmented;
{
    Status               status;
    int                  i, *path, path_length;
    Boolean              path_exists;
    surface_edit_struct  *surface_edit;
    void                 display_path();

    surface_edit = &graphics->three_d.surface_edit;

    status = create_complete_path( surface_edit->n_vertices,
                                   surface_edit->vertices,
                                   surface_edit->polygons,
                                   &path_exists, &path_length, &path );

    if( status == OK && path_exists )
    {
#ifdef DEBUG
        display_path( graphics, surface_edit->polygons, path_length, path );
#else
        for_less( i, 0, path_length )
        {
            surface_edit->polygons->visibilities[path[i]] = FALSE;
        }
#endif
        *segmented = TRUE;
    }
    else
    {
        *segmented = FALSE;
    }

    if( status == OK && path_length > 0 )
    {
        FREE1( status, path );
    }

    return( status );
}

private  Status   create_complete_path( n_vertices, vertices, polygons,
                                        path_exists, path_length, path )
    int                n_vertices;
    int                vertices[];
    polygons_struct    *polygons;
    Boolean            *path_exists;
    int                *path_length;
    int                *path[];
{
    Status    status;
    Boolean   exists;
    int       i, next_i, p, len, *poly_path;
    Status    find_path_between_polygons();

    status = OK;

    *path_length = 0;

    *path_exists = TRUE;

    for_less( i, 0, n_vertices )
    {
        next_i = (i + 1) % n_vertices;

        if( status == OK )
        {
            status = find_path_between_polygons( vertices[i], vertices[next_i],
                          polygons->n_items,
                          polygons->end_indices, polygons->visibilities,
                          polygons->neighbours, &exists, &len, &poly_path );
        }

        if( status == OK )
        {
            if( !exists )
            {
                *path_exists = FALSE;
                break;
            }

            for_less( p, 0, len-1 )
            {
                ADD_ELEMENT_TO_ARRAY( status, *path_length, *path,
                                      poly_path[p], int, DEFAULT_CHUNK_SIZE );
            }
        }

        if( status == OK && len > 0 )
        {
            FREE1( status, poly_path );
        }
    }

    return( status );
}

#ifdef  DEBUG
private  void  display_path( graphics, polygons, path_length, path )
    graphics_struct   *graphics;
    polygons_struct   *polygons;
    int               path_length;
    int               path[];
{
    int     i, p, start_index, end_index;
    String  number;
    Point   centroid;
    void    draw_text_3d();

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

        draw_text_3d( graphics, &centroid, &GREEN, number );
    }
}
#endif
