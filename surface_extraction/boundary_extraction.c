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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/surface_extraction/boundary_extraction.c,v 1.18 1996-04-10 13:59:35 david Exp $";
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
    int             offset );
private  void  add_face(
    Volume               volume,
    int                  indices[N_DIMENSIONS],
    int                  c,
    int                  offset,
    polygons_struct      *polygons,
    int                  ***point_lookup );

#define  INVALID_INDEX   -1

public  void  create_voxelated_surface(
    Volume           volume,
    Real             min_value,
    Real             max_value,
    Volume           label_volume,
    Real             min_invalid_label,
    Real             max_invalid_label,
    polygons_struct  *polygons )
{
    int                 indices[N_DIMENSIONS], sizes[N_DIMENSIONS];
    int                 c, offset, x, y, z;
    progress_struct     progress;
    int                 **point_lookup[2], **tmp;
    Real                value;
    BOOLEAN             inside;

    initialize_polygons( polygons, WHITE, (Surfprop *) NULL );

    get_default_surfprop( &polygons->surfprop );

    get_volume_sizes( volume, sizes );

    ALLOC2D( point_lookup[0], sizes[X], sizes[Y] );
    ALLOC2D( point_lookup[1], sizes[X], sizes[Y] );

    for_less( z, 0, 2 )
    {
        for_less( x, 0, sizes[X] )
            for_less( y, 0, sizes[Y] )
                point_lookup[z][x][y] = INVALID_INDEX;
    }

    initialize_progress_report( &progress, FALSE,
                                sizes[X] * sizes[Z],
                                "Extracting boundary" );

    for_less( indices[Z], 0, sizes[Z] )
    {
        tmp = point_lookup[0];
        point_lookup[0] = point_lookup[1];
        point_lookup[1] = tmp;

        for_less( x, 0, sizes[X] )
            for_less( y, 0, sizes[Y] )
                point_lookup[1][x][y] = INVALID_INDEX;

        for_less( indices[X], 0, sizes[X] )
        {
            for_less( indices[Y], 0, sizes[Y] )
            {
                value = get_volume_real_value( volume,
                                  indices[X], indices[Y], indices[Z], 0, 0 );
                inside = min_value <= value && value <= max_value;
                if( !inside )
                    continue;

                for_less( c, 0, N_DIMENSIONS )
                {
                    for( offset = -1;  offset <= 1;  offset += 2 )
                    {
                        if( face_is_boundary( volume, min_value, max_value,
                                              label_volume,
                                              min_invalid_label,
                                              max_invalid_label,
                                              indices, c,offset) )
                        {
                            add_face( volume, indices, c, offset,
                                      polygons, point_lookup );
                        }
                    }
                }
            }

            update_progress_report( &progress, 
                                    indices[X] + 1 + indices[Z] * sizes[X] );
        }
    }

    terminate_progress_report( &progress );

    FREE2D( point_lookup[0] );
    FREE2D( point_lookup[1] );

    if( polygons->n_points > 0 )
    {
        ALLOC( polygons->normals, polygons->n_points );

        compute_polygon_normals( polygons );
    }
}

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

private  int  get_point_index(
    Volume               volume,
    polygons_struct      *polygons,
    int                  ***point_lookup,
    int                  x,
    int                  y,
    int                  z,
    int                  z_slice )
{
    int           point_index;
    Real          x_w, y_w, z_w, voxel[MAX_DIMENSIONS];
    Point         point;

    point_index = point_lookup[z][x][y];

    if( point_index == INVALID_INDEX )
    {
        point_index = polygons->n_points;
        point_lookup[z][x][y] = point_index;
        voxel[X] = (Real) x - 0.5;
        voxel[Y] = (Real) y - 0.5;
        voxel[Z] = (Real) (z + z_slice) - 0.5;
        convert_voxel_to_world( volume, voxel, &x_w, &y_w, &z_w );
        fill_Point( point, x_w, y_w, z_w );
        ADD_ELEMENT_TO_ARRAY( polygons->points, polygons->n_points,
                              point, DEFAULT_CHUNK_SIZE );
    }

    return( point_index );
}

private  void  add_face(
    Volume               volume,
    int                  indices[N_DIMENSIONS],
    int                  c,
    int                  offset,
    polygons_struct      *polygons,
    int                  ***point_lookup )
{
    int      a1, a2, point_ids[4], point_indices[N_DIMENSIONS], n_indices, i;

    if( offset == -1 )
    {
        a1 = (c + 1) % N_DIMENSIONS;
        a2 = (c + 2) % N_DIMENSIONS;
    }
    else
    {
        a1 = (c + 2) % N_DIMENSIONS;
        a2 = (c + 1) % N_DIMENSIONS;
    }

    point_indices[0] = indices[0];
    point_indices[1] = indices[1];
    point_indices[2] = indices[2];
    if( offset == 1 )
        point_indices[c] += 1;

    point_indices[a1] = indices[a1];
    point_indices[a2] = indices[a2];
    point_ids[0] = get_point_index( volume, polygons, point_lookup,
                                    point_indices[0],
                                    point_indices[1],
                                    point_indices[2] - indices[Z],
                                    indices[Z] );

    point_indices[a1] = indices[a1];
    point_indices[a2] = indices[a2] + 1;
    point_ids[1] = get_point_index( volume, polygons, point_lookup,
                                    point_indices[0],
                                    point_indices[1],
                                    point_indices[2] - indices[Z],
                                    indices[Z] );

    point_indices[a1] = indices[a1] + 1;
    point_indices[a2] = indices[a2] + 1;
    point_ids[2] = get_point_index( volume, polygons, point_lookup,
                                    point_indices[0],
                                    point_indices[1],
                                    point_indices[2] - indices[Z],
                                    indices[Z] );

    point_indices[a1] = indices[a1] + 1;
    point_indices[a2] = indices[a2];
    point_ids[3] = get_point_index( volume, polygons, point_lookup,
                                    point_indices[0],
                                    point_indices[1],
                                    point_indices[2] - indices[Z],
                                    indices[Z] );

    n_indices = NUMBER_INDICES( *polygons );

    for_less( i, 0, 4 )
    {
        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices,
                              point_ids[i], DEFAULT_CHUNK_SIZE);
    }

    ADD_ELEMENT_TO_ARRAY( polygons->end_indices, polygons->n_items,
                          n_indices, DEFAULT_CHUNK_SIZE );
}
