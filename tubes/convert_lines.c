
#include  <def_display.h>

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
        object = create_object( QUADMESH );

        quadmesh = get_quadmesh_ptr( object );

        quadmesh->colour_flag = ONE_COLOUR;

        ALLOC( quadmesh->colours, 1 );
        quadmesh->colours[0] = lines->colours[0];
        get_default_surfprop( &quadmesh->surfprop );

        line_size = GET_OBJECT_SIZE( *lines, l );

        quadmesh->m = line_size;
        quadmesh->m_closed = FALSE;
        quadmesh->n = n_around;
        quadmesh->n_closed = TRUE;

        ALLOC( quadmesh->points, quadmesh->m * quadmesh->n );
        ALLOC( quadmesh->normals, quadmesh->m * quadmesh->n );
        ALLOC( points, line_size );

        for_less( i, 0, line_size )
        {
            points[i] = lines->points[lines->indices[
                           POINT_INDEX(lines->end_indices,l,i)]];
        }

        generate_tube( line_size, points, n_around, radius,
                       quadmesh->points, quadmesh->normals );
 
        FREE( points );

        add_object_to_model( get_current_model(display), object );
    }
}
