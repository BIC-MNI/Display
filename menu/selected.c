
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

    status = OK;

    for_less( i, 0, N_selected_displayed )
    {
        if( status == OK )
        {
            status = create_object( &object, TEXT );
        }

        if( status == OK )
        {
            text = object->ptr.text;

            fill_Point( text->origin,
                        Selected_x_origin + Selected_x_delta * (Real) i,
                        Selected_y_origin + Selected_y_delta * (Real) i, 0.0 );
        }

        if( status == OK )
        {
            status = add_object_to_model( model, object );
        }
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

    (void) strcpy( model->object_list[index]->ptr.text->text, name );
    model->object_list[index]->visibility = TRUE;
    
    model->object_list[index]->ptr.text->colour = *col;
}

public  void  rebuild_selected_list( graphics, menu_window )
    graphics_struct   *graphics;
    graphics_struct   *menu_window;
{
    int            i, start, selected_index;
    Colour         col;
    String         name, label;
    model_struct   *model;
    model_struct   *get_current_model();
    void           get_object_name();
    model_struct   *get_graphics_model();
    void           set_update_required();

    model = get_graphics_model( menu_window, SELECTED_MODEL );

    if( model->n_objects == 0 )
    {
        create_selected_text( model );
    }

    for_less( i, 0, N_selected_displayed )
    {
        model->object_list[i]->visibility = FALSE;
    }

    if( current_object_is_top_level( graphics ) )
    {
        set_text_entry( menu_window, 0, model->filename, &Selected_vis_colour );
    }
    else
    {
        selected_index = get_current_object_index( graphics );
        model = get_current_model( graphics );

        start = selected_index - N_selected_displayed / 2;

        if( start > model->n_objects - N_selected_displayed )
        {
            start = model->n_objects - N_selected_displayed;
        }

        if( start < 0 )
        {
            start = 0;
        }

        for_less( i, 0, N_selected_displayed )
        {
            if( start + i < model->n_objects )
            {
                get_object_name( model->object_list[start+i], name );

                (void) sprintf( label, "%3d: %s", start + i + 1, name );

                if( model->object_list[start+i]->visibility )
                {
                    if( start+i == selected_index )
                    {
                        col = Selected_vis_colour;
                    }
                    else
                    {
                        col = Unselected_vis_colour;
                    }
                }
                else
                {
                    if( start+i == selected_index )
                    {
                        col = Selected_invis_colour;
                    }
                    else
                    {
                        col = Unselected_invis_colour;
                    }
                }

                set_text_entry( menu_window, i, label, &col );
            }
        }
    }

    set_update_required( menu_window, NORMAL_PLANES );
}
