#include  <def_display.h>

public  Status   input_tag_file(
    char           filename[],
    Colour         marker_colour,
    VIO_Real           default_size,
    Marker_types   default_type,
    int            *n_objects,
    object_struct  **object_list[] )
{
    Status             status;
    object_struct      *object;
    marker_struct      *marker;
    FILE               *file;
    int                i, n_volumes, n_tag_points, *structure_ids, *patient_ids;
    char               **labels;
    double             *weights;
    double             **tags1, **tags2;

    status = open_file_with_default_suffix( filename, "tag", READ_FILE,
                                            ASCII_FORMAT, &file );

    if( status == VIO_OK )
    {
        status = input_tag_points( file, &n_volumes, &n_tag_points,
                                   &tags1, &tags2, &weights,
                                   &structure_ids, &patient_ids,
                                   &labels );
    }

    if( status == VIO_OK )
    {
        for_less( i, 0, n_tag_points )
        {
            object = create_object( MARKER );
            marker = get_marker_ptr( object );
            fill_Point( marker->position, tags1[i][VIO_X], tags1[i][VIO_Y],tags1[i][VIO_Z]);
            (void) strcpy( marker->label, labels[i] );

            if( structure_ids[i] >= 0 )
                marker->structure_id = structure_ids[i];
            else
                marker->structure_id = -1;

            if( patient_ids[i] >= 0 )
                marker->patient_id = patient_ids[i];
            else
                marker->patient_id = -1;

            if( weights[i] > 0.0 )
                marker->size = weights[i];
            else
                marker->size = default_size;

            marker->colour = marker_colour;
            marker->type = default_type;

            add_object_to_list( n_objects, object_list, object );
        }

        free_tag_points( n_volumes, n_tag_points, tags1, tags2, weights,
                         structure_ids, patient_ids, labels );
    }

    if( status == VIO_OK )
        status = close_file( file );

    return( status );
}
