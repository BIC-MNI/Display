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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/main/transforms.c,v 1.10 2001/05/27 00:19:48 stever Exp $";
#endif


#include  <display.h>

public  void  convert_transform_to_view_space(
    display_struct    *display,
    Transform         *transform,
    Transform         *view_space_transform )
{
    Vector     z_axis;
    Point      centre;

    get_view_z_axis( &display->three_d.view, &z_axis );

    if( Use_cursor_origin )
    {
        centre = display->three_d.cursor.origin;
        transform_point_to_world( &display->three_d.view, &centre, &centre );
    }
    else
    {
        get_view_centre( &display->three_d.view, &centre );
    }

    make_transform_in_coordinate_system( &centre,
                                         &display->three_d.view.x_axis,
                                         &display->three_d.view.y_axis,
                                         &z_axis,
                                         transform, view_space_transform );
}

public  void  apply_transform_in_view_space(
    display_struct    *display,
    Transform         *transform )
{
    Transform  transform_in_cs;

    convert_transform_to_view_space( display, transform, &transform_in_cs );

    transform_model( display, &transform_in_cs );
}

public  void  transform_model(
    display_struct    *display,
    Transform         *transform )
{
    concat_transforms( &display->three_d.view.modeling_transform,
                       &display->three_d.view.modeling_transform,
                       transform );
}

public  void  get_inverse_model_transform(
    display_struct    *display,
    Transform         *inverse_transform )
{
    (void) compute_transform_inverse( &display->three_d.view.modeling_transform,
                                      inverse_transform );
}
