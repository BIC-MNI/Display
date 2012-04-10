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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/structures/window.c,v 1.8 2001/05/27 00:19:56 stever Exp $";
#endif

#include <display.h>

public  void  transform_screen_to_pixels(
    window_struct  *window,
    Point          *screen,
    Point          *pixels )
{
    Point_x(*pixels) = (Point_coord_type) ROUND(
                  (Real) (window->x_size - 1) * (Real) Point_x(*screen) );
    Point_y(*pixels) = (Point_coord_type) ROUND(
                  (Real) (window->y_size - 1) * (Real) Point_y(*screen) );
    Point_z(*pixels) = 0.0f;
}
