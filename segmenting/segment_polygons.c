
#include  <def_graphics.h>
#include  <def_alloc.h>
#include  <def_queue.h>

public  Status  set_visibility_around_poly( polygons, poly,
                                            max_polys_to_do, visibility_flag,
                                            set_colour_flag, colour )
    polygons_struct  *polygons;
    int              poly;
    int              max_polys_to_do;
    Boolean          visibility_flag;
    Boolean          set_colour_flag;
    Colour           *colour;
{
    int                   i, index, neigh, size, n_done;
    Status                status;
    Status                create_polygon_neighbours();
    Status                create_polygons_visibilities();
    Status                set_polygon_per_item_colours();
    unsigned char         *polygons_done_flags;
    QUEUE_STRUCT( int )   queue;

    status = OK;

    if( polygons->n_items > 0 )
    {
        status = create_polygons_visibilities( polygons );

        if( status == OK && polygons->neighbours == (int *) 0 )
            status = create_polygon_neighbours( polygons->n_items,
                            polygons->indices, polygons->end_indices,
                            &polygons->neighbours );

        if( status == OK )
            ALLOC1( status, polygons_done_flags, polygons->n_items,
                    unsigned char );

        if( status == OK && set_colour_flag )
            status = set_polygon_per_item_colours( polygons );

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

                if( set_colour_flag )
                    polygons->colours[poly] = *colour;
                else
                    polygons->visibilities[poly] = visibility_flag;

                ++n_done;

                size = GET_OBJECT_SIZE( *polygons, poly );

                for_less( i, 0, size )
                {
                    index = POINT_INDEX( polygons->end_indices, poly, i );
                    neigh = polygons->neighbours[index];

                    if( neigh >= 0 &&
                        !polygons_done_flags[neigh] &&
                        (polygons->visibilities[neigh] ||
                         (visibility_flag && !set_colour_flag) ) )
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