
#include  <def_graphics.h>

public  Status  create_model_of_surface( surface_rep, descriptors, parameters,
                                         m, n, object )
    surface_rep_struct  *surface_rep;
    double              descriptors[];
    double              parameters[];
    int                 m, n;
    object_struct       **object;
{
    Status                  status;
    Status                  create_object();
    Status                  create_quadmesh_of_surface();
    quadmesh_struct         *quadmesh;

    status = create_object( object, QUADMESH );

    if( status == OK )
    {
        quadmesh = (*object)->ptr.quadmesh;

        status = create_quadmesh_of_surface( surface_rep, descriptors,
                                             parameters, m, n, quadmesh );
    }
 
    return( status );
}

private  Status  create_quadmesh_of_surface( surface_rep, descriptors,
                                             parameters, m, n, quadmesh )
    surface_rep_struct  *surface_rep;
    double              descriptors[];
    double              parameters[];
    int                 m, n;
    quadmesh_struct     *quadmesh;
{
    Status  status;
    int     i, j;
    double  u, v, x, y, z, dux, duy, duz, dvx, dvy, dvz;
    Vector  normal;
    void    get_default_surfprop();
    void    get_surface_normal_from_derivs();

    get_default_surfprop( &quadmesh->surfprop );
    quadmesh->colour_flag = ONE_COLOUR;
    ALLOC( status, quadmesh->colours, 1 );

    if( status == OK )
    {
        quadmesh->colours[0] = WHITE;

        quadmesh->m = m;
        quadmesh->n = n;

        quadmesh->m_closed = FALSE;
        quadmesh->n_closed = FALSE;

        ALLOC( status, quadmesh->points, m * n );
    }

    if( status == OK )
        ALLOC( status, quadmesh->normals, m * n );

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

            fill_Point( quadmesh->points[IJ(i,j,n)], x, y, z );
            quadmesh->normals[IJ(i,j,n)] = normal;
        }
    }

    return( status );
}
