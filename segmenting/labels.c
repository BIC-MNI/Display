#include  <display.h>

public  void  modify_labels_in_range(
    Volume   volume,
    Volume   label_volume,
    int      src_label,
    int      dest_label,
    Real     min_threshold,
    Real     max_threshold )
{
    int      voxel[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS];
    BOOLEAN  must_change;
    Real     value;

    get_volume_sizes( volume, sizes );

    for_less( voxel[X], 0, sizes[X] )
    {
        for_less( voxel[Y], 0, sizes[Y] )
        {
            for_less( voxel[Z], 0, sizes[Z] )
            {
                must_change = (src_label == -1 ||
                    get_volume_label_data( label_volume, voxel ) == src_label);

                if( must_change && min_threshold < max_threshold )
                {
                    GET_VALUE_3D( value, volume, voxel[X], voxel[Y], voxel[Z] );
                    if( value < min_threshold || value > max_threshold )
                        must_change = FALSE;
                }

                if( must_change )
                    set_volume_label_data( label_volume, voxel, dest_label );
            }
        }
    }
}
