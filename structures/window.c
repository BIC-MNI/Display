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

#include <display.h>

  void  transform_screen_to_pixels(
    window_struct  *window,
    VIO_Point          *screen,
    VIO_Point          *pixels )
{
    Point_x(*pixels) = (VIO_Point_coord_type) VIO_ROUND(
                  (VIO_Real) (window->x_size - 1) * (VIO_Real) Point_x(*screen) );
    Point_y(*pixels) = (VIO_Point_coord_type) VIO_ROUND(
                  (VIO_Real) (window->y_size - 1) * (VIO_Real) Point_y(*screen) );
    Point_z(*pixels) = 0.0f;
}
