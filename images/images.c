#include  <def_display.h>

public  Status   save_window_to_file(
    display_struct  *display,
    char            filename[] )
{
    Status          status;
    int             x_size, y_size;
    object_struct   *object;
    pixels_struct   *pixels;

    object = create_object( PIXELS );
    pixels = get_pixels_ptr( object );

    G_get_window_size( display->window, &x_size, &y_size );

    initialize_pixels( pixels, 0, 0, x_size, y_size, 1.0, 1.0, RGB_PIXEL );

    G_read_pixels( display->window, 0, x_size-1, 0, y_size-1,
                   &PIXEL_RGB_COLOUR(*pixels,0,0) );
    
    status = output_graphics_file( filename, BINARY_FORMAT, 1, &object );

    delete_object( object );

    return( status );
}
