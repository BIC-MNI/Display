
#include  <def_graphics.h>
#include  <def_files.h>
#include  <def_globals.h>
#include  <def_string.h>

public  void  regenerate_voxel_marker_labels( graphics )
    graphics_struct   *graphics;
{
    Status                  status;
    object_struct           *object;
    volume_struct           *volume;
    object_traverse_struct  object_traverse;
    Status                  initialize_object_traverse();
    int                     label;
    void                    render_marker_to_volume();
    void                    set_all_volume_auxiliary_data();
    void                    set_slice_window_update();

    if( get_slice_window_volume( graphics, &volume ) )
    {
        set_all_volume_auxiliary_data( volume, ACTIVE_BIT );

        object = graphics->models[THREED_MODEL];

        status = initialize_object_traverse( &object_traverse, 1, &object );

        if( status == OK )
        {
            while( get_next_object_traverse(&object_traverse,&object) )
            {
                if( object->object_type == MARKER )
                {
                    label = lookup_label_colour(
                                     graphics->associated[SLICE_WINDOW],
                                         &object->ptr.marker->colour );
                    render_marker_to_volume( volume, label,
                                             object->ptr.marker );
                }
            }
        }

        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
    }
}

private  void  render_marker_to_volume( volume, label, marker )
    volume_struct   *volume;
    int             label;
    marker_struct   *marker;
{
    Real           xl, xh, yl, yh, zl, zh;
    int            xvl, xvh, yvl, yvh, zvl, zvh, x_voxel, y_voxel, z_voxel;
    void           convert_point_to_voxel();
    unsigned char  *aux_ptr;

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
