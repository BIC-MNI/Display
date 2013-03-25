#include  <mni.h>

public  void  initialize_segmenting_3d(
    Volume    volume,
    Volume    label_volume,
    int       n_dimensions,
    int       voxel_pos,
    int       axis,
    Real      min_threshold,
    Real      max_threshold,
    unsigned char      ****distance_transform,
    unsigned char      ****cuts,
    bitlist_3d_struct  *to_do );

public  VIO_BOOL  expand_labels_3d(
    Volume             label_volume,
    unsigned char      ***distance_transform,
    unsigned char      ***cuts,
    bitlist_3d_struct  *to_do,
    int       n_dimensions,
    int       voxel_pos,
    int       axis );

int  main(
    int   argc,
    char  *argv[] )
{
    Status               status;
    FILE                 *file;
    char                 *input_volume_filename;
    char                 *input_labels_filename;
    char                 *output_labels_filename;
    Volume               volume, label_volume;
    unsigned char        ***dist_transform, ***cuts;
    bitlist_3d_struct    to_do;
    Real                 min_threshold, max_threshold;
    int                  iteration;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &input_volume_filename ) ||
        !get_string_argument( "", &input_labels_filename ) ||
        !get_real_argument( 0.0, &min_threshold ) ||
        !get_real_argument( 0.0, &max_threshold ) ||
        !get_string_argument( "", &output_labels_filename ) )
    {
        print( "Usage: %s  volume labels  min_threshold max_threshold out_labels\n", argv[0] );
        return( 1 );
    }

    if( input_volume( input_volume_filename, 3, XYZ_dimension_names,
                      NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                      TRUE, &volume, (minc_input_options *) NULL ) != OK )
        return( 1 );

    label_volume = create_label_volume( volume );

    if( filename_extension_matches( input_labels_filename, "tag" ) )
    {
        status = open_file( input_labels_filename, READ_FILE, ASCII_FORMAT,
                            &file );
        if( status == OK )
            status = input_tags_as_labels( file, volume, label_volume );

        if( status == OK )
            status = close_file( file );

        if( status != OK )
            return( 1 );
    }
    else if( load_label_volume( input_labels_filename, label_volume ) != OK )
        return( 1 );

    initialize_segmenting_3d( volume, label_volume, N_DIMENSIONS, -1, -1,
                              min_threshold, max_threshold, &dist_transform,
                              &cuts, &to_do );

    iteration = 0;

    while( expand_labels_3d( label_volume, dist_transform, cuts, &to_do,
                             N_DIMENSIONS, -1, -1 ) )
    {
        ++iteration;
        print( "Done iteration %d\n", iteration );
    }

    status = save_label_volume( output_labels_filename, label_volume );

    return( status != OK );
}
