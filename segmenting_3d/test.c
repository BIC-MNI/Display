#include  <mni.h>

public  void  initialize_segmenting_3d(
    Volume    volume,
    Volume    label_volume,
    int       n_dimensions,
    int       voxel_pos,
    int       axis,
    Real      min_threshold,
    Real      max_threshold,
    Volume    *distance_transform,
    Volume    *cuts,
    bitlist_3d_struct  *to_do );

public  BOOLEAN  expand_labels_3d(
    Volume    label_volume,
    Volume    distance_transform,
    Volume    cuts,
    bitlist_3d_struct  *to_do,
    int       n_dimensions,
    int       voxel_pos,
    int       axis );
private  void  print_volume( char [], Volume, int );
private  void  modify_labels( Volume, Volume );

int  main(
    int   argc,
    char  *argv[] )
{
    Status               status;
    char                 *input_volume_filename;
    Volume               volume, label_volume, dist_transform, cuts;
    bitlist_3d_struct    to_do;
    int                  iteration;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &input_volume_filename ) )
    {
        print( "Usage: %s  in_volume\n", argv[0] );
        return( 1 );
    }

    status = input_volume( input_volume_filename, 3, XYZ_dimension_names,
                           NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                           TRUE, &volume, (minc_input_options *) NULL );

    if( status != OK )
        return( 1 );

    label_volume = create_label_volume( volume );

    modify_labels( volume, label_volume );

    initialize_segmenting_3d( volume, label_volume, 2, 0, Z,
                              (Real) '1', 256.0, &dist_transform,
                              &cuts, &to_do );

    iteration = 0;

    print_volume( "Volume", volume, 0 );
    print_volume( "Labels", label_volume, '0' );
    print_volume( "Dist", dist_transform, '0' );
    print_volume( "Cuts", cuts, '0' );

    while( expand_labels_3d( label_volume, dist_transform, cuts, &to_do,
                             2, 0, Z ) )
    {
        ++iteration;
        print( "Done iteration %d\n", iteration );

        print_volume( "--- Labels", label_volume, '0' );
        print_volume( "--- Cuts", cuts, '0' );
    }

    print_volume( "Labels", label_volume, '0' );

    return( 0 );
}

private  void  print_volume(
    char   title[],
    Volume volume,
    int    offset )
{
    int   x, y, z, sizes[MAX_DIMENSIONS];
    int   voxel;

    if( title != (char *) NULL )
        print( "\n----- %s -----\n\n", title );

    get_volume_sizes( volume, sizes );

    for_less( z, 0, sizes[Z] )
    {
        for_less( y, 0, sizes[Y] )
        {
            for_less( x, 0, sizes[X] )
            {
                GET_VOXEL_3D( voxel, volume, x, y, z );
                if( voxel & 128 )
                    voxel -= 128;
                print( "%c", offset + voxel );
            }

            print( "\n" );
        }

        print( "\n\n" );
    }
}

private  void  modify_labels(
    Volume volume,
    Volume label_volume )
{
    int   x, y, z, sizes[MAX_DIMENSIONS];
    int   voxel;

    get_volume_sizes( volume, sizes );

    for_less( x, 0, sizes[X] )
    {
        for_less( y, 0, sizes[Y] )
        {
            for_less( z, 0, sizes[Z] )
            {
                GET_VOXEL_3D( voxel, volume, x, y, z );
                if( voxel == (int) '\n' )
                {
                    SET_VOXEL_3D( volume, x, y, z, '0' );
                }
                else if( voxel > (int) '1' )
                {
                    SET_VOXEL_3D( label_volume, x, y, z, voxel - '0' );
                    SET_VOXEL_3D( volume, x, y, z, '1' );
                }
            }
        }
    }
}
