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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/surface_extraction/boundary_extraction.c,v 1.21 1996-04-19 17:38:52 david Exp $";
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
    polygons_struct             *polygons )
{
    int                  a1, a2, point_index, ind, dim;
    int                  n_indices, point_indices[4], x, y;
    int                  sizes[N_DIMENSIONS];
    voxel_index_struct   corner_index;
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
        corner_index.i[c] = (short) indices[c];
        voxel[c] = -1.0;
    }
    else
    {
        a1 = (c + 2) % N_DIMENSIONS;
        a2 = (c + 1) % N_DIMENSIONS;
        corner_index.i[c] = (short) (indices[c] + 1);
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
        corner_index.i[a1] = (short) x;
        corner_index.i[a2] = (short) y;
        if( !lookup_edge_point_id( sizes,
                                   &surface_extraction->edge_points,
                                   &corner_index, 0, &point_index ) )
        {
            point_index = polygons->n_points;

            record_edge_point_id( sizes, &surface_extraction->edge_points,
                                  &corner_index, 0, point_index );
            voxel[X] = (Real) corner_index.i[X] - 0.5;
            voxel[Y] = (Real) corner_index.i[Y] - 0.5;
            voxel[Z] = (Real) corner_index.i[Z] - 0.5;
            convert_voxel_to_world( volume, voxel, &xw, &yw, &zw );
            fill_Point( point, xw, yw, zw );

            get_vertex_normal( separations,
                               (int) corner_index.i[X] - indices[X],
                               (int) corner_index.i[Y] - indices[Y],
                               (int) corner_index.i[Z] - indices[Z],
                               inside_flags, valid_flags, voxel_normal );

            convert_voxel_vector_to_world( volume, voxel_normal,
                                           &xw, &yw, &zw );
            fill_Vector( normal, xw, yw, zw );
            NORMALIZE_VECTOR( normal, normal );

            ADD_ELEMENT_TO_ARRAY( polygons->points, polygons->n_points,
                                  point, DEFAULT_CHUNK_SIZE );
            --polygons->n_points;
            ADD_ELEMENT_TO_ARRAY( polygons->normals, polygons->n_points,
                                  normal, DEFAULT_CHUNK_SIZE );
        }

        point_indices[ind] = point_index;
        ++ind;
    }

    n_indices = NUMBER_INDICES( *polygons );

    ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices,
                          point_indices[0], DEFAULT_CHUNK_SIZE);
    ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices,
                          point_indices[1], DEFAULT_CHUNK_SIZE);
    ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices,
                          point_indices[3], DEFAULT_CHUNK_SIZE);
    ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices,
                          point_indices[2], DEFAULT_CHUNK_SIZE);

    ADD_ELEMENT_TO_ARRAY( polygons->end_indices, polygons->n_items,
                          n_indices, DEFAULT_CHUNK_SIZE );
}

public  BOOLEAN  extract_voxel_boundary_surface(
    Volume                      volume,
    Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel[] )
{
    int      dim, offset, sizes[N_DIMENSIONS], tx, ty, tz, dx, dy, dz;
    Real     value, label;
    BOOLEAN  contains_surface;
    BOOLEAN  inside_flags[3][3][3];
    BOOLEAN  valid_flags[3][3][3];

    get_volume_sizes( volume, sizes );

    for_inclusive( dx, -1, 1 )
    for_inclusive( dy, -1, 1 )
    for_inclusive( dz, -1, 1 )
    {
        tx = voxel[X] + dx;
        ty = voxel[Y] + dy;
        tz = voxel[Z] + dz;

        if( tx >= 0 && tx < sizes[X] &&
            ty >= 0 && ty < sizes[Y] &&
            tz >= 0 && tz < sizes[Z] )
        {
            value = get_volume_real_value( volume, tx, ty, tz, 0, 0 );

            inside_flags[dx+1][dy+1][dz+1] =
                    surface_extraction->min_value <= value &&
                    value <= surface_extraction->max_value;

            if( label_volume != NULL &&
                surface_extraction->min_invalid_label <=
                surface_extraction->max_invalid_label )
            {
                label = (Real) get_3D_volume_label_data( label_volume,
                                                         tx, ty, tz );
                valid_flags[dx+1][dy+1][dz+1] =
                          ( surface_extraction->min_invalid_label <= label &&
                            label <= surface_extraction->max_invalid_label );
            }
            else
                valid_flags[dx+1][dy+1][dz+1] = TRUE;
        }
        else
        {
            inside_flags[dx+1][dy+1][dz+1] = FALSE;
            valid_flags[dx+1][dy+1][dz+1] = TRUE;
        }
    }

    if( !inside_flags[1][1][1] )
        return( FALSE );

    contains_surface = FALSE;

    for_less( dim, 0, N_DIMENSIONS )
    {
        for( offset = -1;  offset <= 1;  offset += 2 )
        {
            if( face_is_boundary( inside_flags, valid_flags, dim, offset ) )
            {
                add_face( surface_extraction, volume, voxel, dim, offset,
                          inside_flags, valid_flags,
                          surface_extraction->polygons );
                contains_surface = TRUE;
            }
        }
    }

    return( contains_surface );
}
