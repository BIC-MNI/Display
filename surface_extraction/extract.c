
#include  <display.h>
#include  <marching_cube_data.h>

#define  INVALID_ID   -1

typedef  struct
{
    BOOLEAN   checked;
    int       id;
} edge_point_info;

private  int  extract_polygons(
    Volume                      volume,
    surface_extraction_struct   *surface_extraction,
    voxel_index_struct          *voxel_index,
    BOOLEAN                     first_voxel,
    int                         n_polys,
    int                         sizes[],
    voxel_point_type            points_list[] );
private  void  add_point_id_to_relevant_edges(
    Volume              volume,
    voxel_point_type    *edge_info,
    voxel_index_struct  *pt_index,
    int                 pt_id,
    Point_classes       pt_class,
    edge_point_info     edge_point_list[2][2][2][N_DIMENSIONS],
    hash_table_struct   *edge_points );
private  int  add_polygon_to_list(
    Volume                      volume,
    surface_extraction_struct   *surface_extraction,
    voxel_index_struct          *voxel_index,
    int                         size,
    voxel_point_type            points_list[],
    edge_point_info             edge_point_list[2][2][2][N_DIMENSIONS] );
private  int   create_surface_point(
    Volume              volume,
    BOOLEAN             binary_flag,
    Real                min_value,
    Real                max_value,
    polygons_struct     *polygons,
    voxel_index_struct  *voxel,
    int                 edge_intersected,
    Point_classes       *pt_class );

private  BOOLEAN  get_voxel_values(
    Volume                      volume,
    Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel_index[],
    Real                        corner_values[2][2][2] )
{
    Real                   value, label;
    int                    x, y, z, voxel[MAX_DIMENSIONS], n_invalid;

    n_invalid = 0;

    for_less( x, 0, 2 )
    {
        for_less( y, 0, 2 )
        {
            for_less( z, 0, 2 )
            {
                voxel[X] = voxel_index[X] + x;
                voxel[Y] = voxel_index[Y] + y;
                voxel[Z] = voxel_index[Z] + z;

                value = get_volume_real_value( volume, voxel[X], voxel[Y],
                                               voxel[Z], 0, 0 );

                if( label_volume != NULL )
                {
                    label = (Real) get_volume_label_data( label_volume, voxel );
                    if( surface_extraction->valid_min_label <=
                        surface_extraction->valid_max_label )
                    {
                        if( label < surface_extraction->valid_min_label ||
                            label > surface_extraction->valid_max_label ) 
                            ++n_invalid;
                    }
                    else if( surface_extraction->valid_out_min_label <=
                             surface_extraction->valid_out_max_label )
                    {
                        if( label >= surface_extraction->valid_out_min_label ||
                            label <= surface_extraction->valid_out_max_label )
                            ++n_invalid;
                    }
                }

                corner_values[x][y][z] = value;
            }
        }
    }

    if( n_invalid == 0 )
        return( TRUE );
    else if( n_invalid == 8 )
        return( FALSE );
    else
        return( Voxel_validity_if_mixed );
}

public  BOOLEAN  voxel_contains_surface(
    Volume                      volume,
    Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel_index[] )
{
    BOOLEAN                below, above;
    Real                   corner_values[2][2][2];
    int                    x, y, z;

    if( !get_voxel_values( volume, label_volume, surface_extraction,
                           voxel_index, corner_values ) )
        return( FALSE );

    below = FALSE;
    above = FALSE;

    for_less( x, 0, 2 )
    {
        for_less( y, 0, 2 )
        {
            for_less( z, 0, 2 )
            {
                if( corner_values[x][y][z] >= surface_extraction->min_value )
                {
                    if( below )
                        return( TRUE );
                    above = TRUE;
                }
                else if( corner_values[x][y][z] <=
                         surface_extraction->max_value )
                {
                    if( above )
                        return( TRUE );
                    below = TRUE;
                }
            }
        }
    }

    return( FALSE );
}

public  BOOLEAN  extract_voxel_surface(
    Volume                      volume,
    Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    voxel_index_struct          *voxel_index,
    BOOLEAN                     first_voxel )
{
    voxel_point_type       *points_list;
    Real                   corner_values[2][2][2];
    int                    n_polys, n_nondegenerate_polys;
    int                    *sizes, voxel[N_DIMENSIONS];

    voxel[X] = voxel_index->i[X];
    voxel[Y] = voxel_index->i[Y];
    voxel[Z] = voxel_index->i[Z];

    if( !get_voxel_values( volume, label_volume, surface_extraction,
                           voxel, corner_values ) )
        return( FALSE );

    n_polys = compute_isosurface_in_voxel(
                       (Marching_cubes_methods) Marching_cubes_method,
                       corner_values,
                       surface_extraction->binary_flag,
                       surface_extraction->min_value,
                       surface_extraction->max_value,
                       &sizes, &points_list );

    if( n_polys > 0 )
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

private  int  extract_polygons(
    Volume                      volume,
    surface_extraction_struct   *surface_extraction,
    voxel_index_struct          *voxel_index,
    BOOLEAN                     first_voxel,
    int                         n_polys,
    int                         sizes[],
    voxel_point_type            points_list[] )
{
    voxel_point_type       *pt, *poly_points;
    voxel_index_struct     corner_index;
    int                    id, next_index;
    edge_point_info        edge_point_list[2][2][2][N_DIMENSIONS];
    int                    n_nondegenerate_polys, poly, p;
    int                    x, y, z, axis;
    int                    point_ids[MAX_POINTS_PER_VOXEL_POLYGON];
    BOOLEAN                changed, connected;
    unsigned_byte          all_done_value, voxel_flags;

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

                if( !edge_point_list[pt->coord[X]]
                                    [pt->coord[Y]]
                                    [pt->coord[Z]]
                                    [pt->edge_intersected].checked )
                {
                    corner_index.i[X] = voxel_index->i[X] +
                                             pt->coord[X];
                    corner_index.i[Y] = voxel_index->i[Y] +
                                             pt->coord[Y];
                    corner_index.i[Z] = voxel_index->i[Z] +
                                             pt->coord[Z];

                    if( !lookup_edge_point_id( volume,
                                    &surface_extraction->edge_points,
                                    &corner_index,
                                    pt->edge_intersected, &id ) )
                    {
                        id = INVALID_ID;
                    }

                    edge_point_list[pt->coord[X]]
                                   [pt->coord[Y]]
                                   [pt->coord[Z]]
                                   [pt->edge_intersected].id = id;
                }

                point_ids[p] = edge_point_list[pt->coord[X]]
                                              [pt->coord[Y]]
                                              [pt->coord[Z]]
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

    set_voxel_done_flag( volume, surface_extraction->voxel_done_flags,
                         voxel_index, voxel_flags );

    return( n_nondegenerate_polys > 0 );
}

private  int  add_polygon_to_list(
    Volume                      volume,
    surface_extraction_struct   *surface_extraction,
    voxel_index_struct          *voxel_index,
    int                         size,
    voxel_point_type            points_list[],
    edge_point_info             edge_point_list[2][2][2][N_DIMENSIONS] )
{
    polygons_struct        *polygons;
    voxel_point_type       *pt;
    voxel_index_struct     corner_index;
    int                    current_end, next_index, p, next_end, actual_size;
    int                    point_ids[MAX_POINTS_PER_VOXEL_POLYGON];
    BOOLEAN                non_degenerate;
    Point_classes          pt_class;

    if( size < 3 )
    {
        HANDLE_INTERNAL_ERROR( "Size" );
    }

    polygons = surface_extraction->polygons;

    for_less( p, 0, size )
    {
        pt = &points_list[p];

        point_ids[p] = edge_point_list[pt->coord[X]]
                                      [pt->coord[Y]]
                                      [pt->coord[Z]]
                                      [pt->edge_intersected].id;

        if( point_ids[p] == INVALID_ID )
        {
            corner_index.i[X] = voxel_index->i[X] + pt->coord[X];
            corner_index.i[Y] = voxel_index->i[Y] + pt->coord[Y];
            corner_index.i[Z] = voxel_index->i[Z] + pt->coord[Z];

            point_ids[p] = create_surface_point( volume,
                                         surface_extraction->binary_flag,
                                         surface_extraction->min_value,
                                         surface_extraction->max_value,
                                         polygons, &corner_index,
                                         pt->edge_intersected, &pt_class );

            add_point_id_to_relevant_edges( volume, pt,
                           &corner_index, point_ids[p], pt_class,
                           edge_point_list, &surface_extraction->edge_points );
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

        SET_ARRAY_SIZE( polygons->indices, current_end,
                        next_end, DEFAULT_CHUNK_SIZE );

        for_less( p, 0, actual_size )
            polygons->indices[current_end+p] = point_ids[p];

        ADD_ELEMENT_TO_ARRAY( polygons->end_indices, polygons->n_items,
                              next_end, DEFAULT_CHUNK_SIZE );
    }

    return( non_degenerate );
}

private  int   create_surface_point(
    Volume              volume,
    BOOLEAN             binary_flag,
    Real                min_value,
    Real                max_value,
    polygons_struct     *polygons,
    voxel_index_struct  *voxel,
    int                 edge_intersected,
    Point_classes       *pt_class )
{
    int       pt_index;
    Real      x_w, y_w, z_w;
    Real      dx, dy, dz;
    Real      val1, val2, voxel_pos[MAX_DIMENSIONS];
    Point     point;
    Vector    normal;
    int       corner[N_DIMENSIONS];
    Real      ignored;
    Point     point1, point2, iso_point;

    corner[X] = voxel->i[X];
    corner[Y] = voxel->i[Y];
    corner[Z] = voxel->i[Z];

    fill_Point( point1, (Real) corner[X], (Real) corner[Y], (Real) corner[Z] );
    val1 = get_volume_real_value( volume, corner[X], corner[Y], corner[Z], 0,0);

    ++corner[edge_intersected];

    fill_Point( point2, (Real) corner[X], (Real) corner[Y], (Real) corner[Z] );
    val2 = get_volume_real_value( volume, corner[X], corner[Y], corner[Z], 0,0);

    *pt_class = get_isosurface_point( &point1, val1, &point2, val2,
                                      binary_flag, min_value, max_value,
                                      &iso_point );

    if( *pt_class < 0 )
    {
        HANDLE_INTERNAL_ERROR( "create_surface_point" );
    }

    /* ------------------- compute point position ------------------- */

    voxel_pos[X] = Point_x(iso_point);
    voxel_pos[Y] = Point_y(iso_point);
    voxel_pos[Z] = Point_z(iso_point);

    convert_voxel_to_world( volume, voxel_pos, &x_w, &y_w, &z_w );
    fill_Point( point, x_w, y_w, z_w );

    /* --------------------- now get normal ---------------------- */

    evaluate_volume_in_world( volume,
                              Point_x(point), Point_y(point), Point_z(point),
                              Volume_continuity, FALSE,
                              get_volume_real_min(volume),
                              &ignored,
                              &dx, &dy, &dz,
                              NULL, NULL, NULL, NULL, NULL, NULL );

    fill_Vector( normal, dx, dy, dz );

    NORMALIZE_VECTOR( normal, normal );

    if( Normal_towards_lower )
    {
        SCALE_VECTOR( normal, normal, -1.0 );
    }

    /* ------------------- store point and normal  ------------------- */

    SET_ARRAY_SIZE( polygons->points,
                    polygons->n_points, polygons->n_points+1,
                    DEFAULT_CHUNK_SIZE );

    SET_ARRAY_SIZE( polygons->normals, polygons->n_points, polygons->n_points+1,
                    DEFAULT_CHUNK_SIZE );

    pt_index = polygons->n_points;
    polygons->points[pt_index] = point;
    polygons->normals[pt_index] = normal;
    ++polygons->n_points;

    return( pt_index );
}

private  void  add_point_id_to_relevant_edges(
    Volume              volume,
    voxel_point_type    *edge_info,
    voxel_index_struct  *pt_index,
    int                 pt_id,
    Point_classes       pt_class,
    edge_point_info     edge_point_list[2][2][2][N_DIMENSIONS],
    hash_table_struct   *edge_points )
{
    int                 axis;
    int                 cache_pt[N_DIMENSIONS];
    voxel_index_struct  corner;

    if( pt_class == ON_FIRST_CORNER ||
        pt_class == ON_SECOND_CORNER )
    {
        corner = *pt_index;

        cache_pt[X] = edge_info->coord[X];
        cache_pt[Y] = edge_info->coord[Y];
        cache_pt[Z] = edge_info->coord[Z];

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
                record_edge_point_id( volume, edge_points, &corner,
                                      axis, pt_id );
                ++corner.i[axis];
            }

            if( cache_pt[axis] == 1 )
            {
                --cache_pt[axis];
                edge_point_list[cache_pt[X]]
                               [cache_pt[Y]]
                               [cache_pt[Z]]
                               [axis].checked = TRUE;
                edge_point_list[cache_pt[X]]
                               [cache_pt[Y]]
                               [cache_pt[Z]]
                               [axis].id = pt_id;
                ++cache_pt[axis];
            }

            record_edge_point_id( volume, edge_points, &corner, axis, pt_id );

            edge_point_list[cache_pt[X]]
                           [cache_pt[Y]]
                           [cache_pt[Z]]
                           [axis].checked = TRUE;
            edge_point_list[cache_pt[X]]
                           [cache_pt[Y]]
                           [cache_pt[Z]]
                           [axis].id = pt_id;
        }
    }
    else
    {
        record_edge_point_id( volume, edge_points, pt_index,
                              edge_info->edge_intersected, pt_id );

        edge_point_list[edge_info->coord[X]]
                       [edge_info->coord[Y]]
                       [edge_info->coord[Z]]
                       [edge_info->edge_intersected].checked = TRUE;
        edge_point_list[edge_info->coord[X]]
                       [edge_info->coord[Y]]
                       [edge_info->coord[Z]]
                       [edge_info->edge_intersected].id = pt_id;
    }
}
