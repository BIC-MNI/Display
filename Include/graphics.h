#ifndef  DEF_GRAPHICS
#define  DEF_GRAPHICS

#include  <def_graphics_dependent.h>

typedef  struct
{
    Boolean     perspective_flag;
    Point       origin;
    Vector      line_of_sight;
    Vector      horizontal;
    Vector      up;
    Real        front_distance, back_distance;
    Real        window_width, window_height;
} view_struct;

typedef  struct
{
    Colour    colour;
    Surfprop  surfprop;

    int       n_points;
    Point     *points;
    Vector    *normals;

    int       n_triangles;
    int       *triangle_indices;

} triangles_struct;

#endif
