#include  <def_display.h>

private  int  n_evals;

private  void  recursive_scan(
    surface_rep_struct  *surface_rep,
    double              descriptors[],
    Volume              volume,
    double              parameters[],
    Real                u1,
    Real                u2,
    Real                v1,
    Real                v2,
    Point               *centre_voxel,
    Real                max_voxel_dist,
    Real                max_parametric_dist );
private  Boolean  should_subdivide(
    Real   du_parametric,
    Real   dv_parametric,
    Point  points[],
    Real   max_voxel_dist,
    Real   max_parametric_dist );
private  void  convert_surface_uv_to_voxel(
    surface_rep_struct  *surface_rep,
    double              descriptors[],
    Volume              volume,
    double              parameters[],
    Real                u,
    Real                v,
    Point               *point );
void  label_voxel_point(
    Volume         volume,
    Point          *point );

public  void  scan_to_voxels(
    surface_rep_struct  *surface_rep,
    double              descriptors[],
    Volume              volume,
    double              parameters[],
    Real                max_voxel_distance,
    Real                max_parametric_scan_distance )
{
    Point  centre_voxel;

    n_evals = 0;

    convert_surface_uv_to_voxel( surface_rep, descriptors,
                                 volume, parameters, 0.5, 0.5, &centre_voxel );

    recursive_scan( surface_rep, descriptors,
                    volume, parameters, 0.0, 1.0, 0.0, 1.0, &centre_voxel,
                    max_voxel_distance, max_parametric_scan_distance );

    print( "%d evaluations", n_evals );
    (void) flush_file( stdout );
}

private  void  recursive_scan(
    surface_rep_struct  *surface_rep,
    double              descriptors[],
    Volume              volume,
    double              parameters[],
    Real                u1,
    Real                u2,
    Real                v1,
    Real                v2,
    Point               *centre_voxel,
    Real                max_voxel_dist,
    Real                max_parametric_dist )
{
    int              i;
    Real             u14, u34, v14, v34;
    Real             u_middle, v_middle;
    Point            voxels[4];

    label_voxel_point( volume, centre_voxel );

    u14 = INTERPOLATE( 0.25, u1, u2 );
    u34 = INTERPOLATE( 0.75, u1, u2 );
    v14 = INTERPOLATE( 0.25, v1, v2 );
    v34 = INTERPOLATE( 0.75, v1, v2 );

    n_evals += 4;
    convert_surface_uv_to_voxel( surface_rep, descriptors,
                                 volume, parameters, u14, v14, &voxels[0] );
    convert_surface_uv_to_voxel( surface_rep, descriptors,
                                 volume, parameters, u34, v14, &voxels[1] );
    convert_surface_uv_to_voxel( surface_rep, descriptors,
                                 volume, parameters, u14, v34, &voxels[2] );
    convert_surface_uv_to_voxel( surface_rep, descriptors,
                                 volume, parameters, u34, v34, &voxels[3] );

    if( should_subdivide( u2 - u1, v2 - v1, voxels, max_voxel_dist,
                          max_parametric_dist ) )
    {
        u_middle = (u1 + u2) / 2.0;
        v_middle = (v1 + v2) / 2.0;

        recursive_scan( surface_rep, descriptors, volume, parameters,
                        u1, u_middle, v1, v_middle, &voxels[0],
                        max_voxel_dist, max_parametric_dist );
        recursive_scan( surface_rep, descriptors, volume, parameters,
                        u_middle, u2, v1, v_middle, &voxels[1],
                        max_voxel_dist, max_parametric_dist );
        recursive_scan( surface_rep, descriptors, volume, parameters,
                        u1, u_middle, v_middle, v2, &voxels[2],
                        max_voxel_dist, max_parametric_dist );
        recursive_scan( surface_rep, descriptors, volume, parameters,
                        u_middle, u2, v_middle, v2, &voxels[3],
                        max_voxel_dist, max_parametric_dist );
    }
    else
    {
        for_less( i, 0, 4 )
            label_voxel_point( volume, &voxels[i] );
    }
}

private  Boolean  should_subdivide(
    Real   du_parametric,
    Real   dv_parametric,
    Point  points[],
    Real   max_voxel_dist,
    Real   max_parametric_dist )
{
    Point    min_corner, max_corner;
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

private  void  convert_surface_uv_to_voxel(
    surface_rep_struct  *surface_rep,
    double              descriptors[],
    Volume              volume,
    double              parameters[],
    Real                u,
    Real                v,
    Point               *point )
{
    double   x, y, z;
    Real     x_w, y_w, z_w;

    surface_rep->evaluate_surface_at_uv( u, v, descriptors, parameters,
                                 &x, &y, &z,
                                 (double *) 0, (double *) 0, (double *) 0,
                                 (double *) 0, (double *) 0, (double *) 0,
                                 (double *) 0, (double *) 0, (double *) 0,
                                 (double *) 0, (double *) 0, (double *) 0 );

    convert_world_to_voxel( volume, (Real) x, (Real) y, (Real) z,
                            &x_w, &y_w, &z_w );
    fill_Point( *point, x_w, y_w, z_w );
}

void  label_voxel_point(
    Volume         volume,
    Point          *point )
{
    Real  pos[N_DIMENSIONS];

    pos[X] = Point_x(*point);
    pos[Y] = Point_y(*point);
    pos[Z] = Point_z(*point);
    if( voxel_is_within_volume( volume, pos ) )
    {
        set_voxel_label_flag( volume,
                              ROUND( Point_x(*point) ),
                              ROUND( Point_y(*point) ),
                              ROUND( Point_z(*point) ), TRUE );
    }
}
