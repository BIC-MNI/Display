
#include  <def_display.h>

public  void  draw_2d_line(
    display_struct    *display,
    View_types        view_type,
    Colour            colour,
    Real              x1,
    Real              y1,
    Real              x2,
    Real              y2 )
{
    static    Point     end_points[2];
    static    int       end_indices[] = { 2 };
    static    int       indices[]     = { 0, 1 };
    static    lines_struct  lines = {
                                        ONE_COLOUR,
                                        (Colour *) 0,
                                        1,
                                        2,
                                        end_points,
                                        1,
                                        end_indices,
                                        indices
                                    };

    G_set_view_type( display->window, view_type );

    fill_Point( end_points[0], x1, y1, 0.0 );
    fill_Point( end_points[1], x2, y2, 0.0 );

    ALLOC( lines.colours, 1 );

    lines.colours[0] = colour;

    set_render_info( display->window, get_main_render(display) );

    G_draw_lines( display->window, &lines );

    FREE( lines.colours );
}

public  void  draw_2d_rectangle(
    display_struct    *display,
    View_types        view_type,
    Colour            colour,
    Real              x1,
    Real              y1,
    Real              x2,
    Real              y2 )
{
    static    Point     corners[4];
    static    int       end_indices[] = { 5 };
    static    int       indices[]     = { 0, 1, 2, 3, 0 };
    static    lines_struct  lines = {
                                        ONE_COLOUR,
                                        (Colour *) 0,
                                        1,
                                        4,
                                        corners,
                                        1,
                                        end_indices,
                                        indices
                                    };

    G_set_view_type( display->window, view_type );

    fill_Point( corners[0], x1, y1, 0.0 );
    fill_Point( corners[1], x2, y1, 0.0 );
    fill_Point( corners[2], x2, y2, 0.0 );
    fill_Point( corners[3], x1, y2, 0.0 );

    ALLOC( lines.colours, 1 );

    lines.colours[0] = colour;

    set_render_info( display->window, get_main_render(display) );
    G_draw_lines( display->window, &lines );

    FREE( lines.colours );
}

public  void  draw_polygons(
    display_struct    *display,
    polygons_struct   *polygons )
{
    G_set_view_type( display->window, MODEL_VIEW );

    set_render_info( display->window, get_main_render(display) );

    G_draw_polygons( display->window, polygons );
}

public  render_struct  *get_main_render(
    display_struct    *display )
{
    return( &get_model_info(get_model_ptr(display->models[THREED_MODEL]))
                                              ->render );
}

public  void  draw_text_3d(
    display_struct    *display,
    Point             *origin,
    Colour            colour,
    char              str[] )
{
    text_struct    text;

    G_set_view_type( display->window, MODEL_VIEW );

    text.origin = *origin;
    text.colour = colour;
    (void) strcpy( text.string, str );

    G_draw_text( display->window, &text );
}
