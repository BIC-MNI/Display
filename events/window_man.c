
#include  <def_graphics.h>

public  void  initialize_window_events( graphics )
    graphics_struct  *graphics;
{
    DECL_EVENT_FUNCTION( handle_resize );
    DECL_EVENT_FUNCTION( handle_redraw );
    void                 add_action_table_function();

    add_action_table_function( &graphics->action_table, WINDOW_RESIZE_EVENT,
                               handle_resize );
    add_action_table_function( &graphics->action_table, WINDOW_REDRAW_EVENT,
                               handle_redraw );
}

private  DEF_EVENT_FUNCTION( handle_redraw )
    /* ARGSUSED */
{
    graphics->update_required = TRUE;

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_resize )
    /* ARGSUSED */
{
    void   G_update_window_size();

    G_update_window_size( &graphics->window );

    graphics->update_required = TRUE;

    return( OK );
}
