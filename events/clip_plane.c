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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/events/clip_plane.c,v 1.15 1995-10-19 15:51:18 david Exp $";
#endif


#include  <display.h>

private    DEF_EVENT_FUNCTION( start_front_clipping );
private    DEF_EVENT_FUNCTION( turn_off_front_clipping );
private    DEF_EVENT_FUNCTION( handle_update_front );
private    DEF_EVENT_FUNCTION( terminate_front_clipping );
private    DEF_EVENT_FUNCTION( start_back_clipping );
private    DEF_EVENT_FUNCTION( turn_off_back_clipping );
private    DEF_EVENT_FUNCTION( handle_update_back );
private    DEF_EVENT_FUNCTION( terminate_back_clipping );
private  BOOLEAN  perform_clipping(
    display_struct   *display,
    BOOLEAN          front_flag );

public  void  initialize_front_clipping(
    display_struct   *display )
{
    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               turn_off_front_clipping );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_front_clipping );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( turn_off_front_clipping )
{
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_front_clipping );

    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  turn_off_front_clipping );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( start_front_clipping )
{
    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               handle_update_front );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_front_clipping );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_front_clipping );

    record_mouse_position( display );

    return( OK );
}

private  void  update_clipping(
    display_struct   *display,
    BOOLEAN          front_flag )
{
    if( perform_clipping( display, front_flag ) )
    {
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( terminate_front_clipping )
{
    update_clipping( display, TRUE );
    
    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update_front );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_front_clipping );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_front_clipping );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_update_front )
{
    update_clipping( display, TRUE );

    return( OK );
}

public  void  initialize_back_clipping(
    display_struct   *display )
{
    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               turn_off_back_clipping );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_back_clipping );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( turn_off_back_clipping )
{
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  turn_off_back_clipping );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_back_clipping );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( start_back_clipping )
{
    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               handle_update_back );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_back_clipping );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_back_clipping );

    record_mouse_position( display );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( terminate_back_clipping )
{
    update_clipping( display, FALSE );
    
    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update_back );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_back_clipping );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_back_clipping );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_update_back )
{
    update_clipping( display, FALSE );

    return( OK );
}

private  BOOLEAN  perform_clipping(
    display_struct   *display,
    BOOLEAN          front_flag )
{
    BOOLEAN   moved;
    Real      x, y, x_prev, y_prev;
    Real      delta, dist;

    moved = FALSE;

    if( mouse_moved( display, &x, &y, &x_prev, &y_prev ) )
    {
        delta = x - x_prev;

        dist = delta * display->three_d.view.back_distance;

        if( front_flag )
        {
            display->three_d.view.front_distance += dist;

            if( display->three_d.view.front_distance < Closest_front_plane )
                display->three_d.view.front_distance = Closest_front_plane;

            if( display->three_d.view.front_distance >
                display->three_d.view.back_distance )
            {
                display->three_d.view.front_distance =
                display->three_d.view.back_distance;
            }
        }
        else
        {
            display->three_d.view.back_distance += dist;

            if( display->three_d.view.back_distance <
                display->three_d.view.front_distance )
            {
                display->three_d.view.back_distance =
                display->three_d.view.front_distance;
            }
        }
    
        moved = TRUE;
    }

    return( moved );
}
