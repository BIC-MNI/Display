#include  <stdio.h>
#include  <def_graphics.h>
#include  <def_globals.h>

int  main( argc, argv )
    int     argc;
    char    *argv[];
{
    graphics_struct  *graphics;
    graphics_struct  *menu;
    Status           status;
    Status           G_initialize();
    Status           initialize_globals();
    Status           initialize_menu();
    Status           load_graphics_file();
    Status           create_graphics_window();
    Status           delete_graphics_window();
    Status           main_event_loop();
    Status           G_terminate();
    void             reset_view_parameters();
    void             update_view();
    void             set_model_scale();
    void             rebuild_selected_list();
    void             output_alloc_to_file();

    status = initialize_globals();

    if( status == OK )
    {
        status = G_initialize();
    }

    if( status == OK )
    {
        status = create_graphics_window( GRAPHICS_WINDOW,
                                         &graphics, argv[1], 0, 0 );
    }

    if( status == OK )
    {
        status = create_graphics_window( MENU_WINDOW, &menu, argv[1],
                                         Menu_window_width,
                                         Menu_window_height );
    }

    if( status == OK )
    {
        graphics->graphics_window = graphics;
        graphics->menu_window = menu;
        graphics->slice_window = (graphics_struct *) 0;

        menu->graphics_window = graphics;
        menu->menu_window = menu;
        menu->slice_window = (graphics_struct *) 0;

        status = initialize_menu( menu );
    }

    if( status == OK && argc > 1 )
    {
        status = load_graphics_file( graphics, argv[1] );
    }

    if( status == OK )
    {
        rebuild_selected_list( graphics, menu );
    }

    if( status == OK )
    {
        reset_view_parameters( graphics, &Default_line_of_sight,
                               &Default_horizontal );

        update_view( graphics );
    }

    if( status == OK )
    {
        status = main_event_loop();
    }

    if( status == OK && graphics->slice_window != (graphics_struct *) 0 )
    {
        status = delete_graphics_window( graphics->slice_window );
    }

    if( status == OK )
    {
        status = delete_graphics_window( graphics );
    }

    if( status == OK )
    {
        status = delete_graphics_window( menu );
    }

    if( status == OK )
    {
        status = G_terminate();
    }

    output_alloc_to_file( ".alloc_stats" );

    return( (int) status );
}
