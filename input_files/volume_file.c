#include  <def_mni.h>

public  Status   input_volume_file(
    char           filename[],
    Volume         *volume )
{
    Status         status;
    static String  dim_names[] = { MIxspace, MIyspace, MIzspace };

    status = input_volume( filename, dim_names, volume );

    if( get_volume_n_dimensions( *volume ) != N_DIMENSIONS )
    {
        print( "Volume %s has %d dimensions, should have %d\n",
               filename, get_volume_n_dimensions(*volume), N_DIMENSIONS );
        delete_volume( *volume );
        status = ERROR;
    }

    if( status == OK )
        alloc_auxiliary_data( *volume );

    return( status );
}
