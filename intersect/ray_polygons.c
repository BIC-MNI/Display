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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/intersect/ray_polygons.c,v 1.13 1995-07-31 19:54:09 david Exp $";
#endif


#include  <display.h>

public  BOOLEAN  intersect_ray_with_objects_hierarchy(
    display_struct    *display,
    Point             *ray_origin,
    Vector            *ray_direction,
    Object_types      desired_object_type,
    object_struct     **object,
    int               *closest_object_index,
    Point             *intersection_point )
{
    int                      object_index;
    BOOLEAN                  intersects;
    Real                     dist, closest_dist;
    object_struct            *current_object;
    object_traverse_struct   object_traverse;

    intersects = FALSE;
    closest_dist = 1.0e30;

    initialize_object_traverse( &object_traverse, N_MODELS, display->models );

    while( get_next_object_traverse(&object_traverse,&current_object) )
    {
        if( current_object->visibility &&
            (desired_object_type == -1 ||
             desired_object_type == get_object_type(current_object)) )
        {
            if( intersect_ray_with_object( ray_origin, ray_direction,
                                           current_object, &object_index, &dist,
                                           (Real **) NULL ) > 0 )
            {
                if( !intersects || dist < closest_dist )
                {
                    closest_dist = dist;
                    *closest_object_index = object_index;
                    *object = current_object;
                    intersects = TRUE;
                }
            }
        }
    }

    if( intersects )
    {
        GET_POINT_ON_RAY( *intersection_point, *ray_origin, *ray_direction,
                          closest_dist );
    }

    return( intersects );
}
