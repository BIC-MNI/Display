#ifndef  DEF_GLOBALS
#define  DEF_GLOBALS

#include  <def_global_macros.h>

START_GLOBALS
    DEF_GLOBAL( Initial_background_colour, Colour, 0.0 )

    DEF_GLOBAL( Progress_threshold, int, 1000 )

    DEF_GLOBAL( Maximum_display_time, Real, 0.5 )
    DEF_GLOBAL( Event_timeout_min, Real, 0.1 )
    DEF_GLOBAL( Event_timeout_factor, Real, 0.5 )
    DEF_GLOBAL( Size_of_interrupted, int, 1000 )
    DEF_GLOBAL( Interval_of_check, int, 100 )

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
    DEF_GLOBAL( X_menu_dx, Real, 10.0 )
    DEF_GLOBAL( X_menu_dy, Real, 0.0 )
    DEF_GLOBAL( X_menu_text_offset, Real, 2.0 )
    DEF_GLOBAL( Y_menu_origin, Real, 10.0 )
    DEF_GLOBAL( Y_menu_dx, Real, -4.0 )
    DEF_GLOBAL( Y_menu_dy, Real, 20.0 )
    DEF_GLOBAL( Y_menu_text_offset, Real, 2.0 )
    DEF_GLOBAL( Menu_n_chars_per_entry, int, 10 )
    DEF_GLOBAL( Menu_n_lines_per_entry, int, 3 )
    DEF_GLOBAL( Menu_character_height, Real, 20.0 )
    DEF_GLOBAL( Menu_character_width, Real, 20.0 )
    DEF_GLOBAL( Initial_2_sided_flag, Boolean, TRUE )
    DEF_GLOBAL( Initial_backface_flag, Boolean, FALSE )
    DEF_GLOBAL( Visibility_on_input, Boolean, FALSE )

    DEF_GLOBAL( Initial_x_scale, Real, 1.0 )
    DEF_GLOBAL( Initial_y_scale, Real, 1.0 )
    DEF_GLOBAL( Initial_z_scale, Real, 1.0 )

    DEF_GLOBAL( Default_line_of_sight, Vector, 0.0 )
    DEF_GLOBAL( Default_horizontal, Vector, 0.0 )

    DEF_GLOBAL( Monitor_widths_to_eye, Real, 1.5 )

    DEF_GLOBAL( N_selected_displayed, int, 3 )
    DEF_GLOBAL( Selected_x_origin, Real, 0.0 )
    DEF_GLOBAL( Selected_y_origin, Real, 0.0 )
    DEF_GLOBAL( Selected_x_delta, Real, 0.0 )
    DEF_GLOBAL( Selected_y_delta, Real, 0.0 )
    DEF_GLOBAL( Selected_vis_colour, Colour, 0.0 )
    DEF_GLOBAL( Selected_invis_colour, Colour, 0.0 )
    DEF_GLOBAL( Unselected_vis_colour, Colour, 0.0 )
    DEF_GLOBAL( Unselected_invis_colour, Colour, 0.0 )

    DEF_GLOBAL( Viewport_feedback_colour, Colour, 0.0 )
    DEF_GLOBAL( Viewport_min_x_size, Real, 0.1 )
    DEF_GLOBAL( Viewport_min_y_size, Real, 0.1 )

    DEF_GLOBAL( Slice_divider_colour, Colour, 0.0 )
    DEF_GLOBAL( Slice_divider_left, int, 5 )
    DEF_GLOBAL( Slice_divider_right, int, 5 )
    DEF_GLOBAL( Slice_divider_top, int, 5 )
    DEF_GLOBAL( Slice_divider_bottom, int, 25 )
    DEF_GLOBAL( Slice_text_colour, Colour, 0.0 )
    DEF_GLOBAL( Slice_index_offset, Point, 0.0 )
    DEF_GLOBAL_STRING( Slice_index_x_format, "" )
    DEF_GLOBAL_STRING( Slice_index_y_format, "" )
    DEF_GLOBAL_STRING( Slice_index_z_format, "" )
    DEF_GLOBAL_STRING( Slice_index_x_locked_format, "" )
    DEF_GLOBAL_STRING( Slice_index_y_locked_format, "" )
    DEF_GLOBAL_STRING( Slice_index_z_locked_format, "" )
    DEF_GLOBAL_STRING( Slice_index_xc_format, "" )
    DEF_GLOBAL_STRING( Slice_index_yc_format, "" )
    DEF_GLOBAL_STRING( Slice_index_zc_format, "" )
    DEF_GLOBAL_STRING( Slice_probe_x_format, "" )
    DEF_GLOBAL_STRING( Slice_probe_y_format, "" )
    DEF_GLOBAL_STRING( Slice_probe_z_format, "" )
    DEF_GLOBAL_STRING( Slice_probe_val_format, "" )
    DEF_GLOBAL( Probe_x_pos, int, 10 )
    DEF_GLOBAL( Probe_y_pos, int, 10 )
    DEF_GLOBAL( Probe_x_delta, int, 20 )
    DEF_GLOBAL( Probe_y_delta, int, 20 )
    DEF_GLOBAL( Slice_view1_axis1, int, 1 )
    DEF_GLOBAL( Slice_view1_flip1, Boolean, FALSE )
    DEF_GLOBAL( Slice_view1_axis2, int, 2 )
    DEF_GLOBAL( Slice_view1_flip2, Boolean, FALSE )
    DEF_GLOBAL( Slice_view1_axis3, int, 3 )
    DEF_GLOBAL( Slice_view1_flip3, Boolean, FALSE )

    DEF_GLOBAL( Slice_view2_axis1, int, 1 )
    DEF_GLOBAL( Slice_view2_flip1, Boolean, FALSE )
    DEF_GLOBAL( Slice_view2_axis2, int, 2 )
    DEF_GLOBAL( Slice_view2_flip2, Boolean, FALSE )
    DEF_GLOBAL( Slice_view2_axis3, int, 3 )
    DEF_GLOBAL( Slice_view2_flip3, Boolean, FALSE )

    DEF_GLOBAL( Slice_view3_axis1, int, 1 )
    DEF_GLOBAL( Slice_view3_flip1, Boolean, FALSE )
    DEF_GLOBAL( Slice_view3_axis2, int, 2 )
    DEF_GLOBAL( Slice_view3_flip2, Boolean, FALSE )
    DEF_GLOBAL( Slice_view3_axis3, int, 3 )
    DEF_GLOBAL( Slice_view3_flip3, Boolean, FALSE )

    DEF_GLOBAL( Cursor_colour, Colour, 0.0 )
    DEF_GLOBAL( Show_cursor_contours, Boolean, FALSE )
    DEF_GLOBAL( Cursor_colour_on_surface, Colour, 0.0 )
    DEF_GLOBAL( Cursor_colour_off_surface, Colour, 0.0 )
    DEF_GLOBAL( Cursor_beep_on_surface, int, 1 )
    DEF_GLOBAL( Cursor_size_factor, Real, 0.05 )
    DEF_GLOBAL( Cursor_axis_size, Real, 10.0 )
    DEF_GLOBAL( Cursor_start_pixel_x, int, 3 )
    DEF_GLOBAL( Cursor_end_pixel_x, int, 10 )
    DEF_GLOBAL( Cursor_start_pixel_y, int, 3 )
    DEF_GLOBAL( Cursor_end_pixel_y, int, 10 )
    DEF_GLOBAL( Cursor_start_pixel_z, int, 3 )
    DEF_GLOBAL( Cursor_end_pixel_z, int, 10 )
    DEF_GLOBAL( Slice_cursor_colour, Colour, 0.0 )
    DEF_GLOBAL( Cursor_mouse_threshold, Real, 0.02 )
    DEF_GLOBAL( Max_cursor_angle, Real, 10.0 )
    DEF_GLOBAL( Min_cursor_angle_diff, Real, 20.0 )
    DEF_GLOBAL( Cursor_pick_distance, Real, 0.02 )

    DEF_GLOBAL( Extracted_surface_colour, Colour, 0.0 )
    DEF_GLOBAL( Display_surface_in_slices, Boolean, FALSE )
    DEF_GLOBAL( Default_surface_property, Surfprop, 0.0 )

    DEF_GLOBAL( Min_voxels_per_update, int, 50 )
    DEF_GLOBAL( Max_voxels_per_update, int, 300 )
    DEF_GLOBAL( Max_seconds_per_voxel_update, Real, 1.0 )
    DEF_GLOBAL( Max_surface_refinements, int, 5 )
    DEF_GLOBAL( Max_surface_error, Real, 0.1 )
    DEF_GLOBAL( Edge_point_threshold, Real, 1.0 )
    DEF_GLOBAL( Edge_point_new_density, Real, 0.5 )

    DEF_GLOBAL( Max_fast_colour_lookup, int, 100000 )
    DEF_GLOBAL( Colour_table_size, int, 1000 )
    DEF_GLOBAL( Colour_below, Colour, 0.0 )
    DEF_GLOBAL( Min_colour, Colour, 0.0 )
    DEF_GLOBAL( Max_colour, Colour, 0.0 )
    DEF_GLOBAL( Colour_above, Colour, 0.0 )
    DEF_GLOBAL( Interpolation_space, int, 0 )

    DEF_GLOBAL( Normal_towards_lower, Boolean, TRUE )

    DEF_GLOBAL( Display_activities, Boolean, FALSE )
    DEF_GLOBAL( Inactive_voxel_colour, Colour, 0.0 )
    DEF_GLOBAL( Labeled_voxel_colour, Colour, 0.0 )
    DEF_GLOBAL( Inactive_and_labeled_voxel_colour, Colour, 0.0 )

    DEF_GLOBAL( Use_cursor_origin, Boolean, TRUE )

    DEF_GLOBAL( One_active_flag, Boolean, FALSE )

    DEF_GLOBAL( Output_every, int, 100000 )

    DEF_GLOBAL_STRING( Tmp_surface_name, "" )

    DEF_GLOBAL( Marching_cubes_method, int, 1 )

    DEF_GLOBAL( Compute_neighbours_on_input, Boolean, FALSE )
    DEF_GLOBAL( Polygon_bintree_threshold, int, 100 )
    DEF_GLOBAL( Bintree_size_factor, Real, 2.0 )

    DEF_GLOBAL( Marker_colour, Colour, 1.0 )
    DEF_GLOBAL( Marker_size, Real, 1.0 )

    DEF_GLOBAL( Save_format, int, 1 )

    DEF_GLOBAL( Max_smoothing_distance, Real, 1.0 )
    DEF_GLOBAL( Smoothing_ratio, Real, 0.3 )
    DEF_GLOBAL( Smoothing_threshold, Real, 0.05 )

END_GLOBALS
         
#endif
