
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_picking_polygon( graphics )
    graphics_struct   *graphics;
{
    DECL_EVENT_FUNCTION( start_picking_polygon );
    void                 add_action_table_function();

    add_action_table_function( &graphics->action_table,
                               LEFT_MOUSE_DOWN_EVENT, start_picking_polygon );
                               
}

public  DEF_EVENT_FUNCTION( start_picking_polygon )    /* ARGSUSED */
{
    DECL_EVENT_FUNCTION( update_picked_polygon );
    DECL_EVENT_FUNCTION( pick_polygon_point );
    DECL_EVENT_FUNCTION( terminate_picking_polygon );
    void                 push_action_table();
    void                 add_action_table_function();

    push_action_table( &graphics->action_table, LEFT_MOUSE_UP_EVENT );

    push_action_table( &graphics->action_table, TERMINATE_EVENT );

    add_action_table_function( &graphics->action_table,
                               NO_EVENT,
                               update_picked_polygon );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               terminate_picking_polygon );

    add_action_table_function( &graphics->action_table,
                               LEFT_MOUSE_UP_EVENT,
                               terminate_picking_polygon );
}

private  void  remove_events( action_table )
    action_table_struct  *action_table;
{
    void   remove_action_table_function();
    void   pop_action_table();

    pop_action_table( action_table, LEFT_MOUSE_UP_EVENT );
    pop_action_table( action_table, TERMINATE_EVENT );

    remove_action_table_function( action_table, NO_EVENT );
}

private  DEF_EVENT_FUNCTION( terminate_picking_polygon )
    /* ARGSUSED */
{
    void   remove_events();
    void   pick_point_under_mouse();

    remove_events( &graphics->action_table );

    pick_point_under_mouse( graphics );

    return( OK );
}

private  DEF_EVENT_FUNCTION( update_picked_polygon )
    /* ARGSUSED */
{
    void             pick_point_under_mouse();

    pick_point_under_mouse( graphics );

    return( OK );
}

public  Boolean  get_mouse_scene_intersection( graphics, intersection )
    graphics_struct   *graphics;
    Point             *intersection;
{
    Boolean          found;
    Point            origin, transformed_origin;
    Vector           direction, transformed_direction;
    void             convert_mouse_to_ray();
    void             transform_world_to_model();
    void             transform_world_to_model_vector();

    convert_mouse_to_ray( &graphics->three_d.view, &graphics->mouse_position,
                            &origin, &direction );

    transform_world_to_model( &graphics->three_d.view, &origin,
                              &transformed_origin );
    transform_world_to_model_vector( &graphics->three_d.view, &direction,
                                     &transformed_direction );

    found = intersect_ray_with_polygons( graphics, &transformed_origin,
                                         &transformed_direction,
                                         intersection );
    return( found );
}

private  void  pick_point_under_mouse( graphics )
    graphics_struct   *graphics;
{
    Point            intersection_point;
    Boolean          set_current_voxel();
    void             update_cursor();

    if( get_mouse_scene_intersection( graphics, &intersection_point ) )
    {
        graphics->three_d.cursor.origin = intersection_point;
        update_cursor( graphics );
        graphics->update_required = TRUE;

        if( update_voxel_from_cursor( graphics->associated[SLICE_WINDOW] ) )
        {
            graphics->associated[SLICE_WINDOW]->update_required = TRUE;
        }
    }
}
