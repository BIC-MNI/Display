#ifndef  DEF_GLOBALS
#define  DEF_GLOBALS

#include  <def_global_macros.h>

START_GLOBALS
    DEF_GLOBAL( Progress_threshold, int, 1000 )
    DEF_GLOBAL( Event_timeout, Real, 0.1 )
    DEF_GLOBAL( Initial_perspective_flag, Boolean, TRUE )
    DEF_GLOBAL( Closest_front_plane, Real, 1.0e-5 )
    DEF_GLOBAL( Initial_render_mode, int, 1 )
    DEF_GLOBAL( Initial_shading_type, int, 1 )
    DEF_GLOBAL( Initial_light_switch, Boolean, TRUE )
    DEF_GLOBAL( Display_frame_info, Boolean, FALSE )
    DEF_GLOBAL( Display_update_min, Real, 0.0 )
    DEF_GLOBAL( Frame_info_x, Real, 0.8 )
    DEF_GLOBAL( Frame_info_y, Real, 0.1 )
    DEF_GLOBAL( Menu_window_width, int, 400 )
    DEF_GLOBAL( Menu_window_height, int, 150 )
    DEF_GLOBAL( X_menu_origin, Real, 10.0 )
    DEF_GLOBAL( X_menu_dx, Real, 50.0 )
    DEF_GLOBAL( X_menu_dy, Real, 0.0 )
    DEF_GLOBAL( X_menu_box_size, Real, 40.0 )
    DEF_GLOBAL( X_menu_text_offset, Real, 2.0 )
    DEF_GLOBAL( Y_menu_origin, Real, 10.0 )
    DEF_GLOBAL( Y_menu_dx, Real, -4.0 )
    DEF_GLOBAL( Y_menu_dy, Real, 20.0 )
    DEF_GLOBAL( Y_menu_box_size, Real, 15.0 )
    DEF_GLOBAL( Y_menu_text_offset, Real, 2.0 )
    DEF_GLOBAL( Initial_2_sided_flag, Boolean, TRUE )
    DEF_GLOBAL( Initial_backface_flag, Boolean, FALSE )
    DEF_GLOBAL( Visibility_on_input, Boolean, FALSE )

    DEF_GLOBAL( Initial_x_scale, Real, 1.0 )
    DEF_GLOBAL( Initial_y_scale, Real, 1.0 )
    DEF_GLOBAL( Initial_z_scale, Real, 1.0 )

    DEF_GLOBAL( Default_line_of_sight, Vector, 0.0 )
    DEF_GLOBAL( Default_horizontal, Vector, 0.0 )

    DEF_GLOBAL( Monitor_widths_to_eye, Real, 1.5 )
END_GLOBALS
         
#endif
