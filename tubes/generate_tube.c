
#include  <def_geometry.h>
#include  <def_alloc.h>
#include  <def_math.h>

public  void  generate_tube_elbow_normals( n_points, points, derivs )
    int    n_points;
    Point  points[];
    Vector derivs[];
{
    int      i;
    Boolean  wrap_around;

    wrap_around = EQUAL_POINTS( points[0], points[n_points-1] );

    for_less( i, 0, n_points )
    {
        end = i;
        do
        {
            ++end;
            if( end == n_points && wrap_around )
                end = 0;
        }
        while( end < n_points && end != i &&
               EQUAL_POINTS(points[i],points[end]) );

        SUB_POINTS( derivs[i], points[end], points[i] );

        if( null_Vector( &derivs[i] )
            fill_Vector( &derivs[i], 0.0, 0.0, 1.0 );
    }
}

public  void  generate_tube( n_points, points, derivs, n_around, radius,
                             tube_points, tube_normals )
    int    n_points;
    Point  points[];
    Vector derivs[];
    int    n_around;
    Real   radius;
    Point  tube_points[];
    Vector tube_normals[];
{
    int     i;
    Real    *x, *y, angle;
    Vector  hor, vert;
    void    get_noncolinear_vector();
    void    fill_in_ellipse_points();
    Status  status;

    ALLOC1( status, x, n_around, Real );

    if( status == OK )
        ALLOC1( status, y, n_around, Real );

    if( status == OK )
    {
        for_less( i, 0, n_around )
        {
            angle = (double) i / (double) n_around * 2.0 * PI;
            x[i] = radius * cos( angle );
            y[i] = radius * sin( angle );
        }
    }

    get_noncolinear_vector( &derivs[i], &hor );

    for_less( i, 0, n_points )
    {
        CROSS_VECTORS( vert, derivs[i], hor );
        CROSS_VECTORS( hor, vert, derivs[i] );

        NORMALIZE_VECTOR( hor, hor );
        NORMALIZE_VECTOR( vert, vert );

        fill_in_ellipse_points( &tube_points[IJ(i,0,n_around)],
                                &tube_normals[IJ(i,0,n_around)],
                                &points[i], n_around, x, y, &hor, &vert );
    }
}

private  void   fill_in_ellipse_points( tube_points, tube_normals, centre,
                                        n_around, x, y,
                                        hor, vert, plane_normal )
    Point   tube_points[];
    Vector  tube_normals[];
    Point   *centre;
    int     n_around;
    Real    x[], y[];
    Vector  *hor;
    Vector  *vert;
{
    int      i;
    Vector   h, v, offset;

    for_less( i, 0, n_around )
    {
        SCALE_VECTOR( h, hor, x[i] );
        SCALE_VECTOR( v, vert, y[i] );

        ADD_VECTORS( offset, h, v );

        ADD_POINT_VECTOR( tube_points[i], *centre, offset );
        NORMALIZE_VECTOR( tube_normals[i], offset );
    }
}
