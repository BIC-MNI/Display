
#include  <def_graphics.h>
#include  <def_globals.h>

#define  BOX_INDEX     0
#define  X_AXIS_INDEX  1
#define  Y_AXIS_INDEX  2
#define  Z_AXIS_INDEX  3

static    Status          create_box();
static    Status          create_axis();
static    void            fill_in_box_points();
static    void            fill_in_axis_points();

public  Status  rebuild_cursor_icon( graphics )
    graphics_struct   *graphics;
{
    Status          status;
    int             axis_index;
    Status          add_object_to_model();
    object_struct   *object;
    model_struct    *model;
    model_struct    *get_graphics_model();

    model = get_graphics_model( graphics, CURSOR_MODEL );

    status = OK;

    if( model->n_objects == 0 )
    {
        status = create_box( &object );

        if( status == OK )
        {
            status = add_object_to_model( model, object );
        }

        for_less( axis_index, 0, N_DIMENSIONS )
        {
            status = create_axis( &object, axis_index );

            if( status == OK )
            {
                status = add_object_to_model( model, object );
            }
        }
    }

    if( status == OK )
    {
        fill_in_box_points( graphics->three_d.cursor.box_size,
                            model->object_list[BOX_INDEX] );
        fill_in_axis_points( graphics->three_d.cursor.axis_size,
                             X_AXIS, model->object_list[X_AXIS_INDEX] );
        fill_in_axis_points( graphics->three_d.cursor.axis_size,
                             Y_AXIS, model->object_list[Y_AXIS_INDEX] );
        fill_in_axis_points( graphics->three_d.cursor.axis_size,
                             Z_AXIS, model->object_list[Z_AXIS_INDEX] );
    }

    return( status );
}

public  void  update_cursor_colour( graphics, colour )
    graphics_struct  *graphics;
    Colour           *colour;
{
    model_struct    *model;
    model_struct    *get_graphics_model();

    model = get_graphics_model( graphics, CURSOR_MODEL );

    model->object_list[BOX_INDEX]->ptr.lines->colour = *colour;
}

private  Status   create_box( object )
    object_struct  **object;
{
    Status         status;
    Status         create_lines_object();
    lines_struct   *lines;

    status = create_lines_object( object, &Cursor_colour, 8, 4, 16 );

    if( status == OK )
    {
        lines = (*object)->ptr.lines;

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

    return( status );
}

private  void  fill_in_box_points( size, object )
    Real           size[];
    object_struct  *object;
{
    Real           half_size[N_DIMENSIONS];
    lines_struct   *lines;

    lines = object->ptr.lines;

    half_size[X_AXIS] = size[X_AXIS] / 2.0;
    half_size[Y_AXIS] = size[Y_AXIS] / 2.0;
    half_size[Z_AXIS] = size[Z_AXIS] / 2.0;

    fill_Point( lines->points[0],
                -half_size[X_AXIS], -half_size[Y_AXIS], -half_size[Z_AXIS] );
    fill_Point( lines->points[1],
                -half_size[X_AXIS], -half_size[Y_AXIS],  half_size[Z_AXIS] );
    fill_Point( lines->points[2],
                -half_size[X_AXIS],  half_size[Y_AXIS], -half_size[Z_AXIS] );
    fill_Point( lines->points[3],
                -half_size[X_AXIS],  half_size[Y_AXIS],  half_size[Z_AXIS] );
    fill_Point( lines->points[4],
                 half_size[X_AXIS], -half_size[Y_AXIS], -half_size[Z_AXIS] );
    fill_Point( lines->points[5],
                 half_size[X_AXIS], -half_size[Y_AXIS],  half_size[Z_AXIS] );
    fill_Point( lines->points[6],
                 half_size[X_AXIS],  half_size[Y_AXIS], -half_size[Z_AXIS] );
    fill_Point( lines->points[7],
                 half_size[X_AXIS],  half_size[Y_AXIS],  half_size[Z_AXIS] );
}

private  Status   create_axis( object, axis_index )
    object_struct  **object;
    int            axis_index;
{
    Status         status;
    Status         create_lines_object();
    static Colour  *axis_colours[N_DIMENSIONS] = { &RED, &GREEN, &BLUE };
    lines_struct   *lines;

    status = create_lines_object( object, axis_colours[axis_index], 2, 1, 2 );

    if( status == OK )
    {
        lines = (*object)->ptr.lines;

        lines->end_indices[0] = 2;

        lines->indices[0] = 0;
        lines->indices[1] = 1;
    }

    return( status );
}

private  void  fill_in_axis_points( size, axis_index, object )
    Real           size;
    int            axis_index;
    object_struct  *object;
{
    lines_struct   *lines;

    lines = object->ptr.lines;

    fill_Point( lines->points[0], 0.0, 0.0, 0.0 );
    fill_Point( lines->points[1], 0.0, 0.0, 0.0 );

    Point_coord( lines->points[1], axis_index ) = size;
}
