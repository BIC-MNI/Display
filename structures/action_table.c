
#include  <def_standard.h>
#include  <def_graphics.h>

private  DEF_EVENT_FUNCTION(  null_function )     /* ARGSUSED */
{
    /* function does nothing */
    return( OK );
}

private  void  set_action_table_function( action_table, event_type, function )
    action_table_struct   *action_table;
    event_types           event_type;
    event_function_type   function;
{
    int                 i;
    action_table_entry  *t;

    t = &action_table->event_info[(int)event_type];

    i = t->last_index[t->stack_index];
    t->actions[i] = function;
}

public  void  add_action_table_function( action_table, event_type, function )
    action_table_struct   *action_table;
    event_types           event_type;
    event_function_type   function;
{
    action_table_entry  *t;

    t = &action_table->event_info[(int)event_type];

    if( t->last_index[t->stack_index] >= MAX_ACTIONS-1 )
    {
        HANDLE_INTERNAL_ERROR( "add action table function" );
    }
    else
    {
        ++t->last_index[t->stack_index];
        set_action_table_function( action_table, event_type, function );
    }
}

public  void  remove_action_table_function( action_table, event_type )
    action_table_struct   *action_table;
    event_types           event_type;
{
    int                 i, min_allowed;
    action_table_entry  *t;

    t = &action_table->event_info[(int)event_type];
    i = t->last_index[t->stack_index];

    if( t->stack_index > 0 )
        min_allowed = t->last_index[t->stack_index-1]+1;
    else
        min_allowed = 0;

    if( i < min_allowed )
    {
        HANDLE_INTERNAL_ERROR( "remove action table function" );
    }
    else
    {
        --t->last_index[t->stack_index];
    }
}

public  void  push_action_table( action_table, event_type )
    action_table_struct   *action_table;
    event_types           event_type;
{
    action_table_entry  *t;

    t = &action_table->event_info[(int)event_type];

    if( t->stack_index >= MAX_ACTION_STACK-1 )
    {
        HANDLE_INTERNAL_ERROR( "push action table" );
    }
    else
    {
        ++t->stack_index;
        t->last_index[t->stack_index] = t->last_index[t->stack_index-1];
    }
}

public  void  pop_action_table( action_table, event_type )
    action_table_struct   *action_table;
    event_types           event_type;
{
    action_table_entry  *t;

    t = &action_table->event_info[(int)event_type];

    if( t->stack_index <= 0 )
    {
        HANDLE_INTERNAL_ERROR( "pop action table" );
    }
    else
    {
        --t->stack_index;
    }
}

public  int  get_event_actions( action_table, event_type, actions_list )
    action_table_struct   *action_table;
    event_types           event_type;
    event_function_type   *actions_list[];
{
    int                 n_actions, start_i, end_i;
    action_table_entry  *t;

    t = &action_table->event_info[(int)event_type];

    if( t->stack_index == 0 )
        start_i = 0;
    else
        start_i = t->last_index[t->stack_index-1] + 1;

    end_i = t->last_index[t->stack_index];

    n_actions = end_i - start_i + 1;

    if( n_actions > 0 )
    {
        *actions_list = &t->actions[start_i];
    }

    return( n_actions );
}

private  void  turn_off_action( action_table, event_type )
    action_table_struct   *action_table;
    event_types           event_type;
{
    set_action_table_function( action_table, event_type, null_function );
}

public  void  initialize_action_table( action_table )
    action_table_struct   *action_table;
{
    event_types           event;
    DECL_EVENT_FUNCTION(   mouse_movement_event );
    DECL_EVENT_FUNCTION(   initialize_mouse_movement );
    DECL_EVENT_FUNCTION(   initialize_virtual_spaceball );
    DECL_EVENT_FUNCTION(   keyboard_event );
    DECL_EVENT_FUNCTION(   left_mouse_down_event );
    DECL_EVENT_FUNCTION(   middle_mouse_down_event );
    DECL_EVENT_FUNCTION(   right_mouse_down_event );

    for_enum( event, NUM_EVENT_TYPES, event_types )
    {
        action_table->event_info[(int)event].stack_index = 0;
        action_table->event_info[(int)event].last_index[0] = -1;
    }

    add_action_table_function( action_table, MOUSE_MOVEMENT_EVENT,
                               mouse_movement_event );
    add_action_table_function( action_table, LEFT_MOUSE_DOWN_EVENT,
                               left_mouse_down_event );
    add_action_table_function( action_table, MIDDLE_MOUSE_DOWN_EVENT,
                               middle_mouse_down_event );
    add_action_table_function( action_table, RIGHT_MOUSE_DOWN_EVENT,
                               right_mouse_down_event );
    add_action_table_function( action_table, KEYBOARD_EVENT,
                               keyboard_event );
}

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
