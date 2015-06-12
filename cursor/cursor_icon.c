/**
 * \file cursor_icon.c
 * \brief Functions to create and manipulate the 3D window cursor model.
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

#include  <display.h>

#define  BOX_INDEX     0
#define  X_INDEX  1
#define  Y_INDEX  2
#define  Z_INDEX  3

static  void   create_box(
    object_struct  **object );
static  void  fill_in_box_points(
    VIO_Real           size[],
    object_struct  *object );
static  void   create_axis(
    object_struct  **object,
    int            axis_index );
static  void  fill_in_axis_points(
    VIO_Real           size,
    int            axis_index,
    object_struct  *object );

  void  rebuild_cursor_icon(
    display_struct    *display )
{
    int             axis_index;
    object_struct   *object;
    model_struct    *model;

    model = get_graphics_model( display, CURSOR_MODEL );

    if( model->n_objects == 0 )
    {
        create_box( &object );

        add_object_to_model( model, object );

        for_less( axis_index, 0, VIO_N_DIMENSIONS )
        {
            create_axis( &object, axis_index );

            add_object_to_model( model, object );
        }
    }

    fill_in_box_points( display->three_d.cursor.box_size,
                        model->objects[BOX_INDEX] );
    fill_in_axis_points( display->three_d.cursor.axis_size,
                         VIO_X, model->objects[X_INDEX] );
    fill_in_axis_points( display->three_d.cursor.axis_size,
                         VIO_Y, model->objects[Y_INDEX] );
    fill_in_axis_points( display->three_d.cursor.axis_size,
                         VIO_Z, model->objects[Z_INDEX] );
}

  void  update_cursor_colour(
    display_struct   *display,
    VIO_Colour           colour )
{
    model_struct    *model;

    model = get_graphics_model( display, CURSOR_MODEL );

    get_lines_ptr(model->objects[BOX_INDEX])->colours[0] = colour;
}

static  void   create_box(
    object_struct  **object )
{
    VIO_Colour         col;
    lines_struct   *lines;

    *object = create_object( LINES );

    lines = get_lines_ptr( *object );

    if( get_cursor_bitplanes() == OVERLAY_PLANES )
        col = Cursor_colour;
    else
        col = Cursor_rgb_colour;

    initialize_lines( lines, col );

    ALLOC( lines->points, 8 );
    ALLOC( lines->end_indices, 4 );
    ALLOC( lines->indices, 16 );

    lines->n_points = 8;
    lines->n_items = 4;

    lines->end_indices[0] = 4;
    lines->end_indices[1] = 8;
    lines->end_indices[2] = 12;
    lines->end_indices[3] = 16;

    lines->indices[0] = 0;
    lines->indices[1] = 1;
    lines->indices[2] = 5;
    lines->indices[3] = 4;

    lines->indices[4] = 1;
    lines->indices[5] = 3;
    lines->indices[6] = 7;
    lines->indices[7] = 5;

    lines->indices[8] = 3;
    lines->indices[9] = 2;
    lines->indices[10] = 6;
    lines->indices[11] = 7;

    lines->indices[12] = 2;
    lines->indices[13] = 0;
    lines->indices[14] = 4;
    lines->indices[15] = 6;
}

static  void  fill_in_box_points(
    VIO_Real           size[],
    object_struct  *object )
{
    VIO_Real           half_size[VIO_N_DIMENSIONS];
    lines_struct   *lines;

    lines = get_lines_ptr( object );

    half_size[VIO_X] = size[VIO_X] / 2.0;
    half_size[VIO_Y] = size[VIO_Y] / 2.0;
    half_size[VIO_Z] = size[VIO_Z] / 2.0;

    fill_Point( lines->points[0],
                -half_size[VIO_X], -half_size[VIO_Y], -half_size[VIO_Z] );
    fill_Point( lines->points[1],
                -half_size[VIO_X], -half_size[VIO_Y],  half_size[VIO_Z] );
    fill_Point( lines->points[2],
                -half_size[VIO_X],  half_size[VIO_Y], -half_size[VIO_Z] );
    fill_Point( lines->points[3],
                -half_size[VIO_X],  half_size[VIO_Y],  half_size[VIO_Z] );
    fill_Point( lines->points[4],
                 half_size[VIO_X], -half_size[VIO_Y], -half_size[VIO_Z] );
    fill_Point( lines->points[5],
                 half_size[VIO_X], -half_size[VIO_Y],  half_size[VIO_Z] );
    fill_Point( lines->points[6],
                 half_size[VIO_X],  half_size[VIO_Y], -half_size[VIO_Z] );
    fill_Point( lines->points[7],
                 half_size[VIO_X],  half_size[VIO_Y],  half_size[VIO_Z] );
}

static  void   create_axis(
    object_struct  **object,
    int            axis_index )
{
    static VIO_Colour  axis_colours[VIO_N_DIMENSIONS] = { 1, 2, 3 };
    VIO_Colour         col;
    static VIO_Point   dummy = { { 0.0f, 0.0f, 0.0f } };
    lines_struct   *lines;

    *object = create_object( LINES );
    lines = get_lines_ptr( *object );

    if( get_cursor_bitplanes() == OVERLAY_PLANES )
        col = axis_colours[axis_index];
    else
    {
        switch( axis_index )
        {
        case VIO_X:  col = RED;    break;
        case VIO_Y:  col = GREEN;  break;
        case VIO_Z:  col = BLUE;   break;
        }
    }

    initialize_lines( lines, col );

    add_point_to_line( lines, &dummy );
    add_point_to_line( lines, &dummy );
}

static  void  fill_in_axis_points(
    VIO_Real           size,
    int            axis_index,
    object_struct  *object )
{
    lines_struct   *lines;

    lines = get_lines_ptr( object );

    fill_Point( lines->points[0], 0.0, 0.0, 0.0 );
    fill_Point( lines->points[1], 0.0, 0.0, 0.0 );

    Point_coord( lines->points[1], axis_index ) = (VIO_Point_coord_type) size;
}
