
#include  <def_display.h>

public  Boolean  mouse_moved(
    display_struct   *display,
    Real             *new_x,
    Real             *new_y,
    Real             *old_x,
    Real             *old_y )
{
    Boolean   moved;

    moved = G_get_mouse_position_0_to_1( display->window, new_x, new_y );

    *old_x = Point_x( display->prev_mouse_position );
    *old_y = Point_y( display->prev_mouse_position );

    if( moved )
    {
        moved = *new_x != *old_x || *new_y != *old_y;

        record_mouse_position( display );
    }

    return( moved );
}

public  Boolean  pixel_mouse_moved(
    display_struct   *display,
    int              *new_x,
    int              *new_y,
    int              *old_x,
    int              *old_y )
{
    Boolean   moved;

    moved = G_get_mouse_position( display->window, new_x, new_y );

    *old_x = ROUND( Point_x( display->prev_mouse_position ) );
    *old_y = ROUND( Point_y( display->prev_mouse_position ) );

    if( moved )
    {
        moved = *new_x != *old_x || *new_y != *old_y;

        record_mouse_pixel_position( display );
    }

    return( moved );
}

public  void  record_mouse_position(
    display_struct   *display )
{
    Real   x, y;

    (void) G_get_mouse_position_0_to_1( display->window, &x, &y );

    fill_Point( display->prev_mouse_position, x, y, 0.0 );
}

public  void  record_mouse_pixel_position(
    display_struct   *display )
{
    int   x, y;

    (void) G_get_mouse_position( display->window, &x, &y );

    fill_Point( display->prev_mouse_position, (Real) x, (Real) y, 0.0 );
}
