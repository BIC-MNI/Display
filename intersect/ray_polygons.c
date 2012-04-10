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


#include  <display.h>


/* FIXME: The handling of desired_object_type seems fragile.  Can you reliably
 * cast an integer to an enum and back ?
 *
 * In pick_point_under_mouse(), one wants to specify "intersect with any object 
 * type".  An out-of-band value of -1 is used, coerced into a variable
 * of type Object_types.  Smart compilers know that the enum type "Object_types"
 * are all non-negative, so the code "if (desired_object_type < 0)" was
 * changed to cast it into an int.
 */

private  void  recursive_intersect_ray_with_objects_hierarchy(
    object_struct     *current_object,
    Point             *ray_origin,
    Vector            *ray_direction,
    Object_types      desired_object_type,
    object_struct     **object,
    int               *closest_object_index,
    Real              *closest_dist,
    BOOLEAN           *found )
{
    int                      i, object_index;
    Real                     dist;
    model_struct             *model;

    if( !current_object->visibility )
        return;

    if( get_object_type(current_object) == MODEL )
    {
        model = get_model_ptr( current_object );

        for_less( i, 0, model->n_objects )
        {
            recursive_intersect_ray_with_objects_hierarchy(
                      model->objects[i], ray_origin, ray_direction,
                      desired_object_type, object,
                      closest_object_index, closest_dist, found );
        }
    }
    else if( ((int) desired_object_type) < 0 ||
             desired_object_type == get_object_type(current_object) )
    {
        if( intersect_ray_with_object( ray_origin, ray_direction,
                                       current_object, &object_index, &dist,
                                       (Real **) NULL ) > 0 )
        {
            if( !*found || dist < *closest_dist )
            {
                *closest_dist = dist;
                *closest_object_index = object_index;
                *object = current_object;
                *found = TRUE;
            }
        }
    }
}

public  BOOLEAN  intersect_ray_with_objects_hierarchy(
    display_struct    *display,
    Point             *ray_origin,
    Vector            *ray_direction,
    Object_types      desired_object_type,
    object_struct     **object,
    int               *closest_object_index,
    Point             *intersection_point )
{
    BOOLEAN                  intersects;
    Real                     closest_dist;

    intersects = FALSE;
    closest_dist = 1.0e30;

    recursive_intersect_ray_with_objects_hierarchy(
                   display->models[THREED_MODEL],
                   ray_origin, ray_direction, desired_object_type,
                   object, closest_object_index, &closest_dist,
                   &intersects );

    if( intersects )
    {
        GET_POINT_ON_RAY( *intersection_point, *ray_origin, *ray_direction,
                          closest_dist );
    }

    return( intersects );
}
