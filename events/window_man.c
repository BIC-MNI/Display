
#include  <display.h>

static    DEF_EVENT_FUNCTION( handle_resize );
static    DEF_EVENT_FUNCTION( handle_redraw );
static    DEF_EVENT_FUNCTION( handle_redraw_overlay );

public  void  initialize_resize_events(
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

private  DEF_EVENT_FUNCTION( handle_redraw_overlay )
{
    set_update_required( display, OVERLAY_PLANES );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_redraw )
{
    set_update_required( display, NORMAL_PLANES );
    set_update_required( display, OVERLAY_PLANES );

    return( OK );
}

/* ARGSUSED */

private  DEF_EVENT_FUNCTION( handle_resize )
{
    set_update_required( display, NORMAL_PLANES );
    set_update_required( display, OVERLAY_PLANES );

    return( OK );
}
