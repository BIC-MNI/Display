#include  <def_files.h>
#include  <def_string.h>
#include  <def_graphics.h>

public  Status   input_volume_file( filename, n_objects, object_list )
    char           filename[];
    int            *n_objects;
    object_struct  ***object_list;
{
    Status         status;
    object_struct  *object;
    volume_struct  *volume;
    Status         create_object();
    Status         input_volume();
    Status         add_object_to_list();

    status = create_object( &object, VOLUME );

    if( status == OK )
    {
        volume = object->ptr.volume;
        volume->sizes[X_AXIS] = 0;
        volume->sizes[Y_AXIS] = 0;
        volume->sizes[Z_AXIS] = 0;
        (void) strcpy( volume->filename, filename );
        volume->roi_filename[0] = (char) 0;
        volume->axis_map[X_AXIS] = X_AXIS;
        volume->axis_map[Y_AXIS] = Y_AXIS;
        volume->axis_map[Z_AXIS] = Z_AXIS;
        volume->axis_flip[X_AXIS] = FALSE;
        volume->axis_flip[Y_AXIS] = FALSE;
        volume->axis_flip[Z_AXIS] = FALSE;
        volume->slice_thickness[X_AXIS] = 1.0;
        volume->slice_thickness[Y_AXIS] = 1.0;
        volume->slice_thickness[Z_AXIS] = 1.0;

        status = input_volume( volume );
    }

    *n_objects = 0;

    if( status == OK )
        status = add_object_to_list( n_objects, object_list, object );

    return( status );
}
