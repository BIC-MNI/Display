#include  <def_stdio.h>
#include  <def_string.h>
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_arguments.h>

int  main( argc, argv )
    int     argc;
    char    *argv[];
{
    arguments_struct arguments;
    char             *filename;
    String           runtime_directory;
    void             extract_directory();
    graphics_struct  *graphics;
    graphics_struct  *menu;
    Status           status;
    Status           initialize_graphics();
    Status           initialize_globals();
    Status           initialize_menu();
    Status           load_graphics_file();
    Status           create_graphics_window();
    Status           main_event_loop();
    Status           terminate_graphics();
    void             reset_view_parameters();
    void             update_view();
    void             set_model_scale();
    void             rebuild_selected_list();
    void             set_update_required();
    void             output_alloc_to_file();
    Status           delete_marching_cubes_table();

    if( getenv("DISPLAY_DIRECTORY") != (char *) 0 )
    {
        (void) strcpy( runtime_directory, getenv("DISPLAY_DIRECTORY") );
    }
    else
    {
        extract_directory( argv[0], runtime_directory );
    }

    status = initialize_globals( runtime_directory );

    if( status == OK )
    {
        status = initialize_graphics();
    }

    if( status == OK )
    {
        status = create_graphics_window( THREE_D_WINDOW,
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
        graphics->associated[THREE_D_WINDOW] = graphics;
        graphics->associated[MENU_WINDOW] = menu;
        graphics->associated[SLICE_WINDOW] = (graphics_struct *) 0;

        menu->associated[THREE_D_WINDOW] = graphics;
        menu->associated[MENU_WINDOW] = menu;
        menu->associated[SLICE_WINDOW] = (graphics_struct *) 0;

        status = initialize_menu( menu, runtime_directory );
    }

    if( status == OK )
    {
        status = initialize_argument_processing( argc, argv, &arguments );
    }

    if( status == OK )
    {
        while( get_string_argument( &arguments, "", &filename ) )
        {
            status = load_graphics_file( graphics, filename );
        }
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

        set_update_required( graphics, NORMAL_PLANES );
    }

    if( status == OK )
    {
        status = main_event_loop();
    }

    (void) terminate_graphics();

    if( status == OK )
    {
        status = delete_marching_cubes_table();
    }

    output_alloc_to_file( ".alloc_stats" );

    if( status != OK )
    {
        PRINT( "Program ended with error %d\n", (int) status );
    }

    return( (int) status );
}
