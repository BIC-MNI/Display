#include  <display.h>

typedef  skiplist_struct  point_lookup_struct;

private  BOOLEAN  face_is_boundary(
    Volume          label_volume,
    int             label,
    int             indices[N_DIMENSIONS],
    int             c,
    int             offset );
private  void  initialize_point_lookup(
    point_lookup_struct  *point_lookup );
private  void  terminate_point_lookup(
    point_lookup_struct  *point_lookup );
private  Status  add_face(
    Volume               volume,
    int                  indices[N_DIMENSIONS],
    int                  c,
    int                  offset,
    polygons_struct      *polygons,
    point_lookup_struct  *point_lookup );

public  void  extract_boundary_of_labeled_voxels(
    Volume           volume,
    Volume           label_volume,
    int              label,
    polygons_struct  *polygons )
{
    int                          indices[N_DIMENSIONS], sizes[N_DIMENSIONS];
    int                          c, offset;
    point_lookup_struct          point_lookup;
    progress_struct              progress;

    initialize_polygons( polygons, WHITE, (Surfprop *) NULL );

    get_default_surfprop( &polygons->surfprop );

    get_volume_sizes( volume, sizes );

    initialize_point_lookup( &point_lookup );

    initialize_progress_report( &progress, FALSE,
                                sizes[X] * sizes[Y] * sizes[Z],
                                "Extracting boundary" );

    for_less( indices[Z], 0, sizes[Z] )
    {
        for_less( indices[X], 0, sizes[X] )
        {
            for_less( indices[Y], 0, sizes[Y] )
            {
                for_less( c, 0, N_DIMENSIONS )
                {
                    for( offset = -1;  offset <= 1;  offset += 2 )
                    {
                        if( face_is_boundary( label_volume,
                                              label, indices, c,offset))
                        {
                            add_face( volume, indices, c, offset,
                                      polygons, &point_lookup );
                        }
                    }
                }
            }

            update_progress_report( &progress, sizes[Y] *
                     (indices[X]+1 + indices[Z] * sizes[X]) );
        }

        print( "N points: %d    N polygons: %d\n", polygons->n_points,
               polygons->n_items );
    }

    terminate_progress_report( &progress );

    terminate_point_lookup( &point_lookup );

    if( polygons->n_points > 0 )
    {
        ALLOC( polygons->normals, polygons->n_points );

        compute_polygon_normals( polygons );
    }
}

private  BOOLEAN  face_is_boundary(
    Volume          label_volume,
    int             label,
    int             indices[N_DIMENSIONS],
    int             c,
    int             offset )
{
    BOOLEAN  inside, neigh_inside;
    BOOLEAN  boundary_flag;
    int      neigh_indices[N_DIMENSIONS];

    boundary_flag = FALSE;

    inside = (get_volume_label_data( label_volume, indices ) &
                          get_max_label()) == label;

    if( inside )
    {
        neigh_indices[0] = indices[0];
        neigh_indices[1] = indices[1];
        neigh_indices[2] = indices[2];
        neigh_indices[c] += offset;

        if( int_voxel_is_within_volume( label_volume, neigh_indices ) )
        {
            neigh_inside = (get_volume_label_data( label_volume,
                                                   neigh_indices ) &
                            get_max_label()) == label;

            if( inside != neigh_inside )
                boundary_flag = TRUE;
        }
        else
            boundary_flag = TRUE;
    }

    return( boundary_flag );
}

typedef  struct
{
    unsigned  short  x, y, z;
    int              point_index;
} point_struct;

private  int  compare_function(
    void   *data1,
    void   *data2 )
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

private  void  initialize_point_lookup(
    point_lookup_struct  *point_lookup )
{
    initialize_skiplist( point_lookup, compare_function );
}

private  void  terminate_point_lookup(
    point_lookup_struct  *point_lookup )
{
    skip_struct    *entry_ptr;
    point_struct   *data_ptr;

    if( get_first_skiplist_entry( point_lookup, &entry_ptr,
                                  (void **) &data_ptr ) )
    {
        FREE( data_ptr );

        while( get_next_skiplist_entry( &entry_ptr, (void **) &data_ptr ) )
        {
            FREE( data_ptr );
        }
    }

    delete_skiplist( point_lookup );
}

private  int  get_point_index(
    Volume               volume,
    polygons_struct      *polygons,
    point_lookup_struct  *point_lookup,
    int                  x,
    int                  y,
    int                  z )
{
    Real          x_w, y_w, z_w, voxel[MAX_DIMENSIONS];
    Point         point;
    point_struct  p, *entry;

    p.x = (unsigned short) x;
    p.y = (unsigned short) y;
    p.z = (unsigned short) z;

    if( !search_skiplist( point_lookup, (void *) &p, (void **) &entry ) )
    {
        ALLOC( entry, 1 );
        *entry = p;
        entry->point_index = polygons->n_points;

        if( !insert_in_skiplist( point_lookup, (void *) entry ) )
        {
            HANDLE_INTERNAL_ERROR( "lookup id for boundary detection" );
        }

        voxel[X] = (Real) x - 0.5;
        voxel[Y] = (Real) y - 0.5;
        voxel[Z] = (Real) z - 0.5;
        convert_voxel_to_world( volume, voxel, &x_w, &y_w, &z_w );
        fill_Point( point, x_w, y_w, z_w );
        ADD_ELEMENT_TO_ARRAY( polygons->points, polygons->n_points,
                              point, DEFAULT_CHUNK_SIZE );
    }

    return( entry->point_index );
}

private  Status  add_face(
    Volume               volume,
    int                  indices[N_DIMENSIONS],
    int                  c,
    int                  offset,
    polygons_struct      *polygons,
    point_lookup_struct  *point_lookup )
{
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
    point_ids[0] = get_point_index( volume, polygons, point_lookup,
                                    point_indices[0],
                                    point_indices[1],
                                    point_indices[2] );

    point_indices[a1] = indices[a1];
    point_indices[a2] = indices[a2] + 1;
    point_ids[1] = get_point_index( volume, polygons, point_lookup,
                                    point_indices[0],
                                    point_indices[1],
                                    point_indices[2] );

    point_indices[a1] = indices[a1] + 1;
    point_indices[a2] = indices[a2] + 1;
    point_ids[2] = get_point_index( volume, polygons, point_lookup,
                                    point_indices[0],
                                    point_indices[1],
                                    point_indices[2] );

    point_indices[a1] = indices[a1] + 1;
    point_indices[a2] = indices[a2];
    point_ids[3] = get_point_index( volume, polygons, point_lookup,
                                    point_indices[0],
                                    point_indices[1],
                                    point_indices[2] );

    n_indices = NUMBER_INDICES( *polygons );

    for_less( i, 0, 4 )
    {
        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices,
                              point_ids[i], DEFAULT_CHUNK_SIZE);
    }

    ADD_ELEMENT_TO_ARRAY( polygons->end_indices, polygons->n_items,
                          n_indices, DEFAULT_CHUNK_SIZE );
}
