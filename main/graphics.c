
#include  <def_graphics.h>

public  void  initialize_graphics( graphics )
    graphics_struct   *graphics;
{
    void    initialize_view();
    void    initialize_lights();
    void    initialize_action_table();
    void    initialize_render();
    void    initialize_objects();

    initialize_view( &graphics->view );
    initialize_lights( graphics->lights );
    initialize_action_table( &graphics->action_table );
    initialize_render( &graphics->render );
    initialize_objects( &graphics->objects );

    graphics->update_required = FALSE;
}

public  void  update_graphics( graphics )
{
    void   G_update_window();

    G_update_window( graphics->window );

    graphics->update_required = FALSE;
}
