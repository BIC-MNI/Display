
#include  <def_graphics.h>
#include  <def_globals.h>

public  Status  initialize_cursor( graphics )
    graphics_struct   *graphics;
{
    Status          status;
    Status          rebuild_cursor_icon();
    Real            size_of_domain();
    void            update_cursor();

    graphics->three_d.cursor.origin = graphics->three_d.centre_of_objects;
    graphics->three_d.cursor.size = 1.0;

    status = rebuild_cursor_icon( graphics );

    graphics->models[CURSOR_MODEL]->visibility = ON;

    update_cursor( graphics );

    return( status );
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
}
