#include  <def_graphics.h>
#include  <def_globals.h>

public  Boolean  update_current_marker( graphics, x, y, z )
    graphics_struct  *graphics;
    int              x;
    int              y;
    int              z;
{
    Status                  status;
    object_traverse_struct  object_traverse;
    Boolean                 found;
    Status                  initialize_object_traverse();
    object_struct           *object, *closest_marker;
    volume_struct           *volume;
    Point                   voxel_pos;
    Real                    dist, closest_dist;
    Real                    distance_between_points();
    void                    convert_voxel_to_point();
    void                    set_current_object();

    status = initialize_object_traverse( &object_traverse, 1,
                                         &graphics->models[THREED_MODEL] );

    if( status == OK )
        (void) get_slice_window_volume( graphics, &volume );

    convert_voxel_to_point( volume, (Real) x, (Real) y, (Real) z, &voxel_pos );

    found = FALSE;
    closest_dist = 0.0;

    while( get_next_object_traverse( &object_traverse, &object ) )
    {
        if( object->object_type == MARKER &&
            points_within_distance( &voxel_pos, &object->ptr.marker->position,
                                    Marker_pick_size ) )
        {
            dist = distance_between_points( &voxel_pos,
                                            &object->ptr.marker->position );

            if( !found || dist < closest_dist )
            {
                found = TRUE;
                closest_dist = dist;
                closest_marker = object;
            }
        }
    }

    if( found && (!get_current_object(graphics,&object) ||
                  object != closest_marker) )
    {
        set_current_object( graphics, closest_marker );
    }

    return( found );
}
