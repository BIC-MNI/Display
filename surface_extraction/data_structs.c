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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/surface_extraction/data_structs.c,v 1.28 2001/05/27 00:19:57 stever Exp $";
#endif


#include  <display.h>

/* ------------------ Voxel queue ------------------------ */

public  void   initialize_voxel_queue(
    voxel_queue_struct  *voxel_queue )
{
    INITIALIZE_QUEUE( *voxel_queue );
}

public  void   insert_in_voxel_queue(
    voxel_queue_struct  *voxel_queue,
    int                 voxel[] )
{
    voxel_index_struct   voxel_indices;

    voxel_indices.i[X] = (short) voxel[X];
    voxel_indices.i[Y] = (short) voxel[Y];
    voxel_indices.i[Z] = (short) voxel[Z];
    INSERT_IN_QUEUE( *voxel_queue, voxel_indices );
}

public  void   get_next_voxel_from_queue(
    voxel_queue_struct    *voxel_queue,
    int                   voxel[] )
{
    voxel_index_struct   voxel_indices;

    REMOVE_FROM_QUEUE( *voxel_queue, voxel_indices );

    voxel[X] = (int) voxel_indices.i[X];
    voxel[Y] = (int) voxel_indices.i[Y];
    voxel[Z] = (int) voxel_indices.i[Z];
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
    bitlist_3d_struct  *voxel_flags,
    int                min_limits[],
    int                max_limits[] )
{
    create_bitlist_3d( max_limits[X] - min_limits[X] + 1,
                       max_limits[Y] - min_limits[Y] + 1,
                       max_limits[Z] - min_limits[Z] + 1, voxel_flags );
}

public  void  delete_voxel_flags(
    bitlist_3d_struct  *voxel_flags )
{
    delete_bitlist_3d( voxel_flags );
}

public  void  clear_voxel_flags(
    bitlist_3d_struct  *voxel_flags )
{
    zero_bitlist_3d( voxel_flags );
}

public  BOOLEAN  get_voxel_flag(
    bitlist_3d_struct   *voxel_flags,
    int                 min_limits[],
    int                 voxel[] )
{
    return( get_bitlist_bit_3d( voxel_flags,
                                voxel[X] - min_limits[X],
                                voxel[Y] - min_limits[Y],
                                voxel[Z] - min_limits[Z] ) );
}

public  void  set_voxel_flag(
    bitlist_3d_struct      *voxel_flags,
    int                    min_limits[],
    int                    voxel[] )
{
    set_bitlist_bit_3d( voxel_flags,
                        voxel[X] - min_limits[X],
                        voxel[Y] - min_limits[Y],
                        voxel[Z] - min_limits[Z], ON );
}

public  void  reset_voxel_flag(
    bitlist_3d_struct      *voxel_flags,
    int                    min_limits[],
    int                    voxel[] )
{
    set_bitlist_bit_3d( voxel_flags,
                        voxel[X] - min_limits[X],
                        voxel[Y] - min_limits[Y],
                        voxel[Z] - min_limits[Z], OFF );
}

/* ------------------ Voxel done flags, 4 bit flag structure --------------- */

public  void  initialize_voxel_done_flags(
    unsigned_byte   **voxel_done_flags,
    int             min_limits[],
    int             max_limits[] )
{
    int   n_voxels;

    n_voxels = (max_limits[X] - min_limits[X] + 1) *
               (max_limits[Y] - min_limits[Y] + 1) *
               (max_limits[Z] - min_limits[Z] + 1);

    if( n_voxels > 0 )
    {
        ALLOC( *voxel_done_flags, (n_voxels + 1) / 2 );

        clear_voxel_done_flags( *voxel_done_flags, min_limits, max_limits );
    }
}

public  void  delete_voxel_done_flags(
    unsigned_byte  voxel_done_flags[] )
{
    if( voxel_done_flags != NULL )
        FREE( voxel_done_flags );
}

public  void  clear_voxel_done_flags(
    unsigned_byte   voxel_done_flags[],
    int             min_limits[],
    int             max_limits[] )
{
    int   i, n_voxels;

    n_voxels = (max_limits[X] - min_limits[X] + 1) *
               (max_limits[Y] - min_limits[Y] + 1) *
               (max_limits[Z] - min_limits[Z] + 1);

    for_less( i, 0, (n_voxels + 1) / 2 )
        voxel_done_flags[i] = 0;
}

public  unsigned_byte  get_voxel_done_flag(
    int                 min_limits[],
    int                 max_limits[],
    unsigned_byte       voxel_done_flags[],
    int                 voxel[] )
{
    int            index, byte_index;
    unsigned_byte  flag;

    index = IJK( voxel[X] - min_limits[X],
                 voxel[Y] - min_limits[Y],
                 voxel[Z] - min_limits[Z],
                 max_limits[Y] - min_limits[Y] + 1, 
                 max_limits[Z] - min_limits[Z] + 1 );

    byte_index = index >> 1;

    if( index & 1 )
        flag = (unsigned_byte) ((int) voxel_done_flags[byte_index] >> 4);
    else
        flag = (unsigned_byte) ((int) voxel_done_flags[byte_index] & 15);

    return( flag );
}

public  void  set_voxel_done_flag(
    int                 min_limits[],
    int                 max_limits[],
    unsigned_byte       voxel_done_flags[],
    int                 voxel[],
    unsigned_byte       flag )
{
    int            index, byte_index;

    index = IJK( voxel[X] - min_limits[X],
                 voxel[Y] - min_limits[Y],
                 voxel[Z] - min_limits[Z],
                 max_limits[Y] - min_limits[Y] + 1, 
                 max_limits[Z] - min_limits[Z] + 1 );

    byte_index = index >> 1;

    if( index & 1 )
        voxel_done_flags[byte_index] = (unsigned_byte)
                 (((int) voxel_done_flags[byte_index] & 15) |
                                       ((int) flag << 4));
    else
        voxel_done_flags[byte_index] = (unsigned_byte)
                 (((int) voxel_done_flags[byte_index] & 240) | (int) flag);
}

/* ----------------- edge points, hash table lookup ------------- */

#define  INITIAL_SIZE         1000

public  void  initialize_edge_points(
    hash_table_struct  *hash_table )
{
    initialize_hash_table( hash_table, INITIAL_SIZE, sizeof(int),
                           Edge_point_threshold, Edge_point_new_density );
}

public  void  delete_edge_points(
    hash_table_struct  *hash_table )
{
    delete_hash_table( hash_table );
}

private  int  get_edge_point_key(
    int                  sizes[],
    int                  x,
    int                  y,
    int                  z,
    int                  edge )
{
    int   key;

    key = IJK( x, y, z, sizes[Y]+1,sizes[Z]+1) * 6 + edge;

    return( key );
}

public  BOOLEAN  lookup_edge_point_id(
    int                 sizes[],
    hash_table_struct   *hash_table,
    int                 x,
    int                 y,
    int                 z,
    int                 edge_intersected,
    int                 *edge_point_id )
{
    int                  key;
    BOOLEAN              exists;

    key = get_edge_point_key( sizes, x, y, z, edge_intersected );

    exists = lookup_in_hash_table( hash_table, key, (void *) edge_point_id );

    return( exists );
}

#define  LEVEL  1000

public  void  record_edge_point_id(
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

public  void  remove_edge_point(
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
