#include  <stdio.h>
#include  <math.h>
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
    Status           input_graphics_file();
    Status           create_graphics_window();
    Status           delete_graphics_window();
    Status           main_event_loop();
    Status           G_terminate();
    Status           make_all_invisible();
    void             reset_view_parameters();
    void             update_view();
    void             get_range_of_objects();

    if( argc != 2 )
    {
        PRINT_ERROR( "Argument.\n" );
        (void) abort();
    }

    status = initialize_globals();

    if( status == OK )
    {
        status = G_initialize();
    }

    if( status == OK )
    {
        status = create_graphics_window( &graphics );
    }

    if( status == OK )
    {
        status = create_graphics_window( &menu );
    }

    if( status == OK )
    {
        graphics->graphics_window = graphics;
        graphics->menu_window = menu;
        menu->graphics_window = graphics;
        menu->menu_window = menu;

        graphics->model.view_type = MODEL_VIEW;
        menu->model.view_type = PIXEL_VIEW;

        status = initialize_menu( menu );
    }

    if( status == OK )
    {
        PRINT( "Inputting objects.\n" );

        status = input_graphics_file( argv[1], &graphics->model.objects );

        graphics->update_required = TRUE;

        PRINT( "Objects input.\n" );
    }

    if( status == OK )
    {
        if( !Visibility_on_input )
        {
            status = make_all_invisible( graphics, menu,
                                         (menu_entry_struct *) 0);
        }
    }

    if( status == OK )
    {
        get_range_of_objects( graphics->model.objects, &graphics->min_limit,
                              &graphics->max_limit );

        ADD_POINTS( graphics->centre_of_objects, graphics->min_limit,
                    graphics->max_limit );
        SCALE_POINT( graphics->centre_of_objects, graphics->centre_of_objects,
                     0.5 );

        reset_view_parameters( graphics );

        update_view( graphics );
    }

    if( status == OK )
    {
        status = main_event_loop();
    }

    if( status == OK )
    {
        status = delete_graphics_window( graphics );
    }

    if( status == OK )
    {
        status = G_terminate();
    }

    return( (int) status );
}
