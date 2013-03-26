/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifndef lint

#endif

#include  <display.h>

private  void  cut_neighbour(
    polygons_struct   *polygons,
    int               new_neighbours[],
    int               poly,
    int               vertex_index,
    VIO_Point             *point,
    int               *next_poly,
    int               *next_vertex_index );

private  void  cut_polygon_neighbours_by_lines(
    polygons_struct   *polygons,
    int               new_neighbours[],
    lines_struct      *lines )
{
    int              line, i, size, poly, point_index, vertex_index;
    int              first_vertex;
    VIO_Point            point;
    progress_struct  progress;

    for_less( line, 0, lines->n_items )
    {
        size = GET_OBJECT_SIZE( *lines, line );

        first_vertex = 0;
        do
        {
            point = lines->points[lines->indices
                         [POINT_INDEX(lines->end_indices,line,first_vertex)]];

            ++first_vertex;
        }
        while( first_vertex < size &&
               !(lookup_polygon_vertex( polygons, &point, &point_index ) &&
                 find_polygon_with_vertex( polygons, point_index,
                                           &poly, &vertex_index )) );

        initialize_progress_report( &progress, FALSE, size,
                                   "Cutting Neighbours" );

        for_less( i, first_vertex, size )
        {
            point = lines->points[lines->indices
                                [POINT_INDEX(lines->end_indices,line,i)]];
            cut_neighbour( polygons, new_neighbours, poly, vertex_index,
                           &point, &poly, &vertex_index );
            update_progress_report( &progress, i+1 );
        }

        terminate_progress_report( &progress );
    }
}

#define  MAX_POLYS   2000

private  void  cut_neighbour(
    polygons_struct   *polygons,
    int               new_neighbours[],
    int               poly,
    int               vertex_index,
    VIO_Point             *point,
    int               *next_poly,
    int               *next_vertex_index )
{
    int        prev_point_index, point_index, size, edge;
    int        i, p, v, step, neighbour, n_polys, prev_vertex_index;
    VIO_BOOL    found, closed;
    int        polys[MAX_POLYS];

    n_polys = get_polygons_around_vertex( polygons, poly, vertex_index,
                                          polys, MAX_POLYS, &closed );

    found = FALSE;
    for_less( i, 0, n_polys )
    {
        size = GET_OBJECT_SIZE( *polygons, polys[i] );

        for_less( p, 0, size )
        {
            point_index = polygons->indices[
                           POINT_INDEX( polygons->end_indices, polys[i], p )];
            if( EQUAL_POINTS( *point, polygons->points[point_index] ) )
            {
                *next_poly = polys[i];
                *next_vertex_index = p;
                found = TRUE;
                break;
            }
        }
        if( found )
            break;
    }

    if( found )
    {
        prev_point_index = polygons->indices[
                     POINT_INDEX( polygons->end_indices, poly, vertex_index ) ];
        prev_vertex_index = find_vertex_index( polygons, *next_poly,
                                               prev_point_index );

        if( (*next_vertex_index - prev_vertex_index + size) % size <= size / 2 )
        {
            step = 1;
        }
        else
        {
            step = -1;
        }

        v = prev_vertex_index;

        while( v != *next_vertex_index )
        {
            if( step == 1 )
                edge = v;
            else
                edge = (v - 1 + size) % size;

            neighbour = polygons->neighbours
                         [POINT_INDEX(polygons->end_indices,*next_poly,edge)];

            if( neighbour != -1 )
            {
                new_neighbours[
                  POINT_INDEX(polygons->end_indices,*next_poly,edge)] = -1;

                edge = find_edge_index( polygons, neighbour,
                         polygons->indices[
                            POINT_INDEX(polygons->end_indices,*next_poly,edge)],
                         polygons->indices[
                            POINT_INDEX(polygons->end_indices,*next_poly,
                                       (edge+1)%size)] );
                if( edge >= 0 )
                    new_neighbours
                       [POINT_INDEX(polygons->end_indices,neighbour,edge)] = -1;
                else
                {
                    print( "Error in cut_neighbour 2.\n" );
                }
            }

            v += step;
            if( v == -1 )
                v = size - 1;
            else if( v == size )
                v = 0;
        }
    }
    else
    {
        *next_poly = poly;
        *next_vertex_index = vertex_index;
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

    initialize_object_traverse( &object_traverse, FALSE, 1, &object );

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
