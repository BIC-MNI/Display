#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

public  void  label_voxel_as_done(
    Volume          volume,
    int             x,
    int             y,
    int             z )
{
    int     voxel[N_DIMENSIONS];
    int     sizes[N_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    for_inclusive( voxel[X], x, x+1 )
    {
        if( voxel[X] < sizes[X] )
        {
            for_inclusive( voxel[Y], y, y+1 )
            {
                if( voxel[Y] < sizes[Y] )
                {
                    for_inclusive( voxel[Z], z, z+1 )
                    {
                        if( voxel[Z] < sizes[Z] )
                        {
                            set_voxel_label_flag( volume, voxel, TRUE );
                        }
                    }
                }
            }
        }
    }
}
