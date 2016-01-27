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
static    DEF_EVENT_FUNCTION( change_magnification );
static    DEF_EVENT_FUNCTION( stop_magnification );
static    DEF_EVENT_FUNCTION( increase_magnification );
static    DEF_EVENT_FUNCTION( decrease_magnification );
static    VIO_BOOL perform_magnification( display_struct *display );

static    DEF_EVENT_FUNCTION(stop_translation);
static    DEF_EVENT_FUNCTION(change_translation);

/** 
 * This function starts the magnification process. It accomplishes
 * this by installing handlers for the middle mouse down event and a
 * termination event.
 *
 * \param display The top-level \a display_struct
 */
void  initialize_magnification( display_struct *display )
{
    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_magnification );
}

static DEF_EVENT_FUNCTION(change_translation)
{
  mouse_translation_update(display);
  return VIO_OK;
}

DEF_EVENT_FUNCTION(start_translation)
{
  push_action_table(&display->action_table, NO_EVENT);

  add_action_table_function(&display->action_table,
                            NO_EVENT,
                            change_translation);
  add_action_table_function(&display->action_table,
                            LEFT_MOUSE_UP_EVENT,
                            stop_translation);
  add_action_table_function(&display->action_table,
                            TERMINATE_INTERACTION_EVENT,
                            stop_translation);
  
  record_mouse_position(display);
  return VIO_OK;
}

static DEF_EVENT_FUNCTION(stop_translation)
{
  remove_action_table_function(&display->action_table,
                               NO_EVENT,
                               change_translation);
  remove_action_table_function(&display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               stop_translation);
  remove_action_table_function(&display->action_table,
                               LEFT_MOUSE_UP_EVENT,
                               stop_translation);

  pop_action_table(&display->action_table, NO_EVENT);
  return VIO_OK;
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
    if (is_ctrl_key_pressed())
    {
        change_current_object_opacity( display, 0.05 );
    }
    else
    {
        magnify_view_size( &display->three_d.view, 1.1 );
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }
    return VIO_OK;
}

static DEF_EVENT_FUNCTION(decrease_magnification)
{
    if (is_ctrl_key_pressed())
    {
        change_current_object_opacity( display, -0.05 );
    }
    else
    {
        magnify_view_size( &display->three_d.view, 1.0 / 1.1 );
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }
    return VIO_OK;
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( start_magnification )
{
    if (!is_shift_key_pressed())
        return VIO_OK;

    push_action_table(&display->action_table, NO_EVENT);

    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               change_magnification );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               stop_magnification );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               stop_magnification );

    record_mouse_position( display );

    return( VIO_ERROR );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( stop_magnification )
{
    perform_magnification( display );
    
    remove_action_table_function( &display->action_table,
                                  NO_EVENT, change_magnification );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  stop_magnification );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  stop_magnification );

    pop_action_table(&display->action_table, NO_EVENT);
    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( change_magnification )
{
    perform_magnification( display );

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

        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }

    return( moved );
}
