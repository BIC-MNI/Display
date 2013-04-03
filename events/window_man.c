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

  void  initialize_resize_events(
    display_struct   *display )
{
    add_action_table_function( &display->action_table, WINDOW_RESIZE_EVENT,
                               handle_resize );
    add_action_table_function( &display->action_table, WINDOW_REDRAW_EVENT,
                               handle_redraw );
    add_action_table_function( &display->action_table, REDRAW_OVERLAY_EVENT,
                               handle_redraw_overlay );
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

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_resize )
{
    set_update_required( display, NORMAL_PLANES );
    set_update_required( display, OVERLAY_PLANES );

    return( VIO_OK );
}
