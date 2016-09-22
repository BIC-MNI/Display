/**
 * \file main.c
 * \brief Main program and argument processing.
 *
 * \copyright
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
*/

#include "config.h"
#include  <display.h>

#define  MAX_TITLE_LENGTH   200

/* These two globals are required for SEAL's LibSul/SULGRAPHDATA */
int debug = 1;
int verbose = 1;

static  void   initialize_global_colours( void );
static  void   initialize_view_to_fit (display_struct  *display );
static  void   initialize_cache ();
static  void   parse_options (int argc, char *argv[],
                              display_struct *graphics);
static  void   create_empty_slice( display_struct *display );

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
                   VIO_SIZEOF_STATIC_ARRAY(display_globals),
                   display_globals, str, variable_name, new_value ) );
}

VIO_Status  set_global_variable_value(
    VIO_STR   variable_name,
    VIO_STR   new_value )
{
    return( set_global_variable( VIO_SIZEOF_STATIC_ARRAY(display_globals),
                                 display_globals, variable_name, new_value ) );
}

/**
 * The main program. Initializes key data structures, reads configuration
 * and command line, creates windows, then drops into the main event loop.
 */
int  main(
    int     argc,
    char    *argv[] )
{
    int              i, view;
    display_struct   *graphics;
    display_struct   *menu, *slice_window, *marker, *label;
    VIO_STR           globals_filename, runtime_directory;
    int               n_directories;
    VIO_STR           *directories;
    VIO_STR           title;

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
            (void) input_globals_file( VIO_SIZEOF_STATIC_ARRAY(display_globals),
                                       display_globals, globals_filename );
        }

        delete_string( globals_filename );

        globals_filename = get_absolute_filename( ".mni-displayrc",
                                                  directories[i] );
        if( file_exists( globals_filename ) )
        {
            (void) input_globals_file( VIO_SIZEOF_STATIC_ARRAY(display_globals),
                                       display_globals, globals_filename );
        }

        delete_string( globals_filename );
    }

    /* Perform critical initialization of global variables before doing
     * anything else.
     */
    Current_colour_coding_type = Initial_colour_coding_type;

    if( n_directories > 0 )
        FREE( directories );

    initialize_graphics();

    if (Single_window)
    {
      Hide_menu_window = FALSE;
      Hide_3D_window = FALSE;
      Hide_marker_window = FALSE;
      create_parent_window();
    }

    title = concat_strings( PROJECT_NAME, ": Menu" );
    if( create_graphics_window( MENU_WINDOW,
                                Graphics_double_buffer_flag, &menu, title,
                                Initial_menu_window_x,
                                Initial_menu_window_y,
                                Initial_menu_window_width,
                                Initial_menu_window_height ) != VIO_OK )
        return( 1 );
    delete_string( title );

    G_set_visibility(menu->window, !Hide_menu_window);

    title = concat_strings( PROJECT_NAME, ": Objects" );
    if( create_graphics_window( MARKER_WINDOW,
                                Graphics_double_buffer_flag, &marker, title,
                                Initial_marker_window_x,
                                Initial_marker_window_y,
                                Initial_marker_window_width,
                                Initial_marker_window_height ) != VIO_OK )
        return( 1 );

    delete_string( title );
    
    G_set_visibility(marker->window, !Hide_marker_window);

    title = concat_strings( PROJECT_NAME, ": Labels" );
    if( create_graphics_window( LABEL_WINDOW,
                                Graphics_double_buffer_flag, &label, title,
                                0, 0, 250, 500 ) != VIO_OK )
        return( 1 );

    delete_string( title );
    G_set_visibility(label->window, TRUE);

    title = concat_strings( PROJECT_NAME, ": 3D View" );

    if( create_graphics_window( THREE_D_WINDOW,
                                Graphics_double_buffer_flag,
                                &graphics, title,
                                Initial_3D_window_x,
                                Initial_3D_window_y,
                                Initial_3D_window_width,
                                Initial_3D_window_height) != VIO_OK )
        return( 1 );
    delete_string( title );

    G_set_transparency_state( graphics->window, Graphics_transparency_flag);
    G_set_visibility(graphics->window, !Hide_3D_window);

    if( initialize_menu( menu, runtime_directory,
     getenv( "HOME" ),
     HARD_CODED_DISPLAY_DIRECTORY1,
     HARD_CODED_DISPLAY_DIRECTORY2,
     MENU_FILENAME ) != VIO_OK )
       return 1;

    delete_string( runtime_directory );

    parse_options(argc, argv, graphics);

    if( !get_slice_window( graphics, &slice_window ) && Show_slice_window )
    {
        create_empty_slice( graphics );
    }
    if( get_slice_window( graphics, &slice_window ) )
    {
        for_less( view, 0, N_SLICE_VIEWS )
            reset_slice_view( slice_window, view );
    }
    else
    {
        slice_window = NULL;
    }

    initialize_cache(  );/*graphics*/
    initialize_view_to_fit( graphics );

    initialize_scroll_magnification( graphics );

    rebuild_selected_list( graphics, marker );
    reset_view_parameters( graphics, &Default_line_of_sight,
                           &Default_horizontal );

    update_view( graphics );
    update_all_menu_text( graphics );

    /* Doublecheck that the 3D cursor is consistent with the current
     * voxel coordinates.
     */
    if (slice_window != NULL)
        update_cursor_from_voxel( slice_window );

    set_update_required( graphics, NORMAL_PLANES );
    set_update_required( marker, NORMAL_PLANES );
    set_update_required( label, NORMAL_PLANES );

    (void) main_event_loop();

    /** \note In the current implementation of GLUT and FreeGLUT, the
     * main loop _never_ returns. So no code is executed after this
     * point. FreeGLUT implements methods to work around this
     * However, these workarounds seem to immediately shut down the
     * windowing system such that any calls to GLUT functions will
     * force an exit anyway, complaining that a glut function was
     * called before glutInit().
     */

    terminate_graphics();

    delete_marching_cubes_table();

    delete_global_variables( VIO_SIZEOF_STATIC_ARRAY(display_globals),
                             display_globals );

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
    Slice_probe_ratio_colour = YELLOW;
    Initial_vertex_under_colour = BLACK;
    Initial_vertex_over_colour = WHITE;
    Measure_colour = YELLOW;
}

static  void      initialize_view_to_fit(
    display_struct  *display )
{
    int      i, c, x, y, z;
    VIO_Real voxel[VIO_N_DIMENSIONS], world[VIO_N_DIMENSIONS];
    int      sizes[VIO_MAX_DIMENSIONS];
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
            voxel[VIO_X] = -0.5 + (VIO_Real) sizes[VIO_X] * (VIO_Real) x;
            voxel[VIO_Y] = -0.5 + (VIO_Real) sizes[VIO_Y] * (VIO_Real) y;
            voxel[VIO_Z] = -0.5 + (VIO_Real) sizes[VIO_Z] * (VIO_Real) z;

            convert_voxel_to_world( volume, voxel,
                                    &world[VIO_X], &world[VIO_Y], &world[VIO_Z] );

            for_less( c, 0, VIO_N_DIMENSIONS )
            {
                if( !found ||
                    world[c] < (VIO_Real) Point_coord(display->three_d.min_limit,c))
                    Point_coord(display->three_d.min_limit,c) = (float) world[c];
                if( !found ||
                    world[c] > (VIO_Real) Point_coord(display->three_d.max_limit,c))
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


/**
 * This function writes the current set of global variables and values
 * to a file. If the file already exists, it will be overwritten.
 *
 * \param filename The desired filename to receive the globals.
 */
void
write_globals_to_file( const VIO_STR filename )
{
  int i;
  FILE *fp;
  const int n_globals = VIO_SIZEOF_STATIC_ARRAY(display_globals);
  global_struct *globals = display_globals;

  if ( open_file( filename, WRITE_FILE, ASCII_FORMAT, &fp) != VIO_OK )
  {
    return;
  }

  for_less (i, 0, n_globals)
  {
    VIO_STR value;

    /* TODO: It would be nicer if the globals.c in BICPL exported a
     * "format_global" function or something to handle this for us,
     * rather than doing an expensive lookup on a table we are already
     * scanning...
     */
    if (get_global_variable( n_globals, globals, globals[i].variable_name,
                             &value ) == VIO_OK)
    {
      fprintf( fp, "%s = %s;\n", globals[i].variable_name, value );
      delete_string( value );
    }
  }
  close_file(fp);
}

/**
 * Try to make sense out of command-line arguments.
 *
 * \param argc The argument count, from main().
 * \param argv The argument vector, also from main().
 * \param graphics A pointer to the display_struct for the 3D window.
 */

static void
parse_options(int argc, char *argv[], display_struct *graphics)
{
  VIO_Status retcode;
  VIO_STR filename;
  VIO_BOOL next_is_label_volume;

  initialize_argument_processing(argc, argv);
  retcode = VIO_OK;
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
            "Set the absolute contrast range.");
      print("  %-25s %s\n", "-hist_range MINIMUM MAXIMUM",
            "Set the histogram contrast range.");
      print("  %-25s %s\n", "-rel_range MINIMUM MAXIMUM",
            "Set the relative contrast range.");
      print("  %-25s %s\n", "-gray",
            "Use gray colour map for subsequently loaded volumes.");
      print("  %-25s %s\n", "-hot",
            "Use hot colour map for subsequently loaded volumes.");
      print("  %-25s %s\n", "-spectral",
            "Use spectral colour map for subsequently loaded volumes.");
      print("  %-25s %s\n", "-red",
            "Use red colour map for subsequently loaded volumes.");
      print("  %-25s %s\n", "-blue",
            "Use blue colour map for subsequently loaded volumes.");
      print("  %-25s %s\n", "-green",
            "Use green colour map for subsequently loaded volumes.");
      print("  %-25s %s\n", "-global NAME VALUE",
            "Set the global variable NAME to VALUE.");
      print("\nReport bugs to %s\n", PACKAGE_BUGREPORT);
      exit(EX_OK);
    }
    else if (equal_strings(filename, "-version"))
    {
#ifndef GLUT_VERSION
#define GLUT_VERSION 0x1FC
#endif
#ifndef FREEGLUT
#define FREEGLUT 0
#endif
      int glutVersion = glutGet(GLUT_VERSION);

      print("%s %s (built %s) git:%s/%s\n", PROJECT_NAME, PROJECT_VERSION, __DATE__,
            GIT_BRANCH, GIT_COMMIT );
      print("%s %d.%d.%d API V%d\n",
            FREEGLUT ? "FreeGLUT" : "GLUT",
            glutVersion / 10000,
            (glutVersion / 100) % 100,
            glutVersion % 100,
            GLUT_API_VERSION);
      print("OpenGL %s\n", glGetString(GL_VERSION));
      exit(EX_OK);
    }
    else if (equal_strings(filename, "-skiperror"))
    {
      Exit_error_load_file = FALSE;
    }
    else if (equal_strings(filename, "-gray"))
    {
      Current_colour_coding_type = GRAY_SCALE;
    }
    else if (equal_strings(filename, "-hot"))
    {
      Current_colour_coding_type = HOT_METAL;
    }
    else if (equal_strings(filename, "-spectral"))
    {
      Current_colour_coding_type = SPECTRAL;
    }
    else if (equal_strings(filename, "-red"))
    {
      Current_colour_coding_type = RED_COLOUR_MAP;
    }
    else if (equal_strings(filename, "-green"))
    {
      Current_colour_coding_type = GREEN_COLOUR_MAP;
    }
    else if (equal_strings(filename, "-blue"))
    {
      Current_colour_coding_type = BLUE_COLOUR_MAP;
    }
    else if (equal_strings(filename, "-ratio"))
    {
      VIO_STR ratio_string;

      if (!get_string_argument(Ratio_volume_index, &ratio_string))
      {
        print_error("Error in arguments after -ratio.\n");
        exit(EX_USAGE);
      }
      Ratio_volume_index = create_string(ratio_string);
    }
    else if (equal_strings(filename, "-label"))
    {
      if (next_is_label_volume)
      {
        print("Ignoring extraneous -label\n");
        retcode = VIO_ERROR;
      }
      next_is_label_volume = TRUE;
    }
    else if (equal_strings(filename, "-labeltags"))
    {
      Tags_from_label = TRUE;
    }
    else if (equal_strings(filename, "-output-label"))
    {
      VIO_STR file_name;

      if (!get_string_argument("", &file_name))
      {
        print_error("Error in arguments after -output.\n");
        exit(EX_USAGE);
      }

      /*
       * get_string_argument will just return a pointer to the correct
       * element of argv[], so while we should be able to count on it
       * existing at a later point in the program, I prefer to make an
       * explicit copy using create_string().
       */
      Output_label_filename = create_string(file_name);
    }
    else if (equal_strings(filename, "-hist_range"))
    {
      VIO_Real lo_val, hi_val;

      if (!get_real_argument(0.0, &lo_val) ||
          !get_real_argument(1.0, &hi_val) ||
          lo_val > hi_val ||
          lo_val < 0 || lo_val > 1 ||
          hi_val < 0 || hi_val > 1)
      {
        print_error("Error in arguments after -hist_range.\n");
        exit(EX_USAGE);
      }
      Initial_histogram_contrast = TRUE;
      Initial_histogram_low = lo_val;
      Initial_histogram_high = hi_val;
    }
    else if (equal_strings(filename, "-rel_range") ||
             equal_strings(filename, "-range"))
    {
      VIO_Real lo_val, hi_val;

      if (!get_real_argument(0.0, &lo_val) || !get_real_argument(1.0, &hi_val))
      {
        print_error("Error in arguments after %s.\n", filename);
        exit(EX_USAGE);
      }

      if (equal_strings(filename, "-rel_range"))
      {
        if ((lo_val > 1.0 || lo_val < 0.0) ||
            (hi_val > 1.0 || hi_val < 0.0) ||
            (hi_val < lo_val))
        {
          print_error("Error in arguments after -rel_range.\n");
          exit(EX_USAGE);
        }
        Initial_coding_range_absolute = FALSE;
      }
      else
      {
        Initial_coding_range_absolute = TRUE;
      }
      Initial_histogram_contrast = FALSE;
      Initial_coding_range_low = lo_val;
      Initial_coding_range_high = hi_val;
    }
    else if (equal_strings(filename, "-global"))
    {
      VIO_STR variable_name, variable_value;

      if (!get_string_argument("", &variable_name) ||
          !get_string_argument("", &variable_value))
      {
        print_error("Error in arguments after -global.\n");
        exit(EX_USAGE);
      }

      if (set_global_variable_value(variable_name, variable_value) != VIO_OK)
      {
        print("Error setting global variable from command line.\n");
        retcode = VIO_ERROR;
      }
    }
    else
    {
      if (filename[0] == '-')
      {
        print("Error: unknown option %s\n", filename);
        retcode = VIO_ERROR;
      }
      else
      {
        VIO_Colour default_colour = WHITE;
        VIO_STR colour_string = NULL;

        initialize_cache();

        colour_string = strchr( filename, ':');
        if (colour_string != NULL)
        {
          *colour_string++ = 0;
          if (string_to_colour( colour_string, &default_colour ) != VIO_OK)
          {
              print("Colour '%s' not recognized.\n", colour_string );
              default_colour = WHITE;
          }
        }

        if (load_graphics_file_with_colour(graphics, filename, 
                                           next_is_label_volume,
                                           default_colour) != VIO_OK)
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
    retcode = VIO_ERROR;
  }

  if (Exit_error_load_file && retcode != VIO_OK)
    exit(EX_USAGE);
}

/**
 * When no volumes are loaded, we can create an "empty" slice view that
 * corresponds to the reported size of the loaded 3D objects. This allows
 * the user to inspect the outline projection of the 3D objects in the
 * slice view window without actually loading a real volume.
 *
 * \param display A pointer to the display_struct for the 3D view
 * window.
 */
static void
create_empty_slice( display_struct *display )
{
  VIO_Volume volume;
  VIO_Point pt_min, pt_max;
  int sizes[VIO_N_DIMENSIONS];
  VIO_Real starts[VIO_N_DIMENSIONS];
  VIO_Real steps[VIO_N_DIMENSIONS];
  VIO_Real extent[VIO_N_DIMENSIONS];
  int i;

  /* Get the size of the loaded 3D objects in world space.
   */
  if (!get_range_of_object( display->models[THREED_MODEL], 
                            FALSE, &pt_min, &pt_max ))
  {
      return;                   /* Nothing loaded, so don't bother.*/
  }

  /* Create a "fake" unsigned byte volume that will be used as the
   * backdrop for projections of the loaded surfaces.
   */
  volume = create_volume(VIO_N_DIMENSIONS, XYZ_dimension_names,
                         NC_BYTE, FALSE, 0, 255);

  /* Use the extent of the 3D objects to set the size of the "fake"
   * volume of so that it will fit the objects cleanly.
   */
  extent[VIO_X] = (int) (ceil(Point_x(pt_max)) - floor(Point_x(pt_min)));
  extent[VIO_Y] = (int) (ceil(Point_y(pt_max)) - floor(Point_y(pt_min)));
  extent[VIO_Z] = (int) (ceil(Point_z(pt_max)) - floor(Point_z(pt_min)));

  /* We use a constant size of 256x256x256 to give a fine-grained
   * projection, so we adjust the steps to give the right extent for
   * the fixed size.
   */
  sizes[VIO_X] = sizes[VIO_Y] = sizes[VIO_Z] = 256;
  for (i = 0; i < VIO_N_DIMENSIONS; i++)
  {
    steps[i] = extent[i] / sizes[i];
  }

  /* The starts are just the lowest point in world space for each axis.
   */
  starts[VIO_X] = floor(Point_x(pt_min));
  starts[VIO_Y] = floor(Point_y(pt_min));
  starts[VIO_Z] = floor(Point_z(pt_min));
  set_volume_sizes( volume, sizes );
  set_volume_starts( volume, starts );
  set_volume_separations( volume, steps );
  alloc_volume_data( volume );
  add_slice_window_volume( display, "", "", volume );
}

#include <unistd.h>
#include <stdarg.h>
#include <libgen.h>

static FILE *
try_popen(const char *command, const char *mode, int *error_code)
{
  FILE *fp = popen(command, "r");
  int c;

  if (fp == NULL)
    return NULL;
  else if ((c = fgetc(fp)) == EOF)
  {
    *error_code = pclose(fp);
    return NULL;
  }
  else
  {
    ungetc(c, fp);
    return fp;
  }
}

#define FILE_OPEN_DIR_MAX VIO_EXTREMELY_LARGE_STRING_SIZE

static char File_open_dir[FILE_OPEN_DIR_MAX] = {0};

/**
 * Set the "file open" directory to be used by default for subsequent
 * file accesses. This is used to initialize the directory shown in the
 * zenity dialog box when used.
 * \param pathname The full file pathname from which we must extract the
 * directory name.
 */

#include <sys/stat.h>

void
set_file_open_directory(const char *pathname)
{
  int length = 0;
  char temp_path[FILE_OPEN_DIR_MAX];
  struct stat stbuf;

  /* First see if it is already a path name.
   */
  if ( stat( pathname, &stbuf ) == 0 && S_ISDIR( stbuf.st_mode ) )
  {
    strncpy( File_open_dir, pathname, FILE_OPEN_DIR_MAX );
  }
  else
  {
    /* Else strip off the non-directory part of the name. */
    strncpy( temp_path, pathname, FILE_OPEN_DIR_MAX );
    strncpy( File_open_dir, dirname(temp_path), FILE_OPEN_DIR_MAX );
  }

  length = strlen(File_open_dir);
  if (File_open_dir[length - 1] != '/' && length < FILE_OPEN_DIR_MAX)
  {
    File_open_dir[length++] = '/';
    File_open_dir[length] = 0;
  }
}

/**
 * Get the file open directory name.
 * \return The initial directory for the file open dialog.
 */
const char *
get_file_open_directory(void)
{
    char temp_path[FILE_OPEN_DIR_MAX];
    if (strlen(File_open_dir) == 0)
    {
      if (getcwd(temp_path, FILE_OPEN_DIR_MAX) != NULL)
        set_file_open_directory(temp_path);
    }
    return File_open_dir;
}

static VIO_BOOL has_no_extension(const char *filename)
{
  const char *dot_p = strrchr(filename, '.');
  const char *start_p = strrchr(filename, '/');
  if (start_p == NULL)
    start_p = strrchr(filename, '\\'); /* for MS-DOS */
  if (start_p == NULL)
    start_p = filename;
  return (dot_p == NULL || dot_p - start_p < 0);
}

/**
 * Return code from pclose() when zenity's cancel button is pressed.
 */
#define ZENITY_CANCELLED 256

/**
 * Prompt the user for filename. Confirm the operation with the user
 * if they are saving a file and the filename they select exists.
 *
 * \param prompt The prompt string to display.
 * \param saving TRUE if saving the file, false otherwise.
 * \param extension The default extension for this file, if any.
 * \param filename Receives the returned filename.
 */
VIO_Status
get_user_file(const char *prompt, VIO_BOOL saving, char *extension,
              VIO_STR *filename)
{
  FILE *in_fp = NULL;
  VIO_Status status = VIO_OK;
  int error_code = 0;

  if (Use_zenity_for_input)
  {
    char command[VIO_EXTREMELY_LARGE_STRING_SIZE];

    snprintf(command, VIO_EXTREMELY_LARGE_STRING_SIZE,
             "%s --title \"MNI Display: %s\" --file-selection --filename=%s",
             Zenity_command,
             prompt, get_file_open_directory());
    if (saving)
    {
      strncat(command, " --save --confirm-overwrite",
              VIO_EXTREMELY_LARGE_STRING_SIZE);
    }
    in_fp = try_popen(command, "r", &error_code);
  }
  if (in_fp == NULL)
  {
    if (error_code != ZENITY_CANCELLED)
    {
      print("%s", prompt);
      in_fp = stdin;
    }
    else
    {
      return VIO_ERROR;
    }
  }
  status = input_string(in_fp, filename, ' ');
  if (in_fp != stdin)
  {
    VIO_STR expanded = expand_filename( *filename );

    pclose(in_fp);

    if( has_no_extension( expanded ) && extension != NULL )
    {
      concat_to_string( &expanded, "." );
      concat_to_string( &expanded, extension );

      if ( file_exists( expanded ) )
      {
        FILE *yn_fp;
        char command[VIO_EXTREMELY_LARGE_STRING_SIZE];

        snprintf(command, VIO_EXTREMELY_LARGE_STRING_SIZE,
                 "%s --title \'MNI Display\' --question --text \'File \"%s\" "
                 "already exists, are you sure you want to replace it?\'",
                 Zenity_command,
                 expanded);

        yn_fp = try_popen(command, "r", &error_code);
        if (yn_fp == NULL)
        {
          status = (error_code == 0) ? VIO_OK : VIO_ERROR;
        }
        else
        {
          status = VIO_ERROR;
          pclose(yn_fp);
        }
      }
    }
    delete_string( expanded );
  }
  else if (saving && status == VIO_OK)
  {
    if (extension != NULL)
    {
      check_clobber_file_default_suffix(*filename, extension);
    }
    else
    {
      check_clobber_file(*filename);
    }
  }

  if (status == VIO_OK)
  {
    /* Now that we have the filename, use it to update the working directory.
     */
    set_file_open_directory(*filename);
  }
  return status;
}

/**
 * Generic function to get user input. This consolidates all of what
 * was previously scattered around the program, such that we can now
 * use a helper (such as zenity) to ask for information from the
 * user.
 * \param prompt A text prompt to display, hopefully explaining the
 * action the user should take.
 * \param format A text string that specifies the input expected.
 * This consists of a string consisting of a series of the characters
 * 'd', 'f', 'r',  'c', or 's' corresponding to an int, float, VIO_Real,
 * char, or VIO_STR value.
 * \returns VIO_OK if successful.
 */
VIO_Status
get_user_input(const char *prompt, const char *format, ...)
{
  const char *cp;
  va_list ap;
  VIO_Real *p_real;
  float *p_float;
  int *p_int;
  VIO_STR *p_str;
  char *p_char;
  FILE *in_fp = NULL;
  int error_code = 0;

  if (Use_zenity_for_input)
  {
    char command[VIO_EXTREMELY_LARGE_STRING_SIZE];
    snprintf(command, VIO_EXTREMELY_LARGE_STRING_SIZE,
             "%s --entry --title=\"MNI Display: Dialog\" --text=\"%s\"",
            Zenity_command,
            prompt);
    in_fp = try_popen(command, "r", &error_code);
  }
  if (in_fp == NULL)
  {
    if (error_code != ZENITY_CANCELLED)
    {
      print("%s", prompt);
      in_fp = stdin;
    }
    else
    {
      return VIO_ERROR;
    }
  }

  va_start(ap, format);
  for (cp = format; *cp != '\0'; cp++)
  {
    switch (*cp)
    {
    case 'r':
      p_real = va_arg(ap, VIO_Real *);
      if (input_real(in_fp, p_real) != VIO_OK)
      {
        return VIO_ERROR;
      }
      break;
    case 'f':
      p_float = va_arg(ap, float *);
      if (input_float(in_fp, p_float) != VIO_OK)
      {
        return VIO_ERROR;
      }
      break;
    case 's':
      p_str = va_arg(ap, VIO_STR *);
      if (*(cp + 1) == '\0')
      {
        /* If last in the format, the string is terminated by a
         * newline rather than a space. This allows inclusion of a
         * space in string inputs.
         */
        if (input_string(in_fp, p_str, '\n') != VIO_OK)
        {
          return VIO_ERROR;
        }
      }
      else
      {
        /* Otherwise terminate the string at a space, like sscanf().
         */
        if (input_string(in_fp, p_str, ' ') != VIO_OK)
        {
          return VIO_ERROR;
        }
      }
      break;
    case 'd':
      p_int = va_arg(ap, int *);
      if (input_int(in_fp, p_int) != VIO_OK)
      {
        return VIO_ERROR;
      }
      break;
    case 'c':
      p_char = va_arg(ap, char *);
      if (input_nonwhite_character(in_fp, p_char) != VIO_OK)
      {
        return VIO_ERROR;
      }
      break;
    default:
      return VIO_ERROR;
    }
  }
  va_end(ap);
  if (in_fp != stdin)
  {
      pclose(in_fp);
  }
  return VIO_OK;
}

/**
 * Prompt the user to select a colour coding type.
 *
 * \param prompt A text prompt to display, hopefully explaining the
 * action the user should take.
 * \param cc_type_ptr Where to put the returned colour coding type.
 * \returns VIO_OK if successful.
 */
VIO_Status
get_user_coding_type(const char *prompt, Colour_coding_types *cc_type_ptr)
{
    FILE *in_fp = NULL;
    int error_code = 0;
    VIO_STR line;
    char *or_bar_ptr;
    static const char colour_coding[] = {
        "Gray Spectral \"Hot Metal\" Red Blue Green User-Defined"
    };

    if (Use_zenity_for_input)
    {
        char command[VIO_EXTREMELY_LARGE_STRING_SIZE];
        snprintf(command, VIO_EXTREMELY_LARGE_STRING_SIZE,
                 "%s --list --title=\"MNI Display: Coding type\" --text=\"%s\" --column \"Colour coding\" %s",
                 Zenity_command,
                 prompt,
                 colour_coding);
        in_fp = try_popen(command, "r", &error_code);
    }
    if (in_fp == NULL)
    {
        if (error_code != ZENITY_CANCELLED)
        {
            print("%s", prompt);
            in_fp = stdin;
        }
        else
        {
            return VIO_ERROR;
        }
    }

    if (input_string(in_fp, &line, '\n') != VIO_OK)
    {
        return VIO_ERROR;
    }

    /* Zenity seems to return a result of the form "Selection|Selection"
     * in some cases (as with a double click on an item). We deal with
     * that here.
     */
    if ((or_bar_ptr = strchr(line, '|')) != NULL)
    {
        *or_bar_ptr = 0;
    }

    if (!strcmp(line, "Gray"))
        *cc_type_ptr = GRAY_SCALE;
    else if (!strcmp(line, "Spectral"))
        *cc_type_ptr = SPECTRAL;
    else if (!strcmp(line, "Hot Metal"))
        *cc_type_ptr = HOT_METAL;
    else if (!strcmp(line, "Red"))
        *cc_type_ptr = RED_COLOUR_MAP;
    else if (!strcmp(line, "Blue"))
        *cc_type_ptr = BLUE_COLOUR_MAP;
    else if (!strcmp(line, "Green"))
        *cc_type_ptr = GREEN_COLOUR_MAP;
    else if (!strcmp(line, "User-Defined"))
        *cc_type_ptr = USER_DEFINED_COLOUR_MAP;
    else
        *cc_type_ptr = SPECTRAL;

    delete_string( line );

    if (in_fp != stdin)
    {
        pclose(in_fp);
    }
    return VIO_OK;
}


/**
 * Prompt the user for filename. Confirm the operation with the user
 * if they are saving a file and the filename they select exists.
 *
 * \param prompt The prompt string to display.
 * \returns VIO_OK if the user chose "yes", VIO_ERROR otherwise.
 */
VIO_Status
get_user_yes_or_no(const char *prompt)
{
  VIO_Status status = VIO_ERROR; /* no by default */
  int error_code = -1;

  if (Use_zenity_for_input)
  {
    char command[VIO_EXTREMELY_LARGE_STRING_SIZE];
    FILE *in_fp;

    snprintf(command, VIO_EXTREMELY_LARGE_STRING_SIZE,
             "%s --title \'MNI Display\' --question --text \"%s?\"",
             Zenity_command, prompt );

    in_fp = try_popen(command, "r", &error_code);
    if (in_fp == NULL)
    {
      error_code = WEXITSTATUS(error_code);
    }
    else
    {
      error_code = 1;
      pclose(in_fp);
    }
    status = (error_code == 0) ? VIO_OK : VIO_ERROR;
  }

  /* Exit status of zero or one indicates user successfully chose
   * either yes (0) or no (1). Otherwise, we need to prompt on the
   * command line.
   */
  if (error_code != 0 && error_code != 1)
  {
    VIO_STR line;
    VIO_BOOL answered = FALSE;
    while (!answered)
    {
      print("%s (yes/no)? ", prompt);
      if (input_string(stdin, &line, '\n') == VIO_OK)
      {
        if (!strcasecmp(line, "yes"))
        {
          status = VIO_OK;
          answered = TRUE;
        }
        else if (!strcasecmp(line, "no"))
        {
          answered = TRUE;
        }
        else
        {
          print("Please answer yes or no.\n");
        }
        delete_string(line);
      }
      else
      {
        print("Sorry, I didn't catch that.\n");
      }
    }
  }

  return status;
}
