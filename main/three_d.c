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

static  void  initialize_three_d_events(
    display_struct  *display );

static void define_lights( display_struct *display );

static void initialize_status( display_struct *display );

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

    G_set_drawing_interrupt_state( display->window,
                                   Maximum_display_time > 0.0 );
    G_set_drawing_interrupt_check_n_objects( display->window,
                                             Interval_of_check );
    G_set_drawing_interrupt_time( display->window, Maximum_display_time );

    three_d->lights[0].colour = Directional_light_colour;
    three_d->lights[0].direction = Light_direction;

    define_lights( display );

    fill_Point( three_d->min_limit, 0.0, 0.0, 0.0 );
    fill_Point( three_d->max_limit, 1.0, 1.0, 1.0 );

    ADD_POINTS( three_d->centre_of_objects, three_d->min_limit,
                three_d->max_limit );
    SCALE_POINT( three_d->centre_of_objects, three_d->centre_of_objects, 0.5 );

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

    initialize_status(display);

    display->three_d.vertex_data_array = NULL;
    display->three_d.vertex_data_count = 0;
    display->three_d.mouse_obj = NULL;
    display->three_d.mouse_point = 0;
}

/**
 * Unhide the 3D and "marker" window if there are objects loaded. This
 * overrides any global settings.
 *
 * \param graphics A pointer to the display_struct of the 3D View window.
 * \param markers A pointer to the display_struct of the object list window.
 */
void
show_three_d_window(display_struct *graphics, 
                    display_struct *markers)
{
    model_struct *model = get_current_model( graphics );
    if (model->n_objects > 1)
    {
        G_set_visibility(graphics->window, TRUE);
        G_set_visibility(markers->window, TRUE);
    }
}

static void
initialize_status(display_struct *display)
{
    model_struct *model_ptr = get_graphics_model( display, STATUS_MODEL );
    object_struct *object_ptr = create_object(TEXT);
    text_struct *text_ptr = get_text_ptr(object_ptr);
    VIO_Point origin;

    fill_Point(origin, 10.0, 10.0, 0.0);
    initialize_text(text_ptr, &origin, Readout_text_rgb_colour,
                    Slice_readout_text_font, Slice_readout_text_font_size);
    text_ptr->string = create_string("");
    set_object_visibility(object_ptr, FALSE);
    add_object_to_model(model_ptr, object_ptr);
}

/**
 * Colour code each of the points associated with an object, by copying
 * the encoded colours from an associated volume.
 */
static void
colour_code_vertices( vertex_data_struct *vtxd_ptr,
                      Colour_flags       *colour_flag,
                      VIO_Colour         *colours[])
{
    int i;
    colour_coding_struct ccs;
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

    initialize_colour_coding(&ccs, GRAY_SCALE, BLACK, WHITE,
                             vtxd_ptr->min_v,
                             vtxd_ptr->max_v);
    set_colour_coding_type(&ccs, SPECTRAL);

    for_less( i, 0, vtxd_ptr->dims[0] )
    {
        (*colours)[i] = get_colour_code(&ccs, vtxd_ptr->data[i]);
    }

    delete_colour_coding(&ccs);
}

/**
 * Associate vertex data with the currently selected graphics
 * object. The object must be a polygon and it must contain same
 * number of vertices as the surface file has data elements.
 * \param display The display_struct of the 3D view window.
 * \param object The object to associate with the vertex data.
 * \param vtx_data_ptr The vertex data to associate with the object.
 */
void
attach_vertex_data(display_struct *display, 
                    object_struct *object,
                    vertex_data_struct *vtxd_ptr)
{
    polygons_struct *polygons = get_polygons_ptr(object);

    ADD_ELEMENT_TO_ARRAY(display->three_d.vertex_data_array,
                         display->three_d.vertex_data_count,
                         vtxd_ptr,
                         1);
    vtxd_ptr->owner = object;
    colour_code_vertices(vtxd_ptr,
                         &polygons->colour_flag, &polygons->colours);
    set_update_required( display, NORMAL_PLANES );
}

/**
 * Finds the surface value associated with the current index in the
 * object. Used to display the surface value in the status line of the
 * 3D view window.
 * \param display The display_struct of the 3D view window.
 * \param object The object whose data we are looking for.
 * \param index The vertex number of the value we want.
 * \param value_ptr The value to return.
 * \returns TRUE if a value is found.
 */
static VIO_BOOL
find_vertex_data(display_struct *display, object_struct *object,
                 int index, VIO_Real *value_ptr)
{
    int i;
    for (i = 0; i < display->three_d.vertex_data_count; i++)
    {
        if (display->three_d.vertex_data_array[i]->owner == object)
        {
            break;
        }
    }
    if (i < display->three_d.vertex_data_count)
    {
        vertex_data_struct *vtxd_ptr = display->three_d.vertex_data_array[i];
        if (index >= 0 && index < vtxd_ptr->dims[0])
        {
            *value_ptr = vtxd_ptr->data[index];
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * Define light sources for the 3D view window.
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

static void
update_status(display_struct *display)
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

    if (!hide_display)
    {
      VIO_Point pts[32];
      int i;
      int n;
      VIO_Real min_d = 1e38;
      int min_i = -1;
      VIO_Point min_pt;

      polygons = get_polygons_ptr(object_ptr);
      poly_index = object_index;
      poly_point = intersection_point;

      n = get_polygon_points(polygons, poly_index, pts);

      for (i = 0; i < n; i++) {
        int x = POINT_INDEX(polygons->end_indices, poly_index, i);
        VIO_Real d = distance_between_points(&pts[i], &poly_point);
        if (d < min_d)
        {
          min_d = d;
          min_i = polygons->indices[x];
          min_pt = pts[i];
        }
      }

      set_object_visibility( text_object_ptr, TRUE );

      if (object_ptr != display->three_d.mouse_obj ||
          min_i != display->three_d.mouse_point)
      {
        VIO_Real value;

        display->three_d.mouse_point = min_i;
        display->three_d.mouse_obj = object_ptr;
        sprintf(buffer, "O#%2d V#%6d P#%6d X %6.3f Y %6.3f Z %6.3f ",
                get_object_index(display, object_ptr),
                min_i, poly_index,
                Point_x(min_pt),
                Point_y(min_pt),
                Point_z(min_pt));

        if (find_vertex_data(display, object_ptr, min_i, &value))
        {
          sprintf(&buffer[strlen(buffer) - 1], "%8.3f", value);
        }
        else
        {
          strcat(buffer, "--------");
        }

        replace_string(&text_ptr->string, create_string(buffer));

        set_update_required( display, NORMAL_PLANES );
      }
    }
    else
    {
        set_object_visibility( text_object_ptr, FALSE );
        set_update_required( display, NORMAL_PLANES );
    }
}

static DEF_EVENT_FUNCTION(handle_mouse_movement)
{
    update_status(display);
    return VIO_OK;
}

static  void  initialize_three_d_events(
    display_struct  *display )
{
    initialize_virtual_spaceball( display );

    initialize_picking_object( display );

    add_action_table_function( &display->action_table, WINDOW_RESIZE_EVENT,
                               handle_resize_three_d );

    add_action_table_function( &display->action_table, NO_EVENT,
                               handle_mouse_movement );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_resize_three_d )
{
    adjust_view_for_aspect( &display->three_d.view, display->window );

    update_view( display );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

  void  delete_three_d(
    display_struct  *display )
{
    delete_string( display->three_d.default_marker_label );
    terminate_current_object( &display->three_d.current_object );
    delete_surface_edit( &display->three_d.surface_edit );
    delete_surface_extraction( display );
}

  void  add_object_to_current_model(
    display_struct   *display,
    object_struct     *object )
{
    model_struct   *model;

    model = get_current_model( display );

    add_object_to_model( model, object );

    set_current_object_index( display, model->n_objects-1 );

    graphics_models_have_changed( display );
}

  display_struct  *get_three_d_window(
    display_struct  *display )
{
    return( display->associated[THREE_D_WINDOW] );
}
