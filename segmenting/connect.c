#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

#define  REGION_OF_INTEREST    1

public  void  disconnect_components(
    Volume          volume,
    int             voxel_indices[],
    int             axis[],
    int             n_labels,
    label_struct    labels[],
    VIO_Real            min_threshold,
    VIO_Real            max_threshold )
{
    VIO_Real          val;
    int           i;
    pixel_struct  **pixels;
    int           x, y, index[N_DIMENSIONS], size[N_DIMENSIONS];
    VIO_BOOL       inside;

    get_volume_sizes( volume, size );

    ALLOC2D( pixels, size[axis[VIO_X]], size[axis[VIO_Y]] );

    index[axis[VIO_Z]] = voxel_indices[axis[VIO_Z]];
    for_less( x, 0, size[axis[VIO_X]] )
    {
        index[axis[VIO_X]] = x;
        for_less( y, 0, size[axis[VIO_Y]] )
        {
            index[axis[VIO_Y]] = y;

            if( get_voxel_activity_flag( volume, index ) )
            {
                GET_VALUE_3D( val, volume, index[0], index[1], index[2] );

                inside = (val >= min_threshold && val <= max_threshold);
            }
            else
                inside = FALSE;

            pixels[x][y].inside = inside;
            pixels[x][y].label = 0;
        }
    }

    for_less( i, 0, n_labels )
    {
        if( labels[i].voxel_indices[axis[VIO_Z]] == voxel_indices[VIO_Z] )
        {
            pixels[labels[i].voxel_indices[axis[VIO_X]]]
                  [labels[i].voxel_indices[axis[VIO_Y]]].label = labels[i].id;
        }
    }

    label_components( size[axis[VIO_X]], size[axis[VIO_Y]], pixels,
                      REGION_OF_INTEREST );

    index[axis[VIO_Z]] = voxel_indices[axis[VIO_Z]];
    for_less( x, 0, size[axis[VIO_X]] )
    {
        index[axis[VIO_X]] = x;
        for_less( y, 0, size[axis[VIO_Y]] )
        {
            index[axis[VIO_Y]] = y;

            if( pixels[x][y].inside )
            {
                if( pixels[x][y].label != REGION_OF_INTEREST )
                    set_voxel_activity_flag( volume, index, FALSE );
                else
                    set_voxel_activity_flag( volume, index, TRUE );
            }
        }
    }

    FREE2D( pixels );
}
