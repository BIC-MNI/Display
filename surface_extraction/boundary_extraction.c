#include  <def_objects.h>
#include  <def_colours.h>
#include  <def_skiplist.h>

typedef  skiplist_struct  point_lookup_struct;

public  Status  extract_boundary_of_labeled_voxels( volume, polygons )
    volume_struct    *volume;
    polygons_struct  *polygons;
{
    Status                       status;
    int                          indices[3], sizes[3], c, offset;
    void                         get_volume_size();
    void                         empty_polygons_struct();
    point_lookup_struct          point_lookup;
    Status                       initialize_point_lookup();
    Status                       terminate_point_lookup();
    Status                       compute_polygon_normals();
    Status                       add_face();

    empty_polygons_struct( polygons );

    ALLOC( status, polygons->colours, 1 );

    if( status == OK )
        polygons->colours[0] = WHITE;
    polygons->colour_flag = ONE_COLOUR;
    get_default_surfprop( &polygons->surfprop );

    get_volume_size( volume, &sizes[X_AXIS], &sizes[Y_AXIS], &sizes[Z_AXIS] );

    if( status == OK )
        status = initialize_point_lookup( &point_lookup );

    for_less( indices[X_AXIS], 0, sizes[X_AXIS] )
    {
        for_less( indices[Y_AXIS], 0, sizes[Y_AXIS] )
        {
            for_less( indices[Z_AXIS], 0, sizes[Z_AXIS] )
            {
                for_less( c, 0, N_DIMENSIONS )
                {
                    for( offset = -1;  offset <= 1;  offset += 2 )
                    {
                        if( status == OK &&
                            face_is_boundary( volume, sizes, indices, c,offset))
                        {
                            status = add_face( indices, c, offset, sizes,
                                               polygons, &point_lookup );
                        }
                    }
                }
            }
        }
    }

    if( status == OK )
        status = terminate_point_lookup( &point_lookup );

    if( status == OK && polygons->n_points > 0 )
    {
        ALLOC( status, polygons->normals, polygons->n_points );

        if( status == OK )
            status = compute_polygon_normals( polygons );
    }

    return( status );
}

private  Boolean  face_is_boundary( volume, sizes, indices, c, offset )
    volume_struct   *volume;
    int             sizes[N_DIMENSIONS];
    int             indices[N_DIMENSIONS];
    int             c;
    int             offset;
{
    Boolean  boundary_flag;
    int      neigh_indices[N_DIMENSIONS];

    boundary_flag = FALSE;

    if( get_voxel_label_flag( volume, indices[0], indices[1], indices[2] ) )
    {
        neigh_indices[0] = indices[0];
        neigh_indices[1] = indices[1];
        neigh_indices[2] = indices[2];
        neigh_indices[c] += offset;

        if( neigh_indices[0] < 0 || neigh_indices[0] >= sizes[0] ||
            neigh_indices[1] < 0 || neigh_indices[1] >= sizes[1] ||
            neigh_indices[2] < 0 || neigh_indices[2] >= sizes[2] ||
            !get_voxel_label_flag( volume, neigh_indices[0], neigh_indices[1],
                                   neigh_indices[2] ) )
        {
            boundary_flag = TRUE;
        }
    }

    return( boundary_flag );
}

typedef  struct
{
    unsigned  short  x, y, z;
    int              point_index;
} point_struct;

private  int  compare_function( data1, data2 )
    void   *data1;
    void   *data2;
{
    point_struct  *d1, *d2;

    d1 = (point_struct *) data1;
    d2 = (point_struct *) data2;

    if( d1->x < d2->x )
        return( -1 );
    else if( d1->x > d2->x )
        return( 1 );
    else if( d1->y < d2->y )
        return( -1 );
    else if( d1->y > d2->y )
        return( 1 );
    else if( d1->z < d2->z )
        return( -1 );
    else if( d1->z > d2->z )
        return( 1 );
    else
        return( 0 );
}

private  Status  initialize_point_lookup( point_lookup )
    point_lookup_struct  *point_lookup;
{
    Status   status;

    status = initialize_skiplist( point_lookup, compare_function );

    return( status );
}

private  Status  terminate_point_lookup( point_lookup )
    point_lookup_struct  *point_lookup;
{
    Status         status;
    skip_struct    *entry_ptr;
    point_struct   *data_ptr;

    status = OK;

    if( get_first_skiplist_entry( point_lookup, &entry_ptr,
                                  (void **) &data_ptr ) )
    {
        FREE( status, data_ptr );

        while( get_next_skiplist_entry( &entry_ptr, (void **) &data_ptr ) )
        {
            if( status == OK )
                FREE( status, data_ptr );
        }
    }

    if( status == OK )
        status = delete_skiplist( point_lookup );

    return( status );
}

private  int  get_point_index( polygons, point_lookup, x, y, z )
    polygons_struct      *polygons;
    point_lookup_struct  *point_lookup;
    int                  x, y, z;
{
    Status        status;
    Point         point;
    int           point_index;
    point_struct  p, *entry;

    p.x = (unsigned short) x;
    p.y = (unsigned short) y;
    p.z = (unsigned short) z;

    if( search_skiplist( point_lookup, (void *) &p, (void **) &entry ) )
    {
        point_index = entry->point_index;
    }
    else
    {
        ALLOC( status, entry, 1 );
        *entry = p;
        fill_Point( point, (Real) x, (Real) y, (Real) z );
        point_index = polygons->n_points;
        ADD_ELEMENT_TO_ARRAY( status, polygons->n_points,
                              polygons->points, point, DEFAULT_CHUNK_SIZE );
    }

    return( point_index );
}

private  Status  add_face( indices, c, offset, sizes, polygons, point_lookup )
    int                  indices[N_DIMENSIONS];
    int                  c;
    int                  offset;
    int                  sizes[N_DIMENSIONS];
    polygons_struct      *polygons;
    point_lookup_struct  *point_lookup;
{
    Status   status;
    int      a1, a2, point_ids[4], point_indices[3], n_indices, i;

    if( offset == 1 )
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
    point_ids[0] = get_point_index( polygons, point_lookup,
                                    point_indices[0],
                                    point_indices[1],
                                    point_indices[2] );

    point_indices[a1] = indices[a1] + 1;
    point_indices[a2] = indices[a2];
    point_ids[1] = get_point_index( polygons, point_lookup,
                                    point_indices[0],
                                    point_indices[1],
                                    point_indices[2] );

    point_indices[a1] = indices[a1] + 1;
    point_indices[a2] = indices[a2] + 1;
    point_ids[2] = get_point_index( polygons, point_lookup,
                                    point_indices[0],
                                    point_indices[1],
                                    point_indices[2] );

    point_indices[a1] = indices[a1];
    point_indices[a2] = indices[a2] + 1;
    point_ids[3] = get_point_index( polygons, point_lookup,
                                    point_indices[0],
                                    point_indices[1],
                                    point_indices[2] );

    n_indices = NUMBER_INDICES( *polygons );

    status = OK;

    for_less( i, 0, 4 )
    {
        if( status == OK )
            ADD_ELEMENT_TO_ARRAY( status, n_indices, polygons->indices,
                                  point_ids[i], DEFAULT_CHUNK_SIZE);
    }

    if( status == OK )
        ADD_ELEMENT_TO_ARRAY( status, polygons->n_items, polygons->end_indices,
                              n_indices, DEFAULT_CHUNK_SIZE );

    return( status );
}
