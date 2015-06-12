/**
 * \file selected.c
 * \brief Build and maintain the object list associated with the marker
 * or object window.
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

static  void  create_selected_text(
    marker_window_struct *marker,
    model_struct         *model )
{
    int            i;
    object_struct  *object;
    VIO_Point      origin;
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
                    marker->selected_x_origin,
                    marker->selected_y_origin - marker->font_size * 2.0 * i,
                    0.0 );

        initialize_text( text, &origin, BLACK,
                         (Font_types) Menu_window_font, marker->font_size );

        add_object_to_model( model, object );
    }
}

static  void  set_text_entry(
    display_struct    *marker_window,
    int               index,
    VIO_STR            name,
    VIO_Colour            col )
{
    model_struct   *model;

    model = get_graphics_model( marker_window, SELECTED_MODEL );

    replace_string( &get_text_ptr(model->objects[index+1])->string,
                    create_string(name) );
    set_object_visibility( model->objects[index+1], TRUE );
    
    get_text_ptr(model->objects[index+1])->colour = col;
}

static  void  get_box_limits(
    marker_window_struct  *marker,
    int                 index,
    VIO_STR              label,
    int                 *x_min,
    int                 *x_max,
    int                 *y_min,
    int                 *y_max )
{
    int            width;

    width = (int) G_get_text_length( label, (Font_types) Menu_window_font,
                                     marker->font_size );

    if( width <= 0 )
        width = 20;

    *x_min = VIO_ROUND( marker->selected_x_origin - marker->selected_x_offset );
    *x_max = VIO_ROUND(*x_min + width + marker->selected_x_offset );

    *y_min = VIO_ROUND( marker->selected_y_origin -
                        marker->font_size * 2.0 * index - 
                        marker->selected_y_offset);
    *y_max = VIO_ROUND( *y_min + marker->font_size + marker->selected_y_offset);
}

static  void  set_current_box(
    marker_window_struct  *marker,
    model_struct        *selected_model,
    int                 index,
    VIO_STR              label )
{
    int            x_start, x_end, y_start, y_end;
    VIO_Point          *points;

    get_box_limits( marker, index, label, &x_start, &x_end, &y_start, &y_end );

    points = get_lines_ptr(selected_model->objects[0])->points;

    fill_Point( points[0], x_start, y_start, 0.0 );
    fill_Point( points[1], x_end, y_start, 0.0 );
    fill_Point( points[2], x_end, y_end, 0.0 );
    fill_Point( points[3], x_start, y_end, 0.0 );

    set_object_visibility( selected_model->objects[0], TRUE );
}

static  void  get_model_objects_visible(
    display_struct    *display,
    int               *start_index,
    int               *n_objects )
{
    int            selected_index;
    model_struct   *model;

    if( current_object_is_top_level( display ) )
    {
        *start_index = 0;
        *n_objects = 1;
    }
    else
    {
        model = get_current_model( display );
        selected_index = get_current_object_index( display );

        *start_index = selected_index - N_selected_displayed / 2;

        if( *start_index > model->n_objects - N_selected_displayed )
            *start_index = model->n_objects - N_selected_displayed;

        if( *start_index < 0 )
            *start_index = 0;

        *n_objects = MIN( N_selected_displayed,
                          model->n_objects - *start_index );
    }
}

static  VIO_STR  get_object_label(
    object_struct   *object,
    int             index )
{
    char      buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    VIO_STR    name;

    name = get_object_name( object );

    (void) sprintf( buffer, "%3d: %s", index, name );

    delete_string( name );

    return( create_string( buffer ) );
}

  void  rebuild_selected_list(
    display_struct    *display,
    display_struct    *marker_window )
{
    int            i, start_index, n_objects, selected_index;
    VIO_Colour         col;
    VIO_STR         label;
    model_struct   *selected_model, *model;
    text_struct    *text;

    selected_model = get_graphics_model( marker_window, SELECTED_MODEL );

    if( selected_model->n_objects == 0 )
        create_selected_text( &marker_window->marker, selected_model );

    for_less( i, 0, N_selected_displayed+1 )
        set_object_visibility( selected_model->objects[i], FALSE );

    model = get_current_model( display );

    get_model_objects_visible( display, &start_index, &n_objects );

    selected_index = get_current_object_index( display );

    for_less( i, start_index, start_index + n_objects )
    {
        label = get_object_label( model->objects[i], i );

        if( get_object_visibility( model->objects[i] ) )
        {
            if( !get_object_colour(model->objects[i], &col ) )
                col = Visible_colour;
        }
        else
            col = Invisible_colour;

        set_text_entry( marker_window, i - start_index, label, col );

        if( i == selected_index )
            set_current_box( &marker_window->marker,
                             selected_model, i - start_index, label );

        delete_string( label );

        text = get_text_ptr( selected_model->objects[i-start_index+1] );
        text->size = marker_window->marker.font_size;
        fill_Point( text->origin,
                    marker_window->marker.selected_x_origin,
                    marker_window->marker.selected_y_origin -
                    marker_window->marker.font_size * 2.0 *
                    (VIO_Real) (i - start_index),
                    0.0 );
    }

    set_update_required( marker_window, NORMAL_PLANES );
}

  VIO_BOOL  mouse_is_on_object_name(
    display_struct    *display,
    int               x,
    int               y,
    object_struct     **object_under_mouse )
{
    int            i, start_index, n_objects;
    int            x_min, x_max, y_min, y_max;
    VIO_STR         label;
    model_struct   *model;
    display_struct *marker_window;

    marker_window = display->associated[MARKER_WINDOW];

    model = get_current_model( display );
    get_model_objects_visible( display, &start_index, &n_objects );

    for_less( i, start_index, start_index + n_objects )
    {
        label = get_object_label( model->objects[i], i );

        get_box_limits( &marker_window->marker, i - start_index, label,
                        &x_min, &x_max, &y_min, &y_max );

        delete_string( label );

        if( x_min <= x && x <= x_max && y_min <= y && y <= y_max )
        {
            *object_under_mouse = model->objects[i];
            return( TRUE );
        }
    }

    return( FALSE );
}
