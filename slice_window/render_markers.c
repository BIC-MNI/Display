
#include  <def_display.h>

private  void  scan_convert_marker(
    display_struct   *slice_window,
    volume_struct    *volume,
    marker_struct    *marker );

public  void  regenerate_voxel_marker_labels(
    display_struct    *display )
{
    display_struct          *slice_window;
    object_struct           *object;
    volume_struct           *volume;
    object_traverse_struct  object_traverse;

    if( get_slice_window_volume( display, &volume ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        set_all_volume_auxiliary_data( volume, ACTIVE_BIT );

        object = display->models[THREED_MODEL];

        initialize_object_traverse( &object_traverse, 1, &object );

        while( get_next_object_traverse(&object_traverse,&object) )
        {
            if( object->object_type == MARKER )
                scan_convert_marker( slice_window, volume,
                                     get_marker_ptr(object) );
        }

        set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
    }
}

public  void  render_marker_to_volume(
    display_struct   *display,
    marker_struct    *marker )
{
    display_struct   *slice_window;
    volume_struct    *volume;

    if( get_slice_window_volume( display, &volume ) )
    {
        slice_window = display->associated[SLICE_WINDOW];
        scan_convert_marker( slice_window, volume, marker );
    }
}

private  void  scan_convert_marker(
    display_struct   *slice_window,
    volume_struct    *volume,
    marker_struct    *marker )
{
    Real           xl, xh, yl, yh, zl, zh;
    int            xvl, xvh, yvl, yvh, zvl, zvh, x_voxel, y_voxel, z_voxel;
    int            label;

    label = lookup_label_colour( slice_window, marker->colour );

    convert_world_to_voxel( volume,
                            Point_x(marker->position) - marker->size,
                            Point_y(marker->position) - marker->size,
                            Point_z(marker->position) - marker->size,
                            &xl, &yl, &zl );

    convert_world_to_voxel( volume,
                            Point_x(marker->position) + marker->size,
                            Point_y(marker->position) + marker->size,
                            Point_z(marker->position) + marker->size,
                            &xh, &yh, &zh );

    xvl = CEILING( xl );
    xvh = (int) xh;
    yvl = CEILING( yl );
    yvh = (int) yh;
    zvl = CEILING( zl );
    zvh = (int) zh;

    for_inclusive( x_voxel, xvl, xvh )
    {
        for_inclusive( y_voxel, yvl, yvh )
        {
            for_inclusive( z_voxel, zvl, zvh )
            {
                if( voxel_is_within_volume( volume,
                            (Real) x_voxel, (Real) y_voxel, (Real) z_voxel) )

                {
                    set_volume_auxiliary_data( volume, x_voxel, y_voxel,
                                               z_voxel, label );
                }
            }
        }
    }
}
