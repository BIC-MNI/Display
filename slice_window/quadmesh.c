#include  <display.h>

public  object_struct   *create_3d_slice_quadmesh(
    Volume         volume,
    int            axis_index,
    Real           voxel_position )
{
    object_struct    *object;
    quadmesh_struct  *quadmesh;

    object = create_object( QUADMESH );
    quadmesh =  get_quadmesh_ptr( object );

    create_slice_quadmesh( volume, axis_index, voxel_position, -1, -1,
                           quadmesh );

    return( object );
}
