#include <def_standard.h>
#include <def_graphics.h>
#include <def_geometry.h>

public  void  assign_view_direction( view, line_of_sight, up )
    view_struct    *view;
    Vector         *line_of_sight;
    Vector         *up;
{
    NORMALIZE_VECTOR( view->line_of_sight, *line_of_sight );
    CROSS_VECTORS( view->horizontal, view->line_of_sight, *up );
    CROSS_VECTORS( view->up, view->horizontal, view->line_of_sight );
    NORMALIZE_VECTOR( view->horizontal, view->horizontal );
    NORMALIZE_VECTOR( view->up, view->up );
}
