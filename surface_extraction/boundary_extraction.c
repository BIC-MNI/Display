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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/surface_extraction/boundary_extraction.c,v 1.20 1996-04-19 15:32:11 david Exp $";
#endif

#include  <display.h>

private  BOOLEAN  face_is_boundary(
    Volume          volume,
    Real            min_value,
    Real            max_value,
    Volume          label_volume,
    Real            min_invalid_label,
    Real            max_invalid_label,
    int             indices[N_DIMENSIONS],
    int             c,
    int             offset )
{
    BOOLEAN  neigh_inside;
    BOOLEAN  boundary_flag;
    Real     value, label;
    int      n_invalid, neigh_indices[N_DIMENSIONS];

    boundary_flag = FALSE;

    neigh_indices[0] = indices[0];
    neigh_indices[1] = indices[1];
    neigh_indices[2] = indices[2];
    neigh_indices[c] += offset;

    if( int_voxel_is_within_volume( volume, neigh_indices ) )
    {
        boundary_flag = TRUE;

        if( label_volume != NULL && min_invalid_label <= max_invalid_label )
        {
            n_invalid = 0;

            label = (Real) get_volume_label_data( label_volume, indices );
            if( min_invalid_label <= label && label <= max_invalid_label )
                ++n_invalid;

            label = (Real) get_volume_label_data( label_volume,
                                                  neigh_indices );
            if( min_invalid_label <= label && label <= max_invalid_label )
                ++n_invalid;

            if( n_invalid == 2 ||
                (n_invalid == 1 && !Voxel_validity_if_mixed) )
            {
                boundary_flag =  FALSE;
            }
        }

        if( boundary_flag )
        {
            value = get_volume_real_value( volume, neigh_indices[X],
                             neigh_indices[Y], neigh_indices[Z], 0, 0 );
            neigh_inside = min_value <= value && value <= max_value;

            if( neigh_inside )
                boundary_flag = FALSE;
        }
    }
    else
        boundary_flag = TRUE;

    return( boundary_flag );
}

private  void  add_face(
    Volume               volume,
    int                  indices[N_DIMENSIONS],
    int                  c,
    int                  offset,
    polygons_struct      *polygons )
{
    int      a1, a2, point_index, point_indices[N_DIMENSIONS], n_indices;
    Real     voxel[N_DIMENSIONS], xw, yw, zw;
    Vector   normal;
    Point    point;

    voxel[X] = 0.0;
    voxel[Y] = 0.0;
    voxel[Z] = 0.0;

    if( offset == -1 )
    {
        a1 = (c + 1) % N_DIMENSIONS;
        a2 = (c + 2) % N_DIMENSIONS;
        point_indices[c] = indices[c];
        voxel[c] = -1.0;
    }
    else
    {
        a1 = (c + 2) % N_DIMENSIONS;
        a2 = (c + 1) % N_DIMENSIONS;
        point_indices[c] = indices[c] + 1;
        voxel[c] = 1.0;
    }

    convert_voxel_normal_vector_to_world( volume, voxel, &xw, &yw, &zw );
    fill_Vector( normal, xw, yw, zw );
    NORMALIZE_VECTOR( normal, normal );

    point_index = polygons->n_points;
    for_less( point_indices[a1], indices[a1], indices[a1] + 2 )
    for_less( point_indices[a2], indices[a2], indices[a2] + 2 )
    {
        voxel[X] = (Real) point_indices[X] - 0.5;
        voxel[Y] = (Real) point_indices[Y] - 0.5;
        voxel[Z] = (Real) point_indices[Z] - 0.5;
        convert_voxel_to_world( volume, voxel, &xw, &yw, &zw );
        fill_Point( point, xw, yw, zw );
        ADD_ELEMENT_TO_ARRAY( polygons->points, polygons->n_points,
                              point, DEFAULT_CHUNK_SIZE );
        --polygons->n_points;
        ADD_ELEMENT_TO_ARRAY( polygons->normals, polygons->n_points,
                              normal, DEFAULT_CHUNK_SIZE );

    }

    n_indices = NUMBER_INDICES( *polygons );

    ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices,
                          point_index, DEFAULT_CHUNK_SIZE);
    ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices,
                          point_index+1, DEFAULT_CHUNK_SIZE);
    ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices,
                          point_index+3, DEFAULT_CHUNK_SIZE);
    ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices,
                          point_index+2, DEFAULT_CHUNK_SIZE);

    ADD_ELEMENT_TO_ARRAY( polygons->end_indices, polygons->n_items,
                          n_indices, DEFAULT_CHUNK_SIZE );
}

public  BOOLEAN  extract_voxel_boundary_surface(
    Volume                      volume,
    Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel[] )
{
    int      dim, offset;
    Real     value;
    BOOLEAN  inside, contains_surface;

    value = get_volume_real_value( volume,
                                   voxel[X], voxel[Y], voxel[Z], 0, 0 );

    inside = surface_extraction->min_value <= value &&
             value <= surface_extraction->max_value;

    if( !inside )
        return( FALSE );

    contains_surface = FALSE;

    for_less( dim, 0, N_DIMENSIONS )
    {
        for( offset = -1;  offset <= 1;  offset += 2 )
        {
            if( face_is_boundary( volume, surface_extraction->min_value,
                                  surface_extraction->max_value,
                                  label_volume,
                                  surface_extraction->min_invalid_label,
                                  surface_extraction->max_invalid_label,
                                  voxel, dim, offset ) )
            {
                add_face( volume, voxel, dim, offset,
                          surface_extraction->polygons );
                contains_surface = TRUE;
            }
        }
    }

    return( contains_surface );
}
