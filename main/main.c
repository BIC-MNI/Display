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
static  void   initialize_ratio (display_struct* display);
static  void   initialize_view_to_fit (display_struct  *display );
static  void   initialize_cache ();
static  void   parse_options (int argc, char *argv[],
                              display_struct *graphics);

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
 * Unhide the 3D and "marker" window if they are loaded.
 */
static void
unhide_if_objects_loaded(display_struct *graphics, display_struct *markers)
{
  model_struct *model = get_current_model( graphics );
  if (model->n_objects > 1)
  {
    G_set_visibility(graphics->window, TRUE);
    G_set_visibility(markers->window, TRUE);
  }
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
    display_struct   *menu, *slice_window, *marker;
    VIO_STR           globals_filename, runtime_directory;
    int               n_directories;
    VIO_STR           *directories;
    VIO_STR           title;

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

    if( n_directories > 0 )
        FREE( directories );

    set_alloc_checking( Alloc_checking_enabled );

    initialize_graphics();

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
     MENU_FILENAME ) != VIO_OK )
       return 1;
     
    if( initialize_marker_window( marker ) != VIO_OK )
       return 1;

    delete_string( runtime_directory );

    parse_options(argc, argv, graphics);

    if( get_slice_window( graphics, &slice_window ) )
    {
        for_less( view, 0, N_SLICE_VIEWS )
            reset_slice_view( slice_window, view );
        initialize_ratio( slice_window );
    }

    initialize_cache(  );/*graphics*/
    initialize_view_to_fit( graphics );

    initialize_scroll_magnification( graphics );

    rebuild_selected_list( graphics, marker );
    reset_view_parameters( graphics, &Default_line_of_sight,
                           &Default_horizontal );

    update_view( graphics );
    update_all_menu_text( graphics );

    unhide_if_objects_loaded(graphics, marker);

    /* Doublecheck that the 3D cursor is consistent with the current
     * voxel coordinates.
     */
    update_cursor_from_voxel( slice_window );

    set_update_required( graphics, NORMAL_PLANES );
    set_update_required( marker, NORMAL_PLANES );

    (void) main_event_loop();

    terminate_graphics();

    delete_marching_cubes_table();

    delete_global_variables( VIO_SIZEOF_STATIC_ARRAY(display_globals),
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

static void initialize_ratio (display_struct* slice_window)
{
  model_struct      *model;
  int               retcode;
  text_struct       *text;

  slice_window->slice.ratio_enabled = FALSE;

  if( string_length(Ratio_volume_index) )
  {
    retcode = sscanf(Ratio_volume_index, Ratio_volume_index_format,
                     &slice_window->slice.ratio_volume_numerator,
                     &slice_window->slice.ratio_volume_denominator);
    if( retcode != 2 )
      fprintf(stderr, "Error: can not parse %s with %s\n",
              Ratio_volume_index, Ratio_volume_index_format);
    else
    {
      slice_window->slice.ratio_enabled = TRUE;
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


/**
 * Try to make sense out of command-line arguments.
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
            "Set the contrast range.");
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
      Initial_colour_coding_type = GRAY_SCALE;
    }
    else if (equal_strings(filename, "-hot"))
    {
      Initial_colour_coding_type = HOT_METAL;
    }
    else if (equal_strings(filename, "-spectral"))
    {
      Initial_colour_coding_type = SPECTRAL;
    }
    else if (equal_strings(filename, "-red"))
    {
      Initial_colour_coding_type = RED_COLOUR_MAP;
    }
    else if (equal_strings(filename, "-green"))
    {
      Initial_colour_coding_type = GREEN_COLOUR_MAP;
    }
    else if (equal_strings(filename, "-blue"))
    {
      Initial_colour_coding_type = BLUE_COLOUR_MAP;
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
    else if (equal_strings(filename, "-range"))
    {
      VIO_Real lo_val, hi_val;

      if (!get_real_argument(0.0, &lo_val) || !get_real_argument(1.0, &hi_val))
      {
        print_error("Error in arguments after -range.\n");
        exit(EX_USAGE);
      }

      if (Initial_histogram_contrast)
      {
        Initial_histogram_low = lo_val;
        Initial_histogram_high = hi_val;
      }
      else
      {
        Initial_low_absolute_position = lo_val;
        Initial_high_absolute_position = hi_val;
      }
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
        initialize_cache();
        if (load_graphics_file(graphics, filename, next_is_label_volume) != VIO_OK)
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
void
set_file_open_directory(const char *pathname)
{
  int length = 0;
  strncpy(File_open_dir, pathname, FILE_OPEN_DIR_MAX);
  strncpy(File_open_dir, dirname(File_open_dir), FILE_OPEN_DIR_MAX);
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
    if (strlen(File_open_dir) == 0)
    {
      getcwd(File_open_dir, FILE_OPEN_DIR_MAX);
      set_file_open_directory(File_open_dir);
    }
    return File_open_dir;
}

/* Return code from pclose() when zenity's cancel button is pressed.
 */
#define ZENITY_CANCELLED 256

VIO_Status
get_user_file(const char *prompt, VIO_BOOL saving, VIO_STR *filename)
{
  FILE *in_fp = NULL;
  VIO_Status status = VIO_OK;
  int error_code = 0;

  if (Use_zenity_for_input)
  {
    char command[VIO_EXTREMELY_LARGE_STRING_SIZE];

    snprintf(command, VIO_EXTREMELY_LARGE_STRING_SIZE,
             "zenity --title \"Display: %s\" --file-selection --filename=%s",
             prompt, get_file_open_directory());
    if (saving)
    {
      strncat(command, " --save", VIO_EXTREMELY_LARGE_STRING_SIZE);
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
    pclose(in_fp);
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
             "zenity --entry --title=\"Display: Dialog\" --text=\"%s\"",
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
