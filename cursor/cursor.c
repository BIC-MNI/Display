/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif


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

    rebuild_cursor_position_model( display );

    ++display->models_changed_id;
}

public  void  get_cursor_origin(
    display_struct   *display,
    Point            *origin )
{
    display = get_three_d_window( display );

    *origin = display->three_d.cursor.origin;
}
