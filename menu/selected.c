
#include  <def_display.h>

private  void  create_selected_text(
    model_struct   *model )
{
    int            i;
    object_struct  *object;
    Point          origin;
    text_struct    *text;
    lines_struct   *lines;

    object = create_object( LINES );

    lines = get_lines_ptr( object );

    initialize_lines( lines, Selected_colour );

    ALLOC( lines->points, 4 );
    ALLOC( lines->end_indices, 1 );
    ALLOC( lines->indices, 5 );

    lines->n_points = 4;
    lines->n_items = 1;

    lines->indices[0] = 0;
    lines->indices[1] = 1;
    lines->indices[2] = 2;
    lines->indices[3] = 3;
    lines->indices[4] = 0;
    lines->end_indices[0] = 5;

    add_object_to_model( model, object );

    for_less( i, 0, N_selected_displayed )
    {
        object = create_object( TEXT );

        text = get_text_ptr( object );

        fill_Point( origin,
                    Selected_x_origin,
                    Selected_y_origin - Menu_character_height * (Real) i,
                    0.0 );

        initialize_text( text, &origin, BLACK,
                         Menu_window_font, Menu_window_font_size );

        add_object_to_model( model, object );
    }
}

private  void  set_text_entry(
    display_struct    *menu_window,
    int               index,
    char              name[],
    Colour            col )
{
    model_struct   *model;

    model = get_graphics_model( menu_window, SELECTED_MODEL );

    (void) strcpy( get_text_ptr(model->objects[index+1])->string, name );
    set_object_visibility( model->objects[index+1], ON );
    
    get_text_ptr(model->objects[index+1])->colour = col;
}

private  void  set_current_box(
    model_struct   *selected_model,
    int            index,
    char           label[] )
{
    int            width;
    Real           x_start, x_end, y_start, y_end;
    Point          *points;

    width = (int) strwidth( label );

    if( width <= 0 )
        width = 20;

    points = get_lines_ptr(selected_model->objects[0])->points;

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

    set_object_visibility( selected_model->objects[0], ON );
}

public  void  rebuild_selected_list(
    display_struct    *display,
    display_struct    *menu_window )
{
    int            i, start, selected_index;
    Colour         col;
    String         name, label;
    model_struct   *selected_model, *model;

    selected_model = get_graphics_model( menu_window, SELECTED_MODEL );

    if( selected_model->n_objects == 0 )
        create_selected_text( selected_model );

    for_less( i, 0, N_selected_displayed+1 )
        set_object_visibility( selected_model->objects[i], OFF );

    model = get_current_model( display );

    if( current_object_is_top_level( display ) )
    {
        set_text_entry( menu_window, 0, model->filename, Visible_colour );
        set_current_box( selected_model, 0, model->filename );
    }
    else
    {
        selected_index = get_current_object_index( display );

        start = selected_index - N_selected_displayed / 2;

        if( start > model->n_objects - N_selected_displayed )
            start = model->n_objects - N_selected_displayed;

        if( start < 0 )
            start = 0;

        for_less( i, 0, N_selected_displayed )
        {
            if( start + i < model->n_objects )
            {
                get_object_name( model->objects[start+i], name );

                (void) sprintf( label, "%3d: %s", start + i + 1, name );

                if( get_object_visibility( model->objects[start+i] ) )
                {
                    if( !get_object_colour(model->objects[start+i], &col ) )
                        col = Visible_colour;
                }
                else
                    col = Invisible_colour;

                set_text_entry( menu_window, i, label, col );

                if( start+i == selected_index )
                    set_current_box( selected_model, i, label );
            }
        }
    }

    set_update_required( menu_window, NORMAL_PLANES );
}
