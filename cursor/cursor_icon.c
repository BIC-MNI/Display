
#include  <def_graphics.h>
#include  <def_globals.h>

public  Status  rebuild_cursor_icon( graphics )
    graphics_struct   *graphics;
{
    Status          status;
    Status          create_cursor_object();
    Status          add_object_to_model();
    object_struct   *object;
    model_struct    *model;
    model_struct    *get_graphics_model();
    void            fill_in_cursor_points();

    model = get_graphics_model( graphics, CURSOR_MODEL );

    status = OK;

    if( model->n_objects == 0 )
    {
        status = create_cursor_object( &object );

        if( status == OK )
        {
            status = add_object_to_model( model, object );
        }
    }

    if( status == OK )
    {
        fill_in_cursor_points( graphics->three_d.cursor.size,
                               model->object_list[0] );
    }

    return( status );
}

private  Status   create_cursor_object( object )
    object_struct  **object;
{
    Status         status;
    Status         create_object();
    lines_struct   *lines;

    status = create_object( object, LINES );

    if( status == OK )
    {
        lines = (*object)->ptr.lines;

        lines->colour = Cursor_colour;

        lines->n_points = 8;
        lines->n_items = 4;

        CALLOC1( status, lines->points, lines->n_points, Point );
    }

    if( status == OK )
    {
        CALLOC1( status, lines->end_indices, lines->n_items, int );
    }

    if( status == OK )
    {
        CALLOC1( status, lines->indices, 16, int );
    }

    lines->end_indices[0] = 4;
    lines->end_indices[1] = 8;
    lines->end_indices[2] = 12;
    lines->end_indices[2] = 16;

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

    return( status );
}

private  void  fill_in_cursor_points( size, object )
    Real           size;
    object_struct  *object;
{
    Real           half_size;
    lines_struct   *lines;

    lines = object->ptr.lines;

    half_size = size / 2.0;

    fill_Point( lines->points[0], -half_size, -half_size, -half_size );
    fill_Point( lines->points[1], -half_size, -half_size,  half_size );
    fill_Point( lines->points[2], -half_size,  half_size, -half_size );
    fill_Point( lines->points[3], -half_size,  half_size,  half_size );
    fill_Point( lines->points[4],  half_size, -half_size, -half_size );
    fill_Point( lines->points[5],  half_size, -half_size,  half_size );
    fill_Point( lines->points[6],  half_size,  half_size, -half_size );
    fill_Point( lines->points[7],  half_size,  half_size,  half_size );
}
