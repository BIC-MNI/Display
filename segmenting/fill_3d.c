#include  <def_display.h>

typedef struct
{
    int  x, y, z;
} xyz_struct;

public  void  fill_connected_voxels_3d(
    Volume          volume,
    int             x,
    int             y,
    int             z,
    int             min_threshold,
    int             max_threshold )
{
    int                          tx, ty, tz, dx, dy, dz;
    int                          sizes[N_DIMENSIONS];
    xyz_struct                   entry;
    QUEUE_STRUCT( xyz_struct )   queue;
    bitlist_3d_struct            bitlist;
    int                          n_done, val;
    const  Real                  update_every = 10.0;
    Real                         next_message_time;

    get_volume_sizes( volume, sizes );

    create_bitlist_3d( sizes[X], sizes[Y], sizes[Z], &bitlist );

    INITIALIZE_QUEUE( queue );

    set_bitlist_bit_3d( &bitlist, x, y, z, TRUE );
    if( get_voxel_activity_flag( volume, x, y, z ) )
    {
        GET_VOXEL_3D( val, volume, x, y, z );
        if( val >= min_threshold && val <= max_threshold )
        {
            set_voxel_label_flag( volume, x, y, z, TRUE );

            entry.x = x;
            entry.y = y;
            entry.z = z;
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

                if( get_voxel_activity_flag( volume, tx, ty, tz ) )
                {
                    GET_VOXEL_3D( val, volume, tx, ty, tz );
                    if( val >= min_threshold && val <= max_threshold )
                    {
                        set_voxel_label_flag( volume, tx, ty, tz, TRUE );
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
