
#include  <def_display.h>

public  void  label_voxel_as_done(
    volume_struct   *volume,
    int             x,
    int             y,
    int             z )
{
    int                    i, j, k;
    int                    nx, ny, nz;

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
