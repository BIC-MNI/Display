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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/cursor/cursor_icon.c,v 1.24 2001/05/27 00:19:41 stever Exp $";
#endif


#include  <display.h>

#define  BOX_INDEX     0
#define  X_INDEX  1
#define  Y_INDEX  2
#define  Z_INDEX  3

private  void   create_box(
    object_struct  **object );
private  void  fill_in_box_points(
    Real           size[],
    object_struct  *object );
private  void   create_axis(
    object_struct  **object,
    int            axis_index );
private  void  fill_in_axis_points(
    Real           size,
    int            axis_index,
    object_struct  *object );

public  void  rebuild_cursor_icon(
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

        for_less( axis_index, 0, N_DIMENSIONS )
        {
            create_axis( &object, axis_index );

            add_object_to_model( model, object );
        }
    }

    fill_in_box_points( display->three_d.cursor.box_size,
                        model->objects[BOX_INDEX] );
    fill_in_axis_points( display->three_d.cursor.axis_size,
                         X, model->objects[X_INDEX] );
    fill_in_axis_points( display->three_d.cursor.axis_size,
                         Y, model->objects[Y_INDEX] );
    fill_in_axis_points( display->three_d.cursor.axis_size,
                         Z, model->objects[Z_INDEX] );
}

public  void  update_cursor_colour(
    display_struct   *display,
    Colour           colour )
{
    model_struct    *model;

    model = get_graphics_model( display, CURSOR_MODEL );

    get_lines_ptr(model->objects[BOX_INDEX])->colours[0] = colour;
}

private  void   create_box(
    object_struct  **object )
{
    Colour         col;
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

private  void  fill_in_box_points(
    Real           size[],
    object_struct  *object )
{
    Real           half_size[N_DIMENSIONS];
    lines_struct   *lines;

    lines = get_lines_ptr( object );

    half_size[X] = size[X] / 2.0;
    half_size[Y] = size[Y] / 2.0;
    half_size[Z] = size[Z] / 2.0;

    fill_Point( lines->points[0],
                -half_size[X], -half_size[Y], -half_size[Z] );
    fill_Point( lines->points[1],
                -half_size[X], -half_size[Y],  half_size[Z] );
    fill_Point( lines->points[2],
                -half_size[X],  half_size[Y], -half_size[Z] );
    fill_Point( lines->points[3],
                -half_size[X],  half_size[Y],  half_size[Z] );
    fill_Point( lines->points[4],
                 half_size[X], -half_size[Y], -half_size[Z] );
    fill_Point( lines->points[5],
                 half_size[X], -half_size[Y],  half_size[Z] );
    fill_Point( lines->points[6],
                 half_size[X],  half_size[Y], -half_size[Z] );
    fill_Point( lines->points[7],
                 half_size[X],  half_size[Y],  half_size[Z] );
}

private  void   create_axis(
    object_struct  **object,
    int            axis_index )
{
    static Colour  axis_colours[N_DIMENSIONS] = { 1, 2, 3 };
    Colour         col;
    static Point   dummy = { 0.0f, 0.0f, 0.0f };
    lines_struct   *lines;

    *object = create_object( LINES );
    lines = get_lines_ptr( *object );

    if( get_cursor_bitplanes() == OVERLAY_PLANES )
        col = axis_colours[axis_index];
    else
    {
        switch( axis_index )
        {
        case X:  col = RED;    break;
        case Y:  col = GREEN;  break;
        case Z:  col = BLUE;   break;
        }
    }

    initialize_lines( lines, col );

    add_point_to_line( lines, &dummy );
    add_point_to_line( lines, &dummy );
}

private  void  fill_in_axis_points(
    Real           size,
    int            axis_index,
    object_struct  *object )
{
    lines_struct   *lines;

    lines = get_lines_ptr( object );

    fill_Point( lines->points[0], 0.0, 0.0, 0.0 );
    fill_Point( lines->points[1], 0.0, 0.0, 0.0 );

    Point_coord( lines->points[1], axis_index ) = (Point_coord_type) size;
}
