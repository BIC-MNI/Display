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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/intersect/plane_polygons.c,v 1.12 1995-10-19 15:51:38 david Exp $";
#endif


#include  <display.h>

private  void  intersect_plane_polygons(
    Vector            *plane_normal,
    Real              plane_constant,
    polygons_struct   *polygons,
    lines_struct      *lines,
    int               *n_points_alloced,
    int               *n_indices_alloced,
    int               *n_end_indices_alloced );

public  void  intersect_plane_with_polygons(
    display_struct    *display,
    Vector            *plane_normal,
    Real              plane_constant,
    lines_struct      *lines,
    int               *n_points_alloced,
    int               *n_indices_alloced,
    int               *n_end_indices_alloced )
{
    object_struct            *object;
    object_traverse_struct   object_traverse;

    lines->n_items = 0;
    lines->n_points = 0;

    initialize_object_traverse( &object_traverse, N_MODELS, display->models );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == POLYGONS && object->visibility )
        {
            intersect_plane_polygons( plane_normal, plane_constant,
                                      get_polygons_ptr(object), lines,
                                      n_points_alloced, n_indices_alloced,
                                      n_end_indices_alloced );
        }
    }
}

private  void  intersect_plane_polygons(
    Vector            *plane_normal,
    Real              plane_constant,
    polygons_struct   *polygons,
    lines_struct      *lines,
    int               *n_points_alloced,
    int               *n_indices_alloced,
    int               *n_end_indices_alloced )
{
    int       i;

#ifdef NOT_YET
    if( polygons->bintree != (bintree_struct_ptr) 0 )
    {
        intersect_ray_with_bintree( ray_origin, ray_direction,
                                                 polygons->bintree, polygons,
                                                 poly_index, dist );
    }
    else
#endif
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
}

public  BOOLEAN  intersect_plane_one_polygon(
    Vector            *plane_normal,
    Real              plane_constant,
    polygons_struct   *polygons,
    int               poly,
    lines_struct      *lines,
    int               *n_points_alloced,
    int               *n_indices_alloced,
    int               *n_end_indices_alloced )
{
    int       point_index, n_indices;
    Point     points[2];
    BOOLEAN   intersects;

    intersects = get_plane_polygon_intersection( plane_normal, plane_constant,
                                                 polygons, poly, points );

    if( intersects )
    {
        point_index = lines->n_points;

        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( lines->points,
                                        *n_points_alloced, lines->n_points,
                                        points[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( lines->points,
                                        *n_points_alloced, lines->n_points,
                                        points[1], DEFAULT_CHUNK_SIZE );

        n_indices = NUMBER_INDICES( *lines );

        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( lines->indices,
                                        *n_indices_alloced, n_indices,
                                        point_index, DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( lines->indices,
                                        *n_indices_alloced, n_indices,
                                        point_index+1, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( lines->end_indices,
                                        *n_end_indices_alloced, lines->n_items,
                                        n_indices, DEFAULT_CHUNK_SIZE );
    }

    return( intersects );
}
