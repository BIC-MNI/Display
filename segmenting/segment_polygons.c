
#include  <def_graphics.h>
#include  <def_alloc.h>
#include  <def_queue.h>

public  Status  set_visibility_around_poly( polygons, poly,
                                            max_polys_to_do,
                                            set_visibility_flag,
                                            new_visibility,
                                            set_colour_flag, colour )
    polygons_struct  *polygons;
    int              poly;
    int              max_polys_to_do;
    Boolean          set_visibility_flag;
    Boolean          new_visibility;
    Boolean          set_colour_flag;
    Colour           *colour;
{
    int                   i, index, neigh, size, n_done;
    Status                status;
    Status                check_polygons_neighbours_computed();
    Status                create_polygons_visibilities();
    Status                set_polygon_per_item_colours();
    unsigned char         *polygons_done_flags;
    QUEUE_STRUCT( int )   queue;
    void                  modify_polygon();

    status = OK;

    if( polygons->n_items > 0  &&
        should_modify_polygon( polygons, poly, set_visibility_flag,
                               new_visibility ) )
    {
        if( set_visibility_flag )
            status = create_polygons_visibilities( polygons );

        if( status == OK && set_colour_flag )
            status = set_polygon_per_item_colours( polygons );

        if( status == OK )
            status = check_polygons_neighbours_computed( polygons );

        if( status == OK )
            ALLOC1( status, polygons_done_flags, polygons->n_items,
                    unsigned char );

        if( status == OK )
        {
            INITIALIZE_QUEUE( queue );

            for_less( i, 0, polygons->n_items )
                polygons_done_flags[i] = FALSE;

            INSERT_IN_QUEUE( status, queue, int, poly );
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
                        INSERT_IN_QUEUE( status, queue, int, neigh );
                        polygons_done_flags[neigh] = TRUE;
                    }
                }
            }

            PRINT( "Modified %d polygons.\n", n_done );
        }

        if( status == OK )
            DELETE_QUEUE( status, queue );

        if( status == OK )
            FREE1( status, polygons_done_flags );
    }

    return( status );
}

private  void  modify_polygon( polygons, poly, set_visibility_flag,
                               new_visibility, set_colour_flag, colour )
    polygons_struct   *polygons;
    int               poly;
    Boolean           set_visibility_flag;
    Boolean           new_visibility;
    Boolean           set_colour_flag;
    Colour            *colour;
{
    if( set_visibility_flag )
        polygons->visibilities[poly] = new_visibility;

    if( set_colour_flag )
        polygons->colours[poly] = *colour;
}

private  Boolean  should_modify_polygon( polygons, poly, set_visibility_flag,
                                         new_visibility )
    polygons_struct   *polygons;
    int               poly;
    Boolean           set_visibility_flag;
    Boolean           new_visibility;
{
    Boolean  polygon_is_currently_visible;

    polygon_is_currently_visible =
               ( polygons->visibilities == (Smallest_int *) 0 ||
                 polygons->visibilities[poly] );

    if( !set_visibility_flag )
        return( polygon_is_currently_visible );
    else
        return( new_visibility || polygon_is_currently_visible );
}

public  void  crop_polygons_visibilities( polygons, axis_index, position,
                                          cropping_above )
    polygons_struct  *polygons;
    int              axis_index;
    Real             position;
    Boolean          cropping_above;
{
    Status  status;
    Status  create_polygons_visibilities();
    int     i;

    status = create_polygons_visibilities( polygons );

    if( status == OK )
    {
        for_less( i, 0, polygons->n_items )
        {
            if( polygon_on_invisible_side( polygons, i, axis_index, position,
                                           cropping_above ) )
            {
                polygons->visibilities[i] = FALSE;
            }
        }
    }
}

private  Boolean  polygon_on_invisible_side( polygons, poly_index, axis_index,
                                             position, cropping_above )
    polygons_struct  *polygons;
    int              poly_index;
    int              axis_index;
    Real             position;
    Boolean          cropping_above;
{
    Boolean  on_invisible_size_only;
    int      i, point_index, size;
    Real     coord;

    size = GET_OBJECT_SIZE( *polygons, poly_index );

    on_invisible_size_only = TRUE;

    for_less( i, 0, size )
    {
        point_index = polygons->indices[
                    POINT_INDEX(polygons->end_indices,poly_index,i)];

        coord = Point_coord(polygons->points[point_index],axis_index);

        if( coord < position && cropping_above ||
            coord > position && !cropping_above )
        {
            on_invisible_size_only = FALSE;
            break;
        }
    }

    return( on_invisible_size_only );
}
