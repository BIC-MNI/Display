#include  <display.h>

private  void      initialize_global_colours();

#define  HARD_CODED_DISPLAY_DIRECTORY1    "/usr/local/mni/lib"
#define  HARD_CODED_DISPLAY_DIRECTORY2    "/usr/local/lib"
#define  GLOBALS_FILENAME                 "Display.globals"
#define  MENU_FILENAME                    "Display.menu"


#define  GLOBALS_LOOKUP_NAME  display_globals
#include  <globals.h>
#define   DISPLAY_GLOBALS_FILENAME   "Display.globals"

public  Status  change_global_variable(
    char   str[],
    char   variable_name[],
    char   new_value[] )
{
    return( set_or_get_global_variable(
                   SIZEOF_STATIC_ARRAY(display_globals),
                   display_globals, str, variable_name, new_value ) );
}


int  main( argc, argv )
    int     argc;
    char    *argv[];
{
    char             *filename;
    display_struct   *graphics;
    display_struct   *menu;
    Status           status;
    STRING           globals_filename, runtime_directory;
    char             *title;

    if( argc == 1 )
        title = argv[0];
    else
        title = argv[1];

    initialize_global_colours();

    if( getenv( "DISPLAY_DIRECTORY" ) != (char *) NULL )
        (void) strcpy( runtime_directory, getenv( "DISPLAY_DIRECTORY" ) );
    else
        extract_directory( argv[0], runtime_directory );

    (void) sprintf( globals_filename, "%s/%s", HARD_CODED_DISPLAY_DIRECTORY2,
                    DISPLAY_GLOBALS_FILENAME );

    if( file_exists( globals_filename ) )
    {
        status = input_globals_file( SIZEOF_STATIC_ARRAY(display_globals),
                                     display_globals, globals_filename );
    }

    (void) sprintf( globals_filename, "%s/%s", HARD_CODED_DISPLAY_DIRECTORY1,
                    DISPLAY_GLOBALS_FILENAME );

    if( file_exists( globals_filename ) )
    {
        status = input_globals_file( SIZEOF_STATIC_ARRAY(display_globals),
                                     display_globals, globals_filename );
    }

    (void) sprintf( globals_filename, "%s/%s", runtime_directory,
                    DISPLAY_GLOBALS_FILENAME );

    if( file_exists( globals_filename ) )
    {
        status = input_globals_file( SIZEOF_STATIC_ARRAY(display_globals),
                                     display_globals, globals_filename );
    }

    (void) sprintf( globals_filename, "%s/%s", getenv("HOME"),
                    DISPLAY_GLOBALS_FILENAME );

    if( file_exists( globals_filename ) )
    {
        status = input_globals_file( SIZEOF_STATIC_ARRAY(display_globals),
                                     display_globals, globals_filename );
    }

    (void) strcpy( globals_filename, DISPLAY_GLOBALS_FILENAME );

    if( file_exists( globals_filename ) )
    {
        status = input_globals_file( SIZEOF_STATIC_ARRAY(display_globals),
                                     display_globals, globals_filename );
    }

    set_alloc_checking( Alloc_checking_enabled );

    status = OK;

    if( status == OK )
    {
        initialize_graphics();

        status = create_graphics_window( THREE_D_WINDOW,
                                         Graphics_double_buffer_flag,
                                         &graphics, title, 0, 0 );
    }

    if( status == OK )
    {
        status = create_graphics_window( MENU_WINDOW, ON, &menu, title,
                                         Menu_window_width,
                                         Menu_window_height );
    }

    if( status == OK )
    {
        graphics->associated[THREE_D_WINDOW] = graphics;
        graphics->associated[MENU_WINDOW] = menu;
        graphics->associated[SLICE_WINDOW] = (display_struct *) 0;

        menu->associated[THREE_D_WINDOW] = graphics;
        menu->associated[MENU_WINDOW] = menu;
        menu->associated[SLICE_WINDOW] = (display_struct *) 0;

        status = initialize_menu( menu, runtime_directory,
                                  getenv( "HOME" ),
                                  HARD_CODED_DISPLAY_DIRECTORY1,
                                  HARD_CODED_DISPLAY_DIRECTORY2,
                                  MENU_FILENAME );
    }

    if( status == OK )
    {
        initialize_argument_processing( argc, argv );

        while( get_string_argument( "", &filename ) )
        {
            status = load_graphics_file( graphics, filename );
            if( status != OK )
                print( "Error loading %s\n", filename );
        }
    }

    if( status == OK )
        rebuild_selected_list( graphics, menu );

    if( status == OK )
    {
        reset_view_parameters( graphics, &Default_line_of_sight,
                               &Default_horizontal );

        update_view( graphics );

        update_all_menu_text( graphics );

        set_update_required( graphics, NORMAL_PLANES );
    }

    if( status == OK )
        status = main_event_loop();

    terminate_graphics();

    if( status == OK )
        delete_marching_cubes_table();

    output_alloc_to_file( ".alloc_stats" );

    if( status != OK )
        print( "Program ended with error %d\n", (int) status );

    return( (int) status );
}

private  void      initialize_global_colours()
{
    Initial_background_colour = DARK_SLATE_GREY;
    Volume_outline_colour = RED;
    Cross_section_colour = GREEN;
    Ambient_light_colour = WHITE;
    Directional_light_colour = WHITE;
    Menu_character_colour = CYAN;
    Menu_character_inactive_colour = SLATE_GREY;
    Menu_box_colour = WHITE;
    Menu_key_colour = WHITE;
    Default_marker_colour = GREEN;
    Selected_colour = GREEN;
    Visible_colour = WHITE;
    Invisible_colour = GREY;
    Viewport_feedback_colour = GREEN;
    Slice_divider_colour = BLUE;
    Slice_text_colour = GREEN;
    Readout_text_rgb_colour = GREEN;
    Cursor_rgb_colour = BLUE;
    Slice_cursor_colour1 = RED;
    Slice_cursor_colour2 = BLUE;
    Slice_cross_section_colour = GREEN;
    Slice_crop_box_colour = GREEN;
    Extracted_surface_colour = WHITE;
    Colour_below = BLACK;
    User_defined_min_colour = BLACK;
    User_defined_max_colour = WHITE;
    Colour_above = WHITE;
    Colour_bar_tick_colour = WHITE;
    Colour_bar_text_colour = WHITE;
    Colour_bar_limit_colour = GREEN;
    Colour_bar_range_colour = RED;
    Surface_curve_colour = MAGENTA;
    Visible_segmenting_colour = GREEN;
    Invisible_segmenting_colour = BLACK;
    Brush_outline_colour = WHITE;
    Histogram_colour = WHITE;
    Menu_name_colour = GREEN;
}
