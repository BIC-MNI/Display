
#include  <def_standard.h>
#include  <def_graphics.h>

private  DEF_EVENT_FUNCTION(  null_function )     /* ARGSUSED */
{
    /* function does nothing */
    return( OK );
}

public  void  turn_off_action( action_table, event_type )
    action_table_struct   *action_table;
    event_types           event_type;
{
    DECL_EVENT_FUNCTION(  null_function );

    action_table->actions[(int)event_type] = null_function;
}

public  void  initialize_mouse_movement( action_table )
    action_table_struct   *action_table;
{
    DECL_EVENT_FUNCTION(  mouse_movement_event );

    action_table->actions[(int)MOUSE_MOVEMENT_EVENT] = mouse_movement_event;
}

public  void  initialize_action_table( action_table )
    action_table_struct   *action_table;
{
    event_types           event;
    DECL_EVENT_FUNCTION(  keyboard_event );
    DECL_EVENT_FUNCTION(  initialize_virtual_spaceball );
    DECL_EVENT_FUNCTION(  middle_mouse_down_event );
    DECL_EVENT_FUNCTION(  right_mouse_down_event );

    for_enum( event, NUM_EVENT_TYPES, event_types )
    {
        turn_off_action( action_table, event );
    }

    initialize_mouse_movement( action_table );

    action_table->actions[(int)LEFT_MOUSE_DOWN_EVENT] =
                                     initialize_virtual_spaceball;
    action_table->actions[(int)MIDDLE_MOUSE_DOWN_EVENT] =
                          middle_mouse_down_event;

    action_table->actions[(int)RIGHT_MOUSE_DOWN_EVENT] = right_mouse_down_event;
/*
    action_table->actions[(int)KEYBOARD_EVENT] = keyboard_event;
*/
}

#include  <stdio.h>

private  DEF_EVENT_FUNCTION(  keyboard_event )     /* ARGSUSED */
{
    PRINT( "%c", event->event_data.key_pressed );
    (void) fflush( stdout );

    return( OK );
}

private  DEF_EVENT_FUNCTION(  mouse_movement_event )     /* ARGSUSED */
{
    graphics->mouse_position = event->event_data.mouse_position;

    return( OK );
}

private  DEF_EVENT_FUNCTION(  left_mouse_down_event )     /* ARGSUSED */
{
    PRINT( "LEFT Mouse\n" );

    return( OK );
}

private  DEF_EVENT_FUNCTION(  middle_mouse_down_event )     /* ARGSUSED */
{
    PRINT( "MIDDLE Mouse\n" );

    return( ERROR );
}

private  DEF_EVENT_FUNCTION(  right_mouse_down_event )     /* ARGSUSED */
{
    object_struct  *objects;

    objects = graphics->objects;

    if( objects != (object_struct *) 0 )
    {
        while( objects != (object_struct *) 0 && objects->visibility == OFF )
        {
            objects = objects->next;
        }

        if( objects == (object_struct *) 0 )
        {
            graphics->objects->visibility = ON;
        }
        else
        {
            objects->visibility = OFF;

            if( objects->next == (object_struct *) 0 )
            {
                graphics->objects->visibility = ON;
            }
            else
            {
                objects->next->visibility = ON;
            }
        }

        graphics->update_required = TRUE;
    }

    return( OK );
}
