
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  start_segmenting_polygons( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( pick_surface_polygon );
    DECL_EVENT_FUNCTION( terminate_segmenting );
    void                 push_action_table();
    void                 add_action_table_function();
    void                 terminate_any_interactions();

    terminate_any_interactions( graphics );

    push_action_table( &graphics->action_table, MIDDLE_MOUSE_DOWN_EVENT );
    push_action_table( &graphics->action_table, MIDDLE_MOUSE_UP_EVENT );
    push_action_table( &graphics->action_table, TERMINATE_EVENT );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               terminate_segmenting );

    add_action_table_function( &graphics->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               pick_surface_polygon );
}

private  void  remove_events( action_table )
    action_table_struct  *action_table;
{
    void   remove_action_table_function();
    void   pop_action_table();

    pop_action_table( action_table, MIDDLE_MOUSE_DOWN_EVENT );
    pop_action_table( action_table, MIDDLE_MOUSE_UP_EVENT );
    pop_action_table( action_table, TERMINATE_EVENT );
}

private  DEF_EVENT_FUNCTION( terminate_segmenting )
    /* ARGSUSED */
{
    void   remove_events();

    remove_events( &graphics->action_table );

    return( OK );
}

private  DEF_EVENT_FUNCTION( pick_surface_polygon )
    /* ARGSUSED */
{
    int                  poly_index;
    Point                intersection_point;
    polygons_struct      *edit_polygons, *polygons;
    surface_edit_struct  *surface_edit;

    surface_edit = &graphics->three_d.surface_edit;

    if( get_edited_polygon( surface_edit, &edit_polygons ) &&
        get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &intersection_point ) &&
        edit_polygons == polygons )
    {
PRINT( "Polygons %d\n", poly_index );

        if( surface_edit->n_vertices == 0 ||
            poly_index != surface_edit->vertices[surface_edit->n_vertices-1] )
        {
        }
        else
        {
            graphics->update_required = TRUE;
        }
    }

    return( OK );
}
