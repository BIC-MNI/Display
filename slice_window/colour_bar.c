#include  <def_graphics.h>
#include  <def_globals.h>

typedef enum
{
    BAR,
    TICKS,
    FIRST_TEXT
} Colour_bar_objects;

public  Status  initialize_colour_bar( slice_window )
    graphics_struct   *slice_window;
{
    Status            status;
    int               n_vertices;
    object_struct     *object;
    quadmesh_struct   *quadmesh;
    model_struct      *model;
    model_struct      *get_graphics_model();
    colour_bar_struct *colour_bar;
    Status            create_object();
    Status            add_object_to_model();

    colour_bar = &slice_window->slice.colour_bar;

    colour_bar->top_offset = Colour_bar_top_offset;
    colour_bar->bottom_offset = Colour_bar_bottom_offset;
    colour_bar->left_offset = Colour_bar_left_offset;
    colour_bar->bar_width = Colour_bar_width;
    colour_bar->tick_width = Colour_bar_tick_width;
    colour_bar->desired_n_intervals = Colour_bar_desired_intervals;

    model = get_graphics_model( slice_window, COLOUR_BAR_MODEL );

    model->render.render_mode = SHADED_MODE;
    model->render.shading_type = GOURAUD_SHADING;
    model->render.master_light_switch = OFF;
    model->render.backface_flag = ON;

    status = create_object( &object, QUADMESH );

    if( status == OK )
    {
        quadmesh = object->ptr.quadmesh;
        n_vertices = 2 * Colour_bar_resolution;
        quadmesh->colour_flag = PER_VERTEX_COLOURS;
        quadmesh->m_closed = FALSE;
        quadmesh->n_closed = FALSE;
        quadmesh->m = Colour_bar_resolution;
        quadmesh->n = 2;
        ALLOC( status, quadmesh->colours, n_vertices );
    }

    if( status == OK )
        ALLOC( status, quadmesh->points, n_vertices );

    if( status == OK )
        ALLOC( status, quadmesh->normals, n_vertices );

    if( status == OK )
        status = add_object_to_model( model, object );  

/*
    if( status == OK )
        status = create_object( &object, LINES );

    if( status == OK )
    {
        lines = object->ptr.lines;
        lines->colour_flag = ONE_COLOUR;
        ALLOC( status, lines->colours, 1 );
        lines->colours[0] = Colour_bar_tick_colour;
        lines->line_thickness = 1;
        lines->n_points = 0;
        lines->n_items = 0;
    }

    if( status == OK )
        status = add_object_to_model( model, object );  
*/

    return( status );
}

public  void  rebuild_colour_bar( slice_window )
    graphics_struct  *slice_window;
{
    int                 i, x_min, x_max, y_min, y_max;
    Real                bottom, top;
    Real                ratio, y, value, min_value, max_value;
    colour_bar_struct   *colour_bar;
    quadmesh_struct     *quadmesh;
    volume_struct       *volume;
    model_struct        *model;
    model_struct        *get_graphics_model();
    Colour              colour;
    void                get_colour_coding();
    void                get_slice_viewport();

    colour_bar = &slice_window->slice.colour_bar;

    get_slice_viewport( slice_window, -1,  &x_min, &x_max, &y_min, &y_max );

    (void) get_slice_window_volume( slice_window, &volume );

    min_value = (Real) volume->min_value;
    max_value = (Real) volume->max_value;

    model = get_graphics_model( slice_window, COLOUR_BAR_MODEL );

    quadmesh = model->object_list[(int) BAR]->ptr.quadmesh;

    bottom = y_min + colour_bar->bottom_offset;
    top = y_max - colour_bar->top_offset;

    for_less( i, 0, quadmesh->m )
    {
        ratio = (Real) i / (Real) (quadmesh->m-1);

        /* set the points */

        y = INTERPOLATE( ratio, bottom, top );

        fill_Point( quadmesh->points[IJ(i,0,2)],
                    x_min + colour_bar->left_offset, y, 0.0 );

        fill_Point( quadmesh->points[IJ(i,1,2)],
                    x_min + colour_bar->left_offset + colour_bar->bar_width,
                    y, 0.0 );

        /* set the colours */

        value = INTERPOLATE( ratio, min_value, max_value );

        get_colour_coding( &slice_window->slice.colour_coding, value,
                           &colour );

        quadmesh->colours[IJ(i,0,2)] = colour;
        quadmesh->colours[IJ(i,1,2)] = colour;
    }
}
