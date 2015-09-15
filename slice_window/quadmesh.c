/**
 * \file quadmesh.c
 * \brief Creates a quadmesh that represents a single slice of the volume.
 *
 * Contains a single function that is essentially a wrapper for the bicpl
 * function create_slice_quadmesh().
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
 * Creates a quadmesh that represents a slice of the volume perpendicular
 * to the given axis at the given location.
 *
 * Simply creates the quadmesh object and passes it to the bicpl 
 * function create_slice_quadmesh().
 *
 * \param volume The volume used to generate the quadmesh.
 * \param axis_index The index of the perpendicular axis.
 * \param voxel_position The position along the perpendicular axis.
 * \returns The quadmesh object.
 */
object_struct *
create_3d_slice_quadmesh( VIO_Volume volume, int axis_index,
                          VIO_Real voxel_position )
{
    object_struct    *object;
    quadmesh_struct  *quadmesh;

    object = create_object( QUADMESH );
    quadmesh =  get_quadmesh_ptr( object );

    create_slice_quadmesh( volume, axis_index, voxel_position, -1, -1,
                           0.0, 0.0, 0.0, 0.0, quadmesh );

    return( object );
}
