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

#endif

#include "config.h"
#include  <display.h>


#define  MAX_TITLE_LENGTH   200

/* These two globals are required for SEAL's LibSul/SULGRAPHDATA */
int debug = 1;
int verbose = 1;

static  void   initialize_global_colours( void );
static  void   initialize_ratio (display_struct* display);
static  void   initialize_view_to_fit (display_struct  *display );
static  void   initialize_cache ();
static  void   parse_options (int argc, char *argv[],
		display_struct *graphics);
static void  visibility_3D_window(int state);
static void  visibility_marker_window(int state);
static void  visibility_menu_window(int state);

/* The first directory is set using compiler flag -D */
/*#define  HARD_CODED_DISPLAY_DIRECTORY1    "/usr/local/mni/lib"*/
/*#define  HARD_CODED_DISPLAY_DIRECTORY2    "/usr/local/lib"*/
#define  GLOBALS_FILENAME                 "Display.globals"
#define  MENU_FILENAME                    "Display.menu"


#define  GLOBALS_LOOKUP_NAME  display_globals
#include  <bicpl/globals.h>
#define   DISPLAY_GLOBALS_FILENAME   "Display.globals"

  VIO_Status  change_global_variable(
    VIO_STR   str,
    VIO_STR   *variable_name,
    VIO_STR   *new_value )
{
    return( set_or_get_global_variable(
                   SIZEOF_STATIC_ARRAY(display_globals),
                   display_globals, str, variable_name, new_value ) );
}

  VIO_Status  set_global_variable_value(
    VIO_STR   variable_name,
    VIO_STR   new_value )
{
    return( set_global_variable( SIZEOF_STATIC_ARRAY(display_globals),
                                 display_globals, variable_name, new_value ) );
}


int  main(
    int     argc,
    char    *argv[] )
{
    int              i, view;
    VIO_STR           filename;
    display_struct   *graphics;
    display_struct   *menu, *slice_window, *marker;
    VIO_STR           globals_filename, runtime_directory;
    int              n_directories;
    VIO_STR           *directories;
    VIO_STR           title;
    VIO_STR           variable_name, variable_value;
    VIO_BOOL          next_is_label_volume;

    set_alloc_checking( TRUE );

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
    title = concat_strings( PROJECT_NAME, ": 3D View" );

    if( create_graphics_window( THREE_D_WINDOW,
                                Graphics_double_buffer_flag,
                                &graphics, title, 0, 0 ) != OK )
        return( 1 );
    delete_string( title );

	G_set_transparency_state( graphics->window, Graphics_transparency_flag);
	if( Hide_3D_window )
	{
		glutSetWindow(graphics->window->GS_window->WS_window.window_id);
		glutVisibilityFunc(visibility_3D_window);
	}

    title = concat_strings( PROJECT_NAME, ": Menu" );
    if( create_graphics_window( MENU_WINDOW, TRUE, &menu, title,
                                Initial_menu_window_width,
                                Initial_menu_window_height ) != OK )
        return( 1 );
    delete_string( title );
	if( Hide_menu_window )
	{
		glutSetWindow(menu->window->GS_window->WS_window.window_id);
		glutVisibilityFunc(visibility_menu_window);
	}

    title = concat_strings( PROJECT_NAME, ": Marker" );
    if( create_graphics_window( MARKER_WINDOW, TRUE, &marker, title,
                                Initial_marker_window_width,
                                Initial_marker_window_height ) != OK )
    	return( 1 );

    delete_string( title );
    if( Hide_marker_window )
    {
		glutSetWindow(marker->window->GS_window->WS_window.window_id);
		glutVisibilityFunc(visibility_marker_window);
    }


    graphics->associated[THREE_D_WINDOW] = graphics;
    graphics->associated[MENU_WINDOW] = menu;
    graphics->associated[SLICE_WINDOW] = (display_struct *) 0;
    graphics->associated[MARKER_WINDOW] = marker;

    menu->associated[THREE_D_WINDOW] = graphics;
    menu->associated[MENU_WINDOW] = menu;
    menu->associated[SLICE_WINDOW] = (display_struct *) 0;
    menu->associated[MARKER_WINDOW] = marker;

    marker->associated[THREE_D_WINDOW] = graphics;
    marker->associated[MENU_WINDOW] = menu;
    marker->associated[SLICE_WINDOW] = (display_struct *) 0;
    marker->associated[MARKER_WINDOW] = marker;

    if( initialize_menu( menu, runtime_directory,
			 getenv( "HOME" ),
			 HARD_CODED_DISPLAY_DIRECTORY1,
			 HARD_CODED_DISPLAY_DIRECTORY2,
			 MENU_FILENAME ) != OK )
	return 1;

    if( initialize_marker_window( marker ) != OK )
    return 1;

    delete_string( runtime_directory );

    parse_options(argc, argv, graphics);

    if( get_slice_window( graphics, &slice_window ) )
    {
        for_less( view, 0, N_SLICE_VIEWS )
            reset_slice_view( slice_window, view );
        initialize_ratio( slice_window );
    }

    initialize_cache( graphics );
    initialize_view_to_fit( graphics );

    rebuild_selected_list( graphics, marker );
    reset_view_parameters( graphics, &Default_line_of_sight,
                           &Default_horizontal );

    update_view( graphics );
    update_all_menu_text( graphics );
    set_update_required( graphics, NORMAL_PLANES );
    set_update_required( marker, NORMAL_PLANES );

    (void) main_event_loop();

    terminate_graphics();

    delete_marching_cubes_table();

    delete_global_variables( SIZEOF_STATIC_ARRAY(display_globals),
                             display_globals );

    output_alloc_to_file( ".alloc_stats" );

    return( EX_OK );
}

static  void      initialize_global_colours( void )
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
    Slice_probe_ratio_colour = YELLOW;
}

static  void      initialize_view_to_fit(
    display_struct  *display )
{
    int      i, c, x, y, z;
    Real     voxel[VIO_N_DIMENSIONS], world[VIO_N_DIMENSIONS];
    int      sizes[VIO_N_DIMENSIONS];
    VIO_Volume   volume;
    VIO_BOOL  found;

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

            for_less( c, 0, VIO_N_DIMENSIONS )
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

static void initialize_ratio (display_struct* slice_window)
{
	model_struct      *model;
	VIO_Colour             colour;
	int 			   retcode;
	text_struct       *text;

	slice_window->slice.print_probe_ratio = FALSE;

	if( string_length(Ratio_volume_index) )
	{
		retcode = sscanf(Ratio_volume_index, Ratio_volume_index_format,
					&slice_window->slice.ratio_volume_index_numerator,
					&slice_window->slice.ratio_volume_index_denominator);
		if( retcode != 2 )
			fprintf(stderr, "Error: can not parse %s with %s\n",
					Ratio_volume_index, Ratio_volume_index_format);
		else
		{
			slice_window->slice.print_probe_ratio = TRUE;
			model = get_graphics_model( slice_window, SLICE_READOUT_MODEL );
			text = get_text_ptr( model->objects[RATIO_PROBE_INDEX] );
			text->colour = Slice_probe_ratio_colour;
		}
	}
}

static void initialize_cache()
{
	if (!Enable_volume_caching)
		set_n_bytes_cache_threshold(-1);
	else
	{
		if (Volume_cache_threshold >= 0)
			set_n_bytes_cache_threshold(Volume_cache_threshold);

		if (Volume_cache_size >= 0)
			set_default_max_bytes_in_cache(Volume_cache_size);

		if (Volume_cache_block_size > 0)
		{
			int dim, block_sizes[VIO_MAX_DIMENSIONS];

			for_less( dim, 0, VIO_MAX_DIMENSIONS )
				block_sizes[dim] = Volume_cache_block_size;
			set_default_cache_block_sizes(block_sizes);
		}
	}
}


static void parse_options(int argc, char *argv[], display_struct *graphics)
{
	VIO_Status retcode;
	VIO_STR filename;
	VIO_STR globals_filename;
	VIO_STR variable_name, variable_value;
	VIO_BOOL next_is_label_volume;

	initialize_argument_processing(argc, argv);
	retcode = OK;
	next_is_label_volume = FALSE;

	while (get_string_argument("", &filename))
	{
		if (equal_strings(filename, "-help"))
		{
			print("Usage: Display [OPTION1] [FILE1] [OPTION2] [FILE2]...\n"
				  "Interactively display and segment three dimensional images.\n"
				  "\n");
			print("  %-25s %s\n", "-version",
					"Output version information and exit.");
			print("  %-25s %s\n", "-skiperror",
					"Skip on error when parsing arguments or loading file.");
			print("  %-25s %s\n", "-label FILENAME",
					"Interpret FILENAME as a label to be displayed over other images.");
			print("  %-25s %s\n", "-labeltags",
					"Input tags from the label file.");
			print("  %-25s %s\n", "-output-label FILENAME",
					"Use FILENAME to save labels instead of prompting the user.");
			print("  %-25s %s\n", "-ratio N1,N2",
								"Display the images ratio of N1/N2. The first image index is 0.");
			print("  %-25s %s\n", "-range MINIMUM MAXIMUM",
					"Set the contrast range.");
			print("  %-25s %s\n", "-gray",
					"Use gray color map.");
			print("  %-25s %s\n", "-hot",
					"Use hot color map.");
			print("  %-25s %s\n", "-spectral",
					"Use spectral color map.");
			print("  %-25s %s\n", "-global NAME VALUE",
					"Set the global variable NAME to VALUE.");
			print("\nReport bugs to minc-development@bic.mni.mcgill.ca\n");
			exit(EX_OK);
		}
		else if (equal_strings(filename, "-version"))
		{
			print("%s %s\n", PROJECT_NAME, PROJECT_VERSION );
			exit(EX_OK);
		}
		else if (equal_strings(filename, "-skiperror"))
		{
			if( set_global_variable_value("Exit_error_load_file", "FALSE") != OK )
			{
				print("Error setting skiperror variable from command line.\n");
				retcode = ERROR;
			}
		}
		else if (equal_strings(filename, "-gray"))
		{
			if( set_global_variable_value("Initial_colour_coding_type", "0") != OK )
			{
				print("Error setting gray variable from command line.\n");
				retcode = ERROR;
			}
		}
		else if (equal_strings(filename, "-hot"))
		{
			if( set_global_variable_value("Initial_colour_coding_type", "1") != OK )
			{
				print("Error setting hot variable from command line.\n");
				retcode = ERROR;
			}
		}
		else if (equal_strings(filename, "-spectral"))
		{
			if( set_global_variable_value("Initial_colour_coding_type", "13") != OK )
			{
				print("Error setting spectral variable from command line.\n");
				retcode = ERROR;
			}
		}
		else if (equal_strings(filename, "-ratio"))
		{
			if (!get_string_argument("", &variable_value))
			{
				print_error("Error in arguments after -ratio.\n");
				exit(EX_USAGE);
			}

			if( set_global_variable_value("Ratio_volume_index", variable_value) != OK )
			{
				print("Error setting ratio variable from command line.\n");
				retcode = ERROR;
			}
		}
		else if (equal_strings(filename, "-label"))
		{
			if (next_is_label_volume)
			{
				print("Ignoring extraneous -label\n");
				retcode = ERROR;
			}
			next_is_label_volume = TRUE;
		}
		else if (equal_strings(filename, "-labeltags"))
		{
			if( set_global_variable_value("Tags_from_label", "TRUE") != OK )
			{
				print("Error setting labeltags variable from command line.\n");
				retcode = ERROR;
			}
		}
		else if (equal_strings(filename, "-output-label"))
		{
			if (!get_string_argument("", &variable_value))
			{
				print_error("Error in arguments after -output.\n");
				exit(EX_USAGE);
			}

			if( set_global_variable_value("Output_label_filename", variable_value) != OK)
			{
				print("Error setting output variable from command line.\n");
				retcode = ERROR;
			}
		}
		else if (equal_strings(filename, "-range"))
		{
			if (!get_string_argument("", &variable_name)
					|| !get_string_argument("", &variable_value))
			{
				print_error("Error in arguments after -range.\n");
				exit(EX_USAGE);
			}

			if (Initial_histogram_contrast)
			{
				if (set_global_variable_value("Initial_histogram_low", variable_name) != OK
						|| set_global_variable_value("Initial_histogram_high", variable_value) != OK)
				{
					print("Error setting range variable from command line.\n");
					retcode = ERROR;
				}
			}
			else{
				if (set_global_variable_value("Initial_low_absolute_position", variable_name) != OK
						|| set_global_variable_value("Initial_high_absolute_position", variable_value) != OK)
				{
					print("Error setting range variable from command line.\n");
					retcode = ERROR;
				}
			}
		}
		else if (equal_strings(filename, "-global"))
		{
			if (!get_string_argument("", &variable_name)
					|| !get_string_argument("", &variable_value))
			{
				print_error("Error in arguments after -global.\n");
				exit(EX_USAGE);
			}

			if (set_global_variable_value(variable_name, variable_value) != OK)
			{
				print("Error setting global variable from command line.\n");
				retcode = ERROR;
			}
		}
		else
		{
			if (filename[0] == '-')
			{
				print("Error: unknown option %s\n", filename);
				retcode = ERROR;
			}
			else
			{
				initialize_cache();
				if (load_graphics_file(graphics, filename, next_is_label_volume) != OK)
				{
					print("Error loading %s\n", filename);
					if (Exit_error_load_file)
						exit(EX_NOINPUT);
				}
				next_is_label_volume = FALSE;
			}
		}
	}

	if (next_is_label_volume)
	{
		print("Ignoring extraneous -label\n");
		retcode = ERROR;
	}

	if (Exit_error_load_file && retcode != OK)
		exit(EX_USAGE);
}

void visibility_marker_window(int state)
{
	if (Hide_marker_window)
	{
		if (state == GLUT_VISIBLE)
			glutHideWindow();
	}
}

void visibility_3D_window(int state)
{
	if (Hide_3D_window)
	{
		if (state == GLUT_VISIBLE)
			glutHideWindow();
	}
}

void visibility_menu_window(int state)
{
	if (Hide_menu_window)
	{
		if (state == GLUT_VISIBLE)
			glutHideWindow();
	}
}
