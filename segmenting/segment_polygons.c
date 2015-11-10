/**
 * \file segment_polygons.c
 * \brief Helper functions for surface segmentation.
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

static  void  modify_polygon(
    polygons_struct  *polygons,
    int               poly,
    VIO_BOOL          set_visibility_flag,
    VIO_BOOL          new_visibility,
    VIO_BOOL          set_colour_flag,
    VIO_Colour        colour );
static  VIO_BOOL  should_modify_polygon(
    polygons_struct   *polygons,
    int               poly,
    VIO_BOOL          set_visibility_flag,
    VIO_BOOL          new_visibility );
static  VIO_BOOL  polygon_on_invisible_side(
    polygons_struct  *polygons,
    int              poly_index,
    int              axis_index,
    VIO_Real         position,
    VIO_BOOL         cropping_above );

/**
 * Modify the visibility and/or colour of polygons in a neighbourhood around
 * the currently selected polygon.
 * \param polygons A pointer to the polygons_struct
 * \param poly The index of the face
 * \param max_polys_to_do The maximum number of polygons to modify
 * \param set_visibility_flag True if we should set the visibility.
 * \param new_visibility The new visiblility state, if set_visibility_flag
 * is true.
 * \param set_colour_flag True if we should set the colour.
 * \param colour The colour to apply if set_colour_flag is true.
 */
  void  set_visibility_around_poly(
    polygons_struct  *polygons,
    int              poly,
    int              max_polys_to_do,
    VIO_BOOL         set_visibility_flag,
    VIO_BOOL         new_visibility,
    VIO_BOOL         set_colour_flag,
    VIO_Colour       colour )
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

static  void  modify_polygon(
    polygons_struct  *polygons,
    int               poly,
    VIO_BOOL           set_visibility_flag,
    VIO_BOOL           new_visibility,
    VIO_BOOL           set_colour_flag,
    VIO_Colour            colour )
{
    if( set_visibility_flag )
        polygons->visibilities[poly] = (VIO_SCHAR) new_visibility;

    if( set_colour_flag )
        polygons->colours[poly] = colour;
}

/** 
 * See if this polygon should be modified. We only change the polygon's
 * colour or visibility if it is currently visible, or if we are changing 
 * the visibility state.
 */
static  VIO_BOOL  should_modify_polygon(
    polygons_struct   *polygons,
    int               poly,
    VIO_BOOL           set_visibility_flag,
    VIO_BOOL           new_visibility )
{
    VIO_BOOL  polygon_is_currently_visible;

    polygon_is_currently_visible =
               ( polygons->visibilities == (VIO_SCHAR *) 0 ||
                 polygons->visibilities[poly] );

    if( !set_visibility_flag )
        return( polygon_is_currently_visible );
    else
        return( new_visibility || polygon_is_currently_visible );
}

  void  crop_polygons_visibilities(
    polygons_struct  *polygons,
    int              axis_index,
    VIO_Real             position,
    VIO_BOOL          cropping_above )
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

static  VIO_BOOL  polygon_on_invisible_side(
    polygons_struct  *polygons,
    int              poly_index,
    int              axis_index,
    VIO_Real         position,
    VIO_BOOL         cropping_above )
{
    VIO_BOOL on_invisible_size_only;
    int      i, point_index, size;
    VIO_Real coord;

    size = GET_OBJECT_SIZE( *polygons, poly_index );

    on_invisible_size_only = TRUE;

    for_less( i, 0, size )
    {
        point_index = polygons->indices[
                    POINT_INDEX(polygons->end_indices,poly_index,i)];

        coord = (VIO_Real) Point_coord(polygons->points[point_index],axis_index);

        if( (coord < position && cropping_above) ||
            (coord > position && !cropping_above) )
        {
            on_invisible_size_only = FALSE;
            break;
        }
    }

    return( on_invisible_size_only );
}
