
#include  <def_graphics.h>

public  Boolean  intersect_ray_with_objects( graphics, ray_origin,
                                             ray_direction,
                                             object,
                                             object_index,
                                             intersection_point )
    graphics_struct   *graphics;
    Point             *ray_origin;
    Vector            *ray_direction;
    object_struct     **object;
    int               *object_index;
    Point             *intersection_point;
{
    Status                   status;
    Boolean                  intersects;
    Boolean                  intersect_ray_polygons();
    Boolean                  intersect_ray_with_marker();
    Real                     dist;
    object_struct            *current_object;
    Status                   initialize_object_traverse();
    object_traverse_struct   object_traverse;

    intersects = FALSE;
    dist = 1.0e30;

    status = initialize_object_traverse( &object_traverse, N_MODELS,
                                         graphics->models );

    if( status == OK )
    {
        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( current_object->visibility )
            {
                if( current_object->object_type == POLYGONS )
                {
                    if( intersect_ray_polygons( ray_origin, ray_direction,
                                                current_object->ptr.polygons,
                                                object_index, &dist ) )
                    {
                        *object = current_object;
                        intersects = TRUE;
                    }
                }
                else if( current_object->object_type == MARKER )
                {
                    if( intersect_ray_with_marker( ray_origin, ray_direction,
                                                   current_object->ptr.marker,
                                                   &dist ) )
                    {
                        *object_index = -1;
                        *object = current_object;
                        intersects = TRUE;
                    }
                }
            }
        }

        if( intersects )
        {
            GET_POINT_ON_RAY( *intersection_point, *ray_origin, *ray_direction,
                              dist );
        }
    }

    return( intersects );
}

public  Boolean  intersect_ray_polygons( ray_origin, ray_direction,
                                         polygons, poly_index, dist )
    Point            *ray_origin;
    Vector           *ray_direction;
    polygons_struct  *polygons;
    int              *poly_index;
    Real             *dist;
{
    int       i;
    Boolean   intersects;
    Boolean   intersect_ray_polygon();
    Boolean   intersect_ray_with_bintree();

    if( polygons->bintree != (bintree_struct *) 0 )
    {
        intersects = intersect_ray_with_bintree( ray_origin, ray_direction,
                                                 polygons->bintree, polygons,
                                                 poly_index, dist );
    }
    else
    {
        intersects = FALSE;

        for_less( i, 0, polygons->n_items )
        {
            if( intersect_ray_polygon( ray_origin, ray_direction,
                                       dist, polygons, i ) )
            {
                *poly_index = i;
                intersects = TRUE;
            }
        }
    }

    return( intersects );
}

public  Boolean  intersect_ray_with_marker( ray_origin, ray_direction,
                                            marker, dist )
    Point            *ray_origin;
    Vector           *ray_direction;
    marker_struct    *marker;
    Real             *dist;
{
    int       c, enter, leave;
    Real      t_int[2], t_min, t_max, delta, box_low, box_high, origin;
    Boolean   intersects;

    t_min = 0.0;
    t_max = 1.0e30;

    intersects = TRUE;

    for_less( c, 0, N_DIMENSIONS )
    {
        box_low = Point_coord(marker->position,c) - marker->size / 2.0;
        box_high = Point_coord(marker->position,c) + marker->size / 2.0;

        origin = Point_coord(*ray_origin,c);

        delta = Point_coord( *ray_direction, c );
        if( delta == 0.0 )
        {
            if( origin < box_low || origin > box_high )
            {
                intersects = FALSE;
                break;
            }
        }
        else
        {
            if( delta < 0.0 )
            {
                enter = 1;
                leave = 0;
            }
            else
            {
                enter = 0;
                leave = 1;
            }

            t_int[0] = (box_low - origin) / delta;
            t_int[1] = (box_high - origin) / delta;

            if( t_int[enter] > t_min )
            {
                t_min = t_int[enter];
                if( t_min > t_max )
                {
                    intersects = FALSE;
                    break;
                }
            }

            if( t_int[leave] < t_max )
            {
                t_max = t_int[leave];
                if( t_min > t_max )
                {
                    intersects = FALSE;
                    break;
                }
            }
        }
    }

    if( intersects )
        *dist = t_min;

    return( intersects );
}
