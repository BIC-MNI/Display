/**
 * \file render_ops.c
 * \brief Menu commands to control 3D rendering parameters and views.
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

static  object_struct  *get_model_object(
    display_struct    *display )
{
    object_struct    *current_object;

    if( !get_current_object( display, &current_object ) ||
        current_object->object_type != MODEL )
    {
        current_object = get_current_model_object( display );
    }

    return( current_object );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_render_mode )
{
    object_struct            *model_object;
    VIO_BOOL                  shaded_mode;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    switch (get_model_info(get_model_ptr(model_object))->render.shaded_mode)
    {
    case OVERLAY:
      shaded_mode = WIREFRAME;
      break;
    case POINT:
      shaded_mode = FILLED;
      break;
    case WIREFRAME:
      shaded_mode = POINT;
      break;
    case FILLED:
    default:
      shaded_mode = OVERLAY;
      break;
    }

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            get_model_info(get_model_ptr(model_object))->render.shaded_mode =
                                                  shaded_mode;
        }
    }

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_render_mode )
{
    object_struct   *model_object;
    char            *mode_name;

    model_object = get_model_object( display );

    switch (get_model_info( get_model_ptr( model_object ))->render.shaded_mode)
    {
    case OVERLAY:
      mode_name = "Overlay";
      break;
    case POINT:
      mode_name = "Point";
      break;
    case WIREFRAME:
      mode_name = "Wireframe";
      break;
    case FILLED:
    default:
      mode_name = "Shaded";
      break;
    }
    set_menu_text_string( menu_window, menu_entry, mode_name );
    return( TRUE );
}


/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_shading )
{
    object_struct            *model_object;
    Shading_types            new_shading_type;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    if( get_model_info(get_model_ptr(model_object))->render.shading_type ==
        FLAT_SHADING )
    {
        new_shading_type = GOURAUD_SHADING;
    }
    else
    {
        new_shading_type = FLAT_SHADING;
    }

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            get_model_info(get_model_ptr(model_object))->render.shading_type =
                                              new_shading_type;
        }
    }

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_shading )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_boolean( menu_window, menu_entry,
     (VIO_BOOL) get_model_info(get_model_ptr(model_object))->render.shading_type,
     "Flat", "Gouraud" );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_lights )
{
    object_struct            *model_object;
    VIO_BOOL                  new_light_switch;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_light_switch = !get_model_info(get_model_ptr(model_object))->render.
                       master_light_switch;

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            get_model_info(get_model_ptr(model_object))->render.
                                      master_light_switch = new_light_switch;
        }
    }

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_lights )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_on_off( menu_window, menu_entry,
                          get_model_info(get_model_ptr(model_object))->render.
                          master_light_switch );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_two_sided )
{
    object_struct            *model_object;
    VIO_BOOL                  new_flag;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_flag = !get_model_info(get_model_ptr(model_object))->render.
               two_sided_surface_flag;

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            get_model_info(get_model_ptr(model_object))->render.
                                    two_sided_surface_flag = new_flag;
        }
    }

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_two_sided )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_on_off( menu_window, menu_entry,
                          get_model_info(get_model_ptr(model_object))->render.
                          two_sided_surface_flag );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_backfacing )
{
    object_struct            *model_object;
    VIO_BOOL                  new_flag;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_flag = !get_model_info(get_model_ptr(model_object))->render.
               backface_flag;

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            get_model_info(get_model_ptr(model_object))->render.
                                     backface_flag = new_flag;
        }
    }

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_backfacing )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_on_off( menu_window, menu_entry,
                          get_model_info(get_model_ptr(model_object))->render.
                          backface_flag );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_line_curve_flag )
{
    object_struct            *model_object;
    VIO_BOOL                  new_flag;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_flag = !get_model_info(get_model_ptr(model_object))->render.
               render_lines_as_curves;

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
        {
            get_model_info(get_model_ptr(object))->render.
                                  render_lines_as_curves = new_flag;
        }
    }

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_line_curve_flag )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_on_off( menu_window, menu_entry,
                          get_model_info(get_model_ptr(model_object))->render.
                          render_lines_as_curves );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_marker_label_flag )
{
    object_struct            *model_object;
    VIO_BOOL                  new_flag;
    object_struct            *object;
    object_traverse_struct   object_traverse;

    model_object = get_model_object( display );

    new_flag = !get_model_info(get_model_ptr(model_object))->render.
               show_marker_labels;

    initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MODEL )
            get_model_info(get_model_ptr(model_object))->render.
                                   show_marker_labels = new_flag;
    }

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_marker_label_flag )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_on_off( menu_window, menu_entry,
                          get_model_info(get_model_ptr(model_object))->render.
                          show_marker_labels );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_n_curve_segments )
{
    int                      n_segments;
    object_struct            *model_object;
    object_struct            *object;
    object_traverse_struct   object_traverse;
    char                     prompt[VIO_EXTREMELY_LARGE_STRING_SIZE];

    model_object = get_model_object( display );


    sprintf( prompt, "Current number of curve segments is %d.\n"
             "Enter number of curve segments: ",
            get_model_info(get_model_ptr(model_object))->
                                   render.n_curve_segments );

    if( get_user_input(prompt, "d", &n_segments ) == VIO_OK && n_segments > 0 )
    {
        initialize_object_traverse( &object_traverse, FALSE, 1, &model_object );

        while( get_next_object_traverse(&object_traverse,&object) )
        {
            if( object->object_type == MODEL )
            {
                get_model_info(get_model_ptr(model_object))->render.
                                       n_curve_segments = n_segments;
            }
        }

        set_update_required( display, NORMAL_PLANES );

        print( "New number of curve segments: %d\n", n_segments );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_n_curve_segments )
{
    object_struct   *model_object;

    model_object = get_model_object( display );

    set_menu_text_int( menu_window, menu_entry,
                       get_model_info(get_model_ptr(model_object))->render.
                       n_curve_segments );

    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_double_buffer_threed )
{
    VIO_BOOL   double_buffer;

    double_buffer = !G_get_double_buffer_state( display->window );

    G_set_double_buffer_state( display->window, double_buffer );

    set_update_required( display, NORMAL_PLANES );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_double_buffer_threed )
{
    set_menu_text_on_off( menu_window, menu_entry,
                          G_get_double_buffer_state(display->window) );

    return( G_can_switch_double_buffering() );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( toggle_double_buffer_slice )
{
    VIO_BOOL           double_buffer;
    display_struct    *slice_window;

    slice_window = get_display_by_type( SLICE_WINDOW );

    if( slice_window != (display_struct  *) 0 )
    {
        double_buffer = !G_get_double_buffer_state( slice_window->window );

        G_set_double_buffer_state( slice_window->window, double_buffer );

        set_slice_viewport_update( slice_window, FULL_WINDOW_MODEL );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(toggle_double_buffer_slice )
{
    display_struct  *slice_window;
    VIO_BOOL         state;

    state = get_slice_window( display, &slice_window );

    set_menu_text_on_off( menu_window, menu_entry, state &&
                          G_get_double_buffer_state( slice_window->window ) );

    return( state && G_can_switch_double_buffering() );
}


/**
 * \brief Command to change the background colour of the Display windows.
 */
DEF_MENU_FUNCTION( change_background_colour )
{
  VIO_STR    line;
  VIO_Colour col;

  if ( get_user_input( "Enter colour name or 3 or 4 colour components: ",
                           "s", &line) == VIO_OK &&
       string_to_colour( line, &col ) == VIO_OK )
  {
    display_struct **windows;
    int            n_windows = get_list_of_windows( &windows );
    int            i;

    for_less( i, 0, n_windows )
    {
      if ( windows[i] != NULL )
      {
        G_set_background_colour( windows[i]->window, col );
        if ( windows[i]->window_type == SLICE_WINDOW )
        {
          set_slice_viewport_update( windows[i], FULL_WINDOW_MODEL );
        }
        else
        {
          set_update_required( windows[i], NORMAL_PLANES );
        }
      }
    }
    delete_string( line );
  }
  return( VIO_OK );
}

DEF_MENU_UPDATE( change_background_colour )
{
    return( TRUE );
}
