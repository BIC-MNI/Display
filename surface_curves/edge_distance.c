#include  <def_objects.h>

typedef  struct
{
    int             changed;
    Real            distance;
    int             from_point;
} vertex_struct;

public  Boolean  distance_along_polygons( polygons, p1, poly1, p2, poly2,
                                          dist, lines )
    polygons_struct   *polygons;
    Point             *p1;
    int               poly1;
    Point             *p2;
    int               poly2;
    Real              *dist;
    lines_struct      *lines;
{
    Status          status;
    Boolean         found;
    int             last_vertex;
    Status          create_path();
    vertex_struct   *vertices;

    ALLOC1( status, vertices, polygons->n_points, vertex_struct );

    found = find_shortest_path( polygons, p1, poly1, p2, poly2, dist,
                                &last_vertex, vertices);

    if( found )
    {
        status = create_path( polygons, p1, p2, last_vertex,
                              vertices, lines );
    }

    if( status == OK )
        FREE1( status, vertices );

    return( found );
}

private  Boolean  find_shortest_path( polygons, p1, poly1, p2, poly2,
                                      path_dist, last_vertex, vertices )
    polygons_struct   *polygons;
    Point             *p1;
    int               poly1;
    Point             *p2;
    int               poly2;
    Real              *path_dist;
    int               *last_vertex;
    vertex_struct     vertices[];
{
    int                    i, size, e, point_index, poly, i1, i2;
    int                    point_index1, point_index2, delta;
    Real                   dist, distance1, dist_sum;
    Real                   distance_between_points();
    Boolean                found, changed, iteration;

    for_less( i, 0, polygons->n_points )
    {
        vertices[i].from_point = -2;
        vertices[i].distance = -1.0;
        vertices[i].changed = -1;
    }

    size = GET_OBJECT_SIZE( *polygons, poly1 );

    iteration = 0;

    for_less( e, 0, size )
    {
        point_index = polygons->indices[
                         POINT_INDEX( polygons->end_indices, poly1, e )];
        dist = distance_between_points( &polygons->points[point_index], p1 );

        vertices[point_index].from_point = -1;
        vertices[point_index].distance = dist;
        vertices[point_index].changed = iteration;
    }

    found = FALSE;
    changed = TRUE;

    while( changed )
    {
        ++iteration;

        (void) fprintf( stderr, "Iteration %d\n", iteration );

        changed = FALSE;

        for_less( poly, 0, polygons->n_items )
        {
            if( polygons->visibilities != (Smallest_int *) 0 &&
                !polygons->visibilities[poly] )
                continue;

            size = GET_OBJECT_SIZE( *polygons, poly );

            for_less( i1, 0, size )
            {
                point_index1 = polygons->indices[
                                POINT_INDEX(polygons->end_indices,poly,i1)];

                if( vertices[point_index1].changed >= iteration-1 )
                {
                    distance1 = vertices[point_index1].distance;

                    if( vertices[point_index1].from_point != -2 &&
                        (!found || distance1 < *path_dist) )
                    {
                        for( delta = -1;  delta <= 1;  delta += 2 )
                        {
                        i2 = (i1 + delta + size) % size;
                        point_index2 = polygons->indices[
                                POINT_INDEX(polygons->end_indices,poly,i2)];

                        if( vertices[point_index2].from_point == -2 ||
                            distance1 < vertices[point_index2].distance )
                        {
                            dist_sum = distance1 + distance_between_points(
                                       &polygons->points[point_index1],
                                       &polygons->points[point_index2] );

                            if( vertices[point_index2].from_point == -2 ||
                                dist_sum < vertices[point_index2].distance )
                            {
                                vertices[point_index2].changed = iteration;
                                vertices[point_index2].distance = dist_sum;
                                vertices[point_index2].from_point =
                                                       point_index1;
                                changed = TRUE;
                            }
                        }
                        }
                    }
                }
            }
        }

        size = GET_OBJECT_SIZE( *polygons, poly2 );

        for_less( e, 0, size )
        {
            point_index = polygons->indices[
                             POINT_INDEX( polygons->end_indices, poly2, e )];

            if( vertices[point_index].from_point >= -1 )
            {
                dist = distance_between_points( &polygons->points[point_index],
                                                p2 )
                       + vertices[point_index].distance;

                if( !found || dist < *path_dist )
                {
                    found = TRUE;
                    *path_dist = dist;
                    *last_vertex = point_index;
                }
            }
        }
    }

    return( found );
}

private  Status  create_path( polygons, p1, p2, last_vertex, vertices, lines )
    polygons_struct   *polygons;
    Point             *p1;
    Point             *p2;
    int               last_vertex;
    vertex_struct     vertices[];
    lines_struct      *lines;
{
    Status          status;
    Status          begin_adding_points_to_line();
    Status          add_point_to_line();

    status = begin_adding_points_to_line( lines );

    if( status == OK )
        status = add_point_to_line( lines, p2 );

    while( status == OK && last_vertex >= 0 )
    {
        status = add_point_to_line( lines, &polygons->points[last_vertex] );
        last_vertex = vertices[last_vertex].from_point;
    }

    if( status == OK )
        status = add_point_to_line( lines, p1 );

    return( status );
}
