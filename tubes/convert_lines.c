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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/tubes/convert_lines.c,v 1.10 2001-05-26 23:04:10 stever Exp $";
#endif


#include  <display.h>

public  void  convert_lines_to_tubes_objects(
    display_struct    *display,
    lines_struct      *lines,
    int               n_around,
    Real              radius )
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

        add_object_to_model( get_current_model(display), object );
    }

    if( n_quadmeshes > 0 )
    {
        FREE( quadmeshes );
    }
}
