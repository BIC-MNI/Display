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
    int              i;
    model_struct     *model;
    model_struct     *get_graphics_model();
    Boolean          get_range_of_object();
    void             rebuild_selected_list();

    status = initialize_globals();

    if( status == OK )
    {
        status = G_initialize();
    }

    if( status == OK )
    {
        status = create_graphics_window( &graphics, argv[1], 0, 0 );
    }

    if( status == OK )
    {
        status = create_graphics_window( &menu, argv[1],
                                         Menu_window_width,
                                         Menu_window_height );
    }

    if( status == OK )
    {
        graphics->graphics_window = graphics;
        graphics->menu_window = menu;
        menu->graphics_window = graphics;
        menu->menu_window = menu;

        for_less( i, 0, N_MODELS )
        {
            model = get_graphics_model( graphics, i );
            model->view_type = MODEL_VIEW;
        }

        for_less( i, 0, N_MODELS )
        {
            model = get_graphics_model( menu, i );
            model->view_type = PIXEL_VIEW;
        }

        status = initialize_menu( menu );
    }

    if( status == OK )
    {
        rebuild_selected_list( graphics, menu );
    }

    if( status == OK )
    {
        fill_Point( graphics->min_limit, 0.0, 0.0, 0.0 );
        fill_Point( graphics->max_limit, 1.0, 1.0, 1.0 );

        ADD_POINTS( graphics->centre_of_objects, graphics->min_limit,
                    graphics->max_limit );
        SCALE_POINT( graphics->centre_of_objects,
                     graphics->centre_of_objects,
                     0.5 );
    }

    if( status == OK && argc > 1 )
    {
        status = load_graphics_file( graphics, argv[1] );
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
