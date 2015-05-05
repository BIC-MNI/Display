/**
 * \file virt_sb.c
 * \brief Functions to implement rotation of the 3D object view 
 * ("virtual spaceball").
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

static    DEF_EVENT_FUNCTION( start_virtual_spaceball );
static    DEF_EVENT_FUNCTION( turn_off_virtual_spaceball );
static    DEF_EVENT_FUNCTION( handle_update_rotation );
static    DEF_EVENT_FUNCTION( terminate_rotation );
static    DEF_EVENT_FUNCTION( handle_update_translation );
static    DEF_EVENT_FUNCTION( terminate_translation );
static  VIO_BOOL  perform_rotation(
    display_struct   *display );
static  VIO_BOOL  perform_cursor_translation(
    display_struct   *display );
static  VIO_BOOL  mouse_close_to_cursor(
    display_struct    *display );

/**
 * Install the event hooks necessary to tie object rotation to the middle
 * mouse button.
 *
 * \param display The display_struct of the three-D object window.
 */
void  initialize_virtual_spaceball(
    display_struct   *display )
{
    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_virtual_spaceball );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               turn_off_virtual_spaceball );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( turn_off_virtual_spaceball )
{
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_virtual_spaceball );

    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  turn_off_virtual_spaceball );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( start_virtual_spaceball )
{
    if( mouse_close_to_cursor( display ) )
    {
        add_action_table_function( &display->action_table,
                                   NO_EVENT, handle_update_translation );

        add_action_table_function( &display->action_table,
                                   MIDDLE_MOUSE_UP_EVENT,
                                   terminate_translation );

        add_action_table_function( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   terminate_translation );
    }
    else
    {
        add_action_table_function( &display->action_table,
                                   NO_EVENT, handle_update_rotation );

        add_action_table_function( &display->action_table,
                                   MIDDLE_MOUSE_UP_EVENT, terminate_rotation );

        add_action_table_function( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   terminate_rotation );
    }

    record_mouse_position( display );

    return( VIO_OK );
}

static  void  update_rotation(
    display_struct   *display )
{
    if( perform_rotation( display ) )
    {
        update_view( display );

        set_update_required( display, NORMAL_PLANES );
    }
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_update_rotation )
{
    update_rotation( display );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( terminate_rotation )
{
    update_rotation( display );
    
    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update_rotation );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT, terminate_rotation );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_rotation );

    return( VIO_OK );
}

static  VIO_BOOL  perform_rotation(
    display_struct   *display )
{
    VIO_Real           x, y;
    VIO_Transform      transform;
    VIO_BOOL        moved;

    moved = FALSE;

    if( G_get_mouse_position_0_to_1( display->window, &x, &y ) &&
        get_spaceball_transform( display,
                                 (VIO_Real) Point_x(display->prev_mouse_position),
                                 (VIO_Real) Point_y(display->prev_mouse_position),
                                 x, y, &transform ) )
    {
        transform_model( display, &transform );
        record_mouse_position( display );
        moved = TRUE;
    }

    return( moved );
}

static  void  update_translation(
    display_struct   *display )
{
    if( perform_cursor_translation( display ) )
    {
        set_update_required( display, get_cursor_bitplanes() );

        if( update_voxel_from_cursor( display->associated[SLICE_WINDOW] ) )
        {
            set_update_required( display->associated[SLICE_WINDOW],
                                 NORMAL_PLANES );
        }
    }
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( handle_update_translation )
{
    update_translation( display );

    return( VIO_OK );
}

/* ARGSUSED */

static  DEF_EVENT_FUNCTION( terminate_translation )
{
    update_translation( display );
    
    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update_translation );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_translation );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_translation );

    return( VIO_OK );
}

static  VIO_BOOL  perform_cursor_translation(
    display_struct   *display )
{
    VIO_Vector       mouse_dir, offset, axis_direction;
    VIO_Vector       ray_direction, transformed_direction;
    VIO_Point        ray_origin, transformed_origin;
    int          axis_index, best_axis, second_best_axis, a1, a2;
    VIO_Point        pt, pt_screen, cursor_screen, new_screen_origin, new_cursor;
    VIO_BOOL      moved;
    VIO_Real         mag_mouse, mag_axis[VIO_N_DIMENSIONS], dot_prod[VIO_N_DIMENSIONS];
    VIO_Real         angle[VIO_N_DIMENSIONS], mouse_dist;
    VIO_Real         x, y, x_prev, y_prev;
    VIO_Vector       axis_screen[VIO_N_DIMENSIONS];

    moved = FALSE;

    if( mouse_moved( display, &x, &y, &x_prev, &y_prev ) )
    {
        fill_Vector( mouse_dir, x - x_prev, y - y_prev, 0.0 );

        mag_mouse = MAGNITUDE( mouse_dir );

        if( mag_mouse > Cursor_mouse_threshold )
            moved = TRUE;
    }

    if( moved )
    {
        pt = display->three_d.cursor.origin;
        transform_point_to_screen( &display->three_d.view, &pt,
                                   &cursor_screen );

        for_less( axis_index, 0, VIO_N_DIMENSIONS )
        {
            pt = display->three_d.cursor.origin;
            Point_coord(pt,axis_index) += 1.0f;
            transform_point_to_screen( &display->three_d.view, &pt,
                                       &pt_screen );
            SUB_POINTS( axis_screen[axis_index], pt_screen, cursor_screen );
            mag_axis[axis_index] = MAGNITUDE( axis_screen[axis_index] );

            if( mag_axis[axis_index] == 0.0 )
            {
                dot_prod[axis_index] = 0.0;
                angle[axis_index] = 90.0;
            }
            else
            {
                dot_prod[axis_index] =
                         DOT_VECTORS( mouse_dir,axis_screen[axis_index]) /
                         mag_mouse / mag_axis[axis_index];
                angle[axis_index] = acos( (double) VIO_FABS(dot_prod[axis_index]) )
                                    * VIO_RAD_TO_DEG;
            }
        }

        best_axis = VIO_X;
        for_inclusive( axis_index, VIO_Y, VIO_Z )
        {
            if( VIO_FABS(dot_prod[axis_index]) > VIO_FABS(dot_prod[best_axis]) )
            {
                best_axis = axis_index;
            }
        }

        a1 = (best_axis + 1) % VIO_N_DIMENSIONS;
        a2 = (best_axis + 2) % VIO_N_DIMENSIONS;

        if( VIO_FABS(dot_prod[a1]) > VIO_FABS(dot_prod[a2]) )
            second_best_axis = a1;
        else
            second_best_axis = a2;

        if( angle[best_axis] > Max_cursor_angle ||
            angle[second_best_axis] - angle[best_axis] < Min_cursor_angle_diff )
        {
            moved = FALSE;
        }
    }

    if( moved )
    {
        mouse_dist = dot_prod[best_axis] * mag_mouse;

        SCALE_VECTOR( offset, axis_screen[best_axis],
                      mouse_dist / mag_axis[best_axis] );

        ADD_POINT_VECTOR( new_screen_origin, cursor_screen, offset );

        fill_Vector( axis_direction, 0.0, 0.0, 0.0 );
        Vector_coord( axis_direction, best_axis ) = 1.0f;

        convert_screen_to_ray( &display->three_d.view,
                               (VIO_Real) Point_x(new_screen_origin),
                               (VIO_Real) Point_y(new_screen_origin),
                               &ray_origin, &ray_direction );
        transform_world_to_model( &display->three_d.view, &ray_origin,
                                  &transformed_origin );
        transform_world_to_model_vector( &display->three_d.view,
                                         &ray_direction,
                                         &transformed_direction );
        moved = get_nearest_point_on_lines(
                     &display->three_d.cursor.origin,
                     &axis_direction,
                     &transformed_origin, &transformed_direction, &new_cursor );
    }

    if( moved )
    {
        display->three_d.cursor.origin = new_cursor;

        update_cursor( display );

        record_mouse_position( display );
    }

    return( moved );
}

static  VIO_BOOL  mouse_close_to_cursor(
    display_struct    *display )
{
    VIO_BOOL  close;
    VIO_Point    cursor_screen, cursor_pixels, mouse_pixels, mouse;
    VIO_Vector   diff_vector;
    VIO_Real     x, y, diff;

    close = FALSE;

    if( G_get_mouse_position_0_to_1( display->window, &x, &y ) )
    {
        transform_point_to_screen( &display->three_d.view,
                                   &display->three_d.cursor.origin,
                                   &cursor_screen );

        transform_screen_to_pixels( display->window,
                                    &cursor_screen, &cursor_pixels );

        fill_Point( mouse, x, y, 0.0 );
        transform_screen_to_pixels( display->window, &mouse, &mouse_pixels );

        SUB_POINTS( diff_vector, cursor_pixels, mouse_pixels );

        diff = MAGNITUDE( diff_vector );

        close = (diff < Cursor_pick_distance);
    }

    return( close );
}
