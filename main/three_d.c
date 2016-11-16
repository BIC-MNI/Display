/**
 * \file three_d.c
 * \brief Basic initialization for the 3D view window.
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

#include  <display.h>
#include <math.h>

static  void  initialize_three_d_events( display_struct *display );

static void define_lights( display_struct *display );

static void initialize_status_line( display_struct *display );
static void update_status_line(display_struct *display);

static void initialize_vertex_colour_bar( display_struct *display,
                                          colour_bar_struct *cb_ptr );
static void update_vertex_colour_coding( display_struct *display,
                                         vertex_data_struct *vtxd_ptr);
static void update_vertex_colour_bar( display_struct *display,
                                      vertex_data_struct *vtxd_ptr );
void set_vertex_colour_bar_visibility(display_struct *display, VIO_BOOL visible);
static vertex_data_struct *find_vertex_data( display_struct *display,
                                            object_struct *object);
static VIO_BOOL get_vertex_value(display_struct *display,
                                 object_struct *object,
                                 int index, VIO_Real *value_ptr,
                                 VIO_STR *column_ptr);

/* Parameters of the vertex data colour bar in the 3D view window.
 */
#define VTX_COLOURBAR_X 10
#define VTX_COLOURBAR_WIDTH 20
#define VTX_COLOURBAR_Y 30
#define VTX_COLOURBAR_HEIGHT 200
#define VTX_TICK_WIDTH 10
#define VTX_TOL 4

/**
 * Initializes the 3D window structure (three_d_window_struct).
 *
 * \param display The display_struct of the 3D view window.
 */
void  initialize_three_d_window(
    display_struct   *display )
{
    static VIO_Vector      line_of_sight = { { 0.0f, 0.0f, -1.0f } };
    static VIO_Vector      horizontal = { { 1.0f, 0.0f, 0.0f } };
    three_d_window_struct  *three_d;

    initialize_resize_events( display );

    three_d = &display->three_d;

    three_d->default_marker_structure_id = Default_marker_structure_id;
    three_d->default_marker_patient_id = Default_marker_patient_id;
    three_d->default_marker_type = (Marker_types) Default_marker_type;
    three_d->default_marker_size = Default_marker_size;
    three_d->default_marker_colour = Default_marker_colour;
    three_d->default_marker_label = create_string( Default_marker_label );

    initialize_view( &three_d->view, &line_of_sight, &horizontal );
    three_d->view.perspective_flag = Initial_perspective_flag;
    adjust_view_for_aspect( &three_d->view, display->window );
    update_view( display );

    initialize_lights( three_d->lights );

    G_set_drawing_interrupt_state( display->window, FALSE );

    three_d->lights[0].colour = Directional_light_colour;
    three_d->lights[0].direction = Light_direction;

    define_lights( display );

    fill_Point( three_d->min_limit, 0.0, 0.0, 0.0 );
    fill_Point( three_d->max_limit, 1.0, 1.0, 1.0 );

    ADD_POINTS( three_d->centre_of_objects, three_d->min_limit,
                three_d->max_limit );
    SCALE_POINT( three_d->centre_of_objects, three_d->centre_of_objects, 0.5 );

    /* Set up the colour bar first so it sees mouse events first! */
    display->three_d.colour_bar.top_offset = Colour_bar_top_offset;
    display->three_d.colour_bar.bottom_offset = VTX_COLOURBAR_Y;
    display->three_d.colour_bar.left_offset = VTX_COLOURBAR_X;
    display->three_d.colour_bar.bar_width = VTX_COLOURBAR_WIDTH;
    display->three_d.colour_bar.tick_width = VTX_TICK_WIDTH;
    display->three_d.colour_bar.desired_n_intervals = Colour_bar_desired_intervals;
    initialize_vertex_colour_bar( display, &display->three_d.colour_bar );

    initialize_three_d_events( display );

    reset_view_parameters( display, &Default_line_of_sight,
                           &Default_horizontal );

    update_view( display );

    initialize_surface_edit( &display->three_d.surface_edit );

    initialize_surface_extraction( display );

    initialize_current_object( display );

    initialize_cursor( display );

    initialize_surface_curve( display );

    initialize_volume_cross_section( display );

    G_set_window_update_min_interval( display->window,
                                      Min_interval_between_updates );

    initialize_status_line(display);

    display->three_d.vertex_data_array = NULL;
    display->three_d.vertex_data_count = 0;

    display->three_d.mouse_obj = NULL;
    display->three_d.mouse_point = -1;
    display->three_d.animation_axis = -1;
}

/**
 * \brief Show the 3D and "marker" window if any objects are loaded or created. 
 *
 * This overrides any global settings.
 *
 * We want to automatically force the window to be visible if either
 * of two conditions are met:
 *
 *  1. A new 3D object has been loaded.
 *  2. A surface has been created.
 *
 * The first check is easy, in that the root model will definitely
 * contain at least two objects if a file has been loaded.
 *
 * The second check is harder and requires that we inspect the special
 * polygons object in the first position of the model, and see if it
 * contains any points or items.
 *
 * \param graphics A pointer to the display_struct of the 3D View window.
 * \param markers A pointer to the display_struct of the object list window.
 */
void
show_three_d_window( display_struct *graphics,
                     display_struct *markers )
{
    model_struct *model_ptr = get_graphics_model( graphics, THREED_MODEL );

    if ( model_ptr->n_objects >= 1 ) /* At least one object? */
    {
      if ( model_ptr->n_objects == 1 )
      {
        object_struct *object_ptr = model_ptr->objects[0];
        polygons_struct *poly_ptr;
        if ( object_ptr->object_type != POLYGONS )
        {
          return;         /* Sanity check, should never reach this. */
        }

        /* See if a surface has been created.
         */
        poly_ptr = get_polygons_ptr( object_ptr );
        if ( poly_ptr->n_points == 0 && poly_ptr->n_items == 0 )
        {
          return;
        }
      }
      G_set_visibility( graphics->window, TRUE );
      G_set_visibility( markers->window, TRUE );
    }
}

/**
 * Create the graphical objects and data structures used to maintain the
 * status line in the 3D view window.
 *
 * \param display The display_struct for the 3D view window.
 */
static void
initialize_status_line(display_struct *display)
{
    model_struct *model_ptr = get_graphics_model( display, STATUS_MODEL );
    object_struct *object_ptr = create_object(TEXT);
    text_struct *text_ptr = get_text_ptr(object_ptr);
    VIO_Point origin;

    fill_Point(origin, 10.0, 10.0, 0.0);
    initialize_text(text_ptr, &origin, Readout_text_rgb_colour,
                    Slice_readout_text_font, Slice_readout_text_font_size);
    set_object_visibility(object_ptr, FALSE);
    add_object_to_model(model_ptr, object_ptr);
}

/**
 * Define light sources for the 3D view window.
 *
 * \param display The display_struct of the 3D view window.
 */
static void
define_lights( display_struct   *display )
{
    three_d_window_struct  *three_d;

    three_d = &display->three_d;

    G_set_ambient_light( display->window, Ambient_light_colour );
    G_define_light( display->window, 0,
                    three_d->lights[0].light_type,
                    three_d->lights[0].colour,
                    &three_d->lights[0].direction,
                    &three_d->lights[0].position,
                    three_d->lights[0].spot_exponent,
                    three_d->lights[0].spot_angle );

    G_set_light_state( display->window, 0, three_d->lights[0].state );
}

static    DEF_EVENT_FUNCTION( handle_resize_three_d );
static    DEF_EVENT_FUNCTION( handle_mouse_movement );
static    DEF_EVENT_FUNCTION( handle_left_down );
static    DEF_EVENT_FUNCTION( handle_middle_down );
static    DEF_EVENT_FUNCTION( adjust_lo_limit );
static    DEF_EVENT_FUNCTION( adjust_hi_limit );
static    DEF_EVENT_FUNCTION( finish_lo_limit );
static    DEF_EVENT_FUNCTION( finish_hi_limit );

/**
 * Handle general mouse movement. The only normal purpose for this is to
 * update the 3D view window's status line.
 *
 * \param display The display_struct of the 3D view window.
 * \param event_type The event code for this event.
 * \param key_pressed The keyboard code associated with this event, if any.
 *
 * \returns VIO_OK if the event processing should continue.
 */
static DEF_EVENT_FUNCTION(handle_mouse_movement)
{
    int x, y, ox, oy;

    if( pixel_mouse_moved( display, &x, &y, &ox, &oy ))
    {
        update_status_line( display );
    }

    if (display->three_d.animation_axis >= 0)
    {
        VIO_Transform transform;

        VIO_Real radians_per_frame = (2 * M_PI *
                                      Min_interval_between_updates *
                                      display->three_d.animation_rpm);

        make_rotation_transform( radians_per_frame,
                                 display->three_d.animation_axis,
                                 &transform );

        apply_transform_in_view_space( display, &transform );

        update_view( display );

        set_update_required( display, NORMAL_PLANES );
    }
    return VIO_OK;
}

DEF_MENU_FUNCTION(animation_toggle)
{
  if (display->three_d.animation_axis < VIO_X)
  {
    int volume_index;
    int axis_index;

    if (get_axis_index_under_mouse( display, &volume_index, &axis_index) &&
        (axis_index >= VIO_X && axis_index <= VIO_Z))
    {
      display->three_d.animation_axis = axis_index;
    }
    else
    {
      /* For now, just rotate around the Z axis */
      display->three_d.animation_axis = VIO_Z;
    }
    display->three_d.animation_rpm = 1.0 / 5.0; /* 0.2 revolutions/second */
  }
  else
  {
    display->three_d.animation_axis = -1;
  }
  return VIO_OK;
}

DEF_MENU_UPDATE(animation_toggle)
{
  return TRUE;
}

/**
 * Install handlers for various window events we need to accept.
 *
 * \param display The display_struct of the 3D view window.
 */
static  void  initialize_three_d_events(
    display_struct  *display )
{
    add_action_table_function( &display->action_table, WINDOW_RESIZE_EVENT,
                               handle_resize_three_d );

    add_action_table_function( &display->action_table, NO_EVENT,
                               handle_mouse_movement );

    initialize_magnification( display );

    /* Now install the other mouse handling functions.
     */
    initialize_virtual_spaceball( display );

    initialize_picking_object( display );
}

/* ARGSUSED */

/**
 * Event handler for window resize operations.
 *
 * \param display The display_struct of the 3D view window.
 * \param event_type The event code for this event.
 * \param key_pressed The keyboard code associated with this event, if any.
 *
 * \returns VIO_OK if the event processing should continue.
 */
static  DEF_EVENT_FUNCTION( handle_resize_three_d )
{
    adjust_view_for_aspect( &display->three_d.view, display->window );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/**
 * Delete the resources allocated for the 3D view window.
 *
 * \param display The display_struct for the 3D view window.
 */
void  delete_three_d(
    display_struct  *display )
{
    int i;
    delete_string( display->three_d.default_marker_label );
    delete_surface_extraction( display );

    for_less(i, 0, display->three_d.vertex_data_count )
    {
      vertex_data_struct *vtxd_ptr = display->three_d.vertex_data_array[i];
      delete_vertex_data( vtxd_ptr );
    }
    FREE( display->three_d.vertex_data_array);
}

/**
 * Add the given object to the currently selected model.
 * Makes the new object the currently selected object, and 
 * shows the 3D and object windows if necessary.
 *
 * \param display The display_struct of the 3D view window.
 * \param object The graphical object to add to the current model.
 */
void  add_object_to_current_model(
    display_struct   *display,
    object_struct     *object )
{
    model_struct   *model;

    model = get_current_model( display );

    add_object_to_model( model, object );

    set_current_object( display, object );

    show_three_d_window( display, get_display_by_type( MARKER_WINDOW ) );

    graphics_models_have_changed( display );
}

/**
 * Returns a pointer to display_struct of the the 3D view window,
 * given any other display_struct.
 *
 * \param display A pointer to any window's display_struct.
 * \returns A pointer to the 3D view window's display_struct.
 */
display_struct  *get_three_d_window( display_struct  *display )
{
    return( get_display_by_type( THREE_D_WINDOW ) );
}

/**
 * Redraw the status line for the 3D view window.
 *
 * The status line displays information about the vertex nearest the
 * current mouse position.
 *
 * \param display The display_struct of the 3D view window.
 */
static void
update_status_line( display_struct *display )
{
    int              object_index;
    VIO_Point        intersection_point;
    object_struct    *object_ptr;
    polygons_struct  *polygons;
    int              poly_index;
    VIO_Point        poly_point;
    char             buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    VIO_BOOL         hide_display;

    model_struct *model_ptr = get_graphics_model( display, STATUS_MODEL );
    object_struct *text_object_ptr = model_ptr->objects[0];
    text_struct *text_ptr = get_text_ptr(text_object_ptr);

    if( get_mouse_scene_intersection( display, (Object_types) -1,
                                      &object_ptr, &object_index,
                                      &intersection_point )
        && object_ptr->object_type == POLYGONS)
    {
        hide_display = FALSE;
    }
    else if (get_cursor_scene_intersection( display, (Object_types) -1,
                                            &object_ptr, &object_index,
                                            &intersection_point)
             && object_ptr->object_type == POLYGONS)
    {
        hide_display = FALSE;
    }
    else
    {
        hide_display = TRUE;
    }

    if (hide_display)
    {
        if (get_object_visibility( text_object_ptr ))
        {
            set_object_visibility( text_object_ptr, FALSE );
            set_update_required( display, NORMAL_PLANES );
        }
    }
    else
    {
        int i;
        int n_pts_in_poly;
        VIO_Real min_d = 1e38;
        int min_i = -1;
        VIO_Point min_pt = {{0}};

        polygons = get_polygons_ptr(object_ptr);
        poly_index = object_index;
        poly_point = intersection_point;

        n_pts_in_poly = GET_OBJECT_SIZE(*polygons, poly_index);

        for (i = 0; i < n_pts_in_poly; i++)
        {
            int i_in = POINT_INDEX( polygons->end_indices, poly_index, i );
            int i_pt = polygons->indices[i_in];
            VIO_Real d = distance_between_points( &polygons->points[i_pt],
                                                  &poly_point);
            if (d < min_d)
            {
                min_d = d;
                min_i = i_pt;
                min_pt = polygons->points[i_pt];
            }
        }

        set_object_visibility( text_object_ptr, TRUE );

        if (object_ptr != display->three_d.mouse_obj ||
            min_i != display->three_d.mouse_point)
        {
            VIO_Real value;
            VIO_STR column;

            display->three_d.mouse_point = min_i;
            display->three_d.mouse_obj = object_ptr;
            sprintf(buffer, "O#%2d V#%6d P#%6d X %6.3f Y %6.3f Z %6.3f D ",
                    get_object_index(display, object_ptr),
                    min_i, poly_index,
                    Point_x(min_pt),
                    Point_y(min_pt),
                    Point_z(min_pt));

            if (get_vertex_value(display, object_ptr, min_i, &value, &column))
            {
              sprintf(&buffer[strlen(buffer) - 1], "%8.3g (%s)", value, column);
            }
            else
            {
                strcat(buffer, "--------");
            }

            if (strcmp(text_ptr->string, buffer) != 0)
            {
                replace_string(&text_ptr->string, create_string(buffer));

                set_update_required( display, NORMAL_PLANES );
            }
        }
    }
}

/**
 * Colour code each of the points associated with a polygon object, by
 * generating colours based on a set of vertex data.
 *
 * \param display The display_struct of the 3D view window.
 * \param vtxd_ptr The currently active vertex_data_struct
 * \param colour_flag A pointer to the polygon object's colour flag.
 * \param colours A pointer to the polygon object's colour array.
 */
static void
colour_code_vertices( display_struct     *display,
                      vertex_data_struct *vtxd_ptr,
                      Colour_flags       *colour_flag,
                      VIO_Colour         *colours[])
{
    int n_points = vtxd_ptr->dims[0];

    if( *colour_flag != PER_VERTEX_COLOURS )
    {
        if( n_points > 0 )
        {
            REALLOC( *colours, n_points );
        }
        else
        {
            FREE( *colours );
        }
        *colour_flag = PER_VERTEX_COLOURS;
    }

    if (vtxd_ptr->ndims == 1)
    {
        int i;
        for_less( i, 0, vtxd_ptr->dims[0] )
        {
            (*colours)[i] = get_colour_code( &vtxd_ptr->colour_coding,
                                             vtxd_ptr->data[i] );
        }
    }
    else if (vtxd_ptr->ndims == 2)
    {
        int i;
        for_less( i, 0, vtxd_ptr->dims[0] )
        {
            int j = ( i * vtxd_ptr->dims[1] ) + vtxd_ptr->column_index;
            (*colours)[i] = get_colour_code( &vtxd_ptr->colour_coding,
                                             vtxd_ptr->data[j] );
        }
    }
    else
    {
        print_error("Can't handle %d dimensions!", vtxd_ptr->ndims);
    }
}

/**
 * Associate vertex data with the currently selected graphics
 * object. The object must be a polygon and it must contain the same
 * number of vertices as the surface file has data elements.
 *
 * \param display The display_struct of the 3D view window.
 * \param object The object to associate with the vertex data.
 * \param vtxd_ptr The vertex data to associate with the object.
 */
void
attach_vertex_data(display_struct *display,
                    object_struct *object,
                    vertex_data_struct *vtxd_ptr)
{
    ADD_ELEMENT_TO_ARRAY(display->three_d.vertex_data_array,
                         display->three_d.vertex_data_count,
                         vtxd_ptr,
                         1);
    vtxd_ptr->owner = object;
    vtxd_ptr->column_index = 0;

    if (vtxd_ptr->colour_coding.user_defined_n_colour_points == 0)
    {
        initialize_colour_coding( &vtxd_ptr->colour_coding,
                                  Initial_vertex_coding_type,
                                  Initial_vertex_under_colour,
                                  Initial_vertex_over_colour,
                                  0.0, 1.0 );

        set_colour_coding_min_max( &vtxd_ptr->colour_coding,
                                   vtxd_ptr->min_v[0], vtxd_ptr->max_v[0] );
    }

    update_vertex_colour_coding( display, vtxd_ptr );
}

static char *
get_column_name( vertex_data_struct *vtxd_ptr, int index )
{
  static char _name[32];

  if (index >= vtxd_ptr->dims[1] || vtxd_ptr->column_names == NULL ||
      vtxd_ptr->column_names[index] == NULL)
  {
    snprintf( _name, sizeof(_name), "%d", index );
    return _name;
  }
  return vtxd_ptr->column_names[index];
}

/**
 * Select the next column of the vertex data array to display.
 *
 * \param display The display_struct of the 3D view window.
 * \param object The object whose vertex data we should display.
 */
void
advance_vertex_data(display_struct *display, object_struct *object)
{
    int index;
    vertex_data_struct *vtxd_ptr = find_vertex_data( display, object );

    if ( vtxd_ptr == NULL )
        return;

    index = vtxd_ptr->column_index + 1;
    if ( index >= vtxd_ptr->dims[1] )
        index = 0;
    vtxd_ptr->column_index = index;

    print( "Switched to column %d (%s), minimum %g, maximum %g\n",
           index,
           get_column_name( vtxd_ptr, index ),
           vtxd_ptr->min_v[index], vtxd_ptr->max_v[index] );

    set_colour_coding_min_max( &vtxd_ptr->colour_coding,
                               vtxd_ptr->min_v[index], vtxd_ptr->max_v[index] );

    update_vertex_colour_coding( display, vtxd_ptr );
    display->three_d.mouse_point = -1;
    update_status_line( display );
}

/**
 * Called to indicate that the currently selected object may have changed.
 *
 * This function will update the vertex colour bar to reflect the currently
 * selected object, or it will entirely hide the vertex colour bar if there
 * is no vertex data associated with the new object.
 *
 * \param display The display_struct of the 3D view window.
 * \param object The newly selected object.
 */
void
switch_vertex_data( display_struct *display, object_struct *object )
{
    vertex_data_struct *vtxd_ptr = find_vertex_data( display, object );
    if (vtxd_ptr == NULL)
    {
        set_vertex_colour_bar_visibility( display, FALSE );
    }
    else
    {
        update_vertex_colour_coding( display, vtxd_ptr );
    }
}


/**
 * Find the vertex data structure associated with an object, if any.
 *
 * \param display The display_struct for the 3D view window.
 * \param object The object for which we want the vertex data.
 * \returns A vertex data pointer, or NULL if no vertex data is associated
 * with the object.
 */
static vertex_data_struct *
find_vertex_data( display_struct *display, object_struct *object )
{
    int i;

    for (i = 0; i < display->three_d.vertex_data_count; i++)
        if ( display->three_d.vertex_data_array[i]->owner == object )
            return display->three_d.vertex_data_array[i];
    return NULL;
}

/**
 * Finds the surface value associated with the current vertex and
 * column in the object. Used to display the surface value in the
 * status line of the 3D view window.
 *
 * \param display The display_struct of the 3D view window.
 * \param object The object whose data we are looking for.
 * \param index The vertex number of the value we want.
 * \param value_ptr Will hold the current vertex value.
 * \param column_ptr Will hold the currently selected column.
 * \returns TRUE if a value is found.
 */
static VIO_BOOL
get_vertex_value(display_struct *display, object_struct *object,
                 int index, VIO_Real *value_ptr, VIO_STR *column_ptr)
{
    vertex_data_struct *vtxd_ptr = find_vertex_data( display, object );
    if (vtxd_ptr == NULL || index < 0 || index >= vtxd_ptr->dims[0])
      return FALSE;

    *column_ptr = get_column_name( vtxd_ptr, vtxd_ptr->column_index );
    *value_ptr = vtxd_ptr->data[index * vtxd_ptr->dims[1] +
                                vtxd_ptr->column_index];
    return TRUE;
}

/**
 * Change the upper or lower limit of the colour coding for the
 * vertex data based on the position of the mouse.
 *
 * \param display The display_struct of the 3D view window.
 * \param is_lo_limit TRUE if we should adjust the low limit, otherwise
 * we adjust the high limit.
 */
static void
adjust_limit(display_struct *display, VIO_BOOL is_lo_limit)
{
    int x, y;
    VIO_Real lo_range, hi_range;
    VIO_Real lo_limit, hi_limit;
    vertex_data_struct *vtxd_ptr;

    vtxd_ptr = find_vertex_data( display, display->three_d.mouse_obj );
    if (vtxd_ptr == NULL)
    {
        return;
    }

    G_get_mouse_position( display->window, &x, &y );

    get_colour_coding_min_max( &vtxd_ptr->colour_coding, &lo_limit, &hi_limit );

    lo_range = vtxd_ptr->min_v[vtxd_ptr->column_index];
    hi_range = vtxd_ptr->max_v[vtxd_ptr->column_index];

    if (is_lo_limit)
    {
        lo_limit = ((y - VTX_COLOURBAR_Y) / (double) VTX_COLOURBAR_HEIGHT) *
          (hi_range - lo_range) + lo_range;
    }
    else
    {
        hi_limit = ((y - VTX_COLOURBAR_Y) / (double) VTX_COLOURBAR_HEIGHT) *
          (hi_range - lo_range) + lo_range;
    }

    if (lo_limit < lo_range)
        lo_limit = lo_range;
    if (hi_limit > hi_range)
        hi_limit = hi_range;

    set_colour_coding_min_max( &vtxd_ptr->colour_coding, lo_limit, hi_limit );
    update_vertex_colour_coding( display, vtxd_ptr );
}

/**
 * Change the lower limit of the colour coding for the vertex data
 * based on the position of the mouse.
 *
 * \param display The display_struct of the 3D view window.
 * \param event_type The event code for this event.
 * \param key_pressed The keyboard code associated with this event, if any.
 *
 * \returns VIO_OK if the event processing should continue.
 */
static DEF_EVENT_FUNCTION(adjust_lo_limit)
{
    adjust_limit( display, TRUE );
    return VIO_OK;
}

/**
 * Change the lower limit of the colour coding for the vertex data
 * based on the position of the mouse.
 *
 * \param display The display_struct of the 3D view window.
 * \param event_type The event code for this event.
 * \param key_pressed The keyboard code associated with this event, if any.
 *
 * \returns VIO_OK if the event processing should continue.
 */
static DEF_EVENT_FUNCTION(adjust_hi_limit)
{
    adjust_limit( display, FALSE );
    return VIO_OK;
}


/**
 * Finish changing the lower limit of the colour coding for the vertex data.
 * Removes the action handlers installed on the mouse down event.
 *
 * \param display The display_struct of the 3D view window.
 * \param event_type The event code for this event.
 * \param key_pressed The keyboard code associated with this event, if any.
 *
 * \returns VIO_OK if the event processing should continue.
 */
static DEF_EVENT_FUNCTION(finish_lo_limit)
{
  remove_action_table_function( &display->action_table, NO_EVENT,
                                adjust_lo_limit );
  remove_action_table_function( &display->action_table, LEFT_MOUSE_UP_EVENT,
                                finish_lo_limit );
  return VIO_OK;
}

/**
 * Finish changing the upper limit of the colour coding for the vertex data.
 * Removes the action handlers installed on the mouse down event.
 *
 * \param display The display_struct of the 3D view window.
 * \param event_type The event code for this event.
 * \param key_pressed The keyboard code associated with this event, if any.
 *
 * \returns VIO_OK if the event processing should continue.
 */
static DEF_EVENT_FUNCTION(finish_hi_limit)
{
  remove_action_table_function( &display->action_table, NO_EVENT,
                                adjust_hi_limit );
  remove_action_table_function( &display->action_table, LEFT_MOUSE_UP_EVENT,
                                finish_hi_limit );
  return VIO_OK;
}


static DEF_EVENT_FUNCTION(adjust_both_limits)
{
    VIO_Real lo_limit, hi_limit;
    VIO_Real delta;
    vertex_data_struct *vtxd_ptr;

    vtxd_ptr = find_vertex_data( display, display->three_d.mouse_obj );
    if (vtxd_ptr == NULL)
    {
        return VIO_OK;
    }

    get_colour_coding_min_max( &vtxd_ptr->colour_coding, &lo_limit, &hi_limit );

    delta = hi_limit - lo_limit;

    adjust_limit( display, TRUE );

    get_colour_coding_min_max( &vtxd_ptr->colour_coding, &lo_limit, &hi_limit );
    set_colour_coding_min_max( &vtxd_ptr->colour_coding, lo_limit, lo_limit + delta );

    return VIO_OK;
}

static DEF_EVENT_FUNCTION(finish_both_limits)
{
  remove_action_table_function( &display->action_table, NO_EVENT,
                                adjust_both_limits );
  remove_action_table_function( &display->action_table, MIDDLE_MOUSE_UP_EVENT,
                                finish_both_limits );
  return VIO_OK;
}


/**
 * Update vertex colour coding and redisplay associated UI elements.
 *
 * Called whenever either the vertex data or colour coding is changed.
 *
 * \param display The display_struct of the 3D view window.
 * \param vtxd_ptr The vertex_data_struct that has been updated.
 */
static void
update_vertex_colour_coding( display_struct *display,
                             vertex_data_struct *vtxd_ptr)
{
    polygons_struct *polygons = get_polygons_ptr( vtxd_ptr->owner );
    colour_code_vertices( display, vtxd_ptr, &polygons->colour_flag,
                          &polygons->colours );
    update_vertex_colour_bar( display, vtxd_ptr );
    set_vertex_colour_bar_visibility( display, TRUE );
    set_update_required( display, NORMAL_PLANES );
}


/**
 * \brief Copy used-defined colour coding from one colour_coding_struct
 * to another.
 *
 * Since there is only one place in the user interface to load a custom
 * colour coding, it can be useful to copy the loaded custom values into
 * one of the vertex colour coding objects. Unfortunately there was no
 * way to do this. TODO: Move this code to bicpl?
 *
 * \param dst_ptr Pointer to destination colour_coding_struct.
 * \param src_ptr Pointer to source colour_coding_struct.
 * \returns TRUE if the source has valid user-defined colour coding
 * information and the operation was successful, or if the destination
 * already has what appears to be valid colour coding information.
 */
static VIO_BOOL
copy_user_defined_colour_coding(colour_coding_struct *dst_ptr,
                                const colour_coding_struct *src_ptr)
{
    if (src_ptr->user_defined_n_colour_points <= 0)
    {
        return (dst_ptr->user_defined_n_colour_points > 0);
    }

    if (dst_ptr->user_defined_n_colour_points > 0)
        FREE( dst_ptr->user_defined_colour_points );

    ALLOC( dst_ptr->user_defined_colour_points,
           src_ptr->user_defined_n_colour_points );

    dst_ptr->user_defined_n_colour_points =
      src_ptr->user_defined_n_colour_points;

    memcpy(dst_ptr->user_defined_colour_points,
           src_ptr->user_defined_colour_points,
           (src_ptr->user_defined_n_colour_points *
            sizeof (src_ptr->user_defined_colour_points[0])));

    return TRUE;
}

/**
 * \brief Copy the user-defined colour coding from the slice window.
 *
 * \param display A pointer to the display_struct of the 3D view window.
 * \param vtxd_ptr A pointer to the vertex_display_struct to receive the
 * colour coding information.
 * \returns TRUE if it is ok to adopt the user-defined colour coding.
 */
static VIO_BOOL
get_colour_coding_from_slice_view( display_struct *display,
                                   vertex_data_struct *vtxd_ptr)
{
    display_struct *slice_window;
    int            volume_index;

    if (!get_slice_window( display, &slice_window ))
        return FALSE;

    volume_index = get_current_volume_index( slice_window );
    if (volume_index < 0)
        return FALSE;

    return copy_user_defined_colour_coding( &vtxd_ptr->colour_coding,
                   &slice_window->slice.volumes[volume_index].colour_coding );
}

/**
 * Prompt the user to select the colour coding type for the vertex colour
 * coding.
 *
 * Each vertex data object is associated with a particular object. As a
 * result, there can be multiple objects with independent vertex data
 * and colour coding.
 *
 * \param display The display_struct of the 3D window
 * \param vtxd_ptr The selected vertex_data_struct
 */
static void
prompt_vertex_coding_type( display_struct *display,
                           vertex_data_struct *vtxd_ptr )
{
    Colour_coding_types cc_type;
    VIO_Status status;

    status = get_user_coding_type( "Select a new colour coding type",
                                   &cc_type );
    if (status == VIO_OK)
    {
        if (cc_type != USER_DEFINED_COLOUR_MAP ||
            get_colour_coding_from_slice_view( display, vtxd_ptr ))
        {
            set_colour_coding_type( &vtxd_ptr->colour_coding, cc_type );
            update_vertex_colour_coding( display, vtxd_ptr );
        }
    }
}

/**
 * Prompt the user to select the over or under colours for the vertex
 * colour coding.
 *
 * \param display The display_struct of the 3D window
 * \param vtxd_ptr The selected vertex_data_struct
 * \param is_under TRUE if should set the under colour, else set the
 * over colour.
 */
static void
prompt_vertex_coding_colours( display_struct *display,
                              vertex_data_struct *vtxd_ptr,
                              VIO_BOOL is_under)
{
    VIO_STR line;
    char text[VIO_EXTREMELY_LARGE_STRING_SIZE];
    VIO_Colour colour;

    snprintf(text, VIO_EXTREMELY_LARGE_STRING_SIZE,
             "Enter new %s colour name or 3 or 4 colour components: ",
             is_under ? "under" : "over");

    if( get_user_input( text, "s", &line ) == VIO_OK &&
        string_to_colour( line, &colour ) == VIO_OK )
    {
        if (is_under)
        {
            set_colour_coding_under_colour( &vtxd_ptr->colour_coding, colour );
        }
        else
        {
            set_colour_coding_over_colour( &vtxd_ptr->colour_coding, colour );
        }
        delete_string( line );
        update_vertex_colour_coding( display, vtxd_ptr );
    }
}

/**
 * Check whether a point in pixel coordinates lies within the colour bar.
 * \param x The column position of the pixel coordinate.
 * \param y The row position of the pixel coordinate.
 * \param cb_ptr A pointer to the colour_bar_struct.
 */
VIO_BOOL
point_is_inside_colour_bar(int x, int y, colour_bar_struct *cb_ptr)
{
    return (x >= cb_ptr->left_offset &&
            x <= cb_ptr->left_offset + cb_ptr->bar_width + cb_ptr->tick_width &&
            y >= cb_ptr->bottom_offset - VTX_TOL &&
            y <= cb_ptr->bottom_offset + VTX_COLOURBAR_HEIGHT + VTX_TOL);
}

/**
 * Handle middle mouse clicks in the 3D window.
 *
 * This function particularly implements middle mouse clicks in the
 * vertex colour bar. This implements the simultaneous movement of
 * the colour coding bounds.
 */
static DEF_EVENT_FUNCTION( handle_middle_down )
{
    int x, y;

    G_get_mouse_position( display->window, &x, &y );

    if (!point_is_inside_colour_bar(x, y, &display->three_d.colour_bar))
    {
        return VIO_OK;     /* Ignore clicks outside the colour bar. */
    }

    add_action_table_function( &display->action_table, NO_EVENT,
                               adjust_both_limits );
    add_action_table_function( &display->action_table, MIDDLE_MOUSE_UP_EVENT,
                               finish_both_limits );
    return VIO_ERROR;
}

/**
 * Handle left mouse clicks in the vertex colourbar.
 *
 * This function particularly implements left mouse clicks in the
 * vertex colour bar. This implements functions such as the selection of
 * the colour coding bounds, or the choice of over/under colour.
 *
 * \param display The display_struct of the 3D view window.
 * \param event_type The event code for this event.
 * \param key_pressed The keyboard code associated with this event, if any.
 *
 * \returns VIO_OK if this function did _not_ handle this event, or VIO_ERROR
 * if this function function "consumed" the event.
 */
static DEF_EVENT_FUNCTION( handle_left_down )
{
    int x, y;
    VIO_Real lo_limit, hi_limit;
    VIO_Real lo_range, hi_range;
    int lo_y, hi_y;
    vertex_data_struct *vtxd_ptr;

    G_get_mouse_position( display->window, &x, &y );

    if (!point_is_inside_colour_bar(x, y, &display->three_d.colour_bar))
    {
        return VIO_OK;     /* Ignore clicks outside the colour bar. */
    }

    vtxd_ptr = find_vertex_data( display, display->three_d.mouse_obj );
    if (vtxd_ptr == NULL)
    {
        return VIO_OK; /* Ignore clicks when no vertex data is present. */
    }

    get_colour_coding_min_max(&vtxd_ptr->colour_coding,
                              &lo_limit, &hi_limit);

    lo_range = vtxd_ptr->min_v[vtxd_ptr->column_index];

    hi_range = vtxd_ptr->max_v[vtxd_ptr->column_index];

    lo_y = VTX_COLOURBAR_Y + (VTX_COLOURBAR_HEIGHT *
                              (lo_limit - lo_range) / (hi_range - lo_range));
    hi_y = VTX_COLOURBAR_Y + (VTX_COLOURBAR_HEIGHT *
                              (hi_limit - lo_range) / (hi_range - lo_range));

    if (y >= hi_y - VTX_TOL && y <= hi_y + VTX_TOL)
    {
        add_action_table_function( &display->action_table, NO_EVENT,
                                  adjust_hi_limit );
        add_action_table_function( &display->action_table, LEFT_MOUSE_UP_EVENT,
                                  finish_hi_limit );
        return VIO_ERROR;
    }
    else if (y >= lo_y - VTX_TOL && y <= lo_y + VTX_TOL)
    {
        add_action_table_function( &display->action_table, NO_EVENT,
                                   adjust_lo_limit );
        add_action_table_function( &display->action_table, LEFT_MOUSE_UP_EVENT,
                                   finish_lo_limit );
        return VIO_ERROR;
    }
    else if (is_shift_key_pressed())
    {
        if (y <= lo_y)
            prompt_vertex_coding_colours( display, vtxd_ptr, TRUE );
        else if (y >= hi_y)
            prompt_vertex_coding_colours( display, vtxd_ptr, FALSE );
        else
            prompt_vertex_coding_type( display, vtxd_ptr );
        return VIO_ERROR;
    }
    return VIO_OK;
}

/**
 * Initialize colour bar objects for vertex display.
 *
 * \param display The display_struct of the 3D view window.
 * \param cb_ptr The colour bar structure.
 */
static void
initialize_vertex_colour_bar( display_struct *display,
                              colour_bar_struct *cb_ptr )
{
    object_struct     *object;
    pixels_struct     *pixels;
    lines_struct      *lines;
    model_struct      *model;
    model_info_struct *model_info;

    model = get_graphics_model( display, VTX_CODING_MODEL );

    model_info = get_model_info( model );

    model_info->view_type = PIXEL_VIEW;

    object = create_object( PIXELS );

    pixels = get_pixels_ptr( object );

    initialize_pixels( pixels, VTX_COLOURBAR_X, VTX_COLOURBAR_Y,
                       VTX_COLOURBAR_WIDTH, VTX_COLOURBAR_HEIGHT,
                       1.0, 1.0, RGB_PIXEL );

    add_object_to_model( model, object );

    object = create_object( LINES );

    lines = get_lines_ptr( object );
    initialize_lines( lines, WHITE );
    delete_lines( lines );
    lines->colour_flag = PER_ITEM_COLOURS;
    lines->line_thickness = 1.0;
    lines->n_points = 0;
    lines->n_items = 0;

    add_object_to_model( model, object );


    add_action_table_function( &display->action_table, LEFT_MOUSE_DOWN_EVENT,
                               handle_left_down );

    add_action_table_function( &display->action_table, MIDDLE_MOUSE_DOWN_EVENT,
                               handle_middle_down );

    set_vertex_colour_bar_visibility( display, FALSE );
}

/**
 * Enables or disables display of the vertex colour bar.
 * \param display The display_struct of the 3D view window.
 * \param visible TRUE if the colour bar should be visible.
 */
void
set_vertex_colour_bar_visibility(display_struct *display, VIO_BOOL visible)
{
  model_struct *model;
  int i;

  model = get_graphics_model( display, VTX_CODING_MODEL );
  for (i = 0; i < model->n_objects; i++)
  {
    set_object_visibility( model->objects[i], visible );
  }
}

/**
 * This function updates the vertex coding colour bar to reflect
 * the current colour coding range and state.
 *
 * \param display The display_struct of the 3D view window.
 * \param vtxd_ptr The selected vertex_data_struct
 */
static void
update_vertex_colour_bar( display_struct *display,
                          vertex_data_struct *vtxd_ptr )
{
  model_struct *model;
  pixels_struct *pixels;
  VIO_Real min_range, max_range;
  VIO_Real min_limit, max_limit;
  VIO_Real ratio;
  VIO_Real value;
  VIO_Colour colour;
  int x, y;

  min_range = vtxd_ptr->min_v[vtxd_ptr->column_index];
  max_range = vtxd_ptr->max_v[vtxd_ptr->column_index];

  get_colour_coding_min_max( &vtxd_ptr->colour_coding,
                             &min_limit, &max_limit );

  model = get_graphics_model( display, VTX_CODING_MODEL );

  pixels = get_pixels_ptr( model->objects[0] );

  for_less( y, 0, pixels->y_size )
  {
    ratio = (VIO_Real) y / (VIO_Real) (pixels->y_size - 1);

    value = VIO_INTERPOLATE( ratio, min_range, max_range );

    colour = get_colour_code( &vtxd_ptr->colour_coding, value );

    for_less( x, 0, pixels->x_size )
    {
      PIXEL_RGB_COLOUR( *pixels, x, y ) = colour;
    }
  }

  /* now rebuild the tick marks and numbers */
  rebuild_ticks_and_text( &display->three_d.colour_bar, model,
                          1,
                          2,
                          min_range, max_range,
                          min_limit, max_limit,
                          VTX_COLOURBAR_Y,
                          VTX_COLOURBAR_Y + VTX_COLOURBAR_HEIGHT,
                          FALSE);
}
