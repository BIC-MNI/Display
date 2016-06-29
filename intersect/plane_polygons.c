/**
 * \file intersect/plane_polygons.c
 *
 * \copyright
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
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

/** 
 * This is more-or-less a duplicate of get_plane_polygon_intersection()
 * from bicpl/Objects/polygons.c, _except_ that it finds and returns
 * _all_ intersection points between the polygon and the plane. This
 * means that in the rare case where there are more than two intersections,
 * we don't drop a segment from the outline.
 *
 * \returns The total number of intersection points found.
 */
static int
get_all_plane_polygon_intersections(
    VIO_Vector       *normal,
    VIO_Real         d,
    polygons_struct  *polygons,
    int              poly,
    VIO_Point        intersection_points[],
    int              max_points )
{
    int i1, i2, edge, size;
    int n_intersections = 0;

    size = GET_OBJECT_SIZE( *polygons, poly );

    for_less( edge, 0, size )
    {
        i1 = polygons->indices[POINT_INDEX(polygons->end_indices,poly,edge)];
        i2 = polygons->indices[
                  POINT_INDEX(polygons->end_indices,poly,(edge+1)%size)];

        if( get_plane_segment_intersection( normal, d, &polygons->points[i1],
                                        &polygons->points[i2],
                                        &intersection_points[n_intersections] ))
        {
            if ( ++n_intersections >= max_points )
                break;
        }
    }

    return( n_intersections );
}

#define MAX_POINTS 16

VIO_BOOL  intersect_plane_one_polygon(
    VIO_Vector        *plane_normal,
    VIO_Real          plane_constant,
    polygons_struct   *polygons,
    int               poly,
    lines_struct      *lines,
    int               *n_points_alloced,
    int               *n_indices_alloced,
    int               *n_end_indices_alloced )
{
    VIO_Point points[MAX_POINTS];
    int       n_intersections;

    n_intersections = get_all_plane_polygon_intersections( plane_normal,
                                                           plane_constant,
                                                           polygons, poly,
                                                           points,
                                                           MAX_POINTS );

    if( n_intersections > 0 )
    {
        int point_index = lines->n_points;
        int n_indices = NUMBER_INDICES( *lines );
        int i;

        for_less( i, 0, n_intersections )
        {
          ADD_ELEMENT_TO_ARRAY_WITH_SIZE( lines->points,
                                          *n_points_alloced, lines->n_points,
                                          points[i], DEFAULT_CHUNK_SIZE );

          ADD_ELEMENT_TO_ARRAY_WITH_SIZE( lines->indices,
                                          *n_indices_alloced, n_indices,
                                          point_index+i, DEFAULT_CHUNK_SIZE );
        }

        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( lines->end_indices,
                                        *n_end_indices_alloced, lines->n_items,
                                        n_indices, DEFAULT_CHUNK_SIZE );
    }

    return( n_intersections >= 2 );
}
