
#include  <display.h>

public  void  convert_lines_to_tubes(
    display_struct    *display,
    lines_struct      *lines,
    int               n_around,
    Real              radius )
{
    int              i, l, line_size;
    object_struct    *object;
    quadmesh_struct  *quadmesh;
    Point            *points;

    for_less( l, 0, lines->n_items )
    {
        line_size = GET_OBJECT_SIZE( *lines, l );

        ALLOC( points, line_size );

        for_less( i, 0, line_size )
        {
            points[i] = lines->points[lines->indices[
                           POINT_INDEX(lines->end_indices,l,i)]];
        }

        object = create_object( QUADMESH );
        quadmesh = get_quadmesh_ptr( object );

        create_tube( line_size, points, n_around, radius, lines->colours[0],
                     quadmesh );
 
        FREE( points );

        add_object_to_model( get_current_model(display), object );
    }
}
