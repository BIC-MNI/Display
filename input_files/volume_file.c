#include  <display.h>

public  Status   input_volume_file(
    char           filename[],
    Volume         *volume )
{
    Status              status;
    nc_type             nc_data_type;
    BOOLEAN             signed_flag;
    Real                voxel_min, voxel_max;
    minc_input_options  options;

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

    set_default_minc_input_options( &options );
    set_minc_input_vector_to_colour_flag( &options, TRUE );

    status = input_volume( filename, 3, XYZ_dimension_names,
                           nc_data_type, signed_flag, voxel_min, voxel_max,
                           TRUE, volume, &options );

    if( status == OK &&
        get_volume_n_dimensions( *volume ) != N_DIMENSIONS )
    {
        print( "Volume %s has %d dimensions, should have %d\n",
               filename, get_volume_n_dimensions(*volume), N_DIMENSIONS );
        delete_volume( *volume );
        status = ERROR;
    }

    return( status );
}
