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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/events/magnify.c,v 1.15 1995-07-31 19:54:02 david Exp $";
#endif


#include  <display.h>

static    DEF_EVENT_FUNCTION( start_magnification );
static    DEF_EVENT_FUNCTION( turn_off_magnification );
static    DEF_EVENT_FUNCTION( handle_update );
static    DEF_EVENT_FUNCTION( terminate_magnification );
private  BOOLEAN  perform_magnification(
    display_struct   *display );

public  void  initialize_magnification(
    display_struct   *display )
{
    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               turn_off_magnification );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_magnification );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( turn_off_magnification )
{
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  turn_off_magnification );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_magnification );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( start_magnification )
{
    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               handle_update );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_magnification );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_magnification );

    record_mouse_position( display );

    return( OK );
}

private  void  update_magnification(
    display_struct   *display )
{
    if( perform_magnification( display ) )
    {
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( terminate_magnification )
{
    update_magnification( display );
    
    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_magnification );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_magnification );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_update )
{
    update_magnification( display );

    return( OK );
}

private  BOOLEAN  perform_magnification(
    display_struct   *display )
{
    BOOLEAN   moved;
    Real      x, y, x_prev, y_prev, delta, factor;

    moved = FALSE;

    if( mouse_moved( display, &x, &y, &x_prev, &y_prev ) )
    {
        delta = x - x_prev;

        factor = exp( -delta * log( 2.0 ) );

        magnify_view_size( &display->three_d.view, factor );

        moved = TRUE;
    }

    return( moved );
}
