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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/events/mouse.c,v 1.18 1996-04-19 13:25:05 david Exp $";
#endif


#include  <display.h>

public  BOOLEAN  mouse_moved(
    display_struct   *display,
    Real             *new_x,
    Real             *new_y,
    Real             *old_x,
    Real             *old_y )
{
    BOOLEAN   moved;

    moved = G_get_mouse_position_0_to_1( display->window, new_x, new_y );

    *old_x = (Real) Point_x( display->prev_mouse_position );
    *old_y = (Real) Point_y( display->prev_mouse_position );

    if( moved )
    {
        moved = *new_x != *old_x || *new_y != *old_y;

        record_mouse_position( display );
    }

    return( moved );
}

public  BOOLEAN  pixel_mouse_moved(
    display_struct   *display,
    int              *new_x,
    int              *new_y,
    int              *old_x,
    int              *old_y )
{
    BOOLEAN   moved;

    moved = G_get_mouse_position( display->window, new_x, new_y );

    *old_x = ROUND( Point_x( display->prev_mouse_position ) );
    *old_y = ROUND( Point_y( display->prev_mouse_position ) );

    if( moved )
    {
        moved = *new_x != *old_x || *new_y != *old_y;

        record_mouse_pixel_position( display );
    }

    return( moved );
}

public  void  record_mouse_position(
    display_struct   *display )
{
    Real   x, y;

    (void) G_get_mouse_position_0_to_1( display->window, &x, &y );

    fill_Point( display->prev_mouse_position, x, y, 0.0 );
}

public  void  record_mouse_pixel_position(
    display_struct   *display )
{
    int   x, y;

    (void) G_get_mouse_position( display->window, &x, &y );

    fill_Point( display->prev_mouse_position, (Real) x, (Real) y, 0.0 );
}
