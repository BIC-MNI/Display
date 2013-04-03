#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

typedef struct
{
    int  x, y, z;
} xyz_struct;

public  void  fill_connected_voxels_3d(
    VIO_Volume              volume,
    VIO_Volume              label_volume,
    Neighbour_types     connectivity,
    int                 voxel[],
    int                 min_label_threshold,
    int                 max_label_threshold,
    int                 desired_label,
    VIO_Real                min_threshold,
    VIO_Real                max_threshold )
{
    int                          dir, n_dirs, *dx, *dy, *dz;
    int                          x, y, z, tx, ty, tz;
    int                          sizes[N_DIMENSIONS];
    int                          voxel_index[MAX_DIMENSIONS];
    xyz_struct                   entry;
    QUEUE_STRUCT( xyz_struct )   queue;
    bitlist_3d_struct            checked_flags, change_flags;
    int                          n_done;
    const  VIO_Real                  update_every = 10.0;
    VIO_Real                         next_message_time;

    if( !should_change_this_one( volume, label_volume, voxel,
                                 min_threshold, max_threshold,
                                 min_label_threshold, max_label_threshold,
                                 desired_label ) )
        return;

    n_dirs = get_3D_neighbour_directions( connectivity, &dx, &dy, &dz );

    get_volume_sizes( volume, sizes );

    create_bitlist_3d( sizes[VIO_X], sizes[VIO_Y], sizes[VIO_Z], &checked_flags );
    create_bitlist_3d( sizes[VIO_X], sizes[VIO_Y], sizes[VIO_Z], &change_flags );

    INITIALIZE_QUEUE( queue );

    set_bitlist_bit_3d( &checked_flags, voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z], TRUE );
    set_bitlist_bit_3d( &change_flags, voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z], TRUE );

    entry.x = voxel[VIO_X];
    entry.y = voxel[VIO_Y];
    entry.z = voxel[VIO_Z];
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

            if( tx >= 0 && tx < sizes[VIO_X] &&
                ty >= 0 && ty < sizes[VIO_Y] &&
                tz >= 0 && tz < sizes[VIO_Z] &&
                !get_bitlist_bit_3d( &checked_flags, tx, ty, tz ) )
            {
                set_bitlist_bit_3d( &checked_flags, tx, ty, tz, TRUE );

                voxel_index[VIO_X] = tx;
                voxel_index[VIO_Y] = ty;
                voxel_index[VIO_Z] = tz;
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

    for_less( voxel_index[VIO_X], 0, sizes[VIO_X] )
    for_less( voxel_index[VIO_Y], 0, sizes[VIO_Y] )
    for_less( voxel_index[VIO_Z], 0, sizes[VIO_Z] )
    {
        if( get_bitlist_bit_3d( &change_flags, voxel_index[VIO_X], voxel_index[VIO_Y],
                                voxel_index[VIO_Z] ) )
        {
            set_volume_label_data( label_volume, voxel_index, desired_label );
        }
    }

    delete_bitlist_3d( &checked_flags );
    delete_bitlist_3d( &change_flags );

    DELETE_QUEUE( queue );
}
