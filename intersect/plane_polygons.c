
#include  <def_graphics.h>

static    Status   intersect_plane_polygons();

public  Status  intersect_plane_with_polygons( graphics, plane_normal,
                                               plane_constant, lines,
                                               n_points_alloced,
                                               n_indices_alloced,
                                               n_end_indices_alloced )
    graphics_struct   *graphics;
    Vector            *plane_normal;
    Real              plane_constant;
    lines_struct      *lines;
    int               *n_points_alloced;
    int               *n_indices_alloced;
    int               *n_end_indices_alloced;
{
    Status                   status;
    object_struct            *object;
    Status                   initialize_object_traverse();
    object_traverse_struct   object_traverse;

    status = OK;

    lines->n_items = 0;
    lines->n_points = 0;

    status = initialize_object_traverse( &object_traverse, N_MODELS,
                                         graphics->models );

    while( status == OK && get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == POLYGONS && object->visibility )
        {
            status = intersect_plane_polygons( plane_normal,
                                               plane_constant,
                                               object->ptr.polygons,
                                               lines,
                                               n_points_alloced,
                                               n_indices_alloced,
                                               n_end_indices_alloced );
        }
    }

    return( status );
}

private  Status  intersect_plane_polygons( plane_normal, plane_constant,
                                           polygons, lines,
                                           n_points_alloced,
                                           n_indices_alloced,
                                           n_end_indices_alloced )
    Vector            *plane_normal;
    Real              plane_constant;
    polygons_struct   *polygons;
    lines_struct      *lines;
    int               *n_points_alloced;
    int               *n_indices_alloced;
    int               *n_end_indices_alloced;
{
    Status    status;
    int       i;
    Boolean   intersect_plane_one_polygon();

    status = OK;

    if( FALSE && polygons->bintree != (bintree_struct *) 0 )
    {
/*
        intersect_ray_with_bintree( ray_origin, ray_direction,
                                                 polygons->bintree, polygons,
                                                 poly_index, dist );
*/
    }
    else
    {
        for_less( i, 0, polygons->n_items )
        {
            (void) intersect_plane_one_polygon( plane_normal, plane_constant,
                                                polygons, i, lines, 
                                                n_points_alloced,
                                                n_indices_alloced,
                                                n_end_indices_alloced );
        }
    }

    return( status );
}

public  Boolean  intersect_plane_one_polygon( plane_normal, plane_constant,
                                              polygons, poly, lines,
                                              n_points_alloced,
                                              n_indices_alloced,
                                              n_end_indices_alloced )
    Vector            *plane_normal;
    Real              plane_constant;
    polygons_struct   *polygons;
    int               poly;
    lines_struct      *lines;
    int               *n_points_alloced;
    int               *n_indices_alloced;
    int               *n_end_indices_alloced;
{
    Status    status;
    int       point_index, n_indices;
    Point     points[2];
    Boolean   intersects;
    Boolean   get_plane_polygon_intersection();

    intersects = get_plane_polygon_intersection( plane_normal, plane_constant,
                                                 polygons, poly, points );

    if( intersects )
    {
        point_index = lines->n_points;

        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( status,
                 *n_points_alloced, lines->n_points, lines->points,
                 points[0], Point, DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( status,
                 *n_points_alloced, lines->n_points, lines->points,
                 points[1], Point, DEFAULT_CHUNK_SIZE );

        n_indices = NUMBER_INDICES( *lines );

        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( status,
                 *n_indices_alloced, n_indices, lines->indices,
                 point_index, int, DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( status,
                 *n_indices_alloced, n_indices, lines->indices,
                 point_index+1, int, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( status,
                 *n_end_indices_alloced, lines->n_items,
                 lines->end_indices,
                 n_indices, int, DEFAULT_CHUNK_SIZE );
    }

    return( intersects );
}
