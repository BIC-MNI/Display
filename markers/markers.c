#include  <display.h>

public  BOOLEAN  update_current_marker(
    display_struct   *display,
    Real             voxel[] )
{
    object_traverse_struct  object_traverse;
    BOOLEAN                 found;
    object_struct           *object, *closest_marker;
    Volume                  volume;
    Point                   voxel_pos;
    Real                    x_w, y_w, z_w;
    Real                    dist, closest_dist;

    initialize_object_traverse( &object_traverse, 1,
                                &display->models[THREED_MODEL] );

    (void) get_slice_window_volume( display, &volume );

    convert_voxel_to_world( volume, voxel, &x_w, &y_w, &z_w );
    fill_Point( voxel_pos, x_w, y_w, z_w );

    found = FALSE;
    closest_dist = 0.0;

    while( get_next_object_traverse( &object_traverse, &object ) )
    {
        if( object->object_type == MARKER &&
            points_within_distance( &voxel_pos,
                                    &get_marker_ptr(object)->position,
                                    Marker_pick_size ) )
        {
            dist = distance_between_points( &voxel_pos,
                                            &get_marker_ptr(object)->position );

            if( !found || dist < closest_dist )
            {
                found = TRUE;
                closest_dist = dist;
                closest_marker = object;
            }
        }
    }

    if( found && (!get_current_object(display,&object) ||
                  object != closest_marker) )
    {
        set_current_object( display, closest_marker );
    }

    return( found );
}
