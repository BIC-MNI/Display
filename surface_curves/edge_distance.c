#include  <mni.h>
#include  <priority_queue.h>

typedef  struct
{
    Real            distance;
    int             from_point;
} vertex_struct;

private  BOOLEAN  find_shortest_path(
    polygons_struct   *polygons,
    Real              curvature_weight,
    Real              min_curvature,
    Real              max_curvature,
    Point             *p1,
    int               poly1,
    Point             *p2,
    int               poly2,
    Real              *path_dist,
    int               *last_vertex,
    vertex_struct     vertices[] );
private  void  create_path(
    polygons_struct   *polygons,
    Point             *p1,
    Point             *p2,
    BOOLEAN           first_flag,
    int               last_vertex,
    vertex_struct     vertices[],
    lines_struct      *lines );

public  BOOLEAN  distance_along_polygons(
    polygons_struct   *polygons,
    Real              curvature_weight,
    Real              min_curvature,
    Real              max_curvature,
    Point             *p1,
    int               poly1,
    Point             *p2,
    int               poly2,
    Real              *dist,
    lines_struct      *lines )
{
    BOOLEAN         found;
    int             last_vertex;
    vertex_struct   *vertices;

    ALLOC( vertices, polygons->n_points );

    check_polygons_neighbours_computed( polygons );

    found = find_shortest_path( polygons, curvature_weight,
                                min_curvature, max_curvature,
                                p1, poly1, p2, poly2, dist,
                                &last_vertex, vertices );

    if( found )
    {
        create_path( polygons, p1, p2, lines->n_points == 0,
                     last_vertex, vertices, lines );
    }

    FREE( vertices );

    return( found );
}

private  Real  weighted_distance(
    polygons_struct  *polygons,
    Real             curvature_weight,
    int              from_point_index,
    int              next_poly_index,
    int              next_vertex_index )
{
    Real   dist, angle, factor;
    int    size, edge, next_point_index, next_vertex, prev_vertex;

    next_point_index = polygons->indices[ POINT_INDEX( polygons->end_indices,
                                   next_poly_index, next_vertex_index )];
    dist = distance_between_points( &polygons->points[from_point_index],
                                    &polygons->points[next_point_index] );

    if( curvature_weight != 0.0 )
    {
        size = GET_OBJECT_SIZE( *polygons, next_poly_index );
        next_vertex = (next_vertex_index + 1) % size;
        prev_vertex = (next_vertex_index - 1 + size) % size;

        if( polygons->indices[ POINT_INDEX( polygons->end_indices,
                        next_poly_index, next_vertex )] == from_point_index )
        {
            edge = next_vertex_index;
        }
        else if( polygons->indices[ POINT_INDEX( polygons->end_indices,
                          next_poly_index, prev_vertex )] == from_point_index )
        {
            edge = prev_vertex;
        }
        else
            edge = -1;

        if( edge != -1 )
        {
            angle = get_polygon_edge_angle( polygons, next_poly_index, edge );

            factor = angle / PI;

            if( curvature_weight < 0.0 )
                dist = dist * (1.0 - curvature_weight * factor);
            else
                dist = dist * (1.0 + curvature_weight * (2.0 - factor));

            if( dist < 0.0 )
                HANDLE_INTERNAL_ERROR( "weighted_distance" );
        }
    }

    return( dist );
}

typedef  struct
{
    int   index_within_poly;
    int   poly_index;
} queue_struct;

enum  { NOT_DONE_YET, VALID_VERTEX, INVALID_VERTEX } Vertex_curvature_classes;

private   void   check_validity_tested(
    polygons_struct   *polygons,
    int               poly,
    int               vertex,
    int               point_index,
    Real              min_curvature,
    Real              max_curvature,
    Smallest_int      vertex_validity[] )
{
    Real    curvature, base_length;
    Point   centroid;
    Vector  normal;

    if( vertex_validity[point_index] == NOT_DONE_YET )
    {
        compute_polygon_point_centroid( polygons, poly, vertex,
                                 point_index, &centroid, &normal,
                                 &base_length, &curvature );

        if( min_curvature <= curvature && curvature <= max_curvature )
            vertex_validity[point_index] = VALID_VERTEX;
        else
            vertex_validity[point_index] = INVALID_VERTEX;
    }
}

private  BOOLEAN  find_shortest_path(
    polygons_struct   *polygons,
    Real              curvature_weight,
    Real              min_curvature,
    Real              max_curvature,
    Point             *p1,
    int               poly1,
    Point             *p2,
    int               poly2,
    Real              *path_dist,
    int               *last_vertex,
    vertex_struct     vertices[] )
{
    int                    i, p, size, point_index, next_point_index;
    Real                   dist;
    Smallest_int           *vertex_validity;
    BOOLEAN                found_path, closed;
    queue_struct           entry;
    int                    n_polys, *polys;
    PRIORITY_QUEUE_STRUCT( queue_struct )   queue;

    ALLOC( vertex_validity, polygons->n_points );

    if( min_curvature < max_curvature )
    {
        for_less( i, 0, polygons->n_points )
            vertex_validity[i] = NOT_DONE_YET;
    }
    else
        for_less( i, 0, polygons->n_points )
            vertex_validity[i] = VALID_VERTEX;

    ALLOC( polys, polygons->n_items );

    for_less( i, 0, polygons->n_points )
    {
        vertices[i].from_point = -2;
        vertices[i].distance = -1.0;
    }

    INITIALIZE_PRIORITY_QUEUE( queue );

    size = GET_OBJECT_SIZE( *polygons, poly2 );

    for_less( p, 0, size )
    {
        point_index = polygons->indices[
                     POINT_INDEX( polygons->end_indices, poly2, p )];

        check_validity_tested( polygons, poly2, p, point_index,
                               min_curvature, max_curvature, vertex_validity );

        if( vertex_validity[point_index] == VALID_VERTEX )
        {
            dist = distance_between_points( &polygons->points[point_index], p2);

            vertices[point_index].from_point = -1;
            vertices[point_index].distance = dist;
            entry.index_within_poly = p;
            entry.poly_index = poly2;
            INSERT_IN_PRIORITY_QUEUE( queue, entry, -dist );
        }
    }

    found_path = FALSE;

    while( !IS_PRIORITY_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_PRIORITY_QUEUE( queue, entry, dist );

        point_index = polygons->indices[
                     POINT_INDEX( polygons->end_indices, entry.poly_index,
                                  entry.index_within_poly )];

        if( found_path && vertices[point_index].distance > *path_dist )
            break;

        n_polys = get_polygons_around_vertex( polygons, entry.poly_index,
                                              entry.index_within_poly, polys,
                                              polygons->n_items, &closed );

        for_less( i, 0, n_polys )
        {
            size = GET_OBJECT_SIZE( *polygons, polys[i] );

            for_less( p, 0, size )
            {
                next_point_index = polygons->indices[
                             POINT_INDEX( polygons->end_indices, polys[i], p )];

                check_validity_tested( polygons, polys[i], p, next_point_index,
                                       min_curvature, max_curvature,
                                       vertex_validity );

                if( vertex_validity[next_point_index] != VALID_VERTEX )
                    continue;

                if( vertices[next_point_index].from_point == -2 ||
                    vertices[next_point_index].distance >
                    vertices[point_index].distance )
                {
                    dist = vertices[point_index].distance +
                       weighted_distance( polygons, curvature_weight,
                                          point_index,
                                          polys[i], p );

                    if( vertices[next_point_index].from_point == -2 ||
                        dist < vertices[next_point_index].distance )
                    {
                        vertices[next_point_index].from_point = point_index;
                        vertices[next_point_index].distance = dist;
                        entry.index_within_poly = p;
                        entry.poly_index = polys[i];
                        INSERT_IN_PRIORITY_QUEUE( queue, entry, -dist );
                    }
                }

                if( polys[i] == poly1 )
                {
                    dist = vertices[next_point_index].distance +
                           distance_between_points(
                              &polygons->points[next_point_index], p1 );

                    if( !found_path || dist < *path_dist )
                    {
                        found_path = TRUE;
                        *path_dist = dist;
                        *last_vertex = next_point_index;
                    }
                }
            }
        }
    }

    DELETE_PRIORITY_QUEUE( queue );

    FREE( polys );

    if( vertex_validity != NULL )
        FREE( vertex_validity );

    return( found_path );
}

private  void  create_path(
    polygons_struct   *polygons,
    Point             *p1,
    Point             *p2,
    BOOLEAN           first_flag,
    int               last_vertex,
    vertex_struct     vertices[],
    lines_struct      *lines )
{
    Point    prev;

    if( first_flag )
    {
        start_new_line( lines );

        add_point_to_line( lines, p1 );

        prev = *p1;
    }
    else
    {
        prev = lines->points[lines->indices[NUMBER_INDICES(*lines)-1]];
    }

    while( last_vertex >= 0 )
    {
        if( !EQUAL_POINTS( prev, polygons->points[last_vertex] ) )
        {
            add_point_to_line( lines, &polygons->points[last_vertex] );
            prev = polygons->points[last_vertex];
        }
        last_vertex = vertices[last_vertex].from_point;
    }

    if( !EQUAL_POINTS( prev, *p2 ) )
        add_point_to_line( lines, p2 );
}

public  void  find_polygon_vertex_nearest_point(
    polygons_struct  *polygons,
    int              poly,
    Point            *point,
    Point            *closest_vertex )
{
    int    size, p;
    Real   dist, closest_dist;
    Point  vertex;

    size = GET_OBJECT_SIZE( *polygons, poly );
    closest_dist = 0.0;  /* for lint */

    for_less( p, 0, size )
    {
        vertex = polygons->points[
                  polygons->indices[POINT_INDEX(polygons->end_indices,poly,p)]];
        dist = distance_between_points( point, &vertex );

        if( p == 0 || dist < closest_dist )
        {
            *closest_vertex = vertex;
            closest_dist = dist;
        }
    }
}
