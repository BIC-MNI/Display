
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  start_picking_polygon( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( pick_polygon_point );
    DECL_EVENT_FUNCTION( terminate_picking_polygon );
    void                 push_action_table();
    void                 add_action_table_function();

    push_action_table( &graphics->action_table, LEFT_MOUSE_DOWN_EVENT );
    push_action_table( &graphics->action_table, LEFT_MOUSE_UP_EVENT );
    push_action_table( &graphics->action_table, TERMINATE_EVENT );

    add_action_table_function( &graphics->action_table,
                               TERMINATE_EVENT,
                               terminate_picking_polygon );

    add_action_table_function( &graphics->action_table,
                               LEFT_MOUSE_DOWN_EVENT,
                               pick_polygon_point );
}

private  void  remove_events( action_table )
    action_table_struct  *action_table;
{
    void   remove_action_table_function();
    void   pop_action_table();

    pop_action_table( action_table, LEFT_MOUSE_DOWN_EVENT );
    pop_action_table( action_table, LEFT_MOUSE_UP_EVENT );
    pop_action_table( action_table, TERMINATE_EVENT );
}

private  DEF_EVENT_FUNCTION( terminate_picking_polygon )
    /* ARGSUSED */
{
    void   remove_events();

    remove_events( &graphics->action_table );

    return( OK );
}

private  DEF_EVENT_FUNCTION( pick_polygon_point )
    /* ARGSUSED */
{
    int              x, y, z;
    Point            origin, transformed_origin, intersection_point;
    Vector           direction, transformed_direction;
    void             convert_mouse_to_ray();
    void             set_current_voxel();
    void             transform_world_to_model();
    void             transform_world_to_model_vector();
    graphics_struct  *slice_window;

    remove_events( &graphics->action_table );

    convert_mouse_to_ray( &graphics->three_d.view, &graphics->mouse_position,
                            &origin, &direction );

    transform_world_to_model( &graphics->three_d.view, &origin,
                              &transformed_origin );
    transform_world_to_model_vector( &graphics->three_d.view, &direction,
                                     &transformed_direction );

    if( intersect_ray_with_polygons( graphics, &transformed_origin,
                                     &transformed_direction,
                                     &intersection_point ) )
    {
        PRINT( "Intersects %g %g %g\n",
               Point_x(intersection_point),
               Point_y(intersection_point),
               Point_z(intersection_point) );

        slice_window = graphics->associated[SLICE_WINDOW];

        if( slice_window != (graphics_struct *) 0 )
        {
            if( convert_point_to_voxel( slice_window, &intersection_point,
                                        &x, &y, &z ))
            {
                set_current_voxel( slice_window, x, y, z );
            }
        }
    }

    return( OK );
}
