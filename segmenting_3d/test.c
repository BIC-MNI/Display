#include  <mni.h>

public  void  initialize_segmenting_3d(
    Volume    volume,
    Volume    label_volume,
    Real      min_threshold,
    Real      max_threshold,
    Volume    *distance_transform,
    Volume    *cuts );
public  BOOLEAN  expand_labels_3d(
    Volume    label_volume,
    Volume    distance_transform,
    Volume    cuts );
private  void  print_volume( Volume );

int  main(
    int   argc,
    char  *argv[] )
{
    Status               status;
    char                 *input_volume_filename;
    char                 *output_volume_filename;
    Volume               volume, label_volume, dist_transform, cuts;
    int                  iteration;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &input_volume_filename ) ||
        !get_string_argument( "", &output_volume_filename ) )
    {
        print( "Usage: %s  in_volume  out_volume\n", argv[0] );
        return( 1 );
    }

    status = input_volume( input_volume_filename, 3, XYZ_dimension_names,
                           NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                           TRUE, &volume, (minc_input_options *) NULL );

    if( status != OK )
        return( 1 );

    label_volume = create_label_volume( volume );

    initialize_segmenting_3d( volume, label_volume,
                              (Real) '1', 256.0, &dist_transform,
                              &cuts );

    iteration = 0;

    while( expand_labels_3d( label_volume, dist_transform, cuts ) )
    {
        ++iteration;
        print( "Done iteration %d\n", iteration );
    }

    print_volume( label_volume );

    return( 0 );
}

private  void  print_volume(
    Volume volume )
{
    int   x, y, z, sizes[MAX_DIMENSIONS];
    int   voxel;

    get_volume_sizes( volume, sizes );

    for_less( y, 0, sizes[Y] )
    {
        for_less( x, 0, sizes[X] )
        {
            GET_VOXEL_3D( voxel, volume, x, y, z );
            print( "%c", '0' + voxel );
        }

        print( "\n" );
    }
}
