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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/surface_extraction/boundary_extraction.c,v 1.32 1997-02-14 17:16:55 david Exp $";
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

private  void  assign_set_numbers(
    BOOLEAN        inside_flags[2][2][2],
    Smallest_int   set_numbers[3][2][2][2] )
{
    static  struct
    {
        int   face_dim;
        int   x_or_y_face;
        int   face_dir;
    }
           cycles[6][4] =
                 {
                     { { Y, 0, 1 }, { Z, 1, 1 }, { Y, 1, 0 }, { Z, 0, 0 } },
                     { { Z, 0, 1 }, { X, 1, 1 }, { Z, 1, 0 }, { X, 0, 0 } },
                     { { X, 0, 1 }, { Y, 1, 1 }, { X, 1, 0 }, { Y, 0, 0 } },
                     { { Z, 0, 1 }, { Y, 1, 1 }, { Z, 1, 0 }, { Y, 0, 0 } },
                     { { X, 0, 1 }, { Z, 1, 1 }, { X, 1, 0 }, { Z, 0, 0 } },
                     { { Y, 0, 1 }, { X, 1, 1 }, { Y, 1, 0 }, { X, 0, 0 } }
                 };

    BOOLEAN  faces_present[N_DIMENSIONS][2][2][2];
    int      dim, x, y, dir, n_connected, conn_index;
    int      i, ind, test_dir, test_dim;
    int      other_dim, which;
    int      a1, a2, v1[N_DIMENSIONS], v2[N_DIMENSIONS];
    int      cur_dim, cur_a1, cur_a2, cur_x, cur_y, cur_dir;
    int      rot_dim, lut;
    int      test_x_face, test_y_face, test_a1, test_a2;
    int      n_found, n_sets;
    static   struct
     {
         int dim, x, y, dir;
     }  list[24];

    for_less( dim, 0, N_DIMENSIONS )
    for_less( x, 0, 2 )
    for_less( y, 0, 2 )
    for_less( dir, 0, 2 )
    {
        a1 = (dim + 1) % N_DIMENSIONS;
        a2 = (dim + 2) % N_DIMENSIONS;
        v1[a1] = x;
        v2[a1] = x;

        v1[a2] = y;
        v2[a2] = y;

        v1[dim] = 1 - dir;
        v2[dim] = dir;

        faces_present[dim][x][y][dir] =
                                  inside_flags[v1[0]][v1[1]][v1[2]] &&
                                  !inside_flags[v2[0]][v2[1]][v2[2]];
        set_numbers[dim][x][y][dir] = (Smallest_int) -1;
    }

    n_sets = 0;
    for_less( dim, 0, N_DIMENSIONS )
    for_less( x, 0, 2 )
    for_less( y, 0, 2 )
    for_less( dir, 0, 2 )
    {
        if( !faces_present[dim][x][y][dir] ||
             set_numbers[dim][x][y][dir] >= 0 )
        {
            continue;
        }

        set_numbers[dim][x][y][dir] = (Smallest_int) n_sets;

        n_connected = 1;
        list[0].dim = dim;
        list[0].x = x;
        list[0].y = y;
        list[0].dir = dir;

        conn_index = 0;

        while( conn_index < n_connected )
        {
            cur_dim = list[conn_index].dim;
            cur_a1 = (cur_dim + 1) % N_DIMENSIONS;
            cur_a2 = (cur_dim + 2) % N_DIMENSIONS;
            cur_x = list[conn_index].x;
            cur_y = list[conn_index].y;
            cur_dir = list[conn_index].dir;
            ++conn_index;

            n_found = 0;
            for_less( lut, 0, 6 )
            {
                rot_dim = lut % 3;
                for_less( ind, 0, 4 )
                {
                    if( cycles[lut][ind].face_dim == cur_dim &&
                        (rot_dim == cur_a1 &&
                         cur_y == cycles[lut][ind].x_or_y_face  ||
                         rot_dim == cur_a2 &&
                         cur_x == cycles[lut][ind].x_or_y_face) &&
                        cycles[lut][ind].face_dir == 1 - cur_dir )
                        break;
                }

                if( ind >= 4 )
                    continue;

                ++n_found;

                for_less( i, 1, 4 )
                {
                    which = (ind + i) % 4;

                    test_dim = cycles[lut][which].face_dim;
                    test_dir = cycles[lut][which].face_dir;
                    test_a1 = (test_dim + 1) % N_DIMENSIONS;
                    test_a2 = (test_dim + 2) % N_DIMENSIONS;
                    other_dim = 3 - rot_dim - test_dim;
                    if( test_a1 == other_dim )
                    {
                        test_x_face = cycles[lut][which].x_or_y_face;
                        if( rot_dim == cur_a1 )
                            test_y_face = cur_x;
                        else if( rot_dim == cur_a2 )
                            test_y_face = cur_y;
                        else
                            handle_internal_error( "rot_dim" );
                    }
                    else if( test_a2 == other_dim )
                    {
                        test_y_face = cycles[lut][which].x_or_y_face;
                        if( rot_dim == cur_a1 )
                            test_x_face = cur_x;
                        else if( rot_dim == cur_a2 )
                            test_x_face = cur_y;
                        else
                            handle_internal_error( "rot_dim" );
                    }
                    else
                        handle_internal_error( "test[other_dim]" );

                    if( faces_present[test_dim][test_x_face][test_y_face]
                                     [test_dir])
                    {
                        if( set_numbers[test_dim][test_x_face][test_y_face]
                                       [test_dir] < 0 )
                        {
                            set_numbers[test_dim][test_x_face][test_y_face]
                                       [test_dir] = (Smallest_int) n_sets;
                            list[n_connected].dim = test_dim;
                            list[n_connected].x = test_x_face;
                            list[n_connected].y = test_y_face;
                            list[n_connected].dir = test_dir;
                            ++n_connected;
                        }
                        break;
                    }
                }

                if( i == 4 )
                {
                    handle_internal_error( "i == 4" );
                }
            }

            if( n_found != 2 )
                handle_internal_error( "lut" );
        }

        ++n_sets;
    }
}

private  int  lookup_edge_id(
    int       face_dim,
    int       x_face,
    int       y_face,
    int       face_dir,
    BOOLEAN   inside_flags[2][2][2] )
{
    static   BOOLEAN  initialized = FALSE;
    static   BOOLEAN  this_case_initialized[2][2][2][2][2][2][2][2];
    static   Smallest_int   lookup[2][2][2][2][2][2][2][2][3][2][2][2];
    int      f0, f1, f2, f3, f4, f5, f6, f7;

    if( !initialized )
    {
        initialized = TRUE;
        for_less( f0, 0, 2 )
        for_less( f1, 0, 2 )
        for_less( f2, 0, 2 )
        for_less( f3, 0, 2 )
        for_less( f4, 0, 2 )
        for_less( f5, 0, 2 )
        for_less( f6, 0, 2 )
        for_less( f7, 0, 2 )
        {
            this_case_initialized[f0][f1][f2][f3][f4][f5][f6][f7] = FALSE;
        }
    }

    if( !this_case_initialized[inside_flags[0][0][0]]
                              [inside_flags[0][0][1]]
                              [inside_flags[0][1][0]]
                              [inside_flags[0][1][1]]
                              [inside_flags[1][0][0]]
                              [inside_flags[1][0][1]]
                              [inside_flags[1][1][0]]
                              [inside_flags[1][1][1]] )
    {
        this_case_initialized[inside_flags[0][0][0]]
                             [inside_flags[0][0][1]]
                             [inside_flags[0][1][0]]
                             [inside_flags[0][1][1]]
                             [inside_flags[1][0][0]]
                             [inside_flags[1][0][1]]
                             [inside_flags[1][1][0]]
                             [inside_flags[1][1][1]] = TRUE;

        assign_set_numbers( inside_flags, lookup[inside_flags[0][0][0]]
                                                [inside_flags[0][0][1]]
                                                [inside_flags[0][1][0]]
                                                [inside_flags[0][1][1]]
                                                [inside_flags[1][0][0]]
                                                [inside_flags[1][0][1]]
                                                [inside_flags[1][1][0]]
                                                [inside_flags[1][1][1]] );
    }

    return( (int) lookup[inside_flags[0][0][0]]
                        [inside_flags[0][0][1]]
                        [inside_flags[0][1][0]]
                        [inside_flags[0][1][1]]
                        [inside_flags[1][0][0]]
                        [inside_flags[1][0][1]]
                        [inside_flags[1][1][0]]
                        [inside_flags[1][1][1]]
                        [face_dim][x_face][y_face][face_dir] );
}

private  int  determine_edge_index(
    int                         x,
    int                         y,
    int                         z,
    int                         face_dim,
    int                         face_offset,
    BOOLEAN                     inside[3][3][3] )
{
    int        a1, a2, x_face, y_face, dir, edge_index;
    int        corner_offset[N_DIMENSIONS], dx, dy, dz;
    int        other[N_DIMENSIONS];
    int        pos[2][N_DIMENSIONS];
    BOOLEAN    inside_flags[2][2][2];

    if( !Duplicate_boundary_vertices )
        return( 0 );

    corner_offset[X] = x;
    corner_offset[Y] = y;
    corner_offset[Z] = z;

    other[0] = (x == 0) ? 0 : 2;
    other[1] = (y == 0) ? 0 : 2;
    other[2] = (z == 0) ? 0 : 2;

    pos[0][0] = MIN( 1, other[0] );
    pos[0][1] = MIN( 1, other[1] );
    pos[0][2] = MIN( 1, other[2] );
    pos[1][0] = MAX( 1, other[0] );
    pos[1][1] = MAX( 1, other[1] );
    pos[1][2] = MAX( 1, other[2] );

    a1 = (face_dim+1) % N_DIMENSIONS;
    a2 = (face_dim+2) % N_DIMENSIONS;

    x_face = (other[a1] == 0) ? 1 : 0;
    y_face = (other[a2] == 0) ? 1 : 0;
    dir = (face_offset == -1) ? 0 : 1;

    for_less( dx, 0, 2 )
    for_less( dy, 0, 2 )
    for_less( dz, 0, 2 )
    {
        inside_flags[dx][dy][dz] = inside[pos[dx][0]][pos[dy][1]][pos[dz][2]];
    }

    edge_index = lookup_edge_id( face_dim, x_face, y_face, dir, inside_flags );

    return( edge_index );
}

private  BOOLEAN  edge_is_duplicate(
    int                         x1,
    int                         y1,
    int                         z1,
    int                         x2,
    int                         y2,
    int                         z2,
    BOOLEAN                     inside[3][3][3],
    int                         *which )
{
    int        a1, a2, x, y, dim, where[N_DIMENSIONS];
    int        other[N_DIMENSIONS], start[N_DIMENSIONS];
    BOOLEAN    inside_flags[2][2];

    if( !Duplicate_boundary_vertices )
        return( FALSE );

    start[0] = MIN( x1, x2 );
    start[1] = MIN( y1, y2 );
    start[2] = MIN( z1, z2 );

    if( x1 != x2 )
        dim = X;
    else if( y1 != y2 )
        dim = Y;
    else if( z1 != z2 )
        dim = Z;
    else
        handle_internal_error( "edge_is_duplicate" );

    a1 = (dim + 1) % N_DIMENSIONS;
    a2 = (dim + 2) % N_DIMENSIONS;

    other[a1] = (start[a1] == 0) ? 0 : 2;
    other[a2] = (start[a2] == 0) ? 0 : 2;
    other[dim] = 1;

    where[dim] = 1;
    for_less( x, 0, 2 )
    for_less( y, 0, 2 )
    {
        where[a1] = (x == 0) ? 1 : other[a1];
        where[a2] = (y == 0) ? 1 : other[a2];
        
        inside_flags[x][y] = inside[where[0]][where[1]][where[2]];
    }

    if( !inside_flags[0][0] || !inside_flags[1][1] ||
        inside_flags[1][0] || inside_flags[0][1] )
        return( FALSE );

    for_less( dim, 0, N_DIMENSIONS )
    {
        if( other[dim] == 0 )
        {
            *which = 1;
            return( TRUE );
        }
        else if( other[dim] == 2 )
        {
            *which = 0;
            return( TRUE );
        }
    }

    handle_internal_error( "edge_is_duplicate which" );
    *which = 0;

    return( TRUE );
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
    int                  a1, a2, point_index, ind, start_index;
    int                  dx, dy, second_vertex[N_DIMENSIONS], dim;
    int                  point_indices[8][5], x, y;
    int                  sizes[N_DIMENSIONS];
    int                  corner_index[N_DIMENSIONS], edge_index, v;
    Real                 voxel[N_DIMENSIONS], xw, yw, zw;
    Real                 voxel_normal[N_DIMENSIONS], separations[N_DIMENSIONS];
    Point                point;
    Vector               normal;

    if( offset == -1 )
    {
        a1 = (c + 1) % N_DIMENSIONS;
        a2 = (c + 2) % N_DIMENSIONS;
        corner_index[c] = indices[c];
    }
    else
    {
        a1 = (c + 2) % N_DIMENSIONS;
        a2 = (c + 1) % N_DIMENSIONS;
        corner_index[c] = indices[c] + 1;
    }

    get_volume_sizes( volume, sizes );
    get_volume_separations( volume, separations );
    for_less( dim, 0, N_DIMENSIONS )
        separations[dim] = FABS( separations[dim] );

    ind = 0;
    for_less( v, 0, 4 )
    {
        switch( v )
        {
        case 0:   x = 0;  y = 0;  dx = 0;  dy = 1;  break;
        case 1:   x = 0;  y = 1;  dx = 1;  dy = 0;  break;
        case 2:   x = 1;  y = 1;  dx = 0;  dy = -1;  break;
        case 3:   x = 1;  y = 0;  dx = -1;  dy = 0;  break;
        default:  break;
        }

        point_indices[ind][a1] = indices[a1] + x;
        point_indices[ind][a2] = indices[a2] + y;
        point_indices[ind][c] = corner_index[c];
        point_indices[ind][3] = determine_edge_index(
                                           point_indices[ind][0] - indices[0],
                                           point_indices[ind][1] - indices[1],
                                           point_indices[ind][2] - indices[2],
                                           c, offset, inside_flags );
        ++ind;

        if( !Duplicate_boundary_vertices )
            continue;

        second_vertex[a1] = indices[a1] + x + dx;
        second_vertex[a2] = indices[a2] + y + dy;
        second_vertex[c] = corner_index[c];
        if( edge_is_duplicate( point_indices[ind-1][0] - indices[0],
                               point_indices[ind-1][1] - indices[1],
                               point_indices[ind-1][2] - indices[2],
                               second_vertex[0] - indices[0],
                               second_vertex[1] - indices[1],
                               second_vertex[2] - indices[2],
                               inside_flags, &edge_index ) )
        {
            for_less( dim, 0, N_DIMENSIONS )
            {
                point_indices[ind][dim] = MIN( point_indices[ind-1][dim],
                                               second_vertex[dim] );
            }

            point_indices[ind][3] = edge_index + 4;
            if( dx != 0 )
                point_indices[ind][4] = a1;
            else
                point_indices[ind][4] = a2;
            ++ind;
        }
    }

    if( poly_index == polygons->n_items )
    {
        start_index = NUMBER_INDICES( *polygons );
        ADD_ELEMENT_TO_ARRAY( polygons->end_indices, polygons->n_items,
                              start_index+ind, DEFAULT_CHUNK_SIZE );

        SET_ARRAY_SIZE( polygons->indices, start_index, start_index+ind,
                        DEFAULT_CHUNK_SIZE );
    }
    else
        start_index = POINT_INDEX( polygons->end_indices, poly_index, 0 );

    for_less( v, 0, ind )
    {
        if( !lookup_edge_point_id( sizes,
                                   &surface_extraction->edge_points,
                                   point_indices[v][X],
                                   point_indices[v][Y],
                                   point_indices[v][Z],
                                   point_indices[v][3], &point_index ) )
        {
            point_index = polygons->n_points;

            record_edge_point_id( sizes, &surface_extraction->edge_points,
                                  point_indices[v][X],
                                  point_indices[v][Y],
                                  point_indices[v][Z],
                                  point_indices[v][3], point_index );

            voxel[X] = (Real) point_indices[v][X] - 0.5;
            voxel[Y] = (Real) point_indices[v][Y] - 0.5;
            voxel[Z] = (Real) point_indices[v][Z] - 0.5;

            if( point_indices[v][3] >= 4 )
                voxel[point_indices[v][4]] += 0.5;

            convert_voxel_to_world( volume, voxel, &xw, &yw, &zw );
            fill_Point( point, xw, yw, zw );

            SET_ARRAY_SIZE( polygons->normals, polygons->n_points,
                            polygons->n_points+1, DEFAULT_CHUNK_SIZE );
            ADD_ELEMENT_TO_ARRAY( polygons->points, polygons->n_points,
                                  point, DEFAULT_CHUNK_SIZE );
        }

        if( point_indices[v][3] < 4 )
        {
            get_vertex_normal( separations,
                               point_indices[v][X] - indices[X],
                               point_indices[v][Y] - indices[Y],
                               point_indices[v][Z] - indices[Z],
                               inside_flags, valid_flags, voxel_normal );

            convert_voxel_vector_to_world( volume, voxel_normal, &xw, &yw, &zw);
            fill_Vector( normal, xw, yw, zw );
            NORMALIZE_VECTOR( normal, normal );
        }
        else
            fill_Vector( normal, 0.0, 0.0, 0.0 );

        polygons->normals[point_index] = normal;

        polygons->indices[start_index+v] = point_index;
    }
}

private  void  get_inside_flags(
    surface_extraction_struct   *surf,
    int                         voxel[],
    BOOLEAN                     inside_flags[3][3][3],
    BOOLEAN                     valid_flags[3][3][3] )
{
    int   dx, dy, dz, x_off, y_off, z_off;

    x_off = voxel[X] - surf->min_block[X];
    y_off = voxel[Y] - surf->min_block[Y];
    z_off = voxel[Z] - surf->min_block[Z];

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

public  void  read_voxellation_block(
    surface_extraction_struct   *surf )
{
    int              sizes[N_DIMENSIONS];
    int              dx, dy, dz, ind, nx, ny, nz;
    Real             values[(SURFACE_BLOCK_SIZE+2)*
                            (SURFACE_BLOCK_SIZE+2)*
                            (SURFACE_BLOCK_SIZE+2)];
    Real             labels[(SURFACE_BLOCK_SIZE+2)*
                            (SURFACE_BLOCK_SIZE+2)*
                            (SURFACE_BLOCK_SIZE+2)];
    Real             value, label;
    Real             min_value, max_value, min_label, max_label;
    Volume           volume, label_volume;

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

    nx = surf->max_block[X] - surf->min_block[X] + 3;
    ny = surf->max_block[Y] - surf->min_block[Y] + 3;
    nz = surf->max_block[Z] - surf->min_block[Z] + 3;

    if( surf->min_block[X] <= 0 || surf->max_block[X] >= sizes[X]-1 ||
        surf->min_block[Y] <= 0 || surf->max_block[Y] >= sizes[Y]-1 ||
        surf->min_block[Z] <= 0 || surf->max_block[Z] >= sizes[Z]-1 ||
        label_volume != NULL &&
        !volume_is_alloced( label_volume ) && !volume_is_cached(label_volume) )
    {
        ind = 0;
        for_inclusive( dx, surf->min_block[X]-1, surf->max_block[X]+1 )
        for_inclusive( dy, surf->min_block[Y]-1, surf->max_block[Y]+1 )
        for_inclusive( dz, surf->min_block[Z]-1, surf->max_block[Z]+1 )
        {
            if( dx < 0 || dx >= sizes[X] ||
                dy < 0 || dy >= sizes[Y] ||
                dz < 0 || dz >= sizes[Z] )
            {
                values[ind] = surf->min_value - 1.0;
                labels[ind] = (Real) surf->min_invalid_label - 1.0;
            }
            else
            {
                values[ind] = get_volume_real_value( volume, dx, dy, dz, 0, 0 );
                if( label_volume != NULL )
                {
                    values[ind] = (Real) get_3D_volume_label_data( label_volume,
                                                               dx, dy, dz );
                }
            }
            ++ind;
        }
    }
    else
    {
        get_volume_value_hyperslab_3d( volume,
                                       surf->min_block[X]-1,
                                       surf->min_block[Y]-1,
                                       surf->min_block[Z]-1,
                                       nx, ny, nz, values );
        if( label_volume != NULL )
        {
            get_volume_value_hyperslab_3d( label_volume,
                                       surf->min_block[X]-1,
                                       surf->min_block[Y]-1,
                                       surf->min_block[Z]-1,
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

public  BOOLEAN  extract_voxel_boundary_surface(
    Volume                      volume,
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

    get_inside_flags( surface_extraction, voxel, inside_flags, valid_flags );

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
