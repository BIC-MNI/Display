
#include  <display.h>

public  void  initialize_cursor(
    display_struct    *display )
{
    display->three_d.cursor.box_size[X] = 1.0;
    display->three_d.cursor.box_size[Y] = 1.0;
    display->three_d.cursor.box_size[Z] = 1.0;
    display->three_d.cursor.axis_size = Cursor_axis_size;

    reset_cursor( display );

    initialize_cursor_plane_outline( display );
}

public  void  reset_cursor(
    display_struct    *display )
{
    display->three_d.cursor.origin = display->three_d.centre_of_objects;

    rebuild_cursor_icon( display );

    display->models[CURSOR_MODEL]->visibility = ON;

    update_cursor( display );
}

public  void  update_cursor_size(
    display_struct    *display )
{
    rebuild_cursor_icon( display );
}

public  void  update_cursor(
    display_struct    *display )
{
    model_struct   *model;

    model = get_graphics_model( display, CURSOR_MODEL );

    make_origin_transform( &display->three_d.cursor.origin,
                           &get_model_info(model)->transform );

    ++display->models_changed_id;
}
