#include <def_display.h>

public  void  transform_screen_to_pixels(
    window_struct  *window,
    Point          *screen,
    Point          *pixels )
{
    Point_x(*pixels) = (Real) (window->x_size - 1) * Point_x(*screen) + 0.5;
    Point_y(*pixels) = (Real) (window->y_size - 1) * Point_y(*screen) + 0.5;
    Point_z(*pixels) = 0.0;
}
