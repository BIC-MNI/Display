
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_bitlist.h>

/* ------------------ Voxel queue ------------------------ */

public  void   initialize_voxel_queue( voxel_queue )
    QUEUE_STRUCT( voxel_index_struct )  *voxel_queue;
{
    INITIALIZE_QUEUE( *voxel_queue );
}

public  Status   insert_in_voxel_queue( voxel_queue, voxel_indices )
    QUEUE_STRUCT( voxel_index_struct )  *voxel_queue;
    voxel_index_struct                  *voxel_indices;
{
    Status   status;

    INSERT_IN_QUEUE( status, *voxel_queue, voxel_index_struct, *voxel_indices );

    return( status );
}

public  void   get_next_voxel_from_queue( voxel_queue, voxel_indices )
    QUEUE_STRUCT( voxel_index_struct )  *voxel_queue;
    voxel_index_struct                  *voxel_indices;
{
    REMOVE_FROM_QUEUE( *voxel_queue, *voxel_indices );
}

public  Boolean  voxels_remaining( voxel_queue )
    QUEUE_STRUCT( voxel_index_struct )  *voxel_queue;
{
    return( !IS_QUEUE_EMPTY( *voxel_queue ) );
}

public  Status  delete_voxel_queue( voxel_queue )
    QUEUE_STRUCT( voxel_index_struct )  *voxel_queue;
{
    Status   status;

    DELETE_QUEUE( status, *voxel_queue );

    return( status );
}

/* ------------------ Voxel flag, 1 bit flag structure --------------- */

public  Status  initialize_voxel_flags( voxel_flags, n_voxels )
    bitlist_struct  *voxel_flags;
    int             n_voxels;
{
    Status   status;

    status = create_bitlist( n_voxels, voxel_flags );

    return( status );
}

public  Status  delete_voxel_flags( voxel_flags )
    bitlist_struct  *voxel_flags;
{
    Status   status;

    status = delete_bitlist( voxel_flags );

    return( status );
}

public  void  clear_voxel_flags( voxel_flags )
    bitlist_struct  *voxel_flags;
{
    zero_bitlist( voxel_flags );
}

public  Boolean  get_voxel_flag( volume, voxel_flags, indices )
    volume_struct       *volume;
    bitlist_struct      *voxel_flags;
    voxel_index_struct  *indices;
{
    return( get_bitlist_bit( voxel_flags, ijk( indices->i[X_AXIS],
                                               indices->i[Y_AXIS],
                                               indices->i[Z_AXIS],
                                               volume->size[Y_AXIS]-1,
                                               volume->size[Z_AXIS]-1 ) ) );
}

public  Status  set_voxel_flag( volume, voxel_flags, indices )
    volume_struct       *volume;
    bitlist_struct      *voxel_flags;
    voxel_index_struct  *indices;
{
    set_bitlist_bit( voxel_flags, ijk( indices->i[X_AXIS],
                                       indices->i[Y_AXIS],
                                       indices->i[Z_AXIS],
                                       volume->size[Y_AXIS]-1,
                                       volume->size[Z_AXIS]-1 ),     ON );

    return( OK );
}

public  Status  reset_voxel_flag( volume, voxel_flags, indices )
    volume_struct       *volume;
    bitlist_struct      *voxel_flags;
    voxel_index_struct  *indices;
{
    set_bitlist_bit( voxel_flags, ijk( indices->i[X_AXIS],
                                       indices->i[Y_AXIS],
                                       indices->i[Z_AXIS],
                                       volume->size[Y_AXIS]-1,
                                       volume->size[Z_AXIS]-1 ),     OFF );

    return( OK );
}

/* ----------------- edge points, hash table lookup ------------- */

#define  INITIAL_SIZE         1000

public  Status  initialize_edge_points( hash_table )
    hash_table_struct  *hash_table;
{
    Status     status;

    status = initialize_hash_table( hash_table,
                                    2, INITIAL_SIZE, Edge_point_threshold,
                                    Edge_point_new_density );

    return( status );
}

public  Status  delete_edge_points( hash_table )
    hash_table_struct  *hash_table;
{
    Status               status;
    hash_table_pointer   hash_ptr;
    edge_point_struct    *edge_info;

    initialize_hash_pointer( &hash_ptr );

    status = OK;

    while( status == OK && get_next_hash_entry( hash_table, &hash_ptr,
                                                (char **) &edge_info ) )
    {
        FREE1( status, edge_info );
    }

    if( status == OK )
    {
        status = delete_hash_table( hash_table );
    }

    return( status );
}

private  void  get_edge_point_keys( volume, voxel, edge_intersected, keys )
    volume_struct        *volume;
    voxel_index_struct   *voxel;
    int                  edge_intersected;
    int                  keys[];
{
    keys[0] = ijk( voxel->i[X_AXIS],
                   voxel->i[Y_AXIS],
                   voxel->i[Z_AXIS],
                   volume->size[Y_AXIS],
                   volume->size[Z_AXIS] );

    keys[1] = edge_intersected;
}

public  Boolean  lookup_edge_point_id( volume, hash_table,
                                       voxel, edge_intersected, edge_point_id )
    volume_struct       *volume;
    hash_table_struct   *hash_table;
    voxel_index_struct  *voxel;
    int                 edge_intersected;
    int                 *edge_point_id;
{
    int                  keys[2];
    Boolean              exists;
    edge_point_struct    *edge_info;

    get_edge_point_keys( volume, voxel, edge_intersected, keys );

    exists = lookup_in_hash_table( hash_table, keys, (char **) &edge_info );

    if( exists )
    {
        *edge_point_id = edge_info->point_index;
    }

    return( exists );
}

public  Status  record_edge_point_id( volume, hash_table,
                                      voxel, edge_intersected, edge_point_id )
    volume_struct       *volume;
    hash_table_struct   *hash_table;
    voxel_index_struct  *voxel;
    int                 edge_intersected;
    int                 edge_point_id;
{
    Status               status;
    int                  keys[2];
    edge_point_struct    *edge_info;

    get_edge_point_keys( volume, voxel, edge_intersected, keys );

    CALLOC1( status, edge_info, 1, edge_point_struct );

    edge_info->point_index = edge_point_id;

    if( status == OK )
    {
        status = insert_in_hash_table( hash_table, keys, (char *) edge_info );
    }

    return( status );
}

public  Status  remove_edge_point( volume, hash_table, voxel, edge_intersected )
    volume_struct       *volume;
    hash_table_struct   *hash_table;
    voxel_index_struct  *voxel;
    int                 edge_intersected;
{
    Status               status;
    int                  keys[2];
    edge_point_struct    *edge_info;

    status = OK;

    get_edge_point_keys( volume, voxel, edge_intersected, keys );

    if( remove_from_hash_table( hash_table, keys, (char **) &edge_info ) )
    {
        FREE1( status, edge_info );
    }

    return( status );
}
