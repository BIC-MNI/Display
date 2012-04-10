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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/slice_window/quadmesh.c,v 1.9 2001/05/27 00:19:54 stever Exp $";
#endif

#include  <display.h>

public  object_struct   *create_3d_slice_quadmesh(
    Volume         volume,
    int            axis_index,
    Real           voxel_position )
{
    object_struct    *object;
    quadmesh_struct  *quadmesh;

    object = create_object( QUADMESH );
    quadmesh =  get_quadmesh_ptr( object );

    create_slice_quadmesh( volume, axis_index, voxel_position, -1, -1,
                           0.0, 0.0, 0.0, 0.0, quadmesh );

    return( object );
}
