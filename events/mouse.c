/**
 * \file mouse.c
 * \brief Functions to track current and previous mouse position.
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

/**
 * Returns true if the mouse has moved, and returns the current and
 * previous mouse positions in fractional units (between zero and one).
 * \param display The display_struct for the relevant window.
 * \param new_x The updated X position (between 0 and 1).
 * \param new_y The updated Y position (between 0 and 1).
 * \param old_x The previous X position (between 0 and 1).
 * \param old_y The previous Y position (between 0 and 1).
 */
VIO_BOOL  mouse_moved(
    display_struct   *display,
    VIO_Real             *new_x,
    VIO_Real             *new_y,
    VIO_Real             *old_x,
    VIO_Real             *old_y )
{
    VIO_BOOL   moved;

    moved = G_get_mouse_position_0_to_1( display->window, new_x, new_y );

    *old_x = (VIO_Real) Point_x( display->prev_mouse_position );
    *old_y = (VIO_Real) Point_y( display->prev_mouse_position );

    if( moved )
    {
        moved = *new_x != *old_x || *new_y != *old_y;

        record_mouse_position( display );
    }

    return( moved );
}

/**
 * Returns true if the mouse has moved, and returns the current and
 * previous mouse positions in pixel units (from zero to the window
 * dimensions in pixels).
 * \param display The display_struct for the relevant window.
 * \param new_x The updated X position (between 0 and window_x).
 * \param new_y The updated Y position (between 0 and window_y).
 * \param old_x The previous X position (between 0 and window_x).
 * \param old_y The previous Y position (between 0 and window_y).
 */
VIO_BOOL  pixel_mouse_moved(
    display_struct   *display,
    int              *new_x,
    int              *new_y,
    int              *old_x,
    int              *old_y )
{
    VIO_BOOL   moved;

    moved = G_get_mouse_position( display->window, new_x, new_y );

    *old_x = VIO_ROUND( Point_x( display->prev_mouse_pixel_position ) );
    *old_y = VIO_ROUND( Point_y( display->prev_mouse_pixel_position ) );

    if( moved )
    {
        moved = *new_x != *old_x || *new_y != *old_y;

        record_mouse_pixel_position( display );
    }

    return( moved );
}

/**
 * Save the current mouse position in the field
 * display_struct::prev_mouse_position. The saved value will be in
 * fractional units in the range 0...1.
 *
 * \param display The display_struct for the relevant window.
 */
void  record_mouse_position(
    display_struct   *display )
{
    VIO_Real   x, y;

    (void) G_get_mouse_position_0_to_1( display->window, &x, &y );

    fill_Point( display->prev_mouse_position, x, y, 0.0 );
}

/**
 * Save the current mouse position in the field
 * display_struct::prev_mouse_pixel_position. The saved value will be in
 * pixel units.
 *
 * \param display The display_struct for the relevant window.
 */
void  record_mouse_pixel_position(
    display_struct   *display )
{
    int   x, y;

    (void) G_get_mouse_position( display->window, &x, &y );

    fill_Point( display->prev_mouse_pixel_position, (VIO_Real) x, (VIO_Real) y, 0.0 );
}
