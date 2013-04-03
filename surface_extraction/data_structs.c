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

/* ------------------ Voxel queue ------------------------ */

  void   initialize_voxel_queue(
    voxel_queue_struct  *voxel_queue )
{
    INITIALIZE_QUEUE( *voxel_queue );
}

  void   insert_in_voxel_queue(
    voxel_queue_struct  *voxel_queue,
    int                 voxel[] )
{
    voxel_index_struct   voxel_indices;

    voxel_indices.i[VIO_X] = (short) voxel[VIO_X];
    voxel_indices.i[VIO_Y] = (short) voxel[VIO_Y];
    voxel_indices.i[VIO_Z] = (short) voxel[VIO_Z];
    INSERT_IN_QUEUE( *voxel_queue, voxel_indices );
}

  void   get_next_voxel_from_queue(
    voxel_queue_struct    *voxel_queue,
    int                   voxel[] )
{
    voxel_index_struct   voxel_indices;

    REMOVE_FROM_QUEUE( *voxel_queue, voxel_indices );

    voxel[VIO_X] = (int) voxel_indices.i[VIO_X];
    voxel[VIO_Y] = (int) voxel_indices.i[VIO_Y];
    voxel[VIO_Z] = (int) voxel_indices.i[VIO_Z];
}

  VIO_BOOL  voxels_remaining(
    voxel_queue_struct  *voxel_queue )
{
    return( !IS_QUEUE_EMPTY( *voxel_queue ) );
}

  void  delete_voxel_queue(
    voxel_queue_struct   *voxel_queue )
{
    DELETE_QUEUE( *voxel_queue );
}

/* ------------------ Voxel flag, 1 bit flag structure --------------- */

  void  initialize_voxel_flags(
    bitlist_3d_struct  *voxel_flags,
    int                min_limits[],
    int                max_limits[] )
{
    create_bitlist_3d( max_limits[VIO_X] - min_limits[VIO_X] + 1,
                       max_limits[VIO_Y] - min_limits[VIO_Y] + 1,
                       max_limits[VIO_Z] - min_limits[VIO_Z] + 1, voxel_flags );
}

  void  delete_voxel_flags(
    bitlist_3d_struct  *voxel_flags )
{
    delete_bitlist_3d( voxel_flags );
}

  void  clear_voxel_flags(
    bitlist_3d_struct  *voxel_flags )
{
    zero_bitlist_3d( voxel_flags );
}

  VIO_BOOL  get_voxel_flag(
    bitlist_3d_struct   *voxel_flags,
    int                 min_limits[],
    int                 voxel[] )
{
    return( get_bitlist_bit_3d( voxel_flags,
                                voxel[VIO_X] - min_limits[VIO_X],
                                voxel[VIO_Y] - min_limits[VIO_Y],
                                voxel[VIO_Z] - min_limits[VIO_Z] ) );
}

  void  set_voxel_flag(
    bitlist_3d_struct      *voxel_flags,
    int                    min_limits[],
    int                    voxel[] )
{
    set_bitlist_bit_3d( voxel_flags,
                        voxel[VIO_X] - min_limits[VIO_X],
                        voxel[VIO_Y] - min_limits[VIO_Y],
                        voxel[VIO_Z] - min_limits[VIO_Z], TRUE );
}

  void  reset_voxel_flag(
    bitlist_3d_struct      *voxel_flags,
    int                    min_limits[],
    int                    voxel[] )
{
    set_bitlist_bit_3d( voxel_flags,
                        voxel[VIO_X] - min_limits[VIO_X],
                        voxel[VIO_Y] - min_limits[VIO_Y],
                        voxel[VIO_Z] - min_limits[VIO_Z], FALSE );
}

/* ------------------ Voxel done flags, 4 bit flag structure --------------- */

  void  initialize_voxel_done_flags(
    VIO_UCHAR   **voxel_done_flags,
    int             min_limits[],
    int             max_limits[] )
{
    int   n_voxels;

    n_voxels = (max_limits[VIO_X] - min_limits[VIO_X] + 1) *
               (max_limits[VIO_Y] - min_limits[VIO_Y] + 1) *
               (max_limits[VIO_Z] - min_limits[VIO_Z] + 1);

    if( n_voxels > 0 )
    {
        ALLOC( *voxel_done_flags, (n_voxels + 1) / 2 );

        clear_voxel_done_flags( *voxel_done_flags, min_limits, max_limits );
    }
}

  void  delete_voxel_done_flags(
    VIO_UCHAR  voxel_done_flags[] )
{
    if( voxel_done_flags != NULL )
        FREE( voxel_done_flags );
}

  void  clear_voxel_done_flags(
    VIO_UCHAR   voxel_done_flags[],
    int             min_limits[],
    int             max_limits[] )
{
    int   i, n_voxels;

    n_voxels = (max_limits[VIO_X] - min_limits[VIO_X] + 1) *
               (max_limits[VIO_Y] - min_limits[VIO_Y] + 1) *
               (max_limits[VIO_Z] - min_limits[VIO_Z] + 1);

    for_less( i, 0, (n_voxels + 1) / 2 )
        voxel_done_flags[i] = 0;
}

  VIO_UCHAR  get_voxel_done_flag(
    int                 min_limits[],
    int                 max_limits[],
    VIO_UCHAR       voxel_done_flags[],
    int                 voxel[] )
{
    int            index, byte_index;
    VIO_UCHAR  flag;

    index = VIO_IJK( voxel[VIO_X] - min_limits[VIO_X],
                 voxel[VIO_Y] - min_limits[VIO_Y],
                 voxel[VIO_Z] - min_limits[VIO_Z],
                 max_limits[VIO_Y] - min_limits[VIO_Y] + 1, 
                 max_limits[VIO_Z] - min_limits[VIO_Z] + 1 );

    byte_index = index >> 1;

    if( index & 1 )
        flag = (VIO_UCHAR) ((int) voxel_done_flags[byte_index] >> 4);
    else
        flag = (VIO_UCHAR) ((int) voxel_done_flags[byte_index] & 15);

    return( flag );
}

  void  set_voxel_done_flag(
    int                 min_limits[],
    int                 max_limits[],
    VIO_UCHAR       voxel_done_flags[],
    int                 voxel[],
    VIO_UCHAR       flag )
{
    int            index, byte_index;

    index = VIO_IJK( voxel[VIO_X] - min_limits[VIO_X],
                 voxel[VIO_Y] - min_limits[VIO_Y],
                 voxel[VIO_Z] - min_limits[VIO_Z],
                 max_limits[VIO_Y] - min_limits[VIO_Y] + 1, 
                 max_limits[VIO_Z] - min_limits[VIO_Z] + 1 );

    byte_index = index >> 1;

    if( index & 1 )
        voxel_done_flags[byte_index] = (VIO_UCHAR)
                 (((int) voxel_done_flags[byte_index] & 15) |
                                       ((int) flag << 4));
    else
        voxel_done_flags[byte_index] = (VIO_UCHAR)
                 (((int) voxel_done_flags[byte_index] & 240) | (int) flag);
}

/* ----------------- edge points, hash table lookup ------------- */

#define  INITIAL_SIZE         1000

  void  initialize_edge_points(
    hash_table_struct  *hash_table )
{
    initialize_hash_table( hash_table, INITIAL_SIZE, sizeof(int),
                           Edge_point_threshold, Edge_point_new_density );
}

  void  delete_edge_points(
    hash_table_struct  *hash_table )
{
    delete_hash_table( hash_table );
}

static  int  get_edge_point_key(
    int                  sizes[],
    int                  x,
    int                  y,
    int                  z,
    int                  edge )
{
    int   key;

    key = VIO_IJK( x, y, z, sizes[VIO_Y]+1,sizes[VIO_Z]+1) * 6 + edge;

    return( key );
}

  VIO_BOOL  lookup_edge_point_id(
    int                 sizes[],
    hash_table_struct   *hash_table,
    int                 x,
    int                 y,
    int                 z,
    int                 edge_intersected,
    int                 *edge_point_id )
{
    int                  key;
    VIO_BOOL              exists;

    key = get_edge_point_key( sizes, x, y, z, edge_intersected );

    exists = lookup_in_hash_table( hash_table, key, (void *) edge_point_id );

    return( exists );
}

#define  LEVEL  1000

  void  record_edge_point_id(
    int                 sizes[],
    hash_table_struct   *hash_table,
    int                 x,
    int                 y,
    int                 z,
    int                 edge_intersected,
    int                 edge_point_id )
{
    int   key;

    key = get_edge_point_key( sizes, x, y, z, edge_intersected );

    insert_in_hash_table( hash_table, key, (void *) &edge_point_id );

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

  void  remove_edge_point(
    int                 sizes[],
    hash_table_struct   *hash_table,
    int                 x,
    int                 y,
    int                 z,
    int                 edge_intersected )
{
    int                  key;

    key = get_edge_point_key( sizes, x, y, z, edge_intersected );

    (void) remove_from_hash_table( hash_table, key, NULL );
}
