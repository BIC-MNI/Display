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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/markers/markers.c,v 1.9 2001-05-26 23:04:01 stever Exp $";
#endif

#include  <display.h>

public  BOOLEAN  update_current_marker(
    display_struct   *display,
    int              volume_index,
    Real             voxel[] )
{
    object_traverse_struct  object_traverse;
    BOOLEAN                 found;
    object_struct           *object, *closest_marker;
    Volume                  volume;
    Point                   voxel_pos;
    Real                    x_w, y_w, z_w;
    Real                    dist, closest_dist;

    initialize_object_traverse( &object_traverse, FALSE, 1,
                                &display->models[THREED_MODEL] );

    volume = get_nth_volume( display, volume_index );

    convert_voxel_to_world( volume, voxel, &x_w, &y_w, &z_w );
    fill_Point( voxel_pos, x_w, y_w, z_w );

    found = FALSE;
    closest_dist = 0.0;

    while( get_next_object_traverse( &object_traverse, &object ) )
    {
        if( object->object_type == MARKER &&
            points_within_distance( &voxel_pos,
                                    &get_marker_ptr(object)->position,
                                    Marker_pick_size ) )
        {
            dist = distance_between_points( &voxel_pos,
                                            &get_marker_ptr(object)->position );

            if( !found || dist < closest_dist )
            {
                found = TRUE;
                closest_dist = dist;
                closest_marker = object;
            }
        }
    }

    if( found && (!get_current_object(display,&object) ||
                  object != closest_marker) )
    {
        set_current_object( display, closest_marker );
    }

    return( found );
}
