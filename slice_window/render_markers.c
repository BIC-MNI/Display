
#include  <def_graphics.h>
#include  <def_files.h>
#include  <def_globals.h>
#include  <def_string.h>

public  void  regenerate_voxel_marker_labels( graphics )
    graphics_struct   *graphics;
{
    Status                  status;
    graphics_struct         *slice_window;
    object_struct           *object;
    volume_struct           *volume;
    object_traverse_struct  object_traverse;
    Status                  initialize_object_traverse();
    void                    scan_convert_marker();
    void                    set_all_volume_auxiliary_data();
    void                    set_slice_window_update();

    if( get_slice_window_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        set_all_volume_auxiliary_data( volume, ACTIVE_BIT );

        object = graphics->models[THREED_MODEL];

        status = initialize_object_traverse( &object_traverse, 1, &object );

        if( status == OK )
        {
            while( get_next_object_traverse(&object_traverse,&object) )
            {
                if( object->object_type == MARKER )
                    scan_convert_marker( slice_window, volume,
                                         object->ptr.marker );
            }
        }

        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
    }
}

public  void  render_marker_to_volume( graphics, marker )
    graphics_struct  *graphics;
    marker_struct    *marker;
{
    graphics_struct  *slice_window;
    volume_struct    *volume;
    void             scan_convert_marker();

    if( get_slice_window_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];
        scan_convert_marker( slice_window, volume, marker );
    }
}

private  void  scan_convert_marker( slice_window, volume, marker )
    graphics_struct  *slice_window;
    volume_struct    *volume;
    marker_struct    *marker;
{
    Real           xl, xh, yl, yh, zl, zh;
    int            xvl, xvh, yvl, yvh, zvl, zvh, x_voxel, y_voxel, z_voxel;
    void           convert_point_to_voxel();
    int            label;
    unsigned char  *aux_ptr;

    label = lookup_label_colour( slice_window, &marker->colour );

    convert_point_to_voxel( volume,
                            Point_x(marker->position) - marker->size,
                            Point_y(marker->position) - marker->size,
                            Point_z(marker->position) - marker->size,
                            &xl, &yl, &zl );

    convert_point_to_voxel( volume,
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
                    aux_ptr = GET_VOLUME_AUX_PTR( *volume,
                                                  x_voxel, y_voxel, z_voxel );

                    *aux_ptr = label;
                }
            }
        }
    }
}
