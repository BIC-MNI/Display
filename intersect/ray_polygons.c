
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
    int            i;
    int            model_index;
    Boolean        intersects;
    Boolean        intersect_ray_polygons();
    object_struct  *object;
    Real           dist;

    intersects = FALSE;
    dist = 1.0e30;

    for_less( model_index, 0, N_MODELS )
    for_less( i, 0, graphics->models[model_index]->ptr.model->n_objects )
    {
        object = graphics->models[model_index]->ptr.model->object_list[i];

        BEGIN_TRAVERSE_OBJECT( status, object )

            if( OBJECT->object_type == POLYGONS )
            {
                if( intersect_ray_polygons( ray_origin, ray_direction,
                                            OBJECT->ptr.polygons,
                                            poly_index, &dist ) )
                {
                    *polygons = OBJECT->ptr.polygons;
                    intersects = TRUE;
                }
            }

        END_TRAVERSE_OBJECT
    }

    if( intersects )
    {
        GET_POINT_ON_RAY( *intersection_point, *ray_origin, *ray_direction,
                          dist );
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
