
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

        lines->n_points = 6;
        lines->n_items = 3;

        CALLOC1( status, lines->points, lines->n_points, Point );
    }

    if( status == OK )
    {
        CALLOC1( status, lines->end_indices, lines->n_items, int );
    }

    if( status == OK )
    {
        CALLOC1( status, lines->indices, 6, int );
    }

    lines->end_indices[0] = 2;
    lines->end_indices[1] = 4;
    lines->end_indices[2] = 6;

    lines->indices[0] = 0;
    lines->indices[1] = 1;
    lines->indices[2] = 2;
    lines->indices[3] = 3;
    lines->indices[4] = 4;
    lines->indices[5] = 5;

    return( status );
}

private  void  fill_in_cursor_points( size, object )
    Real           size;
    object_struct  *object;
{
    lines_struct   *lines;

    lines = object->ptr.lines;

    fill_Point( lines->points[0], -size, 0.0, 0.0 );
    fill_Point( lines->points[1], size, 0.0, 0.0 );
    fill_Point( lines->points[2], 0.0, -size, 0.0 );
    fill_Point( lines->points[3], 0.0, size, 0.0 );
    fill_Point( lines->points[4], 0.0, 0.0, -size );
    fill_Point( lines->points[5], 0.0, 0.0, size );
}
