
#include  <def_graphics.h>
#include  <def_globals.h>

public  void  apply_transform_in_view_space( graphics, transform )
    graphics_struct   *graphics;
    Transform         *transform;
{
    void    make_transform_in_coordinate_system();
    Vector  z_axis;
    Point   centre;
    void    get_view_z_axis();
    void    get_view_centre();
    void    transform_model();
    void    transform_point_to_world();

    get_view_z_axis( &graphics->three_d.view, &z_axis );

    if( Use_cursor_origin )
    {
        centre = graphics->three_d.cursor.origin;
        transform_point_to_world( &graphics->three_d.view, &centre, &centre );
    }
    else
    {
        get_view_centre( &graphics->three_d.view, &centre );
    }

    make_transform_in_coordinate_system( &centre,
                                         &graphics->three_d.view.x_axis,
                                         &graphics->three_d.view.y_axis,
                                         &z_axis,
                                         transform, transform );

    transform_model( graphics, transform );
}
