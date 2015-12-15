/**
 * \file volume_file.c
 * \brief Function to load a volume (e.g. MINC) file.
 *
 * \copyright
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

/** We limit ourselves to 4-dimensional volumes at the moment. RGB volumes
 * (containing a vector_dimension of length three) are a special case.
 */
#define MAX_VOLUME_DIMENSIONS 4

/** This is the order of the dimensions we want for all loaded volumes.
 */
char *XYZT_dimension_names[MAX_VOLUME_DIMENSIONS] = { 
  MIxspace, MIyspace, MIzspace, MItime
};

/**
 * Read a volume file using the volume_io library call input_volume().
 * Also applies cropping and voxel type conversion if specified by the
 * global state.
 * \param filename The path of the file to read.
 * \param volume_ptr A pointer to a VIO_Volume handle that represents
 * the loaded volume.
 * \returns VIO_OK on success, VIO_ERROR on failure.
 */ 
VIO_Status   input_volume_file(
    VIO_STR         filename,
    VIO_Volume      *volume_ptr )
{
    VIO_Status          status;
    nc_type             nc_data_type;
    VIO_BOOL            signed_flag;
    VIO_Real            voxel_min, voxel_max, size_factor;
    minc_input_options  options;
    int                 dim, limits[2][VIO_MAX_DIMENSIONS];
    int                 sizes[VIO_MAX_DIMENSIONS];
    VIO_Volume          volume, cropped_volume;

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

    status = input_volume( filename, MAX_VOLUME_DIMENSIONS,
                           XYZT_dimension_names,
                           nc_data_type, signed_flag, voxel_min, voxel_max,
                           TRUE, &volume, &options );

    if( status == VIO_OK && get_volume_n_dimensions( volume ) < VIO_N_DIMENSIONS )
    {
        print( "Volume %s has %d dimensions, should have at least %d\n",
               filename, get_volume_n_dimensions(volume), VIO_N_DIMENSIONS );
        delete_volume( volume );
        status = VIO_ERROR;
    }

    if( Crop_volumes_on_input )
    {
        if( !find_volume_crop_bounds( volume, -1.0e30, 0.0, limits ) )
        {
            for_less( dim, 0, VIO_N_DIMENSIONS )
            {
                limits[0][dim] = 0;
                limits[1][dim] = 0;
            }
        }

        size_factor = 1.0;
        get_volume_sizes( volume, sizes );

        for_less( dim, 0, VIO_N_DIMENSIONS )
        {
            size_factor *= (VIO_Real) (limits[1][dim] - limits[0][dim] + 1) /
                           (VIO_Real) sizes[dim];
        }

        if( size_factor <= Crop_if_smaller )
        {
            print( "Cropping volume to %3.0f %% size.\n",
                   size_factor * 100.0 + 0.5 );
            cropped_volume = create_cropped_volume( volume, limits );
            delete_volume( volume );
            volume = cropped_volume;
        }
    }

    if( status == VIO_OK )
        *volume_ptr = volume;

    return( status );
}
