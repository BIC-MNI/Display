
#include  <def_standard.h>
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_mouse_events( action_table )
    action_table_struct   *action_table;
{
    DECL_EVENT_FUNCTION(   mouse_movement_event );
    DECL_EVENT_FUNCTION(   middle_mouse_down_event );
    DECL_EVENT_FUNCTION(   right_mouse_down_event );
    void                   add_action_table_function();

    add_action_table_function( action_table, MOUSE_MOVEMENT_EVENT,
                               mouse_movement_event );
    add_action_table_function( action_table, MIDDLE_MOUSE_DOWN_EVENT,
                               middle_mouse_down_event );
    add_action_table_function( action_table, RIGHT_MOUSE_DOWN_EVENT,
                               right_mouse_down_event );
}

private  DEF_EVENT_FUNCTION(  mouse_movement_event )     /* ARGSUSED */
{
    graphics->mouse_position = event->event_data.mouse_position;

    return( OK );
}

private  DEF_EVENT_FUNCTION(  middle_mouse_down_event )     /* ARGSUSED */
{
    PRINT( "MIDDLE Mouse pressed, exitting.\n" );

    return( ERROR );
}

private  DEF_EVENT_FUNCTION(  right_mouse_down_event )     /* ARGSUSED */
{
    void                  add_action_table_function();
    DECL_EVENT_FUNCTION(  handle_no_event );
    DECL_EVENT_FUNCTION(  terminate_clipping );

    add_action_table_function( &graphics->action_table,
                               NO_EVENT,
                               handle_no_event );
    add_action_table_function( &graphics->action_table,
                               RIGHT_MOUSE_UP_EVENT,
                               terminate_clipping );

    graphics->prev_mouse_position = graphics->mouse_position;

    return( OK );
}

private  DEF_EVENT_FUNCTION(  handle_no_event )
{
    void    process_clipping();

    process_clipping( graphics );

    graphics->prev_mouse_position = graphics->mouse_position;

    return( OK );
}

private  DEF_EVENT_FUNCTION(  terminate_clipping )
{
    void                  remove_action_table_function();
    void                  process_clipping();

    process_clipping( graphics );

    remove_action_table_function( &graphics->action_table,
                                  NO_EVENT );
    remove_action_table_function( &graphics->action_table,
                                  RIGHT_MOUSE_UP_EVENT );

    return( OK );
}

private  void  process_clipping( graphics )
    graphics_struct   *graphics;
{
    Real   delta_x, new_front;
    void   update_view();

    delta_x = Point_x(graphics->mouse_position) -
              Point_x(graphics->prev_mouse_position );

    if( delta_x != 0.0 )
    {
        new_front = graphics->view.front_distance + delta_x *
                    (graphics->view.back_distance -
                     graphics->view.front_distance);

        if( new_front <= 0.0 )
        {
            new_front = Closest_front_plane;
        }
        else if( new_front > graphics->view.back_distance )
        {
            new_front = graphics->view.back_distance;
        }

        if( new_front != graphics->view.front_distance )
        {
            graphics->view.front_distance = new_front;
            update_view( graphics );
            graphics->update_required = TRUE;
        }
    }
}
