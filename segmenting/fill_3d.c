#include  <display.h>

typedef struct
{
    int  x, y, z;
} xyz_struct;

public  void  fill_connected_voxels_3d(
    Volume              volume,
    Volume              label_volume,
    Neighbour_types     connectivity,
    int                 voxel[],
    int                 min_label_threshold,
    int                 max_label_threshold,
    int                 desired_label,
    Real                min_threshold,
    Real                max_threshold )
{
    int                          dir, n_dirs, *dx, *dy, *dz;
    int                          x, y, z, tx, ty, tz;
    int                          sizes[N_DIMENSIONS];
    int                          voxel_index[MAX_DIMENSIONS];
    xyz_struct                   entry;
    QUEUE_STRUCT( xyz_struct )   queue;
    bitlist_3d_struct            checked_flags, change_flags;
    int                          n_done;
    Real                         val;
    const  Real                  update_every = 10.0;
    Real                         next_message_time;

    if( !should_change_this_one( volume, label_volume, voxel,
                                 min_threshold, max_threshold,
                                 min_label_threshold, max_label_threshold,
                                 desired_label ) )
        return;

    n_dirs = get_3D_neighbour_directions( connectivity, &dx, &dy, &dz );

    get_volume_sizes( volume, sizes );

    create_bitlist_3d( sizes[X], sizes[Y], sizes[Z], &checked_flags );
    create_bitlist_3d( sizes[X], sizes[Y], sizes[Z], &change_flags );

    INITIALIZE_QUEUE( queue );

    set_bitlist_bit_3d( &checked_flags, voxel[X], voxel[Y], voxel[Z], TRUE );
    set_bitlist_bit_3d( &change_flags, voxel[X], voxel[Y], voxel[Z], TRUE );

    entry.x = voxel[X];
    entry.y = voxel[Y];
    entry.z = voxel[Z];
    INSERT_IN_QUEUE( queue, entry );

    n_done = 0;
    next_message_time = current_realtime_seconds() + update_every;

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;
        z = entry.z;

        for_less( dir, 0, n_dirs )
        {
            tx = x + dx[dir];
            ty = y + dy[dir];
            tz = z + dz[dir];

            if( tx >= 0 && tx < sizes[X] &&
                ty >= 0 && ty < sizes[Y] &&
                tz >= 0 && tz < sizes[Z] &&
                !get_bitlist_bit_3d( &checked_flags, tx, ty, tz ) )
            {
                set_bitlist_bit_3d( &checked_flags, tx, ty, tz, TRUE );

                voxel_index[X] = tx;
                voxel_index[Y] = ty;
                voxel_index[Z] = tz;
                if( should_change_this_one( volume, label_volume, voxel_index,
                                 min_threshold, max_threshold,
                                 min_label_threshold, max_label_threshold,
                                 desired_label ) )
                {
                    set_bitlist_bit_3d( &change_flags, tx, ty, tz, TRUE );
                    entry.x = tx;
                    entry.y = ty;
                    entry.z = tz;
                    INSERT_IN_QUEUE( queue, entry );
                }
            }
        }

        if( (++n_done % 20) == 0 &&
            current_realtime_seconds() > next_message_time )
        {
            next_message_time = current_realtime_seconds() + update_every;
            print( "N in queue = %d\n", NUMBER_IN_QUEUE(queue) );
        }
    }

    for_less( voxel_index[X], 0, sizes[X] )
    for_less( voxel_index[Y], 0, sizes[Y] )
    for_less( voxel_index[Z], 0, sizes[Z] )
    {
        if( get_bitlist_bit_3d( &change_flags, voxel_index[X], voxel_index[Y],
                                voxel_index[Z] ) )
        {
            set_volume_label_data( label_volume, voxel_index, desired_label );
        }
    }

    delete_bitlist_3d( &checked_flags );
    delete_bitlist_3d( &change_flags );

    DELETE_QUEUE( queue );
}
