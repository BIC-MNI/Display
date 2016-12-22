/**
 * \file graphics.c
 * \brief Basic creation and initialization of "graphics" windows.
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <display.h>
#include <assert.h>

static  display_struct  *_windows[N_WINDOW_TYPES];

static  void  initialize_graphics_window(
    display_struct   *display );
static  void  update_graphics_overlay_planes_only(
    display_struct       *display,
    VIO_BOOL              display_flag );
static  void  update_graphics_normal_planes_only(
    display_struct               *display );
static  void  terminate_graphics_window(
    display_struct   *display );

/* --------------------------------------------------------------------- */

/**
 * Retrieves a pointer to the windows array.
 * \param display A pointer to the array of display_struct pointers. Some of
 * the array entries may be NULL.
 * \returns The number of window entries in the list.
 */
int get_list_of_windows(
    display_struct  ***display )
{
    *display = _windows;

    return( N_WINDOW_TYPES );
}

/**
 * Find a window of the given type.
 */
display_struct *get_display_by_type( window_types type )
{
  if ( type >= N_WINDOW_TYPES )
    return NULL;
  return _windows[ type ];
}

/**
 * Find the display_struct associated with the given Gwindow.
 * Uses a linear search, which is probably ok given that N_WINDOW_TYPES
 * is small.
 * \param window The window handle for which we want the display_struct.
 * \return A pointer to the display_struct associated with the window.
 */
display_struct  *lookup_window(
    Gwindow   window )
{
    int              i;
    display_struct   *display;

    display = NULL;

    for_less( i, 0, N_WINDOW_TYPES )
    {
        if( _windows[i] != NULL && _windows[i]->window == window )
        {
            display = _windows[i];
            break;
        }
    }

    return( display );
}

/**
 * Allocates space for a new window of the given type. It is an
 * error to create two windows of the same type.
 *
 * \param window_type The type of the window to create.
 * \return A pointer to a new, uninitialized display_struct.
 */
static display_struct *
get_new_display(window_types window_type)
{
    if ( _windows[window_type] != NULL )
    {
      print_error("Creating a second window of type %d\n", window_type);
      return NULL;
    }

    ALLOC( _windows[window_type], 1 );
    return _windows[window_type];
}

/**
 * Start up the graphics library.
 */
void  initialize_graphics( void )
{
}

/**
 * Stop the graphics library and free any memory associated with it.
 */
void  terminate_graphics( void )
{
    int i;

    for_less( i, 0, N_WINDOW_TYPES )
    {
      if ( _windows[i] != NULL )
      {
        terminate_graphics_window( _windows[i] );
        FREE( _windows[i] );
        _windows[i] = NULL;
      }
    }
}

/**
 * Prints the state of the graphics system, so that it can be 
 * saved in a configuration file, e.g. This assumes knowledge
 * of the relevant global variables. We should find a way to encode
 * this information in the variable structures themselves, and create
 * a function to print a global.
 * \param fp The stream to which we write the data.
 */
void print_graphics_state(FILE *fp)
{
  int i;
  static char *names[] = { "3D", "menu", "slice", "marker" };
  for_less( i, 0, N_WINDOW_TYPES )
  {
    if ( _windows[i] != NULL ) {
      int x, y;
      int cx, cy;
      G_get_window_position( _windows[i]->window, &x, &y );
      G_get_window_size( _windows[i]->window, &cx, &cy );
      
      fprintf(fp, "Initial_%s_window_x = %d;\n", names[i], x);
      fprintf(fp, "Initial_%s_window_y = %d;\n", names[i], y);
      fprintf(fp, "Initial_%s_window_width = %d;\n", names[i], cx);
      fprintf(fp, "Initial_%s_window_height = %d;\n", names[i], cy);
    }
  }
}

static Gwindow _parent_window = NULL;

Gwindow get_parent_window()
{
  return _parent_window;
}

#define MENU_HEIGHT 0.35
#define MARKER_HEIGHT 0.15
#define COLUMN_WIDTH 0.5

/**
 * Organize the windows into the desired locations.
 */
static void
parent_resize( Gwindow win, int _x, int _y, int width, int height, void *data )
{
  int i;
  int n_windows;
  display_struct  **windows, *display;
  int x, y, cx, cy;
  const int M = 2;

  n_windows = get_list_of_windows( &windows );

  for_less( i, 0, n_windows )
  {
    if( windows[i] == NULL )
      continue;

    display = windows[i];

    switch ( display->window_type )
    {
    case MENU_WINDOW:
      x = (int)(width * COLUMN_WIDTH);
      y = height - (int)(height*MENU_HEIGHT) + M;
      cx = (int)(width * COLUMN_WIDTH) - M;
      cy = (int)(height*MENU_HEIGHT) - 2 * M;
      break;

    case MARKER_WINDOW:
      x = (int)(width * COLUMN_WIDTH);
      y = height - (int)(height*(MENU_HEIGHT + MARKER_HEIGHT)) + M;
      cx = (int)(width * COLUMN_WIDTH) - M;
      cy = (int)(height*MARKER_HEIGHT) - M;
      break;

    case THREE_D_WINDOW:
      x = (int)(width * COLUMN_WIDTH);
      y = 0 + M;
      cx = (int)(width * COLUMN_WIDTH) - M;
      cy = height - (int)(height*(MARKER_HEIGHT+MENU_HEIGHT)) - M;
      break;

    case SLICE_WINDOW:
      x = 0 + M;
      y = 0 + M;
      cx = (int)(width * COLUMN_WIDTH) - 2 * M;
      cy = height - 2 * M;
      break;

    default:
      continue;
    }
    G_set_geometry( display->window, x, y, cx, cy );
    G_set_update_flag( display->window );
  }
}

static void parent_update( Gwindow win, void *data )
{
  G_clear_window( win );
}

void create_parent_window()
{
  G_create_child_window( "MNI-Display", 0, 0, 1024, 768, FALSE, FALSE, TRUE, 0,
                         &_parent_window, NULL );
  G_set_update_function( _parent_window, parent_update, NULL );
  G_set_resize_function( _parent_window, parent_resize, NULL );
}

VIO_Status  create_graphics_window(
    window_types      window_type,
    VIO_BOOL          double_buffering,
    display_struct    **display,
    VIO_STR           title,
    int               x,
    int               y,
    int               width,
    int               height )
{
    VIO_Status   status;
    Gwindow      parent = get_parent_window();

    *display = get_new_display( window_type );

    status = G_create_child_window( title, x, y, width, height,
                                    FALSE, double_buffering, TRUE, 2,
                                    &(*display)->window, parent );

    G_set_background_colour( (*display)->window, Initial_background_colour );

    if( status == VIO_OK )
    {
        (*display)->window_type = window_type;
        initialize_graphics_window( *display );
    }
    else
    {
        print( "Cannot open window.\n" );
    }

    return( status );
}

  model_struct  *get_graphics_model(
    display_struct    *display,
    int               model_index )
{
    return( get_model_ptr(display->models[model_index]) );
}

  model_info_struct  *get_model_info(
    model_struct   *model )
{
    return( (model_info_struct *) model->extra_ptr );
}

  Bitplane_types  get_model_bitplanes(
    model_struct   *model )
{
    return( get_model_info(model)->bitplanes );
}

/** Creates a new model an inserts it into the current model.
 */
void  create_model_after_current(
    display_struct   *display )
{
    model_struct   *model;
    object_struct  *new_model;

    model = get_current_model( display );

    new_model = create_object( MODEL );

    initialize_display_model( get_model_ptr(new_model) );
    initialize_3D_model_info( model );

    replace_string( &get_model_ptr(new_model)->filename,
                    create_string("Created") );

    add_object_to_model( model, new_model );
}

  void  initialize_model_info(
    model_struct   *model )
{
    model_info_struct  *model_info;

    ALLOC( model_info, 1 );

    assign_model_extra_ptr( model, (void *) model_info );

    model_info->view_type = MODEL_VIEW;
    model_info->bitplanes = NORMAL_PLANES;
    initialize_render( &model_info->render );
    make_identity_transform( &model_info->transform );
}

  void  initialize_3D_model_info(
    model_struct   *model )
{
    model_info_struct  *model_info;

    model_info = (model_info_struct *) get_model_extra_ptr( model );

    initialize_render_3D( &model_info->render );
}

  void  initialize_display_model(
    model_struct   *model )
{
    initialize_model( model );
    initialize_model_info( model );
}

  void  terminate_display_model(
    model_struct   *model )
{
    model_info_struct  *model_info;

    model_info = (model_info_struct *) get_model_extra_ptr( model );

    FREE( model_info );
}

  Bitplane_types  get_cursor_bitplanes( void )
{
    return( (Bitplane_types) Cursor_bitplanes );
}

static  void  initialize_graphics_window(
    display_struct   *display )
{
    int                 i;
    View_types          view_type;
    model_struct        *model;
    model_info_struct   *model_info;

    display->models_changed_id = 0;

    fill_Point( display->prev_mouse_position, -1, -1, -1);
    fill_Point( display->prev_mouse_pixel_position, -1, -1, -1);

    initialize_action_table( &display->action_table );

    initialize_menu_actions( display );

    for_less( i, 0, N_MODELS )
    {
        display->models[i] = create_object( MODEL );
        model = get_graphics_model( display, i );
        initialize_display_model( model );
        model_info = get_model_info( model );

        model->n_objects = 0;

        switch( display->window_type )
        {
        case THREE_D_WINDOW:
            if (i == STATUS_MODEL)
                view_type = PIXEL_VIEW;
            else
                view_type = MODEL_VIEW;
            break;
        case MENU_WINDOW:      view_type = PIXEL_VIEW;   break;
        case SLICE_WINDOW:     view_type = PIXEL_VIEW;   break;
        case MARKER_WINDOW:    view_type = PIXEL_VIEW;   break;
        default:               print_error("Illegal window type."); break;
        }

        model_info->view_type = view_type;

        if( display->window_type == THREE_D_WINDOW &&
            (i == OVERLAY_MODEL ||
             (i == CURSOR_MODEL && get_cursor_bitplanes() == OVERLAY_PLANES)) &&
            G_has_overlay_planes() )
        {
            model_info->bitplanes = OVERLAY_PLANES;
        }
        else if( display->window_type == SLICE_WINDOW &&
                 i == SLICE_READOUT_MODEL )
        {
            model_info->bitplanes = get_slice_readout_bitplanes();
        }
        else
        {
            model_info->bitplanes = NORMAL_PLANES;
        }

        model->n_objects = 0;
        replace_string( &model->filename, create_string("Top Level") );

        if( display->window_type == THREE_D_WINDOW && i == THREED_MODEL )
        {
            initialize_render_3D( &model_info->render );
        }

        if( display->window_type == THREE_D_WINDOW && i == OVERLAY_MODEL )
        {
            model_info->render.shaded_mode = WIREFRAME;
            model_info->render.master_light_switch = FALSE;
        }

        if( display->window_type == THREE_D_WINDOW && i == MISCELLANEOUS_MODEL )
        {
            model_info->render.shaded_mode = FILLED;
            model_info->render.backface_flag = FALSE;
        }

        make_identity_transform( &model_info->transform );
    }

    if( display->window_type == THREE_D_WINDOW )
        initialize_three_d_window( display );

    if( display->window_type == MENU_WINDOW )
        initialize_menu_window( display );

    if( display->window_type == MARKER_WINDOW )
        initialize_marker_window( display );

    display->frame_number = 0;
    display->update_required[NORMAL_PLANES] = FALSE;
    display->update_required[OVERLAY_PLANES] = FALSE;

    initialize_window_callbacks( display );
}

  void  set_update_required(
    display_struct   *display,
    Bitplane_types   which_bitplanes )
{
    display->update_required[which_bitplanes] = TRUE;
}

  VIO_BOOL  graphics_normal_planes_update_required(
    display_struct   *display )
{
    return( display->update_required[NORMAL_PLANES] );
}

  VIO_BOOL  graphics_update_required(
    display_struct   *display )
{
    return( display->update_required[NORMAL_PLANES] ||
            display->update_required[OVERLAY_PLANES] );
}

  void  graphics_models_have_changed(
    display_struct  *display )
{
    rebuild_selected_list( display, _windows[MARKER_WINDOW] );

    set_update_required( display, NORMAL_PLANES );
    set_update_required( _windows[MARKER_WINDOW], NORMAL_PLANES );

    /*
     * If necessary, tell the slice window it needs to update.
     */
    if (Object_outline_enabled && _windows[SLICE_WINDOW] != NULL)
    {
        set_slice_outline_update( _windows[SLICE_WINDOW], -1 );
        set_update_required( _windows[SLICE_WINDOW], NORMAL_PLANES );
    }

    ++display->models_changed_id;
}

static  void  display_frame_info(
    display_struct   *display,
    int              frame_number,
    VIO_Real             update_time )
{
    text_struct   frame_text;
    char          buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    VIO_STR        frame_time_str;
    VIO_Point         origin;
    model_struct  *model;

    fill_Point( origin, Frame_info_x, Frame_info_y, 0.0 );

    initialize_text( &frame_text, &origin, WHITE, FIXED_FONT, 0.0 );

    snprintf( buffer, sizeof(buffer), "%d: ", frame_number );

    frame_time_str = format_time( "%g %s", update_time );

    frame_text.string = concat_strings( buffer, frame_time_str );

    G_set_view_type( display->window, PIXEL_VIEW );

    model = get_graphics_model( display, THREED_MODEL );

    set_render_info( display->window, &get_model_info(model)->render );
    G_draw_text( display->window, &frame_text );

    delete_string( frame_time_str );
    delete_text( &frame_text );
}

  void  update_graphics( display_struct *display )
{
    if (display->update_required[NORMAL_PLANES] )
    {
        update_graphics_normal_planes_only( display );
    }
    else if( display->update_required[OVERLAY_PLANES] )
    {
        update_graphics_overlay_planes_only( display, TRUE );
    }
}

static  void  draw_in_viewports(
    display_struct               *display,
    Bitplane_types               bitplanes )
{
    int           i, x_min, x_max, y_min, y_max;
    int           x_sub_min, x_sub_max, y_sub_min, y_sub_max;
    VIO_BOOL       draw_model, redrawing_whole_window;

    redrawing_whole_window = FALSE;

    if (display->window_type != SLICE_WINDOW)
    {
        /* Find display all of the opaque objects. */
        for_less( i, 0, N_MODELS )
        {
            display_objects( display->window, display->models[i],
                             bitplanes, DISPLAY_OPAQUE );
        }

        /* Now display all of the translucent objects. */
        for_less( i, 0, N_MODELS )
        {
            display_objects( display->window, display->models[i],
                             bitplanes, DISPLAY_TRANSLUCENT );
        }
    }
    else
    {
        for_less( i, 0, N_MODELS )
        {
            draw_model = display->slice.viewport_update_flags[i][0];
            if( draw_model )
            {
                get_slice_model_viewport( display, i, &x_min, &x_max,
                                          &y_min, &y_max );

                if( i >= SLICE_MODEL1 && i < SLICE_MODEL1 + N_SLICE_VIEWS &&
                    get_slice_subviewport( display, i-SLICE_MODEL1,
                                           &x_sub_min, &x_sub_max,
                                           &y_sub_min, &y_sub_max ) )
                {
                    display->slice.slice_views[i-SLICE_MODEL1].
                                             sub_region_specified = FALSE;

                    if( !redrawing_whole_window )
                    {
                        G_set_viewport( display->window,
                                        x_min + x_sub_min, x_min + x_sub_max,
                                        y_min + y_sub_min, y_min + y_sub_max );
                        G_clear_viewport( display->window,
                                          display->window->background_colour );
                    }

                    G_set_viewport( display->window,
                                    x_min, x_max, y_min, y_max );
                }
                else
                {
                    G_set_viewport( display->window,
                                    x_min, x_max, y_min, y_max );
                    if( !redrawing_whole_window )
                        G_clear_viewport( display->window,
                                          display->window->background_colour );
                }

                if( i == FULL_WINDOW_MODEL )
                    redrawing_whole_window = TRUE;

                if( bitplanes == NORMAL_PLANES &&
                    G_get_double_buffer_state( display->window ) )
                {
                    display->slice.viewport_update_flags[i][0] =
                          display->slice.viewport_update_flags[i][1];
                    display->slice.viewport_update_flags[i][1] = FALSE;
                }
                else
                    display->slice.viewport_update_flags[i][0] = FALSE;
            }

            if( draw_model )
            {
                display_objects( display->window, display->models[i],
                                 bitplanes, DISPLAY_BOTH );
            }
        }
    }
}

static  void  update_graphics_overlay_planes_only(
    display_struct       *display,
    VIO_BOOL              display_flag )
{
    if( G_has_overlay_planes() )
    {
        G_set_bitplanes( display->window, OVERLAY_PLANES );

        if( display_flag )
        {
            draw_in_viewports( display, OVERLAY_PLANES );
        }

        G_update_window( display->window );

        G_set_bitplanes( display->window, NORMAL_PLANES );
    }

    display->update_required[OVERLAY_PLANES] = FALSE;
}

static  void  update_graphics_normal_planes_only( display_struct *display )
{
    VIO_Real      start, end;

    start = current_realtime_seconds();

    draw_in_viewports( display, NORMAL_PLANES );

    end = current_realtime_seconds();

    ++display->frame_number;

    if( Display_frame_info )
        display_frame_info( display, display->frame_number, end - start );

    G_update_window( display->window );

    display->update_required[NORMAL_PLANES] = FALSE;
}

static  void  terminate_graphics_window(
    display_struct   *display )
{
    int      i;

    if( display->window_type == THREE_D_WINDOW )
        delete_three_d( display );

    if( display->window_type == MENU_WINDOW )
        delete_menu( &display->menu );

    if( display->window_type == SLICE_WINDOW )
        delete_slice_window( display );

    for_less( i, 0, N_MODELS )
        delete_object( display->models[i] );
}

  void  update_view(
    display_struct  *display )
{
    G_set_3D_view( display->window,
                   &display->three_d.view.origin,
                   &display->three_d.view.line_of_sight,
                   &display->three_d.view.y_axis,
                   display->three_d.view.front_distance,
                   display->three_d.view.back_distance,
                   display->three_d.view.perspective_flag,
                   display->three_d.view.perspective_distance,
                   display->three_d.view.stereo_flag,
                   display->three_d.view.eye_separation_ratio *
                   display->three_d.view.perspective_distance,
                   display->three_d.view.window_width,
                   display->three_d.view.window_height );
    G_set_modeling_transform( display->window,
                              &display->three_d.view.modeling_transform );
}

  void  fit_view_to_visible_models(
    display_struct   *display )
{
    int     c;
    VIO_Point   min_limit, max_limit;
    VIO_Point   misc_min_limit, misc_max_limit;

    if( !get_range_of_object( display->models[THREED_MODEL],
                              TRUE, &min_limit, &max_limit ) )
    {
        if( !get_range_of_object( display->models[MISCELLANEOUS_MODEL],
                                  TRUE, &min_limit, &max_limit ) )
        {
            if( !get_range_of_volumes( display, &min_limit, &max_limit ) )
            {
                fill_Point( min_limit, -1.0, -1.0, -1.0 );
                fill_Point( max_limit, 1.0, 1.0, 1.0 );
            }
        }
    }
    else if( get_range_of_object( display->models[MISCELLANEOUS_MODEL],
                                  TRUE, &misc_min_limit, &misc_max_limit ) )
    {
        for_less( c, 0, VIO_N_DIMENSIONS )
        {
            if( Point_coord(misc_min_limit,c) < Point_coord(min_limit,c) )
                Point_coord(min_limit,c) = Point_coord(misc_min_limit,c);
            if( Point_coord(misc_max_limit,c) > Point_coord(max_limit,c) )
                Point_coord(max_limit,c) = Point_coord(misc_max_limit,c);
        }
    }

    fit_view_to_domain( &display->three_d.view, &min_limit, &max_limit );
}

  void  reset_view_parameters(
    display_struct   *display,
    VIO_Vector           *line_of_sight,
    VIO_Vector           *horizontal )
{
    initialize_view( &display->three_d.view,
                     line_of_sight, horizontal );
    display->three_d.view.perspective_flag = Initial_perspective_flag;
    set_model_scale( &display->three_d.view,
                     Initial_x_scale, Initial_y_scale, Initial_z_scale );
    adjust_view_for_aspect( &display->three_d.view, display->window );
    fit_view_to_visible_models( display );
}

  VIO_Real  size_of_domain(
    display_struct   *display )
{
    VIO_Vector   diff;

    SUB_POINTS( diff, display->three_d.max_limit, display->three_d.min_limit );

    return( MAGNITUDE(diff) );
}
