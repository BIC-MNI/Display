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
        volume->sizes[X] = 0;
        volume->sizes[Y] = 0;
        volume->sizes[Z] = 0;
        (void) strcpy( volume->filename, filename );
        volume->activity_filename[0] = (char) 0;
        volume->activity_threshold = 0.0;
        volume->axis_map[X] = X;
        volume->axis_map[Y] = Y;
        volume->axis_map[Z] = Z;
        volume->axis_flip[X] = FALSE;
        volume->axis_flip[Y] = FALSE;
        volume->axis_flip[Z] = FALSE;
        volume->slice_thickness[X] = 1.0;
        volume->slice_thickness[Y] = 1.0;
        volume->slice_thickness[Z] = 1.0;

        status = input_volume( volume );
    }

    *n_objects = 0;

    if( status == OK )
        status = add_object_to_list( n_objects, object_list, object );

    return( status );
}
