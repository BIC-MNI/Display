/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/main/main.c,v 1.65 2010-10-29 19:07:52 jgsled Exp $";
#endif

#include  <display.h>

private  STRING   version = "1.5      Oct 29, 2010";

#define  MAX_TITLE_LENGTH   200

/* These two globals are required for SEAL's LibSul/SULGRAPHDATA */
int debug = 1;
int verbose = 1;

private  void      initialize_global_colours( void );
private  void      initialize_view_to_fit(
    display_struct  *display );

/* The first directory is set using compiler flag -D */
/*#define  HARD_CODED_DISPLAY_DIRECTORY1    "/usr/local/mni/lib"*/
#define  HARD_CODED_DISPLAY_DIRECTORY2    "/usr/local/lib"
#define  GLOBALS_FILENAME                 "Display.globals"
#define  MENU_FILENAME                    "Display.menu"


#define  GLOBALS_LOOKUP_NAME  display_globals
#include  <bicpl/globals.h>
#define   DISPLAY_GLOBALS_FILENAME   "Display.globals"

public  Status  change_global_variable(
    STRING   str,
    STRING   *variable_name,
    STRING   *new_value )
{
    return( set_or_get_global_variable(
                   SIZEOF_STATIC_ARRAY(display_globals),
                   display_globals, str, variable_name, new_value ) );
}

public  Status  set_global_variable_value(
    STRING   variable_name,
    STRING   new_value )
{
    return( set_global_variable( SIZEOF_STATIC_ARRAY(display_globals),
                                 display_globals, variable_name, new_value ) );
}

int  main(
    int     argc,
    char    *argv[] )
{
    int              i, view;
    STRING           filename;
    display_struct   *graphics;
    display_struct   *menu, *slice_window;
    STRING           globals_filename, runtime_directory;
    int              n_directories;
    STRING           *directories;
    STRING           title;
    STRING           variable_name, variable_value;
    BOOLEAN          next_is_label_volume;

    set_alloc_checking( TRUE );

    if( argc == 1 )
        title = create_string( argv[0] );
    else
    {
        title = create_string( NULL );
        for_less( i, 1, argc )
        {
            if( i > 1 )
                concat_to_string( &title, " " );

            concat_to_string( &title, argv[i] );

            if( string_length( title ) >= MAX_TITLE_LENGTH )
                break;
        }
    }

    initialize_global_colours();

    if( getenv( "DISPLAY_DIRECTORY" ) != (char *) NULL )
        runtime_directory = create_string( getenv( "DISPLAY_DIRECTORY" ) );
    else
        runtime_directory = extract_directory( argv[0] );

    n_directories = 0;
    directories = NULL;

    ADD_ELEMENT_TO_ARRAY( directories, n_directories,
                          HARD_CODED_DISPLAY_DIRECTORY2, DEFAULT_CHUNK_SIZE );
    ADD_ELEMENT_TO_ARRAY( directories, n_directories,
                          HARD_CODED_DISPLAY_DIRECTORY1, DEFAULT_CHUNK_SIZE );
    ADD_ELEMENT_TO_ARRAY( directories, n_directories,
                          runtime_directory, DEFAULT_CHUNK_SIZE );
    ADD_ELEMENT_TO_ARRAY( directories, n_directories,
                          getenv("HOME"), DEFAULT_CHUNK_SIZE );
    ADD_ELEMENT_TO_ARRAY( directories, n_directories, ".", DEFAULT_CHUNK_SIZE );

    for_less( i, 0, n_directories )
    {
        globals_filename = get_absolute_filename( DISPLAY_GLOBALS_FILENAME,
                                                  directories[i] );

        if( file_exists( globals_filename ) )
        {
            (void) input_globals_file( SIZEOF_STATIC_ARRAY(display_globals),
                                       display_globals, globals_filename );
        }

        delete_string( globals_filename );
    }

    if( n_directories > 0 )
        FREE( directories );

    set_alloc_checking( Alloc_checking_enabled );

    initialize_graphics();

    if( create_graphics_window( THREE_D_WINDOW,
                                Graphics_double_buffer_flag,
                                &graphics, title, 0, 0 ) != OK )
        return( 1 );

    G_set_transparency_state( graphics->window, Graphics_transparency_flag);

    if( create_graphics_window( MENU_WINDOW, ON, &menu, title,
                                Initial_menu_window_width,
                                Initial_menu_window_height ) != OK )
        return( 1 );

    delete_string( title );

    graphics->associated[THREE_D_WINDOW] = graphics;
    graphics->associated[MENU_WINDOW] = menu;
    graphics->associated[SLICE_WINDOW] = (display_struct *) 0;

    menu->associated[THREE_D_WINDOW] = graphics;
    menu->associated[MENU_WINDOW] = menu;
    menu->associated[SLICE_WINDOW] = (display_struct *) 0;

    if( initialize_menu( menu, runtime_directory,
			 getenv( "HOME" ),
			 HARD_CODED_DISPLAY_DIRECTORY1,
			 HARD_CODED_DISPLAY_DIRECTORY2,
			 MENU_FILENAME ) != OK )
	return 1;

    delete_string( runtime_directory );

    initialize_argument_processing( argc, argv );

    next_is_label_volume = FALSE;

    while( get_string_argument( "", &filename ) )
    {
        if( equal_strings( filename, "-version" ) )
        {
            print( "%s:  Version: %s\n", argv[0], version );
            return( 0 );
        }
        else if( equal_strings( filename, "-label" ) )
        {
            if( next_is_label_volume )
                print( "Ignoring extraneous -label\n" );
            next_is_label_volume = TRUE;
        }
        else if( equal_strings( filename, "-global" ) )
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
            if( !Enable_volume_caching )
                set_n_bytes_cache_threshold( -1 );
            else
            {
                if( Volume_cache_threshold >= 0 )
                    set_n_bytes_cache_threshold( Volume_cache_threshold );

                if( Volume_cache_size >= 0 )
                    set_default_max_bytes_in_cache( Volume_cache_size );

                if( Volume_cache_block_size > 0 )
                {
                    int   dim, block_sizes[MAX_DIMENSIONS];

                    for_less( dim, 0, MAX_DIMENSIONS )
                        block_sizes[dim] = Volume_cache_block_size;

                    set_default_cache_block_sizes( block_sizes );
                }
            }

            if( load_graphics_file( graphics, filename,
                                    next_is_label_volume ) != OK )
                print( "Error loading %s\n", filename );

            next_is_label_volume = FALSE;
        }
    }

    if( next_is_label_volume )
        print( "Ignoring extraneous -label\n" );

    if( get_slice_window( graphics, &slice_window ) )
    {
        for_less( view, 0, N_SLICE_VIEWS )
            reset_slice_view( slice_window, view );
    }

    if( !Enable_volume_caching )
        set_n_bytes_cache_threshold( -1 );
    else
    {
        if( Volume_cache_threshold >= 0 )
            set_n_bytes_cache_threshold( Volume_cache_threshold );

        if( Volume_cache_size >= 0 )
            set_default_max_bytes_in_cache( Volume_cache_size );

        if( Volume_cache_block_size > 0 )
        {
            int   dim, block_sizes[MAX_DIMENSIONS];

            for_less( dim, 0, MAX_DIMENSIONS )
                block_sizes[dim] = Volume_cache_block_size;
            set_default_cache_block_sizes( block_sizes );
        }
    }

    initialize_view_to_fit( graphics );

    rebuild_selected_list( graphics, menu );

    reset_view_parameters( graphics, &Default_line_of_sight,
                           &Default_horizontal );

    update_view( graphics );

    update_all_menu_text( graphics );

    set_update_required( graphics, NORMAL_PLANES );

    (void) main_event_loop();

    terminate_graphics();

    delete_marching_cubes_table();

    delete_global_variables( SIZEOF_STATIC_ARRAY(display_globals),
                             display_globals );

    output_alloc_to_file( ".alloc_stats" );

    return( 0 );
}

private  void      initialize_global_colours( void )
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
    Colour_bar_min_limit_colour = GREEN;
    Colour_bar_max_limit_colour = CYAN;
    Colour_bar_range_colour = RED;
    Surface_curve_colour = MAGENTA;
    Visible_segmenting_colour = GREEN;
    Invisible_segmenting_colour = BLACK;
    Brush_outline_colour = GREEN;
    Histogram_colour = WHITE;
    Menu_name_colour = GREEN;
    Cursor_pos_colour = GREEN;
    Unfinished_flag_colour = GREEN;
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
                    world[c] < (Real) Point_coord(display->three_d.min_limit,c))
                    Point_coord(display->three_d.min_limit,c) = (float) world[c];
                if( !found ||
                    world[c] > (Real) Point_coord(display->three_d.max_limit,c))
                    Point_coord(display->three_d.max_limit,c) = (float) world[c];
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
