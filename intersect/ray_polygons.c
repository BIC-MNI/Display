
#include  <def_graphics.h>

public  Boolean  intersect_ray_with_polygons( graphics, ray_origin,
                                              ray_direction,
                                              polygons,
                                              poly_index,
                                              intersection_point )
    graphics_struct   *graphics;
    Point             *ray_origin;
    Vector            *ray_direction;
    polygons_struct   **polygons;
    int               *poly_index;
    Point             *intersection_point;
{
    Status         status;
    Boolean        intersects;
    Boolean        intersect_ray_polygons();
    object_struct  *object;
    Real           dist;
    Status         initialize_object_traverse();
    object_traverse_struct   object_traverse;

    intersects = FALSE;
    dist = 1.0e30;

    status = initialize_object_traverse( &object_traverse, N_MODELS,
                                         graphics->models );

    if( status == OK )
    {
        while( get_next_object_traverse(&object_traverse,&object) )
        {
            if( object->object_type == POLYGONS && object->visibility )
            {
                if( intersect_ray_polygons( ray_origin, ray_direction,
                                            object->ptr.polygons,
                                            poly_index, &dist ) )
                {
                    *polygons = object->ptr.polygons;
                    intersects = TRUE;
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
