
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_marching_cubes.h>
#include  <def_splines.h>
#include  <def_bitlist.h>

#define  INVALID_ID   -1

public  Boolean  extract_voxel_triangles( volume, surface_extraction,
                                          voxel_index )
    volume_struct               *volume;
    surface_extraction_struct   *surface_extraction;
    voxel_index_struct          *voxel_index;
{
    Status                 status;
    polygons_struct        *poly;
    triangle_point_type    *points_list, *pt;
    Real                   corner_values[2][2][2];
    Boolean                active;
    Status                 create_edge_point_list();
    int                    edge_point_list[2][2][2][N_DIMENSIONS];
    Boolean                are_voxel_corners_active();
    int                    n_tris, n_nondegenerate_tris, tri, p, next_end;
    int                    x, y, z, pt_index;
    int                    point_ids[3];

    active = are_voxel_corners_active( volume,
                                       voxel_index->i[X_AXIS],
                                       voxel_index->i[Y_AXIS],
                                       voxel_index->i[Z_AXIS] );

    if( active )
    {
        for_less( x, 0, 2 )
        {
            for_less( y, 0, 2 )
            {
                for_less( z, 0, 2 )
                {
                    corner_values[x][y][z] = (Real) ACCESS_VOLUME_DATA( *volume,
                                                     voxel_index->i[X_AXIS]+x,
                                                     voxel_index->i[Y_AXIS]+y,
                                                     voxel_index->i[Z_AXIS]+z );
                }
            }
        }

        n_tris = compute_isotriangles_in_voxel( corner_values,
                                                surface_extraction->isovalue,
                                                &points_list );
    }

    status = OK;

    n_nondegenerate_tris = 0;

    if( active && n_tris > 0 )
    {
        poly = surface_extraction->triangles;

        status = create_edge_point_list( volume,
                                         surface_extraction->isovalue,
                                         surface_extraction->triangles,
                                         &surface_extraction->edge_points,
                                         voxel_index,
                                         3 * n_tris, points_list,
                                         edge_point_list );
    }

    if( status == OK )
    {
        for_less( tri, 0, n_tris )
        {
            for_less( p, 0, 3 )
            {
                pt_index = 3 * tri + p;
                pt = &points_list[pt_index];

                point_ids[p] = edge_point_list[pt->coord[X_AXIS]]
                                              [pt->coord[Y_AXIS]]
                                              [pt->coord[Z_AXIS]]
                                              [pt->edge_intersected];

                if( point_ids[p] == INVALID_ID )
                {
                    HANDLE_INTERNAL_ERROR( "point_id invalid" );
                    status = ERROR;
                }
            }

            if( point_ids[0] != point_ids[1] &&
                point_ids[1] != point_ids[2] &&
                point_ids[2] != point_ids[0] )
            {
                next_end = NUMBER_INDICES( *poly ) + 3;

                if( status == OK )
                {
                    CHECK_ALLOC1( status, poly->indices,
                                  3 * poly->n_items,
                                  3 * poly->n_items + 3,
                                  int, DEFAULT_CHUNK_SIZE );
                    poly->indices[next_end-3] = point_ids[0];
                    poly->indices[next_end-2] = point_ids[1];
                    poly->indices[next_end-1] = point_ids[2];
                }

                if( status == OK )
                {
                    ADD_ELEMENT_TO_ARRAY( status, poly->n_items,
                                          poly->end_indices,
                                          next_end, int, DEFAULT_CHUNK_SIZE );
                }

                ++n_nondegenerate_tris;
            }
        }
    }

    return( n_nondegenerate_tris > 0 );
}

private  int   create_point( volume, isovalue, tris, voxel, edge_intersected,
                             pt_class )
    volume_struct       *volume;
    Real                isovalue;
    polygons_struct     *tris;
    voxel_index_struct  *voxel;
    int                 edge_intersected;
    Point_classes       *pt_class;
{
    Status    status;
    int       i, x, y, z, pt_index;
    Real      u_bar[N_DIMENSIONS], dx, dy, dz;
    Real      alpha1, alpha2, val1, val2, val, alpha;
    Point     point;
    Vector    normal;
    int       corner[N_DIMENSIONS];
    Real      c000, c001, c002, c003, c010, c011, c012, c013;
    Real      c020, c021, c022, c023, c030, c031, c032, c033;
    Real      c100, c101, c102, c103, c110, c111, c112, c113;
    Real      c120, c121, c122, c123, c130, c131, c132, c133;
    Real      c200, c201, c202, c203, c210, c211, c212, c213;
    Real      c220, c221, c222, c223, c230, c231, c232, c233;
    Real      c300, c301, c302, c303, c310, c311, c312, c313;
    Real      c320, c321, c322, c323, c330, c331, c332, c333;

    corner[X_AXIS] = voxel->i[X_AXIS];
    corner[Y_AXIS] = voxel->i[Y_AXIS];
    corner[Z_AXIS] = voxel->i[Z_AXIS];

    val1 = isovalue - (Real) ACCESS_VOLUME_DATA( *volume,
                             corner[X_AXIS], corner[Y_AXIS], corner[Z_AXIS] );

    ++corner[edge_intersected];

    val2 = isovalue - (Real) ACCESS_VOLUME_DATA( *volume,
                             corner[X_AXIS], corner[Y_AXIS], corner[Z_AXIS] );

    if( val1 == 0.0 )
    {
        *pt_class = ON_FIRST_CORNER;
        alpha = 0.0;
    }
    else if( val2 == 0.0 )
    {
        *pt_class = ON_SECOND_CORNER;
        alpha = 1.0;
    }
    else
    {
        *pt_class = ON_EDGE;
        alpha = val1 / (val1 - val2);
    }

    x = voxel->i[X_AXIS];
    y = voxel->i[Y_AXIS];
    z = voxel->i[Z_AXIS];

    c000 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z-1 );
    c001 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+0 );
    c002 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+1 );
    c003 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y-1, z+2 );

    c010 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z-1 );
    c011 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+0 );
    c012 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+1 );
    c013 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+0, z+2 );

    c020 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z-1 );
    c021 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+0 );
    c022 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+1 );
    c023 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+1, z+2 );

    c030 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z-1 );
    c031 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+0 );
    c032 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+1 );
    c033 = (Real) ACCESS_VOLUME_DATA( *volume, x-1, y+2, z+2 );


    c100 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y-1, z-1 );
    c101 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y-1, z+0 );
    c102 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y-1, z+1 );
    c103 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y-1, z+2 );

    c110 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+0, z-1 );
    c111 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+0, z+0 );
    c112 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+0, z+1 );
    c113 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+0, z+2 );

    c120 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+1, z-1 );
    c121 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+1, z+0 );
    c122 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+1, z+1 );
    c123 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+1, z+2 );

    c130 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+2, z-1 );
    c131 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+2, z+0 );
    c132 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+2, z+1 );
    c133 = (Real) ACCESS_VOLUME_DATA( *volume, x+0, y+2, z+2 );


    c200 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y-1, z-1 );
    c201 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y-1, z+0 );
    c202 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y-1, z+1 );
    c203 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y-1, z+2 );

    c210 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+0, z-1 );
    c211 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+0, z+0 );
    c212 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+0, z+1 );
    c213 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+0, z+2 );

    c220 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+1, z-1 );
    c221 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+1, z+0 );
    c222 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+1, z+1 );
    c223 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+1, z+2 );

    c230 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+2, z-1 );
    c231 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+2, z+0 );
    c232 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+2, z+1 );
    c233 = (Real) ACCESS_VOLUME_DATA( *volume, x+1, y+2, z+2 );


    c300 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y-1, z-1 );
    c301 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y-1, z+0 );
    c302 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y-1, z+1 );
    c303 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y-1, z+2 );

    c310 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+0, z-1 );
    c311 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+0, z+0 );
    c312 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+0, z+1 );
    c313 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+0, z+2 );

    c320 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+1, z-1 );
    c321 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+1, z+0 );
    c322 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+1, z+1 );
    c323 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+1, z+2 );

    c330 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+2, z-1 );
    c331 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+2, z+0 );
    c332 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+2, z+1 );
    c333 = (Real) ACCESS_VOLUME_DATA( *volume, x+2, y+2, z+2 );

    u_bar[X_AXIS] = 0.0;
    u_bar[Y_AXIS] = 0.0;
    u_bar[Z_AXIS] = 0.0;

    alpha1 = 0.0;
    alpha2 = 1.0;

    for_less( i, 0, Max_surface_refinements )
    {
        u_bar[edge_intersected] = alpha;

        CUBIC_TRIVAR( c, u_bar[X_AXIS], u_bar[Y_AXIS], u_bar[Z_AXIS],
                      val );

        val = isovalue - val;

/*
        if( (alpha == 0.0 && val != val1) ||
            (alpha == 1.0 && val != val2) )
        {
            HANDLE_INTERNAL_ERROR( "Surface refinement val\n" );
        }
*/

        if( (val1 <= 0.0 && val <= 0.0) ||
            (val1 >= 0.0 && val >= 0.0) )
        {
            val1 = val;
            alpha1 = alpha;
        }
        else
        {
            val2 = val;
            alpha2 = alpha;
        }

        if( val1 != val2 )
        {
            alpha = alpha1 + val1 / (val1 - val2) * (alpha2 - alpha1);
        }
        else if( val != 0.0 )
        {
            HANDLE_INTERNAL_ERROR( "Surface refinement\n" );
        }

        if( alpha < 0.0 || alpha > 1.0 )
        {
            HANDLE_INTERNAL_ERROR( "Surface refinement alpha\n" );
        }

        if( ABS(val) < Max_surface_error )
        {
            break;
        }
    }

    u_bar[edge_intersected] = alpha;
  
    CUBIC_TRIVAR_DERIV( c, u_bar[X_AXIS], u_bar[Y_AXIS], u_bar[Z_AXIS],
                        dx, dy, dz );

    fill_Vector( normal, dx, dy, dz );

    Vector_x(normal) /= volume->slice_thickness[X_AXIS];
    Vector_y(normal) /= volume->slice_thickness[Y_AXIS];
    Vector_z(normal) /= volume->slice_thickness[Z_AXIS];


    NORMALIZE_VECTOR( normal, normal );

    if( Normal_towards_lower )
    {
        SCALE_VECTOR( normal, normal, -1.0 );
    }

    fill_Point( point, (Real) voxel->i[X_AXIS], (Real) voxel->i[Y_AXIS],
                       (Real) voxel->i[Z_AXIS] )

    Point_coord( point, edge_intersected ) += alpha;

    Point_x(point) *= volume->slice_thickness[X_AXIS];
    Point_y(point) *= volume->slice_thickness[Y_AXIS];
    Point_z(point) *= volume->slice_thickness[Z_AXIS];

    CHECK_ALLOC1( status, tris->points, tris->n_points, tris->n_points+1,
                  Point, DEFAULT_CHUNK_SIZE );

    if( status == OK )
    {
        CHECK_ALLOC1( status, tris->normals,
                      tris->n_points, tris->n_points+1,
                      Vector, DEFAULT_CHUNK_SIZE );
    }

    if( status == OK )
    {
        pt_index = tris->n_points;
        tris->points[pt_index] = point;
        tris->normals[pt_index] = normal;
        ++tris->n_points;
    }

    return( pt_index );
}

private  Status  create_edge_point_list( volume, isovalue, triangles,
                                         edge_points, voxel_index,
                                         n_points, points_list,
                                         edge_point_list )
    volume_struct          *volume;
    Real                   isovalue;
    polygons_struct        *triangles;
    hash_table_struct      *edge_points;
    voxel_index_struct     *voxel_index;
    int                    n_points;
    triangle_point_type    *points_list;
    int                    edge_point_list[2][2][2][N_DIMENSIONS];
{
    Status                 status;
    Status                 add_point_id_to_relevant_edges();
    int                    x, y, z, axis, p, id;
    triangle_point_type    *pt;
    voxel_index_struct     corner_index;
    Boolean                lookup_edge_point_id();
    Point_classes          pt_class;

    status = OK;

    for_less( x, 0, 2 )
    {
        for_less( y, 0, 2 )
        {
            for_less( z, 0, 2 )
            {
                for_less( axis, 0, N_DIMENSIONS )
                {
                    edge_point_list[x][y][z][axis] = INVALID_ID;
                }
            }
        }
    }

    for_less( p, 0, n_points )
    {
        pt = &points_list[p];

        if( edge_point_list[pt->coord[X_AXIS]]
                           [pt->coord[Y_AXIS]]
                           [pt->coord[Z_AXIS]]
                           [pt->edge_intersected] == INVALID_ID )
        {
            corner_index.i[X_AXIS] = voxel_index->i[X_AXIS] + pt->coord[X_AXIS];
            corner_index.i[Y_AXIS] = voxel_index->i[Y_AXIS] + pt->coord[Y_AXIS];
            corner_index.i[Z_AXIS] = voxel_index->i[Z_AXIS] + pt->coord[Z_AXIS];

            if( lookup_edge_point_id( volume, edge_points,
                                       &corner_index,
                                       pt->edge_intersected, &id ) )
            {
                edge_point_list[pt->coord[X_AXIS]]
                               [pt->coord[Y_AXIS]]
                               [pt->coord[Z_AXIS]]
                               [pt->edge_intersected] = id;
            }
            else
            {
                id = create_point( volume, isovalue, triangles, &corner_index,
                                   pt->edge_intersected, &pt_class );

                status = add_point_id_to_relevant_edges( volume, pt,
                           &corner_index, id, pt_class,
                           edge_point_list, edge_points );
            }
        }
    }

    return( status );
}

private  Status  add_point_id_to_relevant_edges( volume, edge_info, pt_index,
                                                 pt_id, pt_class,
                                                 edge_point_list, edge_points )
    volume_struct       *volume;
    triangle_point_type *edge_info;
    voxel_index_struct  *pt_index;
    int                 pt_id;
    Point_classes       pt_class;
    int                 edge_point_list[2][2][2][N_DIMENSIONS];
    hash_table_struct   *edge_points;
{
    Status              status;
    Status              record_edge_point_id();
    int                 axis;
    int                 cache_pt[N_DIMENSIONS];
    voxel_index_struct  corner;

    status = OK;

    if( pt_class == ON_FIRST_CORNER ||
        pt_class == ON_SECOND_CORNER )
    {
        corner = *pt_index;

        cache_pt[X_AXIS] = edge_info->coord[X_AXIS];
        cache_pt[Y_AXIS] = edge_info->coord[Y_AXIS];
        cache_pt[Z_AXIS] = edge_info->coord[Z_AXIS];

        if( pt_class == ON_SECOND_CORNER )
        {
            ++corner.i[edge_info->edge_intersected];
            ++cache_pt[edge_info->edge_intersected];
        }

        for_less( axis, 0, N_DIMENSIONS )
        {
            if( corner.i[axis] > 0 )
            {
                --corner.i[axis];
                status = record_edge_point_id( volume, edge_points, &corner,
                                               axis, pt_id );
                ++corner.i[axis];
            }

            if( cache_pt[axis] == 1 )
            {
                --cache_pt[axis];
                edge_point_list[cache_pt[X_AXIS]]
                               [cache_pt[Y_AXIS]]
                               [cache_pt[Z_AXIS]]
                               [axis] = pt_id;
                ++cache_pt[axis];
            }

            status = record_edge_point_id( volume, edge_points, &corner, axis,
                                           pt_id );

            edge_point_list[cache_pt[X_AXIS]]
                           [cache_pt[Y_AXIS]]
                           [cache_pt[Z_AXIS]]
                           [axis] = pt_id;
        }
    }
    else
    {
        status = record_edge_point_id( volume, edge_points,
                                       pt_index,
                                       edge_info->edge_intersected, pt_id );

        edge_point_list[edge_info->coord[X_AXIS]]
                       [edge_info->coord[Y_AXIS]]
                       [edge_info->coord[Z_AXIS]]
                       [edge_info->edge_intersected] = pt_id;
    }

    return( status );
}
