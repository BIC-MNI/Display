#include  <display.h>

public  void  scan_object_to_current_volume(
    display_struct   *slice_window,
    object_struct    *object )
{
    int                     label;
    object_struct           *current_object;
    object_traverse_struct  object_traverse;

    label = get_current_paint_label( slice_window );

    initialize_object_traverse( &object_traverse, 1, &object );

    while( get_next_object_traverse(&object_traverse,&current_object) )
    {
        scan_object_to_volume( current_object,
                               get_volume(slice_window),
                               get_label_volume(slice_window),
                               label,
                               Max_polygon_scan_distance );
    }
}
