#ifndef  DEF_GRAPHICS_DEPENDENT
#define  DEF_GRAPHICS_DEPENDENT

#define  window   GL_window
#define  normal   GL_normal
#define  poly     GL_poly
#define  String   GL_String
#define  Boolean  GL_Boolean

#include <gl/gl.h>

#undef   window
#undef   normal
#undef   poly
#undef   String
#undef   Boolean

#include <def_view_types.h>

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

typedef  unsigned  long  Pixel_colour;

#define  Pixel_colour_r( p ) \
             ((p) & 255)

#define  Pixel_colour_g( p ) \
             (((p) >> 8) & 255)

#define  Pixel_colour_b( p ) \
             (((p) >> 16) & 255)

#define  RGB_255_TO_PIXEL( r, g, b ) \
             ((r) | ((g) << 8) | ((b) << 16))

#define  RGB_TO_PIXEL( r, g, b, pixel ) \
         { \
             int   r_int, g_int, b_int; \
 \
             r_int = (int) ((r) * 256.0); \
             if( r_int > 255 )  r_int = 255; \
 \
             g_int = (int) ((g) * 256.0); \
             if( g_int > 255 )  g_int = 255; \
 \
             b_int = (int) ((b) * 256.0); \
             if( b_int > 255 )  b_int = 255; \
 \
             (pixel) = RGB_255_TO_PIXEL( r_int, g_int, b_int ); \
         }

#define  COLOUR_TO_PIXEL( col, pixel ) \
         RGB_TO_PIXEL( Colour_r(col), Colour_g(col), Colour_b(col), pixel )

#define  ACCESS_PIXEL( array, x, y, size_x ) \
         (array)[(x) + (size_x) * (y)]

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

#define  fill_Surfprop( s, amb, diff, spec, spec_col, spec_exp, trans ) \
         { \
             Surfprop_a(s) = (amb); \
             Surfprop_d(s) = (diff); \
             Surfprop_s(s) = (spec); \
             Surfprop_sc(s) = (spec_col); \
             Surfprop_se(s) = (spec_exp); \
             Surfprop_t(s) = (trans); \
         }

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
    int           x_origin, y_origin;          
    int           x_size, y_size;          
    Lcolour       background_colour;
    view_types    current_view_type;
    Transform     projection_matrices[(int) N_VIEW_TYPES];
    Transform     viewing_matrices[(int) N_VIEW_TYPES];
    Boolean       this_frame_initialized;
} window_struct;

#define  N_LIGHTS  8

#endif
