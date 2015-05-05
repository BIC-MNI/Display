/**
 * \file mouse_trans.c
 * \brief Functions to implement translation of the 3D object view.
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

static    DEF_EVENT_FUNCTION( start_translation );
static    DEF_EVENT_FUNCTION( turn_off_translation );
static    DEF_EVENT_FUNCTION( handle_update );
static    DEF_EVENT_FUNCTION( terminate_translation );
static  VIO_BOOL  perform_translation( display_struct *display );

void  initialize_translation( display_struct *display )
{
    terminate_any_interactions( display );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               turn_off_translation );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_DOWN_EVENT,
                               start_translation );
}

/* ARGSUSED */

static DEF_EVENT_FUNCTION( turn_off_translation )
{
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  turn_off_translation );

    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_DOWN_EVENT,
                                  start_translation );

    return( VIO_OK );
}

/* ARGSUSED */

static DEF_EVENT_FUNCTION( start_translation )
{
    add_action_table_function( &display->action_table,
                               NO_EVENT,
                               handle_update );

    add_action_table_function( &display->action_table,
                               MIDDLE_MOUSE_UP_EVENT,
                               terminate_translation );

    add_action_table_function( &display->action_table,
                               TERMINATE_INTERACTION_EVENT,
                               terminate_translation );

    record_mouse_position( display );

    return( VIO_OK );
}

void mouse_translation_update( display_struct  *display )
{
    if( perform_translation( display ) )
    {
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }
}

/* ARGSUSED */

static DEF_EVENT_FUNCTION( terminate_translation )
{
    mouse_translation_update( display );
    
    remove_action_table_function( &display->action_table,
                                  NO_EVENT, handle_update );
    remove_action_table_function( &display->action_table,
                                  MIDDLE_MOUSE_UP_EVENT,
                                  terminate_translation );
    remove_action_table_function( &display->action_table,
                                  TERMINATE_INTERACTION_EVENT,
                                  terminate_translation );

    return( VIO_OK );
}

/* ARGSUSED */

static DEF_EVENT_FUNCTION( handle_update )
{
    mouse_translation_update( display );

    return( VIO_OK );
}

static VIO_BOOL perform_translation( display_struct   *display )
{
    VIO_BOOL       moved;
    VIO_Real       x, y, x_prev, y_prev;
    VIO_Vector     delta, hor, vert;
    VIO_Transform  transform;

    moved = FALSE;

    if( mouse_moved( display, &x, &y, &x_prev, &y_prev ) )
    {
        fill_Vector( delta, x - x_prev, y - y_prev, 0.0 );

        get_screen_axes( &display->three_d.view, &hor, &vert );

        SCALE_VECTOR( hor, hor, Point_x(delta) );
        SCALE_VECTOR( vert, vert, Point_y(delta) );

        ADD_VECTORS( delta, hor, vert );

        make_translation_transform( (VIO_Real) Vector_x(delta),
                                    (VIO_Real) Vector_y(delta),
                                    (VIO_Real) Vector_z(delta), &transform );

        transform_model( display, &transform );

        moved = TRUE;
    }

    return( moved );
}
