#include  <def_display.h>

private  void  cut_neighbour(
    polygons_struct   *polygons,
    int               new_neighbours[],
    int               poly,
    int               vertex_index,
    Point             *point,
    int               *next_poly,
    int               *next_vertex_index );

private  void  cut_polygon_neighbours_by_lines(
    polygons_struct   *polygons,
    int               new_neighbours[],
    lines_struct      *lines )
{
    int              line, i, size, poly, point_index, vertex_index;
    Point            point;
    progress_struct  progress;

    for_less( line, 0, lines->n_items )
    {
        point = lines->points[lines->indices
                         [POINT_INDEX(lines->end_indices,line,0)]];

        if( lookup_polygon_vertex( polygons, &point, &point_index ) &&
            find_polygon_with_vertex( polygons, point_index,
                                      &poly, &vertex_index ) )
        {
            size = GET_OBJECT_SIZE( *lines, line );

            initialize_progress_report( &progress, FALSE, size-1,
                                       "Cutting Neighbours" );

            for_less( i, 1, size )
            {
                point = lines->points[lines->indices
                                    [POINT_INDEX(lines->end_indices,line,i)]];
                cut_neighbour( polygons, new_neighbours, poly, vertex_index,
                               &point, &poly, &vertex_index );
                update_progress_report( &progress, i );
            }

            terminate_progress_report( &progress );
        }
    }
}

private  void  cut_neighbour(
    polygons_struct   *polygons,
    int               new_neighbours[],
    int               poly,
    int               vertex_index,
    Point             *point,
    int               *next_poly,
    int               *next_vertex_index )
{
    int        point_index, size, dir, edge;
    int        current_poly, current_index_within_poly, neighbour;
    int        neighbour_point_index, neighbour_index_within_poly;
    Boolean    found, at_least_one_neighbour;

    point_index = polygons->indices[
             POINT_INDEX( polygons->end_indices, poly, vertex_index )];

    size = GET_OBJECT_SIZE( *polygons, poly );

    for( dir = -1;  dir <= 1;  dir +=2 )
    {
        current_poly = poly;
        current_index_within_poly = vertex_index;
        neighbour_index_within_poly = (vertex_index + size + dir)
                                       % size;
        neighbour_point_index = polygons->indices[
                   POINT_INDEX( polygons->end_indices, poly,
                                neighbour_index_within_poly )];

        found = TRUE;

        at_least_one_neighbour = FALSE;

        while( !EQUAL_POINTS( polygons->points[neighbour_point_index],*point) )
        {
            if( !find_next_edge_around_point( polygons,
                            current_poly, current_index_within_poly,
                            neighbour_index_within_poly,
                            &current_poly, &current_index_within_poly,
                            &neighbour_index_within_poly ) ||
                current_poly == poly )
            {
                found = FALSE;
                break;
            }

            neighbour_point_index = polygons->indices[
                   POINT_INDEX( polygons->end_indices, current_poly,
                                neighbour_index_within_poly )];

            at_least_one_neighbour = TRUE;
        }

        if( found || (current_poly == poly && at_least_one_neighbour) )
            break;
    }

    if( found )
    {
        if( neighbour_index_within_poly == 
            (current_index_within_poly + 1) % size )
            edge = current_index_within_poly;
        else
            edge = neighbour_index_within_poly;

        *next_poly = current_poly;
        *next_vertex_index = neighbour_index_within_poly;

        neighbour = polygons->neighbours
                     [POINT_INDEX(polygons->end_indices,current_poly,edge)];

        if( neighbour != -1 )
        {
            new_neighbours[
                  POINT_INDEX(polygons->end_indices,current_poly,edge)] = -1;
            edge = find_edge_index( polygons, neighbour, point_index,
                     polygons->indices[
                       POINT_INDEX( polygons->end_indices, current_poly,
                                    neighbour_index_within_poly )] );
            if( edge >= 0 )
                new_neighbours
                      [POINT_INDEX(polygons->end_indices,neighbour,edge)] = -1;
        }
    }
    else
    {
        print( "Error in cut_neighbour.\n" );
    }
}

public  void  cut_polygon_neighbours_from_lines(
    display_struct     *display,
    polygons_struct    *polygons )
{
    int                     i, *new_neighbours;
    object_struct           *object;
    object_traverse_struct  object_traverse;

    check_polygons_neighbours_computed( polygons );

    ALLOC( new_neighbours, NUMBER_INDICES( *polygons ) );

    for_less( i, 0, NUMBER_INDICES( *polygons ) )
        new_neighbours[i] = polygons->neighbours[i];

    object = display->models[THREED_MODEL];

    initialize_object_traverse( &object_traverse, 1, &object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == LINES )
        {
            cut_polygon_neighbours_by_lines( polygons, new_neighbours,
                                             get_lines_ptr(object) );
        }
    }

    FREE( polygons->neighbours );
    polygons->neighbours = new_neighbours;
}
