
#include  <display.h>

public  BOOLEAN  intersect_ray_with_objects_hierarchy(
    display_struct    *display,
    Point             *ray_origin,
    Vector            *ray_direction,
    object_struct     **object,
    int               *object_index,
    Point             *intersection_point )
{
    BOOLEAN                  intersects;
    Real                     dist;
    object_struct            *current_object;
    object_traverse_struct   object_traverse;

    intersects = FALSE;
    dist = 1.0e30;

    initialize_object_traverse( &object_traverse, N_MODELS, display->models );

    while( get_next_object_traverse(&object_traverse,&current_object) )
    {
        if( current_object->visibility )
        {
            if( intersect_ray_with_object( ray_origin, ray_direction,
                                           current_object, object_index, &dist,
                                           (Real **) NULL ) > 0 )
            {
                *object = current_object;
                intersects = TRUE;
            }
        }
    }

    if( intersects )
    {
        GET_POINT_ON_RAY( *intersection_point, *ray_origin, *ray_direction,
                          dist );
    }

    return( intersects );
}
