/** 
 * \file tubes/convert_lines.c
 * \brief Convert lines to quadmesh "tubes".
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

  void  convert_lines_to_tubes_objects(
    display_struct    *display,
    lines_struct      *lines,
    int               n_around,
    VIO_Real              radius )
{
    int              i, n_quadmeshes;
    object_struct    *object;
    quadmesh_struct  *quadmeshes, *quadmesh;

    n_quadmeshes = convert_lines_to_tubes( lines, n_around, radius,
                                           &quadmeshes );

    for_less( i, 0, n_quadmeshes )
    {
        object = create_object( QUADMESH );
        quadmesh = get_quadmesh_ptr( object );
        *quadmesh = quadmeshes[i];

        add_object_to_current_model( display, object, FALSE );
    }

    if( n_quadmeshes > 0 )
    {
        FREE( quadmeshes );
    }
}
