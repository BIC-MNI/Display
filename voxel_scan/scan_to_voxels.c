#include  <def_graphics.h>

int  n_evals;

public  void  scan_to_voxels( volume, parameters, max_voxel_distance,
                              max_parametric_scan_distance )
    volume_struct    *volume;
    double           parameters[];
    Real             max_voxel_distance;
    Real             max_parametric_scan_distance;
{
    void   recursive_scan();

    n_evals = 0;

    recursive_scan( volume, parameters, 0.0, 1.0, 0.0, 1.0,
                    max_voxel_distance, max_parametric_scan_distance );

    PRINT( "%d evaluations\n", n_evals );
}

private  void  recursive_scan( volume, parameters,
                               u1, u2, v1, v2, 
                               max_voxel_dist, max_parametric_dist )
    volume_struct    *volume;
    double           parameters[];
    Real             u1, u2;
    Real             v1, v2;
    Real             max_voxel_dist;
    Real             max_parametric_dist;
{
    int              i;
    Real             u14, u34, v14, v34;
    Real             u_middle, v_middle;
    Point            points[4];
    void             set_voxel_label_flag();
    void             convert_surface_uv_to_voxel();

    u14 = INTERPOLATE( 0.25, u1, u2 );
    u34 = INTERPOLATE( 0.75, u1, u2 );
    v14 = INTERPOLATE( 0.25, v1, v2 );
    v34 = INTERPOLATE( 0.75, v1, v2 );

    n_evals += 4;
    convert_surface_uv_to_voxel( volume, parameters, u14, v14, &points[0] );
    convert_surface_uv_to_voxel( volume, parameters, u34, v14, &points[1] );
    convert_surface_uv_to_voxel( volume, parameters, u14, v34, &points[2] );
    convert_surface_uv_to_voxel( volume, parameters, u34, v34, &points[3] );

    if( should_subdivide( u2 - u1, v2 - v1, points, max_voxel_dist,
                          max_parametric_dist ) )
    {
        u_middle = (u1 + u2) / 2.0;
        v_middle = (v1 + v2) / 2.0;

        recursive_scan( volume, parameters, u1, u_middle, v1, v_middle,
                        max_voxel_dist, max_parametric_dist );
        recursive_scan( volume, parameters, u_middle, u2, v1, v_middle,
                        max_voxel_dist, max_parametric_dist );
        recursive_scan( volume, parameters, u1, u_middle, v_middle, v2,
                        max_voxel_dist, max_parametric_dist );
        recursive_scan( volume, parameters, u_middle, u2, v_middle, v2,
                        max_voxel_dist, max_parametric_dist );
    }
    else
    {
        for_less( i, 0, 4 )
            set_voxel_label_flag( volume,
                                  ROUND( Point_x(points[i]) ),
                                  ROUND( Point_y(points[i]) ),
                                  ROUND( Point_z(points[i]) ), TRUE );
    }
}

private  Boolean  should_subdivide( du_parametric, dv_parametric, points,
                                    max_voxel_dist, max_parametric_dist )
    Point  points[];
    Real   du_parametric;
    Real   dv_parametric;
    Real   max_voxel_dist;
    Real   max_parametric_dist;
{
    Point    min_corner, max_corner;
    void     get_range_points();
    Boolean  subdivide;

    subdivide = (du_parametric > max_parametric_dist) ||
                       (dv_parametric > max_parametric_dist);

    if( !subdivide )
    {
        get_range_points( 4, points, &min_corner, &max_corner );

        subdivide = 
             (Point_x(max_corner)-Point_x(min_corner) > max_voxel_dist) ||
             (Point_y(max_corner)-Point_y(min_corner) > max_voxel_dist) ||
             (Point_z(max_corner)-Point_z(min_corner) > max_voxel_dist);
    }

    return( subdivide );
}

private  void  convert_surface_uv_to_voxel( volume, parameters, u, v, point )
    volume_struct  *volume;
    double         parameters[];
    Real           u;
    Real           v;
    Point          *point;
{
    double  x, y, z;
    void    evaluate_parametric_surface();
    void    convert_point_to_voxel();

    evaluate_parametric_surface( u, v, parameters, &x, &y, &z,
                                 (double *) 0, (double *) 0, (double *) 0,
                                 (double *) 0, (double *) 0, (double *) 0,
                                 (double *) 0, (double *) 0, (double *) 0,
                                 (double *) 0, (double *) 0, (double *) 0 );

    convert_point_to_voxel( volume, (Real) x, (Real) y, (Real) z,
                            &Point_x(*point), &Point_y(*point),
                            &Point_z(*point) );
}
