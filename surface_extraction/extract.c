
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_marching_cubes.h>
#include  <def_splines.h>
#include  <def_bitlist.h>

#define  INVALID_ID   -1

typedef  struct
{
    Boolean   checked;
    int       id;
} edge_point_info;

public  Boolean  extract_voxel_surface( volume, surface_extraction,
                                        voxel_index, first_voxel )
    volume_struct               *volume;
    surface_extraction_struct   *surface_extraction;
    voxel_index_struct          *voxel_index;
    Boolean                     first_voxel;
{
    voxel_point_type       *points_list;
    Real                   corner_values[2][2][2];
    Boolean                active;
    Boolean                are_voxel_corners_active();
    int                    n_polys, n_nondegenerate_polys;
    int                    x, y, z, *sizes;
    int                    extract_polygons();

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
                    corner_values[x][y][z] = (Real) GET_VOLUME_DATA( *volume,
                                           voxel_index->i[X_AXIS]+x,
                                           voxel_index->i[Y_AXIS]+y,
                                           voxel_index->i[Z_AXIS]+z );
                }
            }
        }

        n_polys = compute_polygons_in_voxel(
                           (Marching_cubes_methods) Marching_cubes_method,
                           corner_values, surface_extraction->isovalue,
                           &sizes, &points_list );
    }

    if( active && n_polys > 0 )
    {
        n_nondegenerate_polys = extract_polygons( volume, surface_extraction,
                      voxel_index, first_voxel, n_polys, sizes, points_list );
    }
    else
    {
        n_nondegenerate_polys = 0;
    }

    return( n_nondegenerate_polys > 0 );
}

private  int  extract_polygons( volume, surface_extraction, voxel_index,
                                first_voxel, n_polys, sizes, points_list )
    volume_struct               *volume;
    surface_extraction_struct   *surface_extraction;
    voxel_index_struct          *voxel_index;
    Boolean                     first_voxel;
    int                         n_polys;
    int                         sizes[];
    voxel_point_type            points_list[];
{
    voxel_point_type       *pt, *poly_points;
    voxel_index_struct     corner_index;
    int                    id, next_index;
    edge_point_info        edge_point_list[2][2][2][N_DIMENSIONS];
    int                    n_nondegenerate_polys, poly, p;
    int                    x, y, z, axis;
    int                    point_ids[MAX_POINTS_PER_VOXEL_POLYGON];
    Boolean                changed, connected;
    unsigned_byte          all_done_value, voxel_flags;
    unsigned_byte          get_voxel_done_flag();
    Status                 set_voxel_done_flag();

    for_less( x, 0, 2 )
    {
        for_less( y, 0, 2 )
        {
            for_less( z, 0, 2 )
            {
                for_less( axis, 0, N_DIMENSIONS )
                {
                    edge_point_list[x][y][z][axis].checked = FALSE;
                }
            }
        }
    }

    voxel_flags = get_voxel_done_flag( volume,
                                       surface_extraction->voxel_done_flags,
                                       voxel_index );

    n_nondegenerate_polys = 0;
    all_done_value = (VOXEL_COMPLETELY_DONE >> (4 - n_polys));

    do
    {
        changed = FALSE;

        poly_points = points_list;
        for_less( poly, 0, n_polys )
        {
            if( (voxel_flags & (1 << poly)) == 0 )
            {
            for_less( p, 0, sizes[poly] )
            {
                pt = &poly_points[p];

                if( !edge_point_list[pt->coord[X_AXIS]]
                                    [pt->coord[Y_AXIS]]
                                    [pt->coord[Z_AXIS]]
                                    [pt->edge_intersected].checked )
                {
                    corner_index.i[X_AXIS] = voxel_index->i[X_AXIS] +
                                             pt->coord[X_AXIS];
                    corner_index.i[Y_AXIS] = voxel_index->i[Y_AXIS] +
                                             pt->coord[Y_AXIS];
                    corner_index.i[Z_AXIS] = voxel_index->i[Z_AXIS] +
                                             pt->coord[Z_AXIS];

                    if( !lookup_edge_point_id( volume,
                                    &surface_extraction->edge_points,
                                    &corner_index,
                                    pt->edge_intersected, &id ) )
                    {
                        id = INVALID_ID;
                    }

                    edge_point_list[pt->coord[X_AXIS]]
                                   [pt->coord[Y_AXIS]]
                                   [pt->coord[Z_AXIS]]
                                   [pt->edge_intersected].id = id;
                }

                point_ids[p] = edge_point_list[pt->coord[X_AXIS]]
                                              [pt->coord[Y_AXIS]]
                                              [pt->coord[Z_AXIS]]
                                              [pt->edge_intersected].id;
            }

            connected = first_voxel;

            for_less( p, 0, sizes[poly] )
            {
                next_index = (p + 1) % sizes[poly];

                if( point_ids[p] != INVALID_ID &&
                    point_ids[next_index] != INVALID_ID )
                {
                    connected = TRUE;
                    break;
                }
            }

            if( connected )
            {
                if( add_polygon_to_list( volume, surface_extraction,
                                         voxel_index,
                                         sizes[poly], poly_points,
                                         edge_point_list ) )
                {
                    ++n_nondegenerate_polys;
                }
                changed = TRUE;
                voxel_flags |= (1 << poly);
            }
            }
            poly_points += sizes[poly];
        }
    }
    while( changed && voxel_flags != all_done_value );

    if( voxel_flags == all_done_value )
    {
        voxel_flags = VOXEL_COMPLETELY_DONE;
    }

    (void) set_voxel_done_flag( volume, surface_extraction->voxel_done_flags,
                                voxel_index, voxel_flags );

    return( n_nondegenerate_polys > 0 );
}

private  int  add_polygon_to_list( volume, surface_extraction, voxel_index,
                                   size, points_list, edge_point_list )
    volume_struct               *volume;
    surface_extraction_struct   *surface_extraction;
    voxel_index_struct          *voxel_index;
    int                         size;
    voxel_point_type            points_list[];
    edge_point_info             edge_point_list[2][2][2][N_DIMENSIONS];
{
    Status                 status;
    Status                 add_point_id_to_relevant_edges();
    polygons_struct        *polygons;
    voxel_point_type       *pt;
    voxel_index_struct     corner_index;
    int                    current_end, next_index, p, next_end, actual_size;
    int                    point_ids[MAX_POINTS_PER_VOXEL_POLYGON];
    Boolean                non_degenerate;
    Point_classes          pt_class;

    status = OK;

    if( size < 3 )
    {
        HANDLE_INTERNAL_ERROR( "Size" );
    }

    polygons = surface_extraction->polygons;

    for_less( p, 0, size )
    {
        pt = &points_list[p];

        point_ids[p] = edge_point_list[pt->coord[X_AXIS]]
                                      [pt->coord[Y_AXIS]]
                                      [pt->coord[Z_AXIS]]
                                      [pt->edge_intersected].id;

        if( point_ids[p] == INVALID_ID )
        {
            corner_index.i[X_AXIS] = voxel_index->i[X_AXIS] + pt->coord[X_AXIS];
            corner_index.i[Y_AXIS] = voxel_index->i[Y_AXIS] + pt->coord[Y_AXIS];
            corner_index.i[Z_AXIS] = voxel_index->i[Z_AXIS] + pt->coord[Z_AXIS];

            point_ids[p] = create_point( volume, surface_extraction->isovalue,
                                         polygons, &corner_index,
                                         pt->edge_intersected, &pt_class );

            if( status == OK )
            {
                status = add_point_id_to_relevant_edges( volume, pt,
                           &corner_index, point_ids[p], pt_class,
                           edge_point_list, &surface_extraction->edge_points );
            }
        }
    }

    actual_size = 0;

    for_less( p, 0, size )
    {
        next_index = (p + 1) % size;
        if( point_ids[p] != point_ids[next_index] )
        {
            point_ids[actual_size] = point_ids[p];
            ++actual_size;
        }
    }

    non_degenerate = (actual_size >= 3);

    if( non_degenerate )
    {
        current_end = NUMBER_INDICES( *polygons );
        next_end = current_end + actual_size;

        CHECK_ALLOC1( status, polygons->indices, current_end,
                      next_end, int, DEFAULT_CHUNK_SIZE );

        if( status == OK )
        {
            for_less( p, 0, actual_size )
            {
                polygons->indices[current_end+p] = point_ids[p];
            }

            ADD_ELEMENT_TO_ARRAY( status, polygons->n_items,
                                  polygons->end_indices,
                                  next_end, int, DEFAULT_CHUNK_SIZE );
        }
    }

    return( non_degenerate );
}

private  int   create_point( volume, isovalue, polygons, voxel,
                             edge_intersected, pt_class )
    volume_struct       *volume;
    Real                isovalue;
    polygons_struct     *polygons;
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
    Real      x_thickness, y_thickness, z_thickness;
    void      get_volume_slice_thickness();

    corner[X_AXIS] = voxel->i[X_AXIS];
    corner[Y_AXIS] = voxel->i[Y_AXIS];
    corner[Z_AXIS] = voxel->i[Z_AXIS];

    val1 = isovalue - (Real) GET_VOLUME_DATA( *volume,
                             corner[X_AXIS], corner[Y_AXIS], corner[Z_AXIS] );

    ++corner[edge_intersected];

    val2 = isovalue - (Real) GET_VOLUME_DATA( *volume,
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

    c000 = (Real) GET_VOLUME_DATA( *volume, x-1, y-1, z-1 );
    c001 = (Real) GET_VOLUME_DATA( *volume, x-1, y-1, z+0 );
    c002 = (Real) GET_VOLUME_DATA( *volume, x-1, y-1, z+1 );
    c003 = (Real) GET_VOLUME_DATA( *volume, x-1, y-1, z+2 );

    c010 = (Real) GET_VOLUME_DATA( *volume, x-1, y+0, z-1 );
    c011 = (Real) GET_VOLUME_DATA( *volume, x-1, y+0, z+0 );
    c012 = (Real) GET_VOLUME_DATA( *volume, x-1, y+0, z+1 );
    c013 = (Real) GET_VOLUME_DATA( *volume, x-1, y+0, z+2 );

    c020 = (Real) GET_VOLUME_DATA( *volume, x-1, y+1, z-1 );
    c021 = (Real) GET_VOLUME_DATA( *volume, x-1, y+1, z+0 );
    c022 = (Real) GET_VOLUME_DATA( *volume, x-1, y+1, z+1 );
    c023 = (Real) GET_VOLUME_DATA( *volume, x-1, y+1, z+2 );

    c030 = (Real) GET_VOLUME_DATA( *volume, x-1, y+2, z-1 );
    c031 = (Real) GET_VOLUME_DATA( *volume, x-1, y+2, z+0 );
    c032 = (Real) GET_VOLUME_DATA( *volume, x-1, y+2, z+1 );
    c033 = (Real) GET_VOLUME_DATA( *volume, x-1, y+2, z+2 );


    c100 = (Real) GET_VOLUME_DATA( *volume, x+0, y-1, z-1 );
    c101 = (Real) GET_VOLUME_DATA( *volume, x+0, y-1, z+0 );
    c102 = (Real) GET_VOLUME_DATA( *volume, x+0, y-1, z+1 );
    c103 = (Real) GET_VOLUME_DATA( *volume, x+0, y-1, z+2 );

    c110 = (Real) GET_VOLUME_DATA( *volume, x+0, y+0, z-1 );
    c111 = (Real) GET_VOLUME_DATA( *volume, x+0, y+0, z+0 );
    c112 = (Real) GET_VOLUME_DATA( *volume, x+0, y+0, z+1 );
    c113 = (Real) GET_VOLUME_DATA( *volume, x+0, y+0, z+2 );

    c120 = (Real) GET_VOLUME_DATA( *volume, x+0, y+1, z-1 );
    c121 = (Real) GET_VOLUME_DATA( *volume, x+0, y+1, z+0 );
    c122 = (Real) GET_VOLUME_DATA( *volume, x+0, y+1, z+1 );
    c123 = (Real) GET_VOLUME_DATA( *volume, x+0, y+1, z+2 );

    c130 = (Real) GET_VOLUME_DATA( *volume, x+0, y+2, z-1 );
    c131 = (Real) GET_VOLUME_DATA( *volume, x+0, y+2, z+0 );
    c132 = (Real) GET_VOLUME_DATA( *volume, x+0, y+2, z+1 );
    c133 = (Real) GET_VOLUME_DATA( *volume, x+0, y+2, z+2 );


    c200 = (Real) GET_VOLUME_DATA( *volume, x+1, y-1, z-1 );
    c201 = (Real) GET_VOLUME_DATA( *volume, x+1, y-1, z+0 );
    c202 = (Real) GET_VOLUME_DATA( *volume, x+1, y-1, z+1 );
    c203 = (Real) GET_VOLUME_DATA( *volume, x+1, y-1, z+2 );

    c210 = (Real) GET_VOLUME_DATA( *volume, x+1, y+0, z-1 );
    c211 = (Real) GET_VOLUME_DATA( *volume, x+1, y+0, z+0 );
    c212 = (Real) GET_VOLUME_DATA( *volume, x+1, y+0, z+1 );
    c213 = (Real) GET_VOLUME_DATA( *volume, x+1, y+0, z+2 );

    c220 = (Real) GET_VOLUME_DATA( *volume, x+1, y+1, z-1 );
    c221 = (Real) GET_VOLUME_DATA( *volume, x+1, y+1, z+0 );
    c222 = (Real) GET_VOLUME_DATA( *volume, x+1, y+1, z+1 );
    c223 = (Real) GET_VOLUME_DATA( *volume, x+1, y+1, z+2 );

    c230 = (Real) GET_VOLUME_DATA( *volume, x+1, y+2, z-1 );
    c231 = (Real) GET_VOLUME_DATA( *volume, x+1, y+2, z+0 );
    c232 = (Real) GET_VOLUME_DATA( *volume, x+1, y+2, z+1 );
    c233 = (Real) GET_VOLUME_DATA( *volume, x+1, y+2, z+2 );


    c300 = (Real) GET_VOLUME_DATA( *volume, x+2, y-1, z-1 );
    c301 = (Real) GET_VOLUME_DATA( *volume, x+2, y-1, z+0 );
    c302 = (Real) GET_VOLUME_DATA( *volume, x+2, y-1, z+1 );
    c303 = (Real) GET_VOLUME_DATA( *volume, x+2, y-1, z+2 );

    c310 = (Real) GET_VOLUME_DATA( *volume, x+2, y+0, z-1 );
    c311 = (Real) GET_VOLUME_DATA( *volume, x+2, y+0, z+0 );
    c312 = (Real) GET_VOLUME_DATA( *volume, x+2, y+0, z+1 );
    c313 = (Real) GET_VOLUME_DATA( *volume, x+2, y+0, z+2 );

    c320 = (Real) GET_VOLUME_DATA( *volume, x+2, y+1, z-1 );
    c321 = (Real) GET_VOLUME_DATA( *volume, x+2, y+1, z+0 );
    c322 = (Real) GET_VOLUME_DATA( *volume, x+2, y+1, z+1 );
    c323 = (Real) GET_VOLUME_DATA( *volume, x+2, y+1, z+2 );

    c330 = (Real) GET_VOLUME_DATA( *volume, x+2, y+2, z-1 );
    c331 = (Real) GET_VOLUME_DATA( *volume, x+2, y+2, z+0 );
    c332 = (Real) GET_VOLUME_DATA( *volume, x+2, y+2, z+1 );
    c333 = (Real) GET_VOLUME_DATA( *volume, x+2, y+2, z+2 );

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

    get_volume_slice_thickness( volume, &x_thickness, &y_thickness,
                                &z_thickness );

    Vector_x(normal) /= x_thickness;
    Vector_y(normal) /= y_thickness;
    Vector_z(normal) /= z_thickness;


    NORMALIZE_VECTOR( normal, normal );

    if( Normal_towards_lower )
    {
        SCALE_VECTOR( normal, normal, -1.0 );
    }

    fill_Point( point, (Real) voxel->i[X_AXIS], (Real) voxel->i[Y_AXIS],
                       (Real) voxel->i[Z_AXIS] )

    Point_coord( point, edge_intersected ) += alpha;

    Point_x(point) *= x_thickness;
    Point_y(point) *= y_thickness;
    Point_z(point) *= z_thickness;

    CHECK_ALLOC1( status, polygons->points,
                  polygons->n_points, polygons->n_points+1,
                  Point, DEFAULT_CHUNK_SIZE );

    if( status == OK )
    {
        CHECK_ALLOC1( status, polygons->normals,
                      polygons->n_points, polygons->n_points+1,
                      Vector, DEFAULT_CHUNK_SIZE );
    }

    if( status == OK )
    {
        pt_index = polygons->n_points;
        polygons->points[pt_index] = point;
        polygons->normals[pt_index] = normal;
        ++polygons->n_points;
    }

    return( pt_index );
}

private  Status  add_point_id_to_relevant_edges( volume, edge_info, pt_index,
                                                 pt_id, pt_class,
                                                 edge_point_list, edge_points )
    volume_struct       *volume;
    voxel_point_type    *edge_info;
    voxel_index_struct  *pt_index;
    int                 pt_id;
    Point_classes       pt_class;
    edge_point_info     edge_point_list[2][2][2][N_DIMENSIONS];
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
                               [axis].checked = TRUE;
                edge_point_list[cache_pt[X_AXIS]]
                               [cache_pt[Y_AXIS]]
                               [cache_pt[Z_AXIS]]
                               [axis].id = pt_id;
                ++cache_pt[axis];
            }

            status = record_edge_point_id( volume, edge_points, &corner, axis,
                                           pt_id );

            edge_point_list[cache_pt[X_AXIS]]
                           [cache_pt[Y_AXIS]]
                           [cache_pt[Z_AXIS]]
                           [axis].checked = TRUE;
            edge_point_list[cache_pt[X_AXIS]]
                           [cache_pt[Y_AXIS]]
                           [cache_pt[Z_AXIS]]
                           [axis].id = pt_id;
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
                       [edge_info->edge_intersected].checked = TRUE;
        edge_point_list[edge_info->coord[X_AXIS]]
                       [edge_info->coord[Y_AXIS]]
                       [edge_info->coord[Z_AXIS]]
                       [edge_info->edge_intersected].id = pt_id;
    }

    return( status );
}
