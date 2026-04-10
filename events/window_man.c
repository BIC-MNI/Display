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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif


#include  <display.h>

static    DEF_EVENT_FUNCTION( handle_resize );
static    DEF_EVENT_FUNCTION( handle_redraw );
static    DEF_EVENT_FUNCTION( handle_redraw_overlay );
static    DEF_EVENT_FUNCTION( handle_window_quit );

  void  initialize_resize_events(
    display_struct   *display )
{
    add_action_table_function( &display->action_table, WINDOW_RESIZE_EVENT,
                               handle_resize );
    add_action_table_function( &display->action_table, WINDOW_REDRAW_EVENT,
                               handle_redraw );
    add_action_table_function( &display->action_table, REDRAW_OVERLAY_EVENT,
                               handle_redraw_overlay );
    add_action_table_function( &display->action_table, WINDOW_QUIT_EVENT,
                               handle_window_quit );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_redraw_overlay )
{
    set_update_required( display, OVERLAY_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_redraw )
{
    set_update_required( display, NORMAL_PLANES );
    set_update_required( display, OVERLAY_PLANES );

    /* For the slice window, set_update_required alone is not enough: it
     * enables the draw_in_viewports() pass but that pass only redraws
     * per-model viewports whose viewport_update_flags are set.  After an
     * expose event (window uncovered, compositor surface loss, etc.) the
     * back buffer may be undefined/black, so every model must be redrawn.
     * set_slice_viewport_update(FULL_WINDOW_MODEL) cascades to all
     * NORMAL_PLANES models and marks both double-buffer slots dirty. */
    if( display->window_type == SLICE_WINDOW )
        set_slice_viewport_update( display, FULL_WINDOW_MODEL );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_resize )
{
    set_update_required( display, NORMAL_PLANES );
    set_update_required( display, OVERLAY_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_window_quit )
{
    return( VIO_QUIT );
}
