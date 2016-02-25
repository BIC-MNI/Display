/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif


#include  <display.h>
#include  <bicpl/marching_cube_data.h>

#define  INVALID_ID   -1

typedef  struct
{
    VIO_BOOL   checked;
    int       id;
} edge_point_info;

static  int  extract_polygons(
    VIO_Volume                      volume,
    surface_extraction_struct   *surface_extraction,
    VIO_Real                        corner_values[2][2][2],
    int                         voxel_index[],
    VIO_BOOL                     first_voxel,
    int                         n_polys,
    int                         sizes[],
    voxel_point_type            points_list[] );
static  void  add_point_id_to_relevant_edges(
    int                 sizes[],
    voxel_point_type    *edge_info,
    int                 pt_index[],
    int                 pt_id,
    Point_classes       pt_class,
    edge_point_info     edge_point_list[2][2][2][VIO_N_DIMENSIONS],
    hash_table_struct   *edge_points );
static  int  add_polygon_to_list(
    VIO_Volume                      volume,
    surface_extraction_struct   *surface_extraction,
    VIO_Real                        corner_values[2][2][2],
    int                         voxel_index[],
    int                         size,
    voxel_point_type            points_list[],
    edge_point_info             edge_point_list[2][2][2][VIO_N_DIMENSIONS] );
static  int   create_surface_point(
    VIO_Volume              volume,
    VIO_Real                corner_values[2][2][2],
    VIO_BOOL             binary_flag,
    VIO_Real                min_value,
    VIO_Real                max_value,
    polygons_struct     *polygons,
    int                 offset[],
    int                 voxel[],
    int                 edge_intersected,
    Point_classes       *pt_class );

static  VIO_BOOL  get_voxel_values(
    VIO_Volume                      volume,
    VIO_Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel_index[],
    VIO_Real                        corner_values[2][2][2] )
{
    VIO_BOOL     valid;
    VIO_Real        value, label;
    int         x, y, z, voxel[VIO_MAX_DIMENSIONS], n_invalid;

    if( !volume_is_alloced( volume ) )
        return( FALSE );

    n_invalid = 0;

    for_less( x, 0, 2 )
    {
        for_less( y, 0, 2 )
        {
            for_less( z, 0, 2 )
            {
                voxel[VIO_X] = voxel_index[VIO_X] + x;
                voxel[VIO_Y] = voxel_index[VIO_Y] + y;
                voxel[VIO_Z] = voxel_index[VIO_Z] + z;

                valid = TRUE;
                if( label_volume != NULL &&
                    surface_extraction->min_invalid_label <=
                    surface_extraction->max_invalid_label )
                {
                    if( label_volume == NULL ||
                        !volume_is_alloced( label_volume ) )
                        label = 0.0;
                    else
                        label = (VIO_Real) get_volume_label_data( label_volume,
                                                              voxel );

                    if( surface_extraction->min_invalid_label <= label &&
                        label <= surface_extraction->max_invalid_label ) 
                    {
                        valid = FALSE;
                        if( !Set_invalid_to_zero )
                            ++n_invalid;
                    }
                }

                if( !valid && Set_invalid_to_zero )
                    value = 0.0;
                else
                {
                    value = get_volume_real_value( volume, voxel[VIO_X], voxel[VIO_Y],
                                                   voxel[VIO_Z], 0, 0 );
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

  VIO_BOOL  voxel_contains_surface(
    VIO_Volume                      volume,
    VIO_Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel_index[] )
{
    VIO_BOOL                below, above, this_below;
    VIO_Real                   corner_values[2][2][2], val;
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
                val = corner_values[x][y][z];
                if( surface_extraction->binary_flag )
                {
                    this_below = (val < surface_extraction->min_value ||
                                  val > surface_extraction->max_value);
                }
                else
                {
                    if( val == surface_extraction->min_value )
                        return( TRUE );

                    this_below = (val < surface_extraction->min_value);
                }
                
                if( !this_below )
                {
                    if( below )
                        return( TRUE );
                    above = TRUE;
                }
                else
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

static  VIO_BOOL  extract_voxel_marching_cubes_surface(
    VIO_Volume                      volume,
    VIO_Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel[],
    VIO_BOOL                     first_voxel )
{
    voxel_point_type       *points_list;
    VIO_Real                   corner_values[2][2][2];
    int                    n_polys, n_nondegenerate_polys;
    int                    *sizes;

    if( !get_voxel_values( volume, label_volume, surface_extraction,
                           voxel, corner_values ) )
        return( FALSE );

    n_polys = compute_isosurface_in_voxel(
                       (Marching_cubes_methods) Marching_cubes_method,
                       voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z],
                       corner_values,
                       surface_extraction->binary_flag,
                       surface_extraction->min_value,
                       surface_extraction->max_value,
                       &sizes, &points_list );

    if( n_polys > 0 )
    {
        n_nondegenerate_polys = extract_polygons( volume, surface_extraction,
              corner_values, voxel, first_voxel, n_polys, sizes, points_list );
    }
    else
    {
        n_nondegenerate_polys = 0;
    }

    return( n_nondegenerate_polys > 0 );
}

  VIO_BOOL  extract_voxel_surface(
    VIO_Volume                      volume,
    VIO_Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel[],
    VIO_BOOL                     first_voxel )
{
    VIO_BOOL   found;

    if( surface_extraction->voxellate_flag )
    {
        found = extract_voxel_boundary_surface( volume,
                                                surface_extraction, voxel );
    }
    else
    {
        found = extract_voxel_marching_cubes_surface( volume, label_volume,
                                                      surface_extraction,
                                                      voxel, first_voxel );
    }

    return( found );
}

static  int  extract_polygons(
    VIO_Volume                      volume,
    surface_extraction_struct   *surface_extraction,
    VIO_Real                        corner_values[2][2][2],
    int                         voxel_index[],
    VIO_BOOL                     first_voxel,
    int                         n_polys,
    int                         sizes[],
    voxel_point_type            points_list[] )
{
    voxel_point_type       *pt, *poly_points;
    int                    corner_index[VIO_N_DIMENSIONS];
    int                    id, next_index;
    edge_point_info        edge_point_list[2][2][2][VIO_N_DIMENSIONS];
    int                    n_added_polys, poly, p;
    int                    x, y, z, axis;
    int                    point_ids[MAX_POINTS_PER_VOXEL_POLYGON];
    int                    volume_sizes[VIO_MAX_DIMENSIONS];
    VIO_BOOL                changed, connected;
    VIO_UCHAR          all_done_value, voxel_flags;

    for_less( x, 0, 2 )
    {
        for_less( y, 0, 2 )
        {
            for_less( z, 0, 2 )
            {
                for_less( axis, 0, VIO_N_DIMENSIONS )
                {
                    edge_point_list[x][y][z][axis].checked = FALSE;
                }
            }
        }
    }

    get_volume_sizes( volume, volume_sizes );

    voxel_flags = get_voxel_done_flag( surface_extraction->min_limits,
                                       surface_extraction->max_limits,
                                       surface_extraction->voxel_done_flags,
                                       voxel_index );

    n_added_polys = 0;
    all_done_value = (VIO_UCHAR) (VOXEL_COMPLETELY_DONE >>
                      (4 - n_polys));

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

                if( !edge_point_list[pt->coord[VIO_X]][pt->coord[VIO_Y]][pt->coord[VIO_Z]]
                                    [pt->edge_intersected].checked )
                {
                    edge_point_list[pt->coord[VIO_X]][pt->coord[VIO_Y]][pt->coord[VIO_Z]]
                                   [pt->edge_intersected].checked = TRUE;

                    corner_index[VIO_X] = voxel_index[VIO_X] + pt->coord[VIO_X];
                    corner_index[VIO_Y] = voxel_index[VIO_Y] + pt->coord[VIO_Y];
                    corner_index[VIO_Z] = voxel_index[VIO_Z] + pt->coord[VIO_Z];

                    if( !lookup_edge_point_id( volume_sizes,
                                    &surface_extraction->edge_points,
                                    corner_index[VIO_X],
                                    corner_index[VIO_Y],
                                    corner_index[VIO_Z],
                                    pt->edge_intersected, &id ) )
                    {
                        id = INVALID_ID;
                    }

                    edge_point_list[pt->coord[VIO_X]][pt->coord[VIO_Y]][pt->coord[VIO_Z]]
                                   [pt->edge_intersected].id = id;
                }

                point_ids[p] = edge_point_list[pt->coord[VIO_X]]
                                              [pt->coord[VIO_Y]]
                                              [pt->coord[VIO_Z]]
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
                                         corner_values,
                                         voxel_index, sizes[poly], poly_points,
                                         edge_point_list ) )
                {
                    ++n_added_polys;
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

    set_voxel_done_flag( surface_extraction->min_limits,
                         surface_extraction->max_limits,
                         surface_extraction->voxel_done_flags,
                         voxel_index, voxel_flags );

    return( n_added_polys > 0 );
}

static  int  add_polygon_to_list(
    VIO_Volume                      volume,
    surface_extraction_struct   *surface_extraction,
    VIO_Real                        corner_values[2][2][2],
    int                         voxel_index[],
    int                         size,
    voxel_point_type            points_list[],
    edge_point_info             edge_point_list[2][2][2][VIO_N_DIMENSIONS] )
{
    polygons_struct        *polygons;
    voxel_point_type       *pt;
    int                    corner_index[VIO_N_DIMENSIONS];
    int                    current_end, next_index, p, next_end, actual_size;
    int                    point_ids[MAX_POINTS_PER_VOXEL_POLYGON];
    int                    sizes[VIO_MAX_DIMENSIONS];
    VIO_BOOL                non_degenerate;
    Point_classes          pt_class;

    if( size < 3 )
    {
        HANDLE_INTERNAL_ERROR( "Size" );
    }

    get_volume_sizes( volume, sizes );

    polygons = surface_extraction->polygons;

    for_less( p, 0, size )
    {
        pt = &points_list[p];

        point_ids[p] = edge_point_list[pt->coord[VIO_X]]
                                      [pt->coord[VIO_Y]]
                                      [pt->coord[VIO_Z]]
                                      [pt->edge_intersected].id;

        if( point_ids[p] == INVALID_ID )
        {
            point_ids[p] = create_surface_point( volume, corner_values,
                                         surface_extraction->binary_flag,
                                         surface_extraction->min_value,
                                         surface_extraction->max_value,
                                         polygons, pt->coord, voxel_index,
                                         pt->edge_intersected, &pt_class );

            corner_index[VIO_X] = voxel_index[VIO_X] + pt->coord[VIO_X];
            corner_index[VIO_Y] = voxel_index[VIO_Y] + pt->coord[VIO_Y];
            corner_index[VIO_Z] = voxel_index[VIO_Z] + pt->coord[VIO_Z];

            add_point_id_to_relevant_edges( sizes, pt,
                           corner_index, point_ids[p], pt_class,
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

static  int   create_surface_point(
    VIO_Volume              volume,
    VIO_Real                corner_values[2][2][2],
    VIO_BOOL             binary_flag,
    VIO_Real                min_value,
    VIO_Real                max_value,
    polygons_struct     *polygons,
    int                 offset[],
    int                 voxel[],
    int                 edge_intersected,
    Point_classes       *pt_class )
{
    int        pt_index;
    VIO_Real   x_w, y_w, z_w;
    VIO_Real   *dx, *dy, *dz;
    VIO_Real   edge_point[VIO_MAX_DIMENSIONS];
    VIO_Point  point;
    VIO_Vector normal;
    VIO_Real   *ignored;
    int        sizes[VIO_MAX_DIMENSIONS];
    int        n_dimensions;
    int        n_values;
    int        i;

    n_dimensions = get_volume_n_dimensions( volume );
    get_volume_sizes( volume, sizes );

    n_values = 1;
    for (i = VIO_N_DIMENSIONS; i < n_dimensions; i++)
    {
        n_values *= sizes[i];
    }

    *pt_class = get_isosurface_point( corner_values, offset, edge_intersected,
                                      binary_flag, min_value, max_value,
                                      edge_point );

    edge_point[0] += (VIO_Real) voxel[0];
    edge_point[1] += (VIO_Real) voxel[1];
    edge_point[2] += (VIO_Real) voxel[2];

    if( *pt_class < 0 )
    {
        HANDLE_INTERNAL_ERROR( "create_surface_point" );
    }

    /* ------------------- compute point position ------------------- */

    convert_voxel_to_world( volume, edge_point, &x_w, &y_w, &z_w );
    fill_Point( point, x_w, y_w, z_w );

    /* --------------------- now get normal ---------------------- */

    ALLOC( dx, n_values );
    ALLOC( dy, n_values );
    ALLOC( dz, n_values );
    ALLOC( ignored, n_values );

    evaluate_volume_in_world( volume,
                              (VIO_Real) Point_x(point),
                              (VIO_Real) Point_y(point),
                              (VIO_Real) Point_z(point),
                              Volume_continuity, FALSE,
                              get_volume_real_min(volume),
                              ignored,
                              dx, dy, dz,
                              NULL, NULL, NULL, NULL, NULL, NULL );

    fill_Vector( normal, dx[0], dy[0], dz[0] );

    FREE( dx );
    FREE( dy );
    FREE( dz );
    FREE( ignored );

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

static  void  add_point_id_to_relevant_edges(
    int                 sizes[],
    voxel_point_type    *edge_info,
    int                 pt_index[],
    int                 pt_id,
    Point_classes       pt_class,
    edge_point_info     edge_point_list[2][2][2][VIO_N_DIMENSIONS],
    hash_table_struct   *edge_points )
{
    int   axis, stored_id;
    int   cache_pt[VIO_N_DIMENSIONS];
    int   corner[VIO_N_DIMENSIONS];

    if( pt_class == ON_FIRST_CORNER ||
        pt_class == ON_SECOND_CORNER )
    {
        corner[VIO_X] = pt_index[VIO_X];
        corner[VIO_Y] = pt_index[VIO_Y];
        corner[VIO_Z] = pt_index[VIO_Z];

        cache_pt[VIO_X] = edge_info->coord[VIO_X];
        cache_pt[VIO_Y] = edge_info->coord[VIO_Y];
        cache_pt[VIO_Z] = edge_info->coord[VIO_Z];

        if( pt_class == ON_SECOND_CORNER )
        {
            ++corner[edge_info->edge_intersected];
            ++cache_pt[edge_info->edge_intersected];
        }

        for_less( axis, 0, VIO_N_DIMENSIONS )
        {
            if( corner[axis] > 0 )
            {
                --corner[axis];
                if( !lookup_edge_point_id( sizes, edge_points,
                                           corner[VIO_X], corner[VIO_Y], corner[VIO_Z],
                                           axis, &stored_id ) )
                {
                    record_edge_point_id( sizes, edge_points,
                                          corner[VIO_X], corner[VIO_Y], corner[VIO_Z],
                                          axis, pt_id );
                }

                ++corner[axis];
            }

            if( cache_pt[axis] == 1 )
            {
                --cache_pt[axis];
                edge_point_list[cache_pt[VIO_X]]
                               [cache_pt[VIO_Y]]
                               [cache_pt[VIO_Z]]
                               [axis].checked = TRUE;
                edge_point_list[cache_pt[VIO_X]]
                               [cache_pt[VIO_Y]]
                               [cache_pt[VIO_Z]]
                               [axis].id = pt_id;
                ++cache_pt[axis];
            }

            if( !lookup_edge_point_id( sizes, edge_points,
                                       corner[VIO_X], corner[VIO_Y], corner[VIO_Z],
                                       axis, &stored_id ) )
            {
                record_edge_point_id( sizes, edge_points,
                                      corner[VIO_X], corner[VIO_Y], corner[VIO_Z],
                                      axis, pt_id );
            }

            edge_point_list[cache_pt[VIO_X]]
                           [cache_pt[VIO_Y]]
                           [cache_pt[VIO_Z]]
                           [axis].checked = TRUE;
            edge_point_list[cache_pt[VIO_X]]
                           [cache_pt[VIO_Y]]
                           [cache_pt[VIO_Z]]
                           [axis].id = pt_id;
        }
    }
    else
    {
        record_edge_point_id( sizes, edge_points,
                              pt_index[VIO_X], pt_index[VIO_Y], pt_index[VIO_Z],
                              edge_info->edge_intersected, pt_id );

        edge_point_list[edge_info->coord[VIO_X]]
                       [edge_info->coord[VIO_Y]]
                       [edge_info->coord[VIO_Z]]
                       [edge_info->edge_intersected].checked = TRUE;
        edge_point_list[edge_info->coord[VIO_X]]
                       [edge_info->coord[VIO_Y]]
                       [edge_info->coord[VIO_Z]]
                       [edge_info->edge_intersected].id = pt_id;
    }
}
