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

static  VIO_BOOL  is_boundary(
    VIO_BOOL         inside_flag,
    VIO_BOOL         valid_flag,
    VIO_BOOL         neigh_inside_flag,
    VIO_BOOL         neigh_valid_flag )
{
    VIO_BOOL  boundary_flag;
    int      n_invalid;

    boundary_flag = FALSE;

    n_invalid = 0;

    if( !valid_flag )
        ++n_invalid;
    if( !neigh_valid_flag )
        ++n_invalid;

    if( n_invalid == 2 ||
        (n_invalid == 1 && !Voxel_validity_if_mixed) )
    {
        boundary_flag =  FALSE;
    }
    else
    {
        boundary_flag = inside_flag && !neigh_inside_flag;
    }

    return( boundary_flag );
}

static  VIO_BOOL  face_is_boundary(
    VIO_BOOL         inside_flags[3][3][3],
    VIO_BOOL         valid_flags[3][3][3],
    int             c,
    int             offset )
{
    int  neigh[VIO_N_DIMENSIONS];

    neigh[0] = 1;
    neigh[1] = 1;
    neigh[2] = 1;
    neigh[c] += offset;

    return( is_boundary( inside_flags[1][1][1], valid_flags[1][1][1],
                         inside_flags[neigh[VIO_X]][neigh[VIO_Y]][neigh[VIO_Z]],
                         valid_flags[neigh[VIO_X]][neigh[VIO_Y]][neigh[VIO_Z]] ) );
}

static  void   get_vertex_normal(
    VIO_Real                        separations[],
    int                         x,
    int                         y,
    int                         z,
    VIO_BOOL                     inside_flags[3][3][3],
    VIO_BOOL                     valid_flags[3][3][3],
    VIO_Real                        voxel_normal[] )
{
    int   ind[VIO_N_DIMENSIONS], corner[VIO_N_DIMENSIONS], neigh[VIO_N_DIMENSIONS];
    int   a1, a2, tx, ty, offset, dim;

    corner[VIO_X] = x;
    corner[VIO_Y] = y;
    corner[VIO_Z] = z;

    voxel_normal[VIO_X] = 0.0;
    voxel_normal[VIO_Y] = 0.0;
    voxel_normal[VIO_Z] = 0.0;

    for_less( dim, 0, VIO_N_DIMENSIONS )
    {
        a1 = (dim + 1) % VIO_N_DIMENSIONS;
        a2 = (dim + 2) % VIO_N_DIMENSIONS;

        for( offset = -1;  offset <= 1;  offset +=2  )
        for_less( tx, 0, 2 )
        for_less( ty, 0, 2 )
        {
            ind[a1] = corner[a1] + tx;
            ind[a2] = corner[a2] + ty;
            ind[dim] = corner[dim];
            neigh[VIO_X] = ind[VIO_X];
            neigh[VIO_Y] = ind[VIO_Y];
            neigh[VIO_Z] = ind[VIO_Z];
            if( offset == -1 )
                ++ind[dim];
            else
                ++neigh[dim];
            
            if( is_boundary( inside_flags[ind[VIO_X]][ind[VIO_Y]][ind[VIO_Z]],
                             valid_flags[ind[VIO_X]][ind[VIO_Y]][ind[VIO_Z]],
                             inside_flags[neigh[VIO_X]][neigh[VIO_Y]][neigh[VIO_Z]],
                             valid_flags[neigh[VIO_X]][neigh[VIO_Y]][neigh[VIO_Z]] ) )
            {
                if( neigh[dim] > ind[dim] )
                    voxel_normal[dim] += 1.0 / separations[dim];
                else
                    voxel_normal[dim] += -1.0 / separations[dim];
            }
        }
    }

    if( voxel_normal[VIO_X] == 0.0 &&     /* shouldn't happen often */
        voxel_normal[VIO_Y] == 0.0 &&
        voxel_normal[VIO_Z] == 0.0 )
    {
        voxel_normal[VIO_X] = 1.0;
        voxel_normal[VIO_Y] = 1.0;
        voxel_normal[VIO_Z] = 1.0;
    }
}

static  void  add_face(
    surface_extraction_struct   *surface_extraction,
    VIO_Volume                      volume,
    int                         indices[VIO_N_DIMENSIONS],
    int                         c,
    int                         offset,
    VIO_BOOL                     inside_flags[3][3][3],
    VIO_BOOL                     valid_flags[3][3][3],
    polygons_struct             *polygons,
    int                         poly_index )
{
    int                  a1, a2, point_index, ind, dim, start_index;
    int                  point_indices[4], x, y;
    int                  sizes[VIO_N_DIMENSIONS];
    int                  corner_index[VIO_N_DIMENSIONS];
    VIO_Real                 voxel[VIO_N_DIMENSIONS], xw, yw, zw;
    VIO_Real                 voxel_normal[VIO_N_DIMENSIONS], separations[VIO_N_DIMENSIONS];
    VIO_Point                point;
    VIO_Vector               normal;

    if( offset == -1 )
    {
        a1 = (c + 1) % VIO_N_DIMENSIONS;
        a2 = (c + 2) % VIO_N_DIMENSIONS;
        corner_index[c] = indices[c];
    }
    else
    {
        a1 = (c + 2) % VIO_N_DIMENSIONS;
        a2 = (c + 1) % VIO_N_DIMENSIONS;
        corner_index[c] = indices[c] + 1;
    }

    get_volume_sizes( volume, sizes );
    get_volume_separations( volume, separations );
    for_less( dim, 0, VIO_N_DIMENSIONS )
        separations[dim] = VIO_FABS( separations[dim] );

    ind = 0;
    for_less( x, indices[a1], indices[a1] + 2 )
    for_less( y, indices[a2], indices[a2] + 2 )
    {
        corner_index[a1] = x;
        corner_index[a2] = y;

        if( !lookup_edge_point_id( sizes,
                                   &surface_extraction->edge_points,
                                   corner_index[VIO_X],
                                   corner_index[VIO_Y],
                                   corner_index[VIO_Z],
                                   0, &point_index ) )
        {
            point_index = polygons->n_points;

            record_edge_point_id( sizes, &surface_extraction->edge_points,
                                  corner_index[VIO_X],
                                  corner_index[VIO_Y],
                                  corner_index[VIO_Z],
                                  0, point_index );
            voxel[VIO_X] = (VIO_Real) corner_index[VIO_X] - 0.5;
            voxel[VIO_Y] = (VIO_Real) corner_index[VIO_Y] - 0.5;
            voxel[VIO_Z] = (VIO_Real) corner_index[VIO_Z] - 0.5;
            convert_voxel_to_world( volume, voxel, &xw, &yw, &zw );
            fill_Point( point, xw, yw, zw );

            SET_ARRAY_SIZE( polygons->normals, polygons->n_points,
                            polygons->n_points+1, DEFAULT_CHUNK_SIZE );
            ADD_ELEMENT_TO_ARRAY( polygons->points, polygons->n_points,
                                  point, DEFAULT_CHUNK_SIZE );
        }

        get_vertex_normal( separations,
                           corner_index[VIO_X] - indices[VIO_X],
                           corner_index[VIO_Y] - indices[VIO_Y],
                           corner_index[VIO_Z] - indices[VIO_Z],
                           inside_flags, valid_flags, voxel_normal );

        convert_voxel_vector_to_world( volume, voxel_normal, &xw, &yw, &zw );
        fill_Vector( normal, xw, yw, zw );
        NORMALIZE_VECTOR( normal, normal );

        polygons->normals[point_index] = normal;

        point_indices[ind] = point_index;
        ++ind;
    }

    if( poly_index == polygons->n_items )
    {
        start_index = NUMBER_INDICES( *polygons );
        ADD_ELEMENT_TO_ARRAY( polygons->end_indices, polygons->n_items,
                              start_index+4, DEFAULT_CHUNK_SIZE );

        SET_ARRAY_SIZE( polygons->indices, start_index, start_index+4,
                        DEFAULT_CHUNK_SIZE );
    }
    else
        start_index = POINT_INDEX( polygons->end_indices, poly_index, 0 );

    polygons->indices[start_index+0] = point_indices[0];
    polygons->indices[start_index+1] = point_indices[1];
    polygons->indices[start_index+2] = point_indices[3];
    polygons->indices[start_index+3] = point_indices[2];
}

static  void  get_inside_flags(
    surface_extraction_struct   *surf,
    int                         voxel[],
    VIO_BOOL                     inside_flags[3][3][3],
    VIO_BOOL                     valid_flags[3][3][3] )
{
    int   dx, dy, dz, x_off, y_off, z_off;

    x_off = voxel[VIO_X] - surf->min_block[VIO_X];
    y_off = voxel[VIO_Y] - surf->min_block[VIO_Y];
    z_off = voxel[VIO_Z] - surf->min_block[VIO_Z];

    for_less( dx, 0, 3 )
    for_less( dy, 0, 3 )
    for_less( dz, 0, 3 )
    {
        inside_flags[dx][dy][dz] = surf->inside_flags[dx+x_off]
                                                     [dy+y_off]
                                                     [dz+z_off];
        valid_flags[dx][dy][dz] = surf->valid_flags[dx+x_off]
                                                   [dy+y_off]
                                                   [dz+z_off];
    }
}

  void  read_voxellation_block(
    surface_extraction_struct   *surf )
{
    int              sizes[VIO_N_DIMENSIONS];
    int              dx, dy, dz, ind, nx, ny, nz;
    VIO_Real             values[(SURFACE_BLOCK_SIZE+2)*
                            (SURFACE_BLOCK_SIZE+2)*
                            (SURFACE_BLOCK_SIZE+2)];
    VIO_Real             labels[(SURFACE_BLOCK_SIZE+2)*
                            (SURFACE_BLOCK_SIZE+2)*
                            (SURFACE_BLOCK_SIZE+2)];
    VIO_Real             value, label;
    VIO_Real             min_value, max_value, min_label, max_label;
    VIO_Volume           volume, label_volume;

    volume = surf->volume;
    label_volume = surf->label_volume;

    get_volume_sizes( volume, sizes );

    if( label_volume != NULL &&
        surf->min_invalid_label <= surf->max_invalid_label )
    {
        min_label = surf->min_invalid_label;
        max_label = surf->max_invalid_label;
    }
    else
        label_volume = NULL;

    nx = surf->max_block[VIO_X] - surf->min_block[VIO_X] + 3;
    ny = surf->max_block[VIO_Y] - surf->min_block[VIO_Y] + 3;
    nz = surf->max_block[VIO_Z] - surf->min_block[VIO_Z] + 3;

    if( surf->min_block[VIO_X] <= 0 || surf->max_block[VIO_X] >= sizes[VIO_X]-1 ||
        surf->min_block[VIO_Y] <= 0 || surf->max_block[VIO_Y] >= sizes[VIO_Y]-1 ||
        surf->min_block[VIO_Z] <= 0 || surf->max_block[VIO_Z] >= sizes[VIO_Z]-1 ||
        label_volume != NULL &&
        !volume_is_alloced( label_volume ) && !volume_is_cached(label_volume) )
    {
        ind = 0;
        for_inclusive( dx, surf->min_block[VIO_X]-1, surf->max_block[VIO_X]+1 )
        for_inclusive( dy, surf->min_block[VIO_Y]-1, surf->max_block[VIO_Y]+1 )
        for_inclusive( dz, surf->min_block[VIO_Z]-1, surf->max_block[VIO_Z]+1 )
        {
            if( dx < 0 || dx >= sizes[VIO_X] ||
                dy < 0 || dy >= sizes[VIO_Y] ||
                dz < 0 || dz >= sizes[VIO_Z] )
            {
                values[ind] = surf->min_value - 1.0;
                labels[ind] = (VIO_Real) surf->min_invalid_label - 1.0;
            }
            else
            {
                values[ind] = get_volume_real_value( volume, dx, dy, dz, 0, 0 );
                if( label_volume != NULL )
                {
                    values[ind] = (VIO_Real) get_3D_volume_label_data( label_volume,
                                                               dx, dy, dz );
                }
            }
            ++ind;
        }
    }
    else
    {
        get_volume_value_hyperslab_3d( volume,
                                       surf->min_block[VIO_X]-1,
                                       surf->min_block[VIO_Y]-1,
                                       surf->min_block[VIO_Z]-1,
                                       nx, ny, nz, values );
        if( label_volume != NULL )
        {
            get_volume_value_hyperslab_3d( label_volume,
                                       surf->min_block[VIO_X]-1,
                                       surf->min_block[VIO_Y]-1,
                                       surf->min_block[VIO_Z]-1,
                                       nx, ny, nz, labels );
        }
    }

    min_value = surf->min_value;
    max_value = surf->max_value;

    ind = 0;
    for_less( dx, 0, nx )
    for_less( dy, 0, ny )
    for_less( dz, 0, nz )
    {
        value = values[ind];

        surf->inside_flags[dx][dy][dz] =
                                (min_value <= value && value <= max_value);

        if( label_volume != NULL )
        {
            label = labels[ind];
            surf->valid_flags[dx][dy][dz] =
                                 label < min_label || label > max_label;
        }
        else
            surf->valid_flags[dx][dy][dz] = TRUE;

        ++ind;
    }
}

  VIO_BOOL  extract_voxel_boundary_surface(
    VIO_Volume                      volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel[] )
{
    int              dim, offset, sizes[VIO_N_DIMENSIONS];
    int              face_index, poly_index, size, start_index, p;
    VIO_BOOL          modified, face_exists, should_exist;
    VIO_BOOL          inside_flags[3][3][3];
    VIO_BOOL          valid_flags[3][3][3];
    polygons_struct  *polygons;

    get_volume_sizes( volume, sizes );
    polygons = surface_extraction->polygons;

    get_inside_flags( surface_extraction, voxel, inside_flags, valid_flags );

    modified = FALSE;

    for_less( dim, 0, VIO_N_DIMENSIONS )
    {
        for( offset = -1;  offset <= 1;  offset += 2 )
        {
            face_index = 2 * dim + (offset+1) / 2;
            face_exists = lookup_edge_point_id( sizes,
                                                &surface_extraction->faces_done,
                                                voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z],
                                                face_index, &poly_index );
            should_exist = face_is_boundary( inside_flags, valid_flags, dim,
                                             offset );

            if( should_exist && !face_exists )
            {
                if( IS_QUEUE_EMPTY( surface_extraction->deleted_faces ) )
                    poly_index = polygons->n_items;
                else
                {
                    REMOVE_FROM_QUEUE( surface_extraction->deleted_faces,
                                       poly_index );
                }

                add_face( surface_extraction, volume, voxel, dim,
                          offset, inside_flags, valid_flags,
                          polygons, poly_index );

                record_edge_point_id( sizes,
                                      &surface_extraction->faces_done,
                                      voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z],
                                      face_index, poly_index );

                modified = TRUE;
            }
            else if( !should_exist && face_exists )
            {
                remove_edge_point( sizes,
                                   &surface_extraction->faces_done,
                                   voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z],
                                   face_index );

                INSERT_IN_QUEUE( surface_extraction->deleted_faces, poly_index);

                size = GET_OBJECT_SIZE( *polygons, poly_index );
                start_index = POINT_INDEX( polygons->end_indices, poly_index,0);
                for_less( p, 0, size )
                    polygons->indices[start_index+p] = 0;

                modified = TRUE;
            }
        }
    }

    return( modified );
}
