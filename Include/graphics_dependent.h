#ifndef  DEF_GRAPHICS_DEPENDENT
#define  DEF_GRAPHICS_DEPENDENT

#include <def_mni.h>
#include <def_geom_structs.h>

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
#undef   BLACK
#undef   WHITE
#undef   RED
#undef   GREEN
#undef   BLUE
#undef   CYAN
#undef   MAGENTA
#undef   YELLOW

#define  MAX_OVERLAY_INDEX     3

#define  ACCESS_PIXEL( array, x, y, size_x ) \
         (array)[(x) + (size_x) * (y)]

typedef  long  Window_id;

typedef  unsigned  long   Lcolour;

typedef  struct
{
    Window_id       window_id;
    Boolean         double_buffer_flag;
    int             x_origin, y_origin;          
    int             x_size, y_size;          
    Lcolour         background_colour_pixel;
    Colour          background_colour;
    View_types      current_view_type;
    Transform       projection_matrices[(int) N_VIEW_TYPES];
    Transform       viewing_matrices[(int) N_VIEW_TYPES];
    Bitplane_types  current_bitplanes;
    Boolean         bitplanes_cleared[N_BITPLANE_TYPES];
    Colour          overlay_colour_table[MAX_OVERLAY_INDEX];
} window_struct;

#define  N_LIGHTS  8

#define  NO_BELL       0
#define  SHORT_BELL    1
#define  LONG_BELL     2

#endif
