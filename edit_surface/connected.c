
#include  <def_graphics.h>
#include  <def_stdio.h>
#include  <def_globals.h>
#include  <def_colours.h>

public  void  turn_off_connected_polygons( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( pick_start_point );
    DECL_EVENT_FUNCTION( terminate_connected );
    void                 push_action_table();
    void                 add_action_table_function();
    polygons_struct      *edit_polygons;
    Boolean              get_edited_polygons();

    if( get_edited_polygons( &graphics->three_d.surface_edit, &edit_polygons ) )
    {
        push_action_table( &graphics->action_table, MIDDLE_MOUSE_DOWN_EVENT );

        add_action_table_function( &graphics->action_table,
                                   MIDDLE_MOUSE_DOWN_EVENT,
                                   pick_start_point );

        add_action_table_function( &graphics->action_table,
                                   TERMINATE_EVENT,
                                   terminate_connected );
    }
}

private  void  remove_events( action_table )
    action_table_struct   *action_table;
{
    void   remove_action_table_function();
    void   pop_action_table();

    pop_action_table( action_table, MIDDLE_MOUSE_DOWN_EVENT );

    remove_action_table_function( action_table, TERMINATE_EVENT );
}

private  DEF_EVENT_FUNCTION( terminate_connected )
    /* ARGSUSED */
{
    remove_events( &graphics->action_table );

    return( OK );
}

private  DEF_EVENT_FUNCTION( pick_start_point )
    /* ARGSUSED */
{
    Status            status;
    Status            make_connected_invisible();
    int               poly_index;
    polygons_struct   *polygons, *edit_polygons;
    Point             intersection_point;
    void              remove_events();
    void              set_update_required();

    remove_events( &graphics->action_table );

    status = OK;

    if( get_edited_polygons( &graphics->three_d.surface_edit, &edit_polygons )
        && get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                         &intersection_point ) &&
        edit_polygons == polygons )
    {
        status = make_connected_invisible( polygons, poly_index );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

private  Status  make_connected_invisible( polygons, poly_index )
    polygons_struct   *polygons;
    int               poly_index;
{
    Status   status;
    int      n, start_index, end_index, neighbour, n_queued, n_alloced, *queue;

    n_queued = 0;
    n_alloced = 0;

    ADD_ELEMENT_TO_ARRAY_WITH_SIZE( status, n_alloced, n_queued, queue,
                                    poly_index, int, DEFAULT_CHUNK_SIZE );

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

                if( status == OK )
                {
                    ADD_ELEMENT_TO_ARRAY_WITH_SIZE( status, n_alloced, n_queued,
                                 queue, neighbour, int, DEFAULT_CHUNK_SIZE );
                }
            }
        }
    }

    if( status == OK && n_alloced > 0 )
    {
        FREE1( status, queue );
    }

    return( status );
}
