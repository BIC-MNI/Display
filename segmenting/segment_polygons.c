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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/segmenting/segment_polygons.c,v 1.14 1996-04-19 13:25:20 david Exp $";
#endif


#include  <display.h>

private  void  modify_polygon(
    polygons_struct  *polygons,
    int               poly,
    BOOLEAN           set_visibility_flag,
    BOOLEAN           new_visibility,
    BOOLEAN           set_colour_flag,
    Colour            colour );
private  BOOLEAN  should_modify_polygon(
    polygons_struct   *polygons,
    int               poly,
    BOOLEAN           set_visibility_flag,
    BOOLEAN           new_visibility );
private  BOOLEAN  polygon_on_invisible_side(
    polygons_struct  *polygons,
    int              poly_index,
    int              axis_index,
    Real             position,
    BOOLEAN          cropping_above );

public  void  set_visibility_around_poly(
    polygons_struct  *polygons,
    int              poly,
    int              max_polys_to_do,
    BOOLEAN          set_visibility_flag,
    BOOLEAN          new_visibility,
    BOOLEAN          set_colour_flag,
    Colour           colour )
{
    int                   i, index, neigh, size, n_done;
    unsigned char         *polygons_done_flags;
    QUEUE_STRUCT( int )   queue;

    if( polygons->n_items > 0  &&
        should_modify_polygon( polygons, poly, set_visibility_flag,
                               new_visibility ) )
    {
        if( set_visibility_flag )
            create_polygons_visibilities( polygons );

        if( set_colour_flag )
            set_polygon_per_item_colours( polygons );

        check_polygons_neighbours_computed( polygons );

        ALLOC( polygons_done_flags, polygons->n_items );

        INITIALIZE_QUEUE( queue );

        for_less( i, 0, polygons->n_items )
            polygons_done_flags[i] = FALSE;

        INSERT_IN_QUEUE( queue, poly );
        polygons_done_flags[poly] = TRUE;

        n_done = 0;

        while( n_done < max_polys_to_do && !IS_QUEUE_EMPTY(queue) )
        {
            REMOVE_FROM_QUEUE( queue, poly );

            modify_polygon( polygons, poly, set_visibility_flag,
                            new_visibility, set_colour_flag, colour );

            ++n_done;

            size = GET_OBJECT_SIZE( *polygons, poly );

            for_less( i, 0, size )
            {
                index = POINT_INDEX( polygons->end_indices, poly, i );
                neigh = polygons->neighbours[index];

                if( neigh >= 0 && !polygons_done_flags[neigh] &&
                    should_modify_polygon( polygons, neigh,
                                           set_visibility_flag,
                                           new_visibility ) )
                {
                    INSERT_IN_QUEUE( queue, neigh );
                    polygons_done_flags[neigh] = TRUE;
                }
            }
        }

        print( "Modified %d polygons.\n", n_done );
    }

    DELETE_QUEUE( queue );

    FREE( polygons_done_flags );
}

private  void  modify_polygon(
    polygons_struct  *polygons,
    int               poly,
    BOOLEAN           set_visibility_flag,
    BOOLEAN           new_visibility,
    BOOLEAN           set_colour_flag,
    Colour            colour )
{
    if( set_visibility_flag )
        polygons->visibilities[poly] = (Smallest_int) new_visibility;

    if( set_colour_flag )
        polygons->colours[poly] = colour;
}

private  BOOLEAN  should_modify_polygon(
    polygons_struct   *polygons,
    int               poly,
    BOOLEAN           set_visibility_flag,
    BOOLEAN           new_visibility )
{
    BOOLEAN  polygon_is_currently_visible;

    polygon_is_currently_visible =
               ( polygons->visibilities == (Smallest_int *) 0 ||
                 polygons->visibilities[poly] );

    if( !set_visibility_flag )
        return( polygon_is_currently_visible );
    else
        return( new_visibility || polygon_is_currently_visible );
}

public  void  crop_polygons_visibilities(
    polygons_struct  *polygons,
    int              axis_index,
    Real             position,
    BOOLEAN          cropping_above )
{
    int     i;

    create_polygons_visibilities( polygons );

    for_less( i, 0, polygons->n_items )
    {
        if( polygon_on_invisible_side( polygons, i, axis_index, position,
                                       cropping_above ) )
        {
            polygons->visibilities[i] = FALSE;
        }
    }
}

private  BOOLEAN  polygon_on_invisible_side(
    polygons_struct  *polygons,
    int              poly_index,
    int              axis_index,
    Real             position,
    BOOLEAN          cropping_above )
{
    BOOLEAN  on_invisible_size_only;
    int      i, point_index, size;
    Real     coord;

    size = GET_OBJECT_SIZE( *polygons, poly_index );

    on_invisible_size_only = TRUE;

    for_less( i, 0, size )
    {
        point_index = polygons->indices[
                    POINT_INDEX(polygons->end_indices,poly_index,i)];

        coord = (Real) Point_coord(polygons->points[point_index],axis_index);

        if( coord < position && cropping_above ||
            coord > position && !cropping_above )
        {
            on_invisible_size_only = FALSE;
            break;
        }
    }

    return( on_invisible_size_only );
}
