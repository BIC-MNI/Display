
#include  <def_graphics.h>

public  void  draw_2d_rectangle( graphics, colour, x1, y1, x2, y2 )
    graphics_struct   *graphics;
    Colour            *colour;
    Real              x1, y1, x2, y2;
{
    void      G_set_view_type();
    static    Point     corners[4];
    static    int       end_indices[] = { 5 };
    static    int       indices[]     = { 0, 1, 2, 3, 0 };
    static    lines_struct  lines = {
                                        { 0.0, 0.0, 0.0 },
                                        4,
                                        corners,
                                        1,
                                        end_indices,
                                        indices
                                    };
    void      G_set_view_type();
    void      G_draw_lines();

    G_set_view_type( &graphics->window, SCREEN_VIEW );

    fill_Point( corners[0], x1, y1, 0.0 );
    fill_Point( corners[1], x2, y1, 0.0 );
    fill_Point( corners[2], x2, y2, 0.0 );
    fill_Point( corners[3], x1, y2, 0.0 );

    lines.colour = *colour;

    G_draw_lines( &graphics->window, &lines, (render_struct *) 0,
                  (update_interrupted_struct *) 0, FALSE );
}
