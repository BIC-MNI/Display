
#include  <def_standard.h>
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  initialize_mouse_events( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION(   mouse_movement_event );
    void                   add_action_table_function();
    void                   terminate_any_interactions();

    terminate_any_interactions( graphics );

    add_action_table_function( &graphics->action_table,
                               MOUSE_MOVEMENT_EVENT,
                               mouse_movement_event );
}

private  DEF_EVENT_FUNCTION(  mouse_movement_event )     /* ARGSUSED */
{
    graphics->mouse_position = event->event_data.mouse_position;

    return( OK );
}

public  Boolean  mouse_moved( graphics )
    graphics_struct  *graphics;
{
    Boolean   moved;

    moved = !EQUAL_POINTS( graphics->mouse_position,
                           graphics->prev_mouse_position );

    graphics->prev_mouse_position = graphics->mouse_position;

    return( moved );
}
