
#include  <def_display.h>

public  void  label_voxel_as_done(
    Volume          volume,
    int             x,
    int             y,
    int             z )
{
    int     i, j, k;
    int     sizes[N_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    for_inclusive( i, x, x+1 )
    {
        if( x < sizes[X] )
        {
            for_inclusive( j, y, y+1 )
            {
                if( y < sizes[Y] )
                {
                    for_inclusive( k, z, z+1 )
                    {
                        if( z < sizes[Z] )
                        {
                            set_voxel_label_flag( volume, i, j, k, TRUE );
                        }
                    }
                }
            }
        }
    }
}
