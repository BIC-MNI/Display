
#include  <display.h>

public  void  convert_lines_to_tubes_objects(
    display_struct    *display,
    lines_struct      *lines,
    int               n_around,
    Real              radius )
{
    int              i, n_quadmeshes;
    object_struct    *object;
    quadmesh_struct  *quadmeshes, *quadmesh;

    n_quadmeshes = convert_lines_to_tubes( lines, n_around, radius,
                                           &quadmeshes );

    for_less( i, 0, n_quadmeshes )
    {
        object = create_object( QUADMESH );
        quadmesh = get_quadmesh_ptr( object );
        *quadmesh = quadmeshes[i];

        add_object_to_model( get_current_model(display), object );
    }

    if( n_quadmeshes > 0 )
    {
        FREE( quadmeshes );
    }
}
