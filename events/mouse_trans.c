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

static  VIO_BOOL  perform_translation( display_struct *display );

void mouse_translation_update( display_struct  *display )
{
    if( perform_translation( display ) )
    {
        update_view( display );
        set_update_required( display, NORMAL_PLANES );
    }
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
