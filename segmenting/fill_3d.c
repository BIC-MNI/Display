#include  <display.h>

typedef struct
{
    int  x, y, z;
} xyz_struct;

public  void  fill_connected_voxels_3d(
    Volume          volume,
    Volume          label_volume,
    int             voxel[],
    int             label,
    Real            min_threshold,
    Real            max_threshold )
{
    int                          x, y, z, tx, ty, tz, dx, dy, dz;
    int                          sizes[N_DIMENSIONS];
    int                          voxel_index[MAX_DIMENSIONS];
    xyz_struct                   entry;
    QUEUE_STRUCT( xyz_struct )   queue;
    bitlist_3d_struct            bitlist;
    int                          n_done;
    Real                         val;
    const  Real                  update_every = 10.0;
    Real                         next_message_time;

    get_volume_sizes( volume, sizes );

    create_bitlist_3d( sizes[X], sizes[Y], sizes[Z], &bitlist );

    INITIALIZE_QUEUE( queue );

    set_bitlist_bit_3d( &bitlist, voxel[X], voxel[Y], voxel[Z], TRUE );
    if( get_volume_label_data( label_volume, voxel ) != label )
    {
        GET_VALUE_3D( val, volume, voxel[X], voxel[Y], voxel[Z] );
        if( min_threshold > max_threshold ||
            val >= min_threshold && val <= max_threshold )
        {
            set_volume_label_data( label_volume, voxel, label );

            entry.x = voxel[X];
            entry.y = voxel[Y];
            entry.z = voxel[Z];
            INSERT_IN_QUEUE( queue, entry );
        }
    }

    n_done = 0;
    next_message_time = current_realtime_seconds() + update_every;

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;
        z = entry.z;

        for_inclusive( dx, -1, 1 )
        for_inclusive( dy, -1, 1 )
        for_inclusive( dz, -1, 1 )
        if( dx != 0 || dy != 0 || dz != 0 )
        {
            tx = x + dx;
            ty = y + dy;
            tz = z + dz;

            if( tx >= 0 && tx < sizes[X] &&
                ty >= 0 && ty < sizes[Y] &&
                tz >= 0 && tz < sizes[Z] &&
                !get_bitlist_bit_3d( &bitlist, tx, ty, tz ) )
            {
                set_bitlist_bit_3d( &bitlist, tx, ty, tz, TRUE );

                voxel_index[X] = tx;
                voxel_index[Y] = ty;
                voxel_index[Z] = tz;
                if( get_volume_label_data( label_volume, voxel_index ) != label)
                {
                    GET_VALUE_3D( val, volume, tx, ty, tz );
                    if( min_threshold > max_threshold ||
                        val >= min_threshold && val <= max_threshold )
                    {
                        set_volume_label_data( label_volume, voxel_index,label);
                        entry.x = tx;
                        entry.y = ty;
                        entry.z = tz;
                        INSERT_IN_QUEUE( queue, entry );
                    }
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

    delete_bitlist_3d( &bitlist );

    DELETE_QUEUE( queue );
}
