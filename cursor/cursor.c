
#include  <def_graphics.h>
#include  <def_globals.h>

public  Status  initialize_cursor( graphics )
    graphics_struct   *graphics;
{
    Status          status;
    Status          rebuild_cursor_icon();
    Real            size_of_domain();
    void            update_cursor();
    Status          initialize_cursor_plane_outline();

    graphics->three_d.cursor.origin = graphics->three_d.centre_of_objects;
    graphics->three_d.cursor.box_size[X_AXIS] = 1.0;
    graphics->three_d.cursor.box_size[Y_AXIS] = 1.0;
    graphics->three_d.cursor.box_size[Z_AXIS] = 1.0;
    graphics->three_d.cursor.axis_size = Cursor_axis_size;

    status = rebuild_cursor_icon( graphics );

    if( status == OK )
        status = initialize_cursor_plane_outline( graphics );

    graphics->models[CURSOR_MODEL]->visibility = ON;

    update_cursor( graphics );

    return( status );
}

public  Status  update_cursor_size( graphics )
    graphics_struct   *graphics;
{
    Status          rebuild_cursor_icon();

    return( rebuild_cursor_icon( graphics ) );
}

public  void  update_cursor( graphics )
    graphics_struct   *graphics;
{
    void           make_origin_transform();
    model_struct   *model;
    model_struct   *get_graphics_model();

    model = get_graphics_model( graphics, CURSOR_MODEL );

    make_origin_transform( &graphics->three_d.cursor.origin,
                           &model->transform );

    ++graphics->models_changed_id;
}
