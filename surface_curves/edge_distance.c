#include  <def_objects.h>
#include  <def_priority_queue.h>

typedef  struct
{
    Real            distance;
    int             from_point;
} vertex_struct;

static  Status   create_path();
static  Boolean  find_shortest_path();

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
    Status          check_polygons_neighbours_computed();
    Boolean         found;
    int             last_vertex;
    vertex_struct   *vertices;

    ALLOC1( status, vertices, polygons->n_points, vertex_struct );

    if( status == OK )
        status = check_polygons_neighbours_computed( polygons );

    found = find_shortest_path( polygons, p1, poly1, p2, poly2, dist,
                                &last_vertex, vertices);

    if( found )
    {
        status = create_path( polygons, p1, p2, lines->n_points == 0,
                              last_vertex, vertices, lines );
    }

    if( status == OK )
        FREE1( status, vertices );

    return( found );
}

typedef  struct
{
    int   index_within_poly;
    int   poly_index;
} queue_struct;

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
    int                    i, p, size, point_index, poly_index;
    int                    dir, index_within_poly, neighbour_index_within_poly;
    int                    neighbour_point_index, current_index_within_poly;
    int                    current_poly, n_done;
    Real                   dist;
    Real                   distance_between_points();
    Boolean                found;
    Status                 status;
    queue_struct           entry;
    PRIORITY_QUEUE_STRUCT( queue_struct )   queue;

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
        dist = distance_between_points( &polygons->points[point_index], p2 );

        vertices[point_index].from_point = -1;
        vertices[point_index].distance = dist;
        entry.index_within_poly = p;
        entry.poly_index = poly2;
        INSERT_IN_PRIORITY_QUEUE( status, queue, queue_struct, entry, -dist );
    }

    found = FALSE;

    while( !IS_PRIORITY_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_PRIORITY_QUEUE( queue, entry, dist );

        if( found && dist > *path_dist )
            break;

        index_within_poly = entry.index_within_poly;
        poly_index = entry.poly_index;
        point_index = polygons->indices[
           POINT_INDEX( polygons->end_indices, poly_index, index_within_poly )];

        size = GET_OBJECT_SIZE( *polygons, poly_index );

        for( dir = -1;  dir <= 1;  dir +=2 )
        {
            neighbour_index_within_poly = (index_within_poly + size + dir)
                                           % size;
            neighbour_point_index = polygons->indices[
                       POINT_INDEX( polygons->end_indices, poly_index,
                                    neighbour_index_within_poly )];

            current_index_within_poly = index_within_poly;
            current_poly = poly_index;
            n_done = 0;

            do
            {
                ++n_done;

                if( current_poly == poly1 )
                {
                    dist = distance_between_points(
                                  &polygons->points[point_index], p1 )
                               + vertices[point_index].distance;

                    if( !found || dist < *path_dist )
                    {
                        found = TRUE;
                        *path_dist = dist;
                        *last_vertex = point_index;
                    }
                }

                dist = vertices[point_index].distance +
                       distance_between_points(
                                   &polygons->points[point_index],
                                   &polygons->points[neighbour_point_index] );

                if( vertices[neighbour_point_index].from_point == -2 ||
                    dist < vertices[neighbour_point_index].distance )
                {
                    vertices[neighbour_point_index].distance = dist;   
                    vertices[neighbour_point_index].from_point = point_index;   

                    if( !found || dist < *path_dist )
                    {
                        entry.index_within_poly = neighbour_index_within_poly;
                        entry.poly_index = current_poly;
                        INSERT_IN_PRIORITY_QUEUE( status, queue, queue_struct,
                                                  entry, -dist );
                    }
                }

                found = find_next_edge_around_point( polygons,
                                current_poly, current_index_within_poly,
                                neighbour_index_within_poly,
                                &current_poly, &current_index_within_poly,
                                &neighbour_index_within_poly );

                if( found )
                {
                    neighbour_point_index = polygons->indices[
                           POINT_INDEX( polygons->end_indices, current_poly,
                                        neighbour_index_within_poly )];
                }
            }
            while( found && current_poly != poly_index &&
                   (polygons->visibilities == (Smallest_int *) 0 ||
                    polygons->visibilities[current_poly]) );

            if( current_poly == poly_index && n_done > 1 )   break;
        }
    }

    DELETE_PRIORITY_QUEUE( status, queue );

    return( found );
}

private  Status  create_path( polygons, p1, p2, first_flag,
                              last_vertex, vertices, lines )
    polygons_struct   *polygons;
    Point             *p1;
    Point             *p2;
    Boolean           first_flag;
    int               last_vertex;
    vertex_struct     vertices[];
    lines_struct      *lines;
{
    Status          status;
    Status          begin_adding_points_to_line();
    Status          add_point_to_line();

    status = OK;

    if( first_flag )
    {
        status = begin_adding_points_to_line( lines );

        if( status == OK )
            status = add_point_to_line( lines, p1 );
    }

    while( status == OK && last_vertex >= 0 )
    {
        status = add_point_to_line( lines, &polygons->points[last_vertex] );
        last_vertex = vertices[last_vertex].from_point;
    }

    if( status == OK )
        status = add_point_to_line( lines, p2 );

    return( status );
}
