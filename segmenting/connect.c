
#include  <def_graphics.h>
#include  <def_connect.h>
#include  <def_alloc.h>
#include  <def_queue.h>

#define  REGION_OF_INTEREST    1

public  Status  disconnect_components( volume, voxel_indices, axis,
                                       n_labels, labels,
                                       min_threshold, max_threshold )
    volume_struct   *volume;
    int             voxel_indices[3];
    int             axis[3];
    int             n_labels;
    label_struct    labels[];
    int             min_threshold;
    int             max_threshold;
{
    Status        status;
    int           val, i;
    void          assign_region_flags();
    pixel_struct  **pixels;
    int           x, y, index[3], size[3];
    void          set_voxel_activity_flag();
    void          get_volume_size();
    Boolean       inside;
    Status        label_components();

    get_volume_size( volume, &size[X], &size[Y], &size[Z] );

    ALLOC2D( status, pixels, size[axis[X]], size[axis[Y]] );

    index[axis[Z]] = voxel_indices[axis[Z]];
    for_less( x, 0, size[axis[X]] )
    {
        index[axis[X]] = x;
        for_less( y, 0, size[axis[Y]] )
        {
            index[axis[Y]] = y;

            if( get_voxel_activity_flag( volume, index[0], index[1], index[2]))
            {
                GET_VOLUME_DATA( val, *volume, index[0], index[1], index[2] );

                inside = (val >= min_threshold && val <=max_threshold);
            }
            else
                inside = FALSE;

            pixels[x][y].inside = inside;
            pixels[x][y].label = 0;
        }
    }

    for_less( i, 0, n_labels )
    {
        if( labels[i].voxel_indices[axis[Z]] == voxel_indices[Z] )
        {
            pixels[labels[i].voxel_indices[axis[X]]]
                  [labels[i].voxel_indices[axis[Y]]].label = labels[i].id;
        }
    }

    status = label_components( size[axis[X]], size[axis[Y]], pixels,
                               REGION_OF_INTEREST );

    index[axis[Z]] = voxel_indices[axis[Z]];
    for_less( x, 0, size[axis[X]] )
    {
        index[axis[X]] = x;
        for_less( y, 0, size[axis[Y]] )
        {
            index[axis[Y]] = y;

            if( pixels[x][y].inside )
            {
                if( pixels[x][y].label != REGION_OF_INTEREST )
                    set_voxel_activity_flag( volume, index[0], index[1],
                                             index[2], FALSE );
                else
                    set_voxel_activity_flag( volume, index[0], index[1],
                                             index[2], TRUE );
            }
        }
    }

    FREE2D( status, pixels );

    return( status );
}
