
#include  <def_graphics.h>
#include  <def_string.h>
#include  <def_globals.h>

private  void  create_selected_text( model )
    model_struct   *model;
{
    int            i;
    Status         status;
    object_struct  *object;
    text_struct    *text;
    Status         add_object_to_model();
    Status         create_object();
    Status         create_lines();

    status = OK;

    if( status == OK )
        status = create_object( &object, LINES );

    if( status == OK )
        status = create_lines( object->ptr.lines, &Selected_colour,
                               4, 1, 5 );

    if( status == OK )
    {
        object->ptr.lines->indices[0] = 0;
        object->ptr.lines->indices[1] = 1;
        object->ptr.lines->indices[2] = 2;
        object->ptr.lines->indices[3] = 3;
        object->ptr.lines->indices[4] = 0;
        object->ptr.lines->end_indices[0] = 5;
    }

    if( status == OK )
        status = add_object_to_model( model, object );

    for_less( i, 0, N_selected_displayed )
    {
        if( status == OK )
            status = create_object( &object, TEXT );

        if( status == OK )
        {
            text = object->ptr.text;
            text->font = Menu_window_font;
            text->size = Menu_window_font_size;

            fill_Point( text->origin,
                        Selected_x_origin,
                        Selected_y_origin - Menu_character_height * (Real) i,
                        0.0 );
        }

        if( status == OK )
            status = add_object_to_model( model, object );
    }
}

private  void  set_text_entry( menu_window, index, name, col )
    graphics_struct   *menu_window;
    int               index;
    char              name[];
    Colour            *col;
{
    model_struct   *model;
    model_struct   *get_graphics_model();

    model = get_graphics_model( menu_window, SELECTED_MODEL );

    (void) strcpy( model->object_list[index+1]->ptr.text->text, name );
    model->object_list[index+1]->visibility = TRUE;
    
    model->object_list[index+1]->ptr.text->colour = *col;
}

private  void  set_current_box( selected_model, index, label )
    model_struct   *selected_model;
    int            index;
    char           *label;
{
    int            width;
    Real           x_start, x_end, y_start, y_end;
    Point          *points;

    width = (int) strwidth( label );

    if( width <= 0 )
        width = 20;

    points = selected_model->object_list[0]->ptr.lines->points;

    x_start = Selected_x_origin;
    y_start = Selected_y_origin - Menu_character_height * (Real) index;
    x_end = x_start + (Real) width;
    y_end = y_start + (Real) Character_height_in_pixels;

    x_start -= Selected_box_x_offset;
    x_end += Selected_box_x_offset;
    y_start -= Selected_box_y_offset;
    y_end += Selected_box_y_offset;

    fill_Point( points[0], x_start, y_start, 0.0 );
    fill_Point( points[1], x_end, y_start, 0.0 );
    fill_Point( points[2], x_end, y_end, 0.0 );
    fill_Point( points[3], x_start, y_end, 0.0 );

    selected_model->object_list[0]->visibility = TRUE;
}

public  void  rebuild_selected_list( graphics, menu_window )
    graphics_struct   *graphics;
    graphics_struct   *menu_window;
{
    int            i, start, selected_index;
    Colour         col;
    String         name, label;
    model_struct   *selected_model, *model;
    model_struct   *get_current_model();
    void           get_object_name();
    model_struct   *get_graphics_model();
    void           set_update_required();
    void           set_current_box();

    selected_model = get_graphics_model( menu_window, SELECTED_MODEL );

    if( selected_model->n_objects == 0 )
        create_selected_text( selected_model );

    for_less( i, 0, N_selected_displayed+1 )
        selected_model->object_list[i]->visibility = FALSE;

    model = get_current_model( graphics );

    if( current_object_is_top_level( graphics ) )
    {
        set_text_entry( menu_window, 0, model->filename, &Visible_colour );
        set_current_box( selected_model, 0, model->filename );
    }
    else
    {
        selected_index = get_current_object_index( graphics );

        start = selected_index - N_selected_displayed / 2;

        if( start > model->n_objects - N_selected_displayed )
            start = model->n_objects - N_selected_displayed;

        if( start < 0 )
            start = 0;

        for_less( i, 0, N_selected_displayed )
        {
            if( start + i < model->n_objects )
            {
                get_object_name( model->object_list[start+i], name );

                (void) sprintf( label, "%3d: %s", start + i + 1, name );

                if( model->object_list[start+i]->visibility )
                {
                    if( !get_object_colour(model->object_list[start+i], &col ) )
                        col = Visible_colour;
                }
                else
                    col = Invisible_colour;

                set_text_entry( menu_window, i, label, &col );

                if( start+i == selected_index )
                    set_current_box( selected_model, i, label );
            }
        }
    }

    set_update_required( menu_window, NORMAL_PLANES );
}
