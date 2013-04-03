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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif


#include  <display.h>

static  DEF_EVENT_FUNCTION( pick_first_corner_point );
static  DEF_EVENT_FUNCTION( show_rectangle_at_mouse );
static  DEF_EVENT_FUNCTION( terminate_picking_markers );
static  DEF_EVENT_FUNCTION( done_picking_markers );
static  DEF_EVENT_FUNCTION( show_picked_square );

  void  start_picking_markers(
    display_struct   *display )
{
    push_action_table( &display->action_table, LEFT_MOUSE_DOWN_EVENT );
    push_action_table( &display->action_table, LEFT_MOUSE_UP_EVENT );
    push_action_table( &display->action_table, TERMINATE_INTERACTION_EVENT );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_picking_markers );

    add_action_table_function( &display->action_table,
                               LEFT_MOUSE_DOWN_EVENT,
                               pick_first_corner_point );

    add_action_table_function( &display->action_table, NO_EVENT,
                               show_rectangle_at_mouse );

    record_mouse_position( display );
    set_update_required( display, NORMAL_PLANES );
}

static  void  remove_events(
    action_table_struct  *action_table )
{
    pop_action_table( action_table, LEFT_MOUSE_DOWN_EVENT );
    pop_action_table( action_table, LEFT_MOUSE_UP_EVENT );
    pop_action_table( action_table, TERMINATE_INTERACTION_EVENT );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( terminate_picking_markers )
{
    remove_action_table_function( &display->action_table, NO_EVENT,
                                  show_rectangle_at_mouse );

    remove_events( &display->action_table );

    return( VIO_OK );
}

static  void  get_coordinates(
    VIO_Real    x1,
    VIO_Real    y1,
    VIO_Real    x2,
    VIO_Real    y2,
    VIO_Real    *x_min,
    VIO_Real    *y_min,
    VIO_Real    *x_max,
    VIO_Real    *y_max )
{
    *x_min = MIN( x1, x2 );
    *x_max = MAX( x1, x2 );
    *y_min = MIN( y1, y2 );
    *y_max = MAX( y1, y2 );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( show_rectangle_at_mouse )
{
    VIO_Real     x, y, x_prev, y_prev, x1, y1, x2, y2;

    if( (graphics_update_required( display ) ||
         mouse_moved(display,&x,&y,&x_prev,&y_prev)) &&
        G_get_mouse_position_0_to_1( display->window, &x, &y ) )
    {
        get_coordinates( x, y, x, y,
                         &x1, &y1, &x2, &y2 );

        draw_2d_rectangle( display, SCREEN_VIEW, Viewport_feedback_colour,
                           x1, y1, x2, y2 );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( pick_first_corner_point )
{
    VIO_Real  x, y;

    if( G_get_mouse_position_0_to_1( display->window, &x, &y ) )
    {
        fill_Point( display->viewport_picking.first_corner, x, y, 0.0 );

        record_mouse_position( display );

        add_action_table_function( &display->action_table,
                                   LEFT_MOUSE_UP_EVENT,
                                   done_picking_markers );

        remove_action_table_function( &display->action_table, NO_EVENT,
                                      show_rectangle_at_mouse );

        add_action_table_function( &display->action_table,
                                   NO_EVENT,
                                   show_picked_square );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( show_picked_square )
{
    VIO_Real   x, y, x_prev, y_prev, x1, y1, x2, y2;

    if( (graphics_update_required( display ) ||
         mouse_moved(display,&x,&y,&x_prev,&y_prev)) &&
        G_get_mouse_position_0_to_1( display->window, &x, &y ) )
    {
        get_coordinates( (VIO_Real) Point_x(display->viewport_picking.first_corner),
                         (VIO_Real) Point_y(display->viewport_picking.first_corner),
                         x, y, &x1, &y1, &x2, &y2 );

        draw_2d_rectangle( display, SCREEN_VIEW,
                           Viewport_feedback_colour, x1, y1, x2, y2 );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( done_picking_markers )
{
    VIO_Real                    x, y, x_min, y_min, x_max, y_max;
    VIO_Point                   screen_pos;
    marker_struct           *marker;
    object_struct           *object, *current_object;
    object_traverse_struct  object_traverse;

    remove_action_table_function( &display->action_table, NO_EVENT,
                                  show_picked_square );

    remove_events( &display->action_table );

    if( G_get_mouse_position_0_to_1( display->window, &x, &y ) )
    {
        get_coordinates( (VIO_Real) Point_x(display->viewport_picking.first_corner),
                         (VIO_Real) Point_y(display->viewport_picking.first_corner),
                         x, y, &x_min, &y_min, &x_max, &y_max );


        object = display->models[THREED_MODEL];
        initialize_object_traverse( &object_traverse, TRUE, 1, &object );

        while( get_next_object_traverse(&object_traverse, &current_object) )
        {
            if( current_object->object_type == MARKER )
            {
                marker = get_marker_ptr( current_object );
                transform_point_to_screen( &display->three_d.view,
                                           &marker->position, &screen_pos );

                if( (VIO_Real) Point_x(screen_pos) >= x_min &&
                    (VIO_Real) Point_x(screen_pos) <= x_max &&
                    (VIO_Real) Point_y(screen_pos) >= y_min &&
                    (VIO_Real) Point_y(screen_pos) <= y_max )
                {
                    marker->type = display->three_d.default_marker_type;
                    marker->colour = display->three_d.default_marker_colour;
                    marker->size = display->three_d.default_marker_size;
                    replace_string( &marker->label,
                        create_string(display->three_d.default_marker_label) );
                    marker->structure_id =
                                   display->three_d.default_marker_structure_id;
                    marker->patient_id =
                                   display->three_d.default_marker_patient_id;
                }
            }
        }

        graphics_models_have_changed( display );
    }
    
    return( VIO_OK );
}
