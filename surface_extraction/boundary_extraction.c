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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/surface_extraction/boundary_extraction.c,v 1.24 1996-05-23 13:48:34 david Exp $";
#endif

#include  <display.h>

private  BOOLEAN  is_boundary(
    BOOLEAN         inside_flag,
    BOOLEAN         valid_flag,
    BOOLEAN         neigh_inside_flag,
    BOOLEAN         neigh_valid_flag )
{
    BOOLEAN  boundary_flag;
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

private  BOOLEAN  face_is_boundary(
    BOOLEAN         inside_flags[3][3][3],
    BOOLEAN         valid_flags[3][3][3],
    int             c,
    int             offset )
{
    int  neigh[N_DIMENSIONS];

    neigh[0] = 1;
    neigh[1] = 1;
    neigh[2] = 1;
    neigh[c] += offset;

    return( is_boundary( inside_flags[1][1][1], valid_flags[1][1][1],
                         inside_flags[neigh[X]][neigh[Y]][neigh[Z]],
                         valid_flags[neigh[X]][neigh[Y]][neigh[Z]] ) );
}

private  void   get_vertex_normal(
    Real                        separations[],
    int                         x,
    int                         y,
    int                         z,
    BOOLEAN                     inside_flags[3][3][3],
    BOOLEAN                     valid_flags[3][3][3],
    Real                        voxel_normal[] )
{
    int   ind[N_DIMENSIONS], corner[N_DIMENSIONS], neigh[N_DIMENSIONS];
    int   a1, a2, tx, ty, offset, dim;

    corner[X] = x;
    corner[Y] = y;
    corner[Z] = z;

    voxel_normal[X] = 0.0;
    voxel_normal[Y] = 0.0;
    voxel_normal[Z] = 0.0;

    for_less( dim, 0, N_DIMENSIONS )
    {
        a1 = (dim + 1) % N_DIMENSIONS;
        a2 = (dim + 2) % N_DIMENSIONS;

        for( offset = -1;  offset <= 1;  offset +=2  )
        for_less( tx, 0, 2 )
        for_less( ty, 0, 2 )
        {
            ind[a1] = corner[a1] + tx;
            ind[a2] = corner[a2] + ty;
            ind[dim] = corner[dim];
            neigh[X] = ind[X];
            neigh[Y] = ind[Y];
            neigh[Z] = ind[Z];
            if( offset == -1 )
                ++ind[dim];
            else
                ++neigh[dim];
            
            if( is_boundary( inside_flags[ind[X]][ind[Y]][ind[Z]],
                             valid_flags[ind[X]][ind[Y]][ind[Z]],
                             inside_flags[neigh[X]][neigh[Y]][neigh[Z]],
                             valid_flags[neigh[X]][neigh[Y]][neigh[Z]] ) )
            {
                if( neigh[dim] > ind[dim] )
                    voxel_normal[dim] += 1.0 / separations[dim];
                else
                    voxel_normal[dim] += -1.0 / separations[dim];
            }
        }
    }

    if( voxel_normal[X] == 0.0 &&     /* shouldn't happen often */
        voxel_normal[Y] == 0.0 &&
        voxel_normal[Z] == 0.0 )
    {
        voxel_normal[X] = 1.0;
        voxel_normal[Y] = 1.0;
        voxel_normal[Z] = 1.0;
    }
}

private  void  add_face(
    surface_extraction_struct   *surface_extraction,
    Volume                      volume,
    int                         indices[N_DIMENSIONS],
    int                         c,
    int                         offset,
    BOOLEAN                     inside_flags[3][3][3],
    BOOLEAN                     valid_flags[3][3][3],
    polygons_struct             *polygons,
    int                         poly_index )
{
    int                  a1, a2, point_index, ind, dim, start_index;
    int                  point_indices[4], x, y;
    int                  sizes[N_DIMENSIONS];
    int                  corner_index[N_DIMENSIONS];
    Real                 voxel[N_DIMENSIONS], xw, yw, zw;
    Real                 voxel_normal[N_DIMENSIONS], separations[N_DIMENSIONS];
    Point                point;
    Vector               normal;

    voxel[X] = 0.0;
    voxel[Y] = 0.0;
    voxel[Z] = 0.0;

    if( offset == -1 )
    {
        a1 = (c + 1) % N_DIMENSIONS;
        a2 = (c + 2) % N_DIMENSIONS;
        corner_index[c] = indices[c];
        voxel[c] = -1.0;
    }
    else
    {
        a1 = (c + 2) % N_DIMENSIONS;
        a2 = (c + 1) % N_DIMENSIONS;
        corner_index[c] = indices[c] + 1;
        voxel[c] = 1.0;
    }

    get_volume_sizes( volume, sizes );
    get_volume_separations( volume, separations );
    for_less( dim, 0, N_DIMENSIONS )
        separations[dim] = FABS( separations[dim] );

    ind = 0;
    for_less( x, indices[a1], indices[a1] + 2 )
    for_less( y, indices[a2], indices[a2] + 2 )
    {
        corner_index[a1] = x;
        corner_index[a2] = y;

        if( !lookup_edge_point_id( sizes,
                                   &surface_extraction->edge_points,
                                   corner_index[X],
                                   corner_index[Y],
                                   corner_index[Z],
                                   0, &point_index ) )
        {
            point_index = polygons->n_points;

            record_edge_point_id( sizes, &surface_extraction->edge_points,
                                  corner_index[X],
                                  corner_index[Y],
                                  corner_index[Z],
                                  0, point_index );
            voxel[X] = (Real) corner_index[X] - 0.5;
            voxel[Y] = (Real) corner_index[Y] - 0.5;
            voxel[Z] = (Real) corner_index[Z] - 0.5;
            convert_voxel_to_world( volume, voxel, &xw, &yw, &zw );
            fill_Point( point, xw, yw, zw );

            SET_ARRAY_SIZE( polygons->normals, polygons->n_points,
                            polygons->n_points+1, DEFAULT_CHUNK_SIZE );
            ADD_ELEMENT_TO_ARRAY( polygons->points, polygons->n_points,
                                  point, DEFAULT_CHUNK_SIZE );
        }

        get_vertex_normal( separations,
                           corner_index[X] - indices[X],
                           corner_index[Y] - indices[Y],
                           corner_index[Z] - indices[Z],
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

private  void  get_inside_flags(
    Volume                      volume,
    Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel[],
    BOOLEAN                     inside_flags[3][3][3],
    BOOLEAN                     valid_flags[3][3][3] )
{
    int              sizes[N_DIMENSIONS];
    int              i, dx, dy, dz, tx, ty, tz;
    BOOLEAN          *valid_ptr, *inside_ptr;
    Real             values[3][3][3], value, label;
    Real             labels[3][3][3], *values_ptr, *labels_ptr;
    Real             min_value, max_value, min_label, max_label;

    get_volume_sizes( volume, sizes );

    if( label_volume != NULL && surface_extraction->min_invalid_label <=
        surface_extraction->max_invalid_label )
    {
        min_label = surface_extraction->min_invalid_label;
        max_label = surface_extraction->max_invalid_label;
    }
    else
    {
        label_volume = NULL;
    }

    if( voxel[X] <= 0 || voxel[X] >= sizes[X]-1 ||
        voxel[Y] <= 0 || voxel[Y] >= sizes[Y]-1 ||
        voxel[Z] <= 0 || voxel[Z] >= sizes[Z]-1 ||
        label_volume != NULL &&
        !volume_is_alloced( label_volume ) && !volume_is_cached(label_volume) )
    {
        for_inclusive( dx, -1, 1 )
        for_inclusive( dy, -1, 1 )
        for_inclusive( dz, -1, 1 )
        {
            tx = voxel[X] + dx;
            ty = voxel[Y] + dy;
            tz = voxel[Z] + dz;
            if( tx < 0 || tx >= sizes[X] ||
                ty < 0 || ty >= sizes[Y] ||
                tz < 0 || tz >= sizes[Z] )
            {
                values[dx+1][dy+1][dz+1] = surface_extraction->min_value - 1.0;
                labels[dx+1][dy+1][dz+1] = (Real) surface_extraction->
                                        min_invalid_label - 1.0;
            }
            else
            {
                values[dx+1][dy+1][dz+1] = get_volume_real_value( volume,
                                                tx, ty, tz, 0, 0 );
                if( label_volume != NULL )
                {
                    values[dx+1][dy+1][dz+1] = (Real) get_3D_volume_label_data(
                                  label_volume, tx, ty, tz );
                }
            }
        }
    }
    else
    {
        get_volume_value_hyperslab_3d( volume,
                                       voxel[X]-1, voxel[Y]-1, voxel[Z]-1,
                                       3, 3, 3, &values[0][0][0] );
        if( label_volume != NULL )
        {
            get_volume_value_hyperslab_3d( label_volume,
                                           voxel[X]-1, voxel[Y]-1, voxel[Z]-1,
                                           3, 3, 3, &labels[0][0][0] );
        }
    }

    min_value = surface_extraction->min_value;
    max_value = surface_extraction->max_value;

    values_ptr = &values[0][0][0];
    valid_ptr = &valid_flags[0][0][0];
    inside_ptr = &inside_flags[0][0][0];
    labels_ptr = &labels[0][0][0];

    for_less( i, 0, 8 )
    {
        value = *values_ptr++;

        *inside_ptr++ = (min_value <= value && value <= max_value);

        if( label_volume != NULL )
        {
            label = *labels_ptr++;
            *valid_ptr = label < min_label || label > max_label;
        }
        else
            *valid_ptr = TRUE;

        ++valid_ptr;
    }
}

public  BOOLEAN  extract_voxel_boundary_surface(
    Volume                      volume,
    Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel[] )
{
    int              dim, offset, sizes[N_DIMENSIONS];
    int              face_index, poly_index, size, start_index, p;
    BOOLEAN          modified, face_exists, should_exist;
    BOOLEAN          inside_flags[3][3][3];
    BOOLEAN          valid_flags[3][3][3];
    polygons_struct  *polygons;

    get_volume_sizes( volume, sizes );
    polygons = surface_extraction->polygons;

    get_inside_flags( volume, label_volume, surface_extraction, voxel,
                      inside_flags, valid_flags );

    modified = FALSE;

    for_less( dim, 0, N_DIMENSIONS )
    {
        for( offset = -1;  offset <= 1;  offset += 2 )
        {
            face_index = 2 * dim + (offset+1) / 2;
            face_exists = lookup_edge_point_id( sizes,
                                                &surface_extraction->faces_done,
                                                voxel[X], voxel[Y], voxel[Z],
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
                                      voxel[X], voxel[Y], voxel[Z],
                                      face_index, poly_index );

                modified = TRUE;
            }
            else if( !should_exist && face_exists )
            {
                remove_edge_point( sizes,
                                   &surface_extraction->faces_done,
                                   voxel[X], voxel[Y], voxel[Z],
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
