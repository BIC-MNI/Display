#include  <display.h>

public  Status   input_volume_file(
    char           filename[],
    Volume         *volume )
{
    Status     status;
    nc_type    nc_data_type;
    BOOLEAN    signed_flag;
    Real       voxel_min, voxel_max;

    if( Convert_volumes_to_byte )
    {
        nc_data_type = NC_BYTE;
        signed_flag = FALSE;
        voxel_min = 0.0;
        voxel_max = 255.0;
    }
    else
    {
        nc_data_type = NC_UNSPECIFIED;
        signed_flag = FALSE;
        voxel_min = 0.0;
        voxel_max = 0.0;
    }

    status = input_volume( filename, 3, XYZ_dimension_names,
                           nc_data_type, signed_flag, voxel_min, voxel_max,
                           TRUE, volume, (minc_input_options *) NULL );

    if( get_volume_n_dimensions( *volume ) != N_DIMENSIONS )
    {
        print( "Volume %s has %d dimensions, should have %d\n",
               filename, get_volume_n_dimensions(*volume), N_DIMENSIONS );
        delete_volume( *volume );
        status = ERROR;
    }

    return( status );
}
