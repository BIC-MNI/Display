#include  <def_display.h>

private  void  cut_neighbour(
    polygons_struct   *polygons,
    Point             *p1,
    Point             *p2 );

private  void  cut_polygon_neighbours_by_lines(
    polygons_struct   *polygons,
    lines_struct      *lines )
{
    int              line, i, size;
    Point            p1, p2;
    progress_struct  progress;

    initialize_progress_report( &progress, FALSE, lines->n_items,
                               "Cutting Neighbours" );

    for_less( line, 0, lines->n_items )
    {
        size = GET_OBJECT_SIZE( *lines, line );

        p2 = lines->points[lines->indices
                                [POINT_INDEX(lines->end_indices,line,0)]];
        for_less( i, 1, size )
        {
            p1 = p2;
            p2 = lines->points[lines->indices
                                [POINT_INDEX(lines->end_indices,line,i)]];
            cut_neighbour( polygons, &p1, &p2 );
        }

        update_progress_report( &progress, line + 1 );
    }

    terminate_progress_report( &progress );
}

private  void  cut_neighbour(
    polygons_struct   *polygons,
    Point             *p1,
    Point             *p2 )
{
    int   index1, index2, poly, edge, neighbour;

    if( lookup_polygon_vertex( polygons, p1, &index1 ) &&
        lookup_polygon_vertex( polygons, p2, &index2 ) &&
        find_polygon_with_edge( polygons, index1, index2, &poly, &edge ) )
    {
        neighbour = polygons->neighbours
                               [POINT_INDEX(polygons->end_indices,poly,edge)];

        if( neighbour != -1 )
        {
            polygons->neighbours[POINT_INDEX(polygons->end_indices,poly,edge)] =
                                                  -1;
            edge = find_edge_index( polygons, neighbour, index1, index2 );
            if( edge >= 0 )
                polygons->neighbours
                      [POINT_INDEX(polygons->end_indices,neighbour,edge)] = -1;
        }
    }
}

public  void  cut_polygon_neighbours_from_lines(
    display_struct     *display,
    polygons_struct    *polygons )
{
    object_struct           *object;
    object_traverse_struct  object_traverse;

    check_polygons_neighbours_computed( polygons );

    object = display->models[THREED_MODEL];

    initialize_object_traverse( &object_traverse, 1, &object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == LINES )
        {
            cut_polygon_neighbours_by_lines( polygons, get_lines_ptr(object) );
        }
    }
}
