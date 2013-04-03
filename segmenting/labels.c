#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include  <display.h>

public  void  modify_labels_in_range(
    Volume   volume,
    Volume   label_volume,
    int      src_label,
    int      dest_label,
    VIO_Real     min_threshold,
    VIO_Real     max_threshold )
{
    int              voxel[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS];
    VIO_BOOL          must_change;
    VIO_Real             value;
    progress_struct  progress;

    get_volume_sizes( volume, sizes );

    initialize_progress_report( &progress, FALSE, sizes[VIO_X] * sizes[VIO_Y],
                                "Modifying Labels" );


    for_less( voxel[VIO_X], 0, sizes[VIO_X] )
    {
        for_less( voxel[VIO_Y], 0, sizes[VIO_Y] )
        {
            for_less( voxel[VIO_Z], 0, sizes[VIO_Z] )
            {
                must_change = (src_label == -1 ||
                    get_volume_label_data( label_volume, voxel ) == src_label);

                if( must_change && min_threshold < max_threshold )
                {
                    value = get_volume_real_value( volume, voxel[VIO_X], voxel[VIO_Y],
                                                   voxel[VIO_Z], 0, 0 );
                    if( value < min_threshold || value > max_threshold )
                        must_change = FALSE;
                }

                if( must_change )
                    set_volume_label_data( label_volume, voxel, dest_label );
            }

            update_progress_report( &progress, voxel[VIO_X] * sizes[VIO_Y] +
                                    voxel[VIO_Y] + 1 );
        }
    }

    terminate_progress_report( &progress );
}
