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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/events/spaceball.c,v 1.4 2001-05-26 23:03:57 stever Exp $";
#endif


#include  <display.h>

private  BOOLEAN  make_spaceball_transform(
    Real       x1,
    Real       y1,
    Real       x2,
    Real       y2,
    Point      *centre,
    Real       x_radius,
    Real       y_radius,
    Transform  *transform );

public  BOOLEAN  get_spaceball_transform(
    display_struct   *display,
    Real             x1,
    Real             y1,
    Real             x2,
    Real             y2,
    Transform        *transform )
{
    static  Point  centre = { 0.5f, 0.5f, 0.0f };
    Transform      spaceball_transform;
    Real           x_radius, y_radius;
    int            x_size, y_size;
    Real           aspect;
    BOOLEAN        exists;

    G_get_window_size( display->window, &x_size, &y_size );
    aspect = (Real) y_size / (Real) x_size;

    if( aspect < 1.0 )
    {
        x_radius = 0.5 * aspect;
        y_radius = 0.5;
    }
    else
    {
        x_radius = 0.5;
        y_radius = 0.5 / aspect;
    }

    exists = FALSE;

    if( make_spaceball_transform( x1, y1, x2, y2, &centre, x_radius, y_radius,
                                  &spaceball_transform ) )
    {
        convert_transform_to_view_space( display, &spaceball_transform,
                                         transform );
        exists = TRUE;
    }

    return( exists );
}

private  BOOLEAN  make_spaceball_transform(
    Real       x1,
    Real       y1,
    Real       x2,
    Real       y2,
    Point      *centre,
    Real       x_radius,
    Real       y_radius,
    Transform  *transform )
{
    BOOLEAN  transform_created;
    Real     x_old, y_old, z_old, x_new, y_new, z_new;
    Real     dist_old, dist_new;
    Real     angle, sin_angle;
    Vector   v0, v1;
    Vector   axis_of_rotation;

    x_old = (x1 - (Real) Point_x(*centre)) / x_radius;
    y_old = (y1 - (Real) Point_y(*centre)) / y_radius;

    x_new = (x2 - (Real) Point_x(*centre)) / x_radius;
    y_new = (y2 - (Real) Point_y(*centre)) / y_radius;

    dist_old = x_old * x_old + y_old * y_old;
    dist_new = x_new * x_new + y_new * y_new;

    transform_created = FALSE;

    if( (x_old != x_new || y_old != y_new) &&
        dist_old <= 1.0 && dist_new <= 1.0 )
    {
        z_old = 1.0 - sqrt( dist_old );
        z_new = 1.0 - sqrt( dist_new );

        fill_Vector( v0, x_old, y_old, z_old );
        fill_Vector( v1, x_new, y_new, z_new );

        CROSS_VECTORS( axis_of_rotation, v0, v1 );

        sin_angle = MAGNITUDE( axis_of_rotation );

        if( sin_angle > 0.0 )
        {
            SCALE_VECTOR( axis_of_rotation, axis_of_rotation, 1.0 / sin_angle );

            angle = asin( (double) sin_angle );
            if( DOT_VECTORS( v0, v1 ) < 0.0 )
            {
                angle += PI / 2.0;
            }

            make_rotation_about_axis( &axis_of_rotation, angle, transform );

            transform_created = TRUE;
        }
    }

    return( transform_created );
}
