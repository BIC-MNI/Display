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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/structures/action_table.c,v 1.15 1995-07-31 19:54:28 david Exp $";
#endif


#include  <display.h>

#ifdef NOT_NEEDED

/* ARGSUSED */

private  DEF_EVENT_FUNCTION(  null_function )
{
    /* function does nothing */
    return( OK );
}

#endif

private  void  set_action_table_function(
    action_table_struct   *action_table,
    Event_types           event_type,
    event_function_type   function )
{
    int                 i;
    action_table_entry  *t;

    t = &action_table->event_info[(int)event_type];

    i = t->last_index[t->stack_index];
    t->actions[i] = function;
}

public  void  add_action_table_function(
    action_table_struct   *action_table,
    Event_types           event_type,
    event_function_type   function )
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

public  void  remove_action_table_function(
    action_table_struct   *action_table,
    Event_types           event_type,
    event_function_type   function )
{
    int                 start, end, pos, i;
    action_table_entry  *t;

    t = &action_table->event_info[(int)event_type];
    end = t->last_index[t->stack_index];

    if( t->stack_index > 0 )
        start = t->last_index[t->stack_index-1]+1;
    else
        start = 0;

    if( end < start )
    {
        HANDLE_INTERNAL_ERROR( "remove action table function" );
    }
    else
    {
        for( pos = end;  pos >= start;  --pos )
        {
            if( t->actions[pos] == function )
                break;
        }

        if( pos >= start )
        {
            --t->last_index[t->stack_index];

            for_less( i, pos, end )
            {
                t->actions[i] = t->actions[i+1];
            }
        }
        else
        {
            HANDLE_INTERNAL_ERROR( "remove action table function index" );
        }
    }
}

public  void  push_action_table(
    action_table_struct   *action_table,
    Event_types           event_type )
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

public  void  pop_action_table(
    action_table_struct   *action_table,
    Event_types           event_type )
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

public  int  get_event_actions(
    action_table_struct   *action_table,
    Event_types           event_type,
    event_function_type   *actions_list[] )
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

#ifdef  NOT_NEEDED

private  void  turn_off_action(
    action_table_struct   *action_table,
    Event_types           event_type )
{
    set_action_table_function( action_table, event_type, null_function );
}

#endif

public  void  initialize_action_table(
    action_table_struct   *action_table )
{
    Event_types           event;

    for_enum( event, N_EVENT_TYPES, Event_types )
    {
        action_table->event_info[(int)event].stack_index = 0;
        action_table->event_info[(int)event].last_index[0] = -1;
    }
}
