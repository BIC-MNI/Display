
#include  <def_geometry.h>
#include  <def_alloc.h>
#include  <def_math.h>

private  void  get_direction( n_points, points, i, wrap_around, dir )
    int      n_points;
    Point    points[];
    int      i;
    Boolean  wrap_around;
    Vector   *dir;
{
    int      end;

    end = i;

    do
    {
        ++end;
        if( end == n_points && wrap_around )
            end = 0;
    }
    while( end < n_points && end != i && EQUAL_POINTS(points[i],points[end]) );

    if( end >= n_points || end == i )
    {
        fill_Vector( *dir, 0.0, 0.0, 0.0 );
    }
    else
    {
        SUB_POINTS( *dir, points[end], points[i] );

        NORMALIZE_VECTOR( *dir, *dir );
    }
}

public  void  generate_tube( n_points, points, n_around, radius,
                             tube_points, tube_normals )
    int    n_points;
    Point  points[];
    int    n_around;
    Real   radius;
    Point  tube_points[];
    Vector tube_normals[];
{
    int      i;
    Real     *x, *y, angle;
    Vector   hor, vert, dir, prev_dir, normal;
    void     project_vector_to_plane();
    void     get_noncolinear_vector();
    void     fill_in_ellipse_points();
    Status   status;
    Boolean  wrap_around;
    void     get_direction();

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

    wrap_around = EQUAL_POINTS( points[0], points[n_points-1] );

    if( wrap_around )
        get_direction( n_points, points, n_points-2, wrap_around, &prev_dir );
    else
        fill_Vector( prev_dir, 0.0, 0.0, 0.0 );

    get_direction( n_points, points, 0, wrap_around, &dir );
    get_noncolinear_vector( &dir, &hor );

    for_less( i, 0, n_points )
    {
/*
        if( DOT_VECTORS( dir, prev_dir ) < 0.0 )
            SCALE_VECTOR( hor, hor, -1.0 );
*/

        CROSS_VECTORS( vert, dir, hor );

        if( null_Vector(&vert) )
        {
            if( DOT_VECTORS( hor, dir ) < 0.0 )
                CROSS_VECTORS( vert, dir, prev_dir )
            else
                CROSS_VECTORS( vert, prev_dir, dir )
        }

        NORMALIZE_VECTOR( vert, vert );

        CROSS_VECTORS( hor, vert, dir );
        NORMALIZE_VECTOR( hor, hor );

        ADD_VECTORS( normal, prev_dir, dir );
        NORMALIZE_VECTOR( normal, normal );

        project_vector_to_plane( &hor, &normal, &hor );
        project_vector_to_plane( &vert, &normal, &vert );

        fill_in_ellipse_points( &tube_points[IJ(i,0,n_around)],
                                &tube_normals[IJ(i,0,n_around)],
                                &points[i], n_around, x, y, &hor, &vert );

        if( i < n_points-1 )
        {
            prev_dir = dir;

            if( wrap_around || i < n_points-2 )
                get_direction( n_points, points, i+1, wrap_around, &dir );
        }
    }

    FREE1( status, x );

    if( status == OK )
        FREE1( status, y );
}

private  void   fill_in_ellipse_points( tube_points, tube_normals, centre,
                                        n_around, x, y,
                                        hor, vert )
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
        SCALE_VECTOR( h, *hor, x[i] );
        SCALE_VECTOR( v, *vert, y[i] );

        ADD_VECTORS( offset, h, v );

        ADD_POINT_VECTOR( tube_points[i], *centre, offset );
        NORMALIZE_VECTOR( tube_normals[i], offset );
    }
}
