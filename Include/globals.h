#ifndef  DEF_GLOBALS
#define  DEF_GLOBALS

#include  <def_geometry.h>

#undef  START_GLOBALS
#undef  END_GLOBALS
#undef  DEF_GLOBAL


#ifdef  IN_GLOBALS_FILE

#ifdef  DEFINING_GLOBALS

#define  START_GLOBALS
#define  END_GLOBALS

#define  DEF_GLOBAL( variable, type, initial ) \
    type  variable = initial;

#else

typedef  enum {
                 Boolean_type,
                 int_type,
                 Real_type,
                 String_type,
                 Point_type,
                 Vector_type,
                 Colour_type,
                 Surfprop_type
              } variable_types;

typedef  struct 
{
    char             *ptr_to_global;
    String           variable_name;
    variable_types   type;
    Boolean          initialized_from_file;
} global_struct;

#define  START_GLOBALS   static  global_struct  globals[] = {
#define  END_GLOBALS                                        };

#define  DEF_GLOBAL( variable, type, initial ) \
       { \
           (char *) &variable, "variable", type/**/_type, FALSE \
       },

#endif

#else

#define  START_GLOBALS
#define  END_GLOBALS

#define  DEF_GLOBAL( variable, type, initial ) \
         extern  type  variable;

#endif

START_GLOBALS
    DEF_GLOBAL( Initial_perspective_flag, Boolean, TRUE )
    DEF_GLOBAL( Closest_front_plane, Real, 1.0e-5 )
    DEF_GLOBAL( Initial_render_mode, int, 1 )
    DEF_GLOBAL( Initial_shading_type, int, 1 )
    DEF_GLOBAL( Initial_light_switch, Boolean, TRUE )
    DEF_GLOBAL( Frame_number_x, Real, 0.8 )
    DEF_GLOBAL( Frame_number_y, Real, 0.1 )
END_GLOBALS
         

#endif
