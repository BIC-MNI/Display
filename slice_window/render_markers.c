
#include  <def_display.h>

private  void  scan_convert_marker(
    display_struct   *slice_window,
    Volume           volume,
    marker_struct    *marker );

public  void  regenerate_voxel_marker_labels(
    display_struct    *display )
{
    display_struct          *slice_window;
    object_struct           *object;
    Volume                  volume;
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
    Volume           volume;

    if( get_slice_window_volume( display, &volume ) )
    {
        slice_window = display->associated[SLICE_WINDOW];
        scan_convert_marker( slice_window, volume, marker );
    }
}

private  void  scan_convert_marker(
    display_struct   *slice_window,
    Volume           volume,
    marker_struct    *marker )
{
    Real           low[N_DIMENSIONS], high[N_DIMENSIONS];
    int            min_voxel[N_DIMENSIONS], max_voxel[N_DIMENSIONS];
    Real           voxel[N_DIMENSIONS];
    int            c, label;

    label = lookup_label_colour( slice_window, marker->colour );

    convert_world_to_voxel( volume,
                            Point_x(marker->position) - marker->size,
                            Point_y(marker->position) - marker->size,
                            Point_z(marker->position) - marker->size,
                            &low[X], &low[Y], &low[Z] );

    convert_world_to_voxel( volume,
                            Point_x(marker->position) + marker->size,
                            Point_y(marker->position) + marker->size,
                            Point_z(marker->position) + marker->size,
                            &high[X], &high[Y], &high[Z] );

    for_less( c, 0, N_DIMENSIONS )
    {
        if( IS_INT(low[c]) )
            min_voxel[c] = (int) low[c];
        else
            min_voxel[c] = (int) low[c] + 1;
        max_voxel[c] = (int) high[c];

        if( min_voxel[c] > max_voxel[c] )
        {
            min_voxel[c] = ROUND( (low[c] + high[c]) / 2.0 );
            max_voxel[c] = min_voxel[c];
        }
    }

    for_inclusive( voxel[X], min_voxel[X], max_voxel[X] )
    {
        for_inclusive( voxel[Y], min_voxel[Y], max_voxel[Y] )
        {
            for_inclusive( voxel[Z], min_voxel[Z], max_voxel[Z] )
            {
                if( voxel_is_within_volume( volume, voxel ) )

                {
                    set_volume_auxiliary_data( volume, voxel[X], voxel[Y],
                                               voxel[Z], label );
                }
            }
        }
    }
}
