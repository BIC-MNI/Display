#include  <def_mni.h>

public  Status   input_volume_file(
    char           filename[],
    volume_struct  *volume )
{
    Status         status;

/*
    (*volume)->sizes[X] = 0;
    (*volume)->sizes[Y] = 0;
    (*volume)->sizes[Z] = 0;
    (*volume)->activity_filename[0] = (char) 0;
    (*volume)->activity_threshold = 0.0;
    (*volume)->axis_map[X] = X;
    (*volume)->axis_map[Y] = Y;
    (*volume)->axis_map[Z] = Z;
    (*volume)->axis_flip[X] = FALSE;
    (*volume)->axis_flip[Y] = FALSE;
    (*volume)->axis_flip[Z] = FALSE;
    (*volume)->slice_thickness[X] = 1.0;
    (*volume)->slice_thickness[Y] = 1.0;
    (*volume)->slice_thickness[Z] = 1.0;
*/

    status = input_volume( filename, volume );

    return( status );
}
