
#include  <display.h>

/* ------------------ Voxel queue ------------------------ */

public  void   initialize_voxel_queue(
    voxel_queue_struct  *voxel_queue )
{
    INITIALIZE_QUEUE( *voxel_queue );
}

public  void   insert_in_voxel_queue(
    voxel_queue_struct  *voxel_queue,
    voxel_index_struct  *voxel_indices )
{
    INSERT_IN_QUEUE( *voxel_queue, *voxel_indices );
}

public  void   get_next_voxel_from_queue(
    voxel_queue_struct    *voxel_queue,
    voxel_index_struct    *voxel_indices )
{
    REMOVE_FROM_QUEUE( *voxel_queue, *voxel_indices );
}

public  BOOLEAN  voxels_remaining(
    voxel_queue_struct  *voxel_queue )
{
    return( !IS_QUEUE_EMPTY( *voxel_queue ) );
}

public  void  delete_voxel_queue(
    voxel_queue_struct   *voxel_queue )
{
    DELETE_QUEUE( *voxel_queue );
}

/* ------------------ Voxel flag, 1 bit flag structure --------------- */

public  void  initialize_voxel_flags(
    bitlist_struct  *voxel_flags,
    int             n_voxels )
{
    create_bitlist( n_voxels, voxel_flags );
}

public  void  delete_voxel_flags(
    bitlist_struct  *voxel_flags )
{
    delete_bitlist( voxel_flags );
}

public  void  clear_voxel_flags(
    bitlist_struct  *voxel_flags )
{
    zero_bitlist( voxel_flags );
}

public  BOOLEAN  get_voxel_flag(
    Volume              volume,
    bitlist_struct      *voxel_flags,
    voxel_index_struct  *indices )
{
    int   sizes[N_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    return( get_bitlist_bit( voxel_flags, IJK( indices->i[X],
                                               indices->i[Y],
                                               indices->i[Z],
                                               sizes[Y]-1, sizes[Z]-1 ) ) );
}

public  void  set_voxel_flag(
    Volume              volume,
    bitlist_struct      *voxel_flags,
    voxel_index_struct  *indices )
{
    int   sizes[N_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    set_bitlist_bit( voxel_flags, IJK( indices->i[X],
                                       indices->i[Y],
                                       indices->i[Z],
                                       sizes[Y]-1, sizes[Z]-1 ),     ON );
}

public  void  reset_voxel_flag(
    Volume              volume,
    bitlist_struct      *voxel_flags,
    voxel_index_struct  *indices )
{
    int   sizes[N_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    set_bitlist_bit( voxel_flags, IJK( indices->i[X],
                                       indices->i[Y],
                                       indices->i[Z],
                                       sizes[Y]-1, sizes[Z]-1 ),     OFF );
}

/* ------------------ Voxel done flags, 4 bit flag structure --------------- */

public  void  initialize_voxel_done_flags(
    unsigned_byte   **voxel_done_flags,
    int             n_voxels )
{
    if( n_voxels > 0 )
    {
        ALLOC( *voxel_done_flags, (n_voxels + 1) / 2 );

        clear_voxel_done_flags( *voxel_done_flags, n_voxels );
    }
}

public  void  delete_voxel_done_flags(
    unsigned_byte  voxel_done_flags[] )
{
    if( voxel_done_flags != (unsigned_byte *) 0 )
        FREE( voxel_done_flags );
}

public  void  clear_voxel_done_flags(
    unsigned_byte   voxel_done_flags[],
    int             n_voxels )
{
    int   i;

    for_less( i, 0, n_voxels/2 )
        voxel_done_flags[i] = 0;
}

public  unsigned_byte  get_voxel_done_flag(
    Volume              volume,
    unsigned_byte       voxel_done_flags[],
    voxel_index_struct  *indices )
{
    int            index, byte_index;
    unsigned_byte  flag;
    int            sizes[N_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    index = IJK( indices->i[X], indices->i[Y], indices->i[Z],
                 sizes[Y]-1, sizes[Z]-1 );

    byte_index = index >> 1;

    if( index & 1 )
        flag = voxel_done_flags[byte_index] >> 4;
    else
        flag = voxel_done_flags[byte_index] & 15;

    return( flag );
}

public  void  set_voxel_done_flag(
    Volume              volume,
    unsigned_byte       voxel_done_flags[],
    voxel_index_struct  *indices,
    unsigned_byte       flag )
{
    int            index, byte_index;
    int            sizes[N_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    index = IJK( indices->i[X], indices->i[Y], indices->i[Z],
                 sizes[Y]-1, sizes[Z]-1 );

    byte_index = index >> 1;

    if( index & 1 )
        voxel_done_flags[byte_index] = (voxel_done_flags[byte_index] & 15) |
                                       (flag << 4);
    else
        voxel_done_flags[byte_index] = (voxel_done_flags[byte_index] & 240) |
                                       flag;
}

/* ----------------- edge points, hash table lookup ------------- */

#define  INITIAL_SIZE         1000

public  void  initialize_edge_points(
    hash_table_struct  *hash_table )
{
    initialize_hash_table( hash_table, 2, INITIAL_SIZE, Edge_point_threshold,
                           Edge_point_new_density );
}

public  void  delete_edge_points(
    hash_table_struct  *hash_table )
{
    hash_table_pointer   hash_ptr;
    edge_point_struct    *edge_info;

    initialize_hash_pointer( &hash_ptr );

    while( get_next_hash_entry( hash_table, &hash_ptr, (void **) &edge_info ) )
    {
        FREE( edge_info );
    }

    delete_hash_table( hash_table );
}

private  void  get_edge_point_keys(
    Volume               volume,
    voxel_index_struct   *voxel,
    int                  edge_intersected,
    int                  keys[] )
{
    int   sizes[N_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    keys[0] = IJK( voxel->i[X], voxel->i[Y], voxel->i[Z], sizes[Y], sizes[Z] );

    keys[1] = edge_intersected;
}

public  BOOLEAN  lookup_edge_point_id(
    Volume              volume,
    hash_table_struct   *hash_table,
    voxel_index_struct  *voxel,
    int                 edge_intersected,
    int                 *edge_point_id )
{
    int                  keys[2];
    BOOLEAN              exists;
    edge_point_struct    *edge_info;

    get_edge_point_keys( volume, voxel, edge_intersected, keys );

    exists = lookup_in_hash_table( hash_table, keys, (void **) &edge_info );

    if( exists )
        *edge_point_id = edge_info->point_index;

    return( exists );
}

#define  LEVEL  1000

public  void  record_edge_point_id(
    Volume              volume,
    hash_table_struct   *hash_table,
    voxel_index_struct  *voxel,
    int                 edge_intersected,
    int                 edge_point_id )
{
    int                  keys[2];
    edge_point_struct    *edge_info;

    get_edge_point_keys( volume, voxel, edge_intersected, keys );

    ALLOC( edge_info, 1 );

    edge_info->point_index = edge_point_id;

    insert_in_hash_table( hash_table, keys, (void *) edge_info );

#ifdef STATS
{
    static   int   current_size = 0;

    if( hash_table->n_entries / LEVEL != current_size )
    {
        print( "Edge table %d\n", hash_table->n_entries );
        current_size = hash_table->n_entries / LEVEL;
    }
}
#endif
}

public  void  remove_edge_point(
    Volume              volume,
    hash_table_struct   *hash_table,
    voxel_index_struct  *voxel,
    int                 edge_intersected )
{
    int                  keys[2];
    edge_point_struct    *edge_info;

    get_edge_point_keys( volume, voxel, edge_intersected, keys );

    if( remove_from_hash_table( hash_table, keys, (void **) &edge_info ) )
        FREE( edge_info );
}
