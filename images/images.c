#include  <display.h>
#include  <images.h>

public  Status   save_window_to_file(
    display_struct  *display,
    char            filename[],
    int             x_min,
    int             x_max,
    int             y_min,
    int             y_max )
{
    Status          status;
    int             x_size, y_size;
    pixels_struct   pixels;

    if( x_min > x_max || y_min > y_max )
    {
        G_get_window_size( display->window, &x_size, &y_size );
        x_min = 0;
        x_max = x_size-1;
        y_min = 0;
        y_max = y_size-1;
    }
    else
    {
        x_size = x_max - x_min + 1;
        y_size = y_max - y_min + 1;
    }

    initialize_pixels( &pixels, 0, 0, x_size, y_size, 1.0, 1.0, RGB_PIXEL );

    G_read_pixels( display->window, x_min, x_max, y_min, y_max,
                   &PIXEL_RGB_COLOUR(pixels,0,0) );

    status = output_rgb_file( filename, &pixels );

    delete_pixels( &pixels );

    return( status );
}
