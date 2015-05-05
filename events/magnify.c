/** 
 * \file magnify.c
 * \brief Functions to implement zooming of the 3D object view.
 *
 * \copyright
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
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

static    DEF_EVENT_FUNCTION( start_magnification );
static    DEF_EVENT_FUNCTION( turn_off_magnification );
static    DEF_EVENT_FUNCTION( handle_update );
static    DEF_EVENT_FUNCTION( terminate_magnification );
static    DEF_EVENT_FUNCTION( increase_magnification );
static    DEF_EVENT_FUNCTION( decrease_magnification );
static    VIO_BOOL perform_magnification( display_struct *display );

/** 
 * This function starts the magnification process. It accomplishes this
 * by installing handlers for the middle mouse down event (\a start_magnification)
 * and a termination event (\a turn_off_magnification).
 * \param display The top-level \a display_struct
 */
void  initialize_magnification( display_struct *display )
{
    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               turn_off_magnification );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_magnification );

}
static DEF_EVENT_FUNCTION(change_translation)
{
  mouse_translation_update(display);
  return VIO_OK;
}

static DEF_EVENT_FUNCTION(start_translation)
{
  add_action_table_function(&display->action_table,
                            NO_EVENT,
                            change_translation);
  record_mouse_position(display);
}

static DEF_EVENT_FUNCTION(stop_translation)
{
  remove_action_table_function(&display->action_table,
                               NO_EVENT,
                               change_translation);
}

/**
 * Set up event handling such that the scroll wheel will control
 * magnification of the 3D window.
 */
void initialize_scroll_magnification(display_struct *display)
{
    add_action_table_function(&display->action_table,
                              SCROLL_UP_EVENT,
                              increase_magnification);

    add_action_table_function(&display->action_table,
                              SCROLL_DOWN_EVENT,
                              decrease_magnification);

    add_action_table_function( &display->action_table,
                               RIGHT_MOUSE_DOWN_EVENT,
                               start_translation );

    add_action_table_function( &display->action_table,
                               RIGHT_MOUSE_UP_EVENT,
                               stop_translation );
}

static DEF_EVENT_FUNCTION(increase_magnification)
{
    magnify_view_size( &display->three_d.view, 1.1 );
    update_view( display );
    set_update_required( display, NORMAL_PLANES );
}

static DEF_EVENT_FUNCTION(decrease_magnification)
{
    magnify_view_size( &display->three_d.view, 1.0 / 1.1 );
    update_view( display );
    set_update_required( display, NORMAL_PLANES );
}

/* ARGSUSED */

/** Stops any magnification in progress.
 */
static  DEF_EVENT_FUNCTION( turn_off_magnification )
{
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  turn_off_magnification );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_magnification );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( start_magnification )
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

    return( VIO_OK );
}

static  void  update_magnification(
    display_struct   *display )
{
    if( perform_magnification( display ) )
    {
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( terminate_magnification )
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

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_update )
{
    update_magnification( display );

    return( VIO_OK );
}

/**
 * Scale the view size up or down according to the mouse position. 
 * Calls \c magnify_view_size to do the real work.
 */
static  VIO_BOOL  perform_magnification(
    display_struct   *display )
{
    VIO_BOOL   moved;
    VIO_Real      x, y, x_prev, y_prev, delta, factor;

    moved = FALSE;

    if( mouse_moved( display, &x, &y, &x_prev, &y_prev ) )
    {
        delta = x - x_prev;

        factor = exp( delta * log( 2.0 ) );

        magnify_view_size( &display->three_d.view, factor );

        moved = TRUE;
    }

    return( moved );
}
