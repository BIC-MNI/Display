/**
 * \file cursor/cursor.c
 * \brief Basic functions for managing the cursor location.
 *
 * The cursor location is conceptually shared between the 3D and slice 
 * view windows, but it is stored in the three_d_window_struct.
 *
 * \copyright
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
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>

#include  <display.h>

/**
 * Set the initial size and location of the cursor to defaults.
 * \param display The display_struct of the 3D window.
 */
void
initialize_cursor(display_struct *display)
{
    assert(display->window_type == THREE_D_WINDOW);

    display->three_d.cursor.box_size[VIO_X] = 1.0;
    display->three_d.cursor.box_size[VIO_Y] = 1.0;
    display->three_d.cursor.box_size[VIO_Z] = 1.0;
    display->three_d.cursor.axis_size = Cursor_axis_size;

    reset_cursor( display );

    initialize_cursor_plane_outline( display );
}

/**
 * Moves the cursor to the centre of the 3D view and makes it visible.
 * \param display The display_struct of the 3D window.
 */
void
reset_cursor(display_struct *display)
{
    assert(display->window_type == THREE_D_WINDOW);

    display->three_d.cursor.origin = display->three_d.centre_of_objects;

    rebuild_cursor_icon( display );

    display->models[CURSOR_MODEL]->visibility = TRUE;

    update_cursor( display );
}

/**
 * Rebuilds the cursor in response to a change in the size parameters.
 * \param display The display_struct of the 3D window.
 */
void
update_cursor_size(display_struct *display)
{
    assert(display->window_type == THREE_D_WINDOW);

    rebuild_cursor_icon( display );
}

/**
 * Called to rebuild the cursor location and transform after the
 * cursor position has been updated. Called from update_cursor_from_voxel()
 * in view.c.
 *
 * \param display The display_struct of the 3D window.
 */
void
update_cursor(display_struct *display)
{
    model_struct   *model;

    assert(display->window_type == THREE_D_WINDOW);

    model = get_graphics_model( display, CURSOR_MODEL );

    make_origin_transform( &display->three_d.cursor.origin,
                           &get_model_info(model)->transform );

    rebuild_cursor_position_model( display );

    ++display->models_changed_id;
}

/**
 * Sets the current cursor location in world coordinates.
 *
 * \param display Any of the top-level display_struct objects.
 * \param origin The new cursor position in world coordinates.
 */
void
set_cursor_origin(display_struct *display, const VIO_Point *origin)
{
  display_struct *three_d_ptr = get_three_d_window( display );
  three_d_ptr->three_d.cursor.origin = *origin;
  update_cursor( three_d_ptr );
  set_update_required( three_d_ptr, get_cursor_bitplanes() );
}

/**
 * Returns the current cursor location in world coordinates.
 *
 * \param display Any of the top-level display_struct objects.
 * \param origin The returned cursor position in world coordinates.
 */
void
get_cursor_origin(display_struct *display, VIO_Point *origin )
{
    display_struct *three_d_ptr = get_three_d_window( display );

    *origin = three_d_ptr->three_d.cursor.origin;
}
