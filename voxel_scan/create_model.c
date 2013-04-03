#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

private  void  create_quadmesh_of_surface(
    surface_rep_struct  *surface_rep,
    double              descriptors[],
    double              parameters[],
    int                 m,
    int                 n,
    quadmesh_struct     *quadmesh );

public  void  create_model_of_surface(
    surface_rep_struct  *surface_rep,
    double              descriptors[],
    double              parameters[],
    int                 m,
    int                 n,
    object_struct       **object )
{
    quadmesh_struct         *quadmesh;

    *object = create_object( QUADMESH );

    quadmesh = get_quadmesh_ptr( *object );

    create_quadmesh_of_surface( surface_rep, descriptors,
                                parameters, m, n, quadmesh );
}

private  void  create_quadmesh_of_surface(
    surface_rep_struct  *surface_rep,
    double              descriptors[],
    double              parameters[],
    int                 m,
    int                 n,
    quadmesh_struct     *quadmesh )
{
    int     i, j;
    double  u, v, x, y, z, dux, duy, duz, dvx, dvy, dvz;
    Vector  normal;

    get_default_surfprop( &quadmesh->surfprop );
    quadmesh->colour_flag = ONE_COLOUR;
    ALLOC( quadmesh->colours, 1 );

    quadmesh->colours[0] = WHITE;

    quadmesh->m = m;
    quadmesh->n = n;

    quadmesh->m_closed = FALSE;
    quadmesh->n_closed = FALSE;

    ALLOC( quadmesh->points, m * n );

    ALLOC( quadmesh->normals, m * n );

    for_less( i, 0, m )
    {
        u = (double) i / (double) (m-1);
        for_less( j, 0, n )
        {
            v = (double) j / (double) (n-1);
            surface_rep->evaluate_surface_at_uv( u, v, descriptors, parameters,
                                  &x, &y, &z,
                                  &dux, &duy, &duz,
                                  &dvx, &dvy, &dvz,
                                  (double *) 0, (double *) 0, (double *) 0,
                                  (double *) 0, (double *) 0, (double *) 0 );

            get_surface_normal_from_derivs( dux, duy, duz, dvx, dvy, dvz,
                                            &normal );

            fill_Point( quadmesh->points[VIO_IJ(i,j,n)], x, y, z );
            quadmesh->normals[VIO_IJ(i,j,n)] = normal;
        }
    }
}
