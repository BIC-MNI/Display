#include  <def_graphics.h>
#include  <def_globals.h>

public  Status  create_cursor_contours( graphics )
    graphics_struct  *graphics;
{
    Status          status;
    int             axis;
    Status          add_object_to_model();
    object_struct   *object;
    model_struct    *model;
    model_struct    *get_graphics_model();

    model = get_graphics_model( graphics, CURSOR_MODEL );

    for_less( axis, 0, 3 )
    {
        status = create_lines_object( &object, &Cursor_colour, 0, 0, 0 );

        if( status == OK )
            status = add_object_to_model( model, object );

        if( status == OK )
        {
            graphics->cursor_contours.contours[axis].lines = object->ptr.lines;
            graphics->cursor_contours.contours[axis].n_points_alloced = 0;
            graphics->cursor_contours.contours[axis].n_indices_alloced = 0;
            graphics->cursor_contours.contours[axis].n_end_indices_alloced = 0;
        }
    }

    return( status );
}

public  Status  update_cursor_contours( graphics )
    graphics_struct  *graphics;
{
    return( OK );
}
