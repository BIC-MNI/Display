
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_bitlist.h>

public  Boolean  are_voxel_corners_active( volume, x, y, z )
    volume_struct   *volume;
    int             x, y, z;
{
    if( One_active_flag )
    return( get_voxel_activity_flag( volume, x  , y  , z   ) ||
            get_voxel_activity_flag( volume, x  , y  , z+1 ) ||
            get_voxel_activity_flag( volume, x  , y+1, z   ) ||
            get_voxel_activity_flag( volume, x  , y+1, z+1 ) ||
            get_voxel_activity_flag( volume, x+1, y  , z   ) ||
            get_voxel_activity_flag( volume, x+1, y  , z+1 ) ||
            get_voxel_activity_flag( volume, x+1, y+1, z   ) ||
            get_voxel_activity_flag( volume, x+1, y+1, z+1 ) );
    else
    return( get_voxel_activity_flag( volume, x  , y  , z   ) &&
            get_voxel_activity_flag( volume, x  , y  , z+1 ) &&
            get_voxel_activity_flag( volume, x  , y+1, z   ) &&
            get_voxel_activity_flag( volume, x  , y+1, z+1 ) &&
            get_voxel_activity_flag( volume, x+1, y  , z   ) &&
            get_voxel_activity_flag( volume, x+1, y  , z+1 ) &&
            get_voxel_activity_flag( volume, x+1, y+1, z   ) &&
            get_voxel_activity_flag( volume, x+1, y+1, z+1 ) );
}

public  void  label_voxel_as_done( volume, x, y, z )
    volume_struct   *volume;
    int             x, y, z;
{
    int                    i, j, k;
    int                    nx, ny, nz;
    void                   get_volume_size();
    void                   set_voxel_label_flag();

    get_volume_size( volume, &nx, &ny, &nz );

    for_inclusive( i, x, x+1 )
    {
        if( x < nx )
        {
            for_inclusive( j, y, y+1 )
            {
                if( y < ny )
                {
                    for_inclusive( k, z, z+1 )
                    {
                        if( z < nz )
                        {
                            set_voxel_label_flag( volume, i, j, k, TRUE );
                        }
                    }
                }
            }
        }
    }
}
