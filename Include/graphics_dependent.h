#ifndef  DEF_GRAPHICS_DEPENDENT
#define  DEF_GRAPHICS_DEPENDENT

#define  String   GL_String
#define  Boolean   GL_Boolean

#include <gl/gl.h>

#undef   String
#undef   Boolean

#define  N_DIMENSIONS  3

#define  X_AXIS        0
#define  Y_AXIS        1
#define  Z_AXIS        2

typedef  struct
{
    Coord   coords[N_DIMENSIONS];
} Point;

#define  Point_coord( point, coord ) ((point).coords[coord])
#define  Point_x( point ) Point_coord( point, X_AXIS )
#define  Point_y( point ) Point_coord( point, Y_AXIS )
#define  Point_z( point ) Point_coord( point, Z_AXIS )
#define  fill_Point( point, x, y, z ) \
            { \
                Point_x(point) = (x); \
                Point_y(point) = (y); \
                Point_z(point) = (z); \
            }

typedef  struct
{
    Coord   coords[N_DIMENSIONS];
} Vector;

#define  Vector_coord( vector, coord ) ((vector).coords[coord])
#define  Vector_x( vector ) Vector_coord( vector, X_AXIS )
#define  Vector_y( vector ) Vector_coord( vector, Y_AXIS )
#define  Vector_z( vector ) Vector_coord( vector, Z_AXIS )
#define  fill_Vector( vector, x, y, z ) \
            { \
                Vector_x(vector) = (x); \
                Vector_y(vector) = (y); \
                Vector_z(vector) = (z); \
            }

#define  RED_INDEX     0
#define  GREEN_INDEX   1
#define  BLUE_INDEX    2

typedef  struct
{
    float   rgb[3];
} Colour;

#define  Colour_comp( colour, comp ) ((colour).rgb[comp])
#define  Colour_r( colour ) Colour_comp( colour, RED_INDEX )
#define  Colour_g( colour ) Colour_comp( colour, GREEN_INDEX )
#define  Colour_b( colour ) Colour_comp( colour, BLUE_INDEX )
#define  fill_Colour( colour, r, g, b ) \
            { \
                Colour_r(colour) = (r); \
                Colour_g(colour) = (g); \
                Colour_b(colour) = (b); \
            }

typedef  struct
{
    float   a, d, s;
    Colour  sc;
    float   se;
    float   t;
} Surfprop;;

#define  Surfprop_a( surfprop )  ((surfprop).a)
#define  Surfprop_d( surfprop )  ((surfprop).d)
#define  Surfprop_s( surfprop )  ((surfprop).s)
#define  Surfprop_sc( surfprop )  ((surfprop).sc)
#define  Surfprop_se( surfprop )  ((surfprop).se)
#define  Surfprop_t( surfprop )  ((surfprop).t)

typedef  struct
{
    Matrix    m;
} Transform;

#define  Transform_elem( t, i, j ) ((t).m[j][i])

typedef  long  Window_id;

typedef  unsigned  long   Lcolour;

typedef  struct
{
    Window_id     window_id;
    Lcolour       background_colour;
} window_struct;

#endif
