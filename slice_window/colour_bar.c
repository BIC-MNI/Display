#include  <def_display.h>

private  Real  get_y_pos(
    Real    value,
    Real    min_value,
    Real    max_value,
    Real    bottom,
    Real    top );

typedef enum
{
    BAR,
    TICKS,
    FIRST_TEXT
} Colour_bar_objects;

public  void  initialize_colour_bar(
    display_struct    *slice_window )
{
    int               n_vertices;
    object_struct     *object;
    quadmesh_struct   *quadmesh;
    lines_struct      *lines;
    model_struct      *model;
    model_info_struct *model_info;
    colour_bar_struct *colour_bar;

    colour_bar = &slice_window->slice.colour_bar;

    colour_bar->top_offset = Colour_bar_top_offset;
    colour_bar->bottom_offset = Colour_bar_bottom_offset;
    colour_bar->left_offset = Colour_bar_left_offset;
    colour_bar->bar_width = Colour_bar_width;
    colour_bar->tick_width = Colour_bar_tick_width;
    colour_bar->desired_n_intervals = Colour_bar_desired_intervals;

    model = get_graphics_model( slice_window, COLOUR_BAR_MODEL );
    model_info = get_model_info( model );

    model_info->render.shaded_mode = ON;
    model_info->render.shading_type = GOURAUD_SHADING;
    model_info->render.master_light_switch = OFF;
    model_info->render.backface_flag = OFF;

    object = create_object( QUADMESH );

    quadmesh = get_quadmesh_ptr( object );
    n_vertices = 2 * Colour_bar_resolution;
    quadmesh->colour_flag = PER_VERTEX_COLOURS;
    quadmesh->m_closed = FALSE;
    quadmesh->n_closed = FALSE;
    quadmesh->m = Colour_bar_resolution;
    quadmesh->n = 2;

    ALLOC( quadmesh->colours, n_vertices );
    ALLOC( quadmesh->points, n_vertices );

    quadmesh->normals = (Vector *) NULL;

    add_object_to_model( model, object );  

    object = create_object( LINES );

    lines = get_lines_ptr( object );
    lines->colour_flag = PER_ITEM_COLOURS;
    lines->line_thickness = 1;
    lines->n_points = 0;
    lines->n_items = 0;

    add_object_to_model( model, object );  
}

typedef  struct
{
    int      priority;
    Real     value;
} number_entry;

public  void  rebuild_colour_bar(
    display_struct   *slice_window )
{
    int                 i, x_min, x_max, y_min, y_max;
    Real                x, y, bottom, top, range, delta;
    Real                ratio, last_y, next_y, value, min_value, max_value;
    Real                start_threshold, end_threshold;
    Real                x_tick_start, x_tick_end;
    Point               point;
    Colour              colour;
    colour_bar_struct   *colour_bar;
    lines_struct        *lines;
    object_struct       *object;
    text_struct         *text;
    quadmesh_struct     *quadmesh;
    Volume              volume;
    int                 n_numbers;
    number_entry        entry, *numbers;
    model_struct        *model;

    colour_bar = &slice_window->slice.colour_bar;

    get_slice_viewport( slice_window, -1,  &x_min, &x_max, &y_min, &y_max );

    (void) get_slice_window_volume( slice_window, &volume );

    get_volume_real_range( volume, &min_value, &max_value );

    start_threshold = (Real) slice_window->slice.colour_coding.min_value;
    end_threshold = (Real) slice_window->slice.colour_coding.max_value;

    model = get_graphics_model( slice_window, COLOUR_BAR_MODEL );

    /* rebuild the points */

    quadmesh = get_quadmesh_ptr( model->objects[BAR] );

    bottom = y_min + colour_bar->bottom_offset;
    top = y_max - colour_bar->top_offset;

    for_less( i, 0, quadmesh->m )
    {
        ratio = (Real) i / (Real) (quadmesh->m-1);

        /* set the points */

        y = INTERPOLATE( ratio, bottom, top );

        fill_Point( quadmesh->points[IJ(i,1,2)],
                    x_min + colour_bar->left_offset, y, 0.0 );

        fill_Point( quadmesh->points[IJ(i,0,2)],
                    x_min + colour_bar->left_offset + colour_bar->bar_width,
                    y, 0.0 );

        /* set the colours */

        value = INTERPOLATE( ratio, min_value, max_value );

        colour = get_colour_code( &slice_window->slice.colour_coding, value );

        quadmesh->colours[IJ(i,0,2)] = colour;
        quadmesh->colours[IJ(i,1,2)] = colour;
    }

    /* now rebuild the tick marks and numbers */

    lines = get_lines_ptr( model->objects[TICKS] );

    delete_lines( lines );

    lines->n_points = 0;
    lines->n_items = 0;

    x_tick_start = x_min + colour_bar->left_offset + colour_bar->bar_width;
    x_tick_end = x_tick_start + colour_bar->tick_width;

    while( model->n_objects > FIRST_TEXT )
    {
        delete_object( model->objects[FIRST_TEXT] );
        remove_object_from_model( model, FIRST_TEXT );
    }

    range = max_value - min_value;

    delta = get_good_round_value( range /
                                  (Real) Colour_bar_desired_intervals );

    n_numbers = 0;

    value = min_value;
    while( value < max_value )
    {
        entry.value = value;
        if( (value == start_threshold || value == end_threshold) )
            entry.priority = 2;
        else if( value == min_value || value == max_value )
            entry.priority = 1;
        else
            entry.priority = 0;
           
        ADD_ELEMENT_TO_ARRAY( numbers, n_numbers, entry, DEFAULT_CHUNK_SIZE );

        if( value < start_threshold && value + delta > start_threshold )
        {
            entry.value = start_threshold;
            entry.priority = 2;
            ADD_ELEMENT_TO_ARRAY( numbers, n_numbers, entry,
                                  DEFAULT_CHUNK_SIZE );
        }

        if( value < end_threshold && value + delta > end_threshold )
        {
            entry.value = end_threshold;
            entry.priority = 2;
            ADD_ELEMENT_TO_ARRAY( numbers, n_numbers, entry,
                                  DEFAULT_CHUNK_SIZE );
        }

        if( value < max_value && value + delta > max_value )
            value = max_value;
        else
            value += delta;
    }

    last_y = 0.0;

    for_less( i, 0, n_numbers )
    {
        y = get_y_pos( numbers[i].value, min_value, max_value, bottom,
                       top );

        if( i < n_numbers-1 )
            next_y = get_y_pos( numbers[i+1].value, min_value, max_value,
                                bottom, top );

        if( (n_numbers == 0 || y - last_y > Colour_bar_closest_text) &&
            (i == n_numbers-1 ||
                next_y - y > Colour_bar_closest_text ||
                numbers[i].priority > numbers[i+1].priority ) )
        {
            if( numbers[i].priority == 2 )
                colour = Colour_bar_limit_colour;
            else
                colour = Colour_bar_text_colour;

            SET_ARRAY_SIZE( lines->colours, lines->n_items,
                            lines->n_items+1, DEFAULT_CHUNK_SIZE );

            lines->colours[lines->n_items] = colour;

            start_new_line( lines );

            if( numbers[i].priority == 2 )
                x = x_min + colour_bar->left_offset;
            else
                x = x_tick_start;

            fill_Point( point, x, y, 0.0 );

            add_point_to_line( lines, &point );

            fill_Point( point, x_tick_end, y, 0.0 );
            add_point_to_line( lines, &point );

            object = create_object( TEXT );

            text = get_text_ptr( object );
            text->font = (Font_types) Colour_bar_text_font;
            text->size = Colour_bar_text_size;
            text->colour = colour;
            fill_Point( text->origin, x_tick_end,
                        y - G_get_text_height( text->font, text->size ) / 2.0,
                        0.0 );
            (void) sprintf( text->string, Colour_bar_number_format,
                            numbers[i].value );

            add_object_to_model( model, object );

            last_y = y;
        }
    }

    if( n_numbers > 0 )
        FREE( numbers );
}

private  Real  get_y_pos(
    Real    value,
    Real    min_value,
    Real    max_value,
    Real    bottom,
    Real    top )
{
    if( min_value != max_value )
        return( INTERPOLATE( (value - min_value) / (max_value - min_value ),
                             bottom, top ) );
    else
        return( 0.0 );
}

public  Boolean  mouse_within_colour_bar(
    display_struct      *slice_window,
    Real                x,
    Real                y,
    Real                *ratio )
{
    int                 x_min, x_max, y_min, y_max;
    Real                top, bottom;
    Boolean             within;
    colour_bar_struct   *colour_bar;

    colour_bar = &slice_window->slice.colour_bar;

    get_slice_viewport( slice_window, -1, &x_min, &x_max, &y_min, &y_max );

    bottom = (Real) y_min + colour_bar->bottom_offset;
    top = (Real) y_max - colour_bar->top_offset;

    within = y >= bottom && y <= top &&
             ((Real) x_min + colour_bar->left_offset <= x) &&
             (x <= (Real) x_min + colour_bar->left_offset +
                   colour_bar->bar_width + colour_bar->tick_width);

    if( within )
    {
        if( bottom == top )
            *ratio = 0.0;
        else
            *ratio = (y - bottom) / (top - bottom);
    }

    return( within );
}
