
#include  <def_graphics.h>

static    DECL_EVENT_FUNCTION( handle_resize );
static    DECL_EVENT_FUNCTION( handle_redraw );

public  void  initialize_window_events( graphics )
    graphics_struct  *graphics;
{
    void                 add_action_table_function();

    add_action_table_function( &graphics->action_table, WINDOW_RESIZE_EVENT,
                               handle_resize );
    add_action_table_function( &graphics->action_table, WINDOW_REDRAW_EVENT,
                               handle_redraw );
}

private  DEF_EVENT_FUNCTION( handle_redraw )
    /* ARGSUSED */
{
    void     set_update_required();

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

private  DEF_EVENT_FUNCTION( handle_resize )
    /* ARGSUSED */
{
    void   G_update_window_size();
    void   set_update_required();

    G_update_window_size( &graphics->window );

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}
