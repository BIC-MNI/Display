#include  <display.h>

private  void      initialize_global_colours();
private  void      initialize_view_to_fit(
    display_struct  *display );

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

public  Status  set_global_variable_value(
    char   variable_name[],
    char   new_value[] )
{
    return( set_global_variable( SIZEOF_STATIC_ARRAY(display_globals),
                                 display_globals, variable_name, new_value ) );
}

int  main(
    int     argc,
    char    *argv[] )
{
    char             *filename;
    display_struct   *graphics;
    display_struct   *menu;
    Status           status;
    STRING           globals_filename, runtime_directory;
    char             *title, *variable_name, *variable_value;
    BOOLEAN          next_is_label_volume;

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

        next_is_label_volume = FALSE;

        while( get_string_argument( "", &filename ) )
        {
            if( strcmp( filename, "-label" ) == 0 )
            {
                if( next_is_label_volume )
                    print( "Ignoring extraneous -label\n" );
                next_is_label_volume = TRUE;
            }
            else if( strcmp( filename, "-global" ) == 0 )
            {
                if( !get_string_argument( "", &variable_name ) ||
                    !get_string_argument( "", &variable_value ) )
                {
                    print_error( "Error in arguments after -global.\n" );
                    return( 1 );
                }

                if( set_global_variable_value( variable_name, variable_value )
                                                     != OK )
                {
                    print("Error setting global variable from command line.\n");
                }
            }
            else
            {
                status = load_graphics_file( graphics, filename,
                                             next_is_label_volume );
                if( status != OK )
                    print( "Error loading %s\n", filename );
                next_is_label_volume = FALSE;
            }
        }

        if( next_is_label_volume )
            print( "Ignoring extraneous -label\n" );
    }

    initialize_view_to_fit( graphics );

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
    Brush_outline_colour = GREEN;
    Histogram_colour = WHITE;
    Menu_name_colour = GREEN;
}

private  void      initialize_view_to_fit(
    display_struct  *display )
{
    int      i, c, x, y, z;
    Real     voxel[N_DIMENSIONS], world[N_DIMENSIONS];
    int      sizes[N_DIMENSIONS];
    Volume   volume;
    BOOLEAN  found;

    found = get_range_of_object( display->models[THREED_MODEL], FALSE,
                                 &display->three_d.min_limit,
                                 &display->three_d.max_limit );

    for_less( i, 0, get_n_volumes(display) )
    {
        volume = get_nth_volume( display, i );

        get_volume_sizes( volume, sizes );

        for_less( x, 0, 2 )
        for_less( y, 0, 2 )
        for_less( z, 0, 2 )
        {
            voxel[X] = -0.5 + (Real) sizes[X] * (Real) x;
            voxel[Y] = -0.5 + (Real) sizes[Y] * (Real) y;
            voxel[Z] = -0.5 + (Real) sizes[Z] * (Real) z;

            convert_voxel_to_world( volume, voxel,
                                    &world[X], &world[Y], &world[Z] );

            for_less( c, 0, N_DIMENSIONS )
            {
                if( !found ||
                    world[c] < Point_coord(display->three_d.min_limit,c) )
                    Point_coord(display->three_d.min_limit,c) = world[c];
                if( !found ||
                    world[c] > Point_coord(display->three_d.max_limit,c) )
                    Point_coord(display->three_d.max_limit,c) = world[c];
            }
            
            found = TRUE;
        }
    }

    if( !found )
    {
        fill_Point( display->three_d.min_limit, -1.0, -1.0, -1.0 );
        fill_Point( display->three_d.max_limit, 1.0, 1.0, 1.0 );
    }

    ADD_POINTS( display->three_d.centre_of_objects,
                display->three_d.min_limit,
                display->three_d.max_limit );
    SCALE_POINT( display->three_d.centre_of_objects,
                 display->three_d.centre_of_objects,
                 0.5 );

    reset_cursor( display );
}
