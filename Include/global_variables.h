START_GLOBALS
    DEF_GLOBAL( Alloc_checking_enabled, BOOLEAN, TRUE )

    DEF_GLOBAL_COLOUR( Initial_background_colour )

    DEF_GLOBAL( Progress_threshold, int, 1000 )

    DEF_GLOBAL( Maximum_display_time, Real, 0.3 )
    DEF_GLOBAL( Check_event_time, Real, 0.2 )
    DEF_GLOBAL( Event_timeout_min, Real, 0.5 )
    DEF_GLOBAL( Min_interval_between_events, Real, 0.01 )
    DEF_GLOBAL( Max_background_seconds, Real, 0.2 )
    DEF_GLOBAL( Event_timeout_factor, Real, 0.5 )
    DEF_GLOBAL( Size_of_interrupted, int, 250 )
    DEF_GLOBAL( Interval_of_check, int, 250 )

    DEF_GLOBAL( Initial_perspective_flag, BOOLEAN, FALSE )
    DEF_GLOBAL( Perspective_distance_factor, Real, 2.0 )
    DEF_GLOBAL( Closest_front_plane, Real, 1.0e-5 )
    DEF_GLOBAL( Initial_render_mode, BOOLEAN, FALSE )
    DEF_GLOBAL( Initial_shading_type, int, 1 )
    DEF_GLOBAL( Initial_light_switch, BOOLEAN, TRUE )
    DEF_GLOBAL( Slice_double_buffer_flag, BOOLEAN, TRUE )
    DEF_GLOBAL( Graphics_double_buffer_flag, BOOLEAN, TRUE )
    DEF_GLOBAL( Graphics_transparency_flag, BOOLEAN, TRUE )
    DEF_GLOBAL( Slice_readout_plane, int, 1 )
    DEF_GLOBAL( Display_frame_info, BOOLEAN, FALSE )
    DEF_GLOBAL( Display_update_min, Real, 1.0 )
    DEF_GLOBAL( Frame_info_x, Real, 10.0 )
    DEF_GLOBAL( Frame_info_y, Real, 10.0 )
    DEF_GLOBAL( Initial_n_pixels_redraw, int, 100 )
    DEF_GLOBAL( Initial_slice_update_time, Real, 0.01 )
    DEF_GLOBAL( Initial_total_slice_update_time1, Real, 0.5 )
    DEF_GLOBAL( Initial_total_slice_update_time2, Real, 2.0 )
    DEF_GLOBAL( Slice_event_check_time, Real, 0.25 )
    DEF_GLOBAL( Initial_incremental_update, BOOLEAN, TRUE )

    DEF_GLOBAL_COLOUR( Volume_outline_colour )
    DEF_GLOBAL_COLOUR( Cross_section_colour )
    DEF_GLOBAL5( Cross_section_spr, Surfprop, 0.3, 0.6, 0.6, 40.0, 1.0 )

    DEF_GLOBAL_COLOUR( Ambient_light_colour )
    DEF_GLOBAL_COLOUR( Directional_light_colour )
    DEF_GLOBAL3( Light_direction, Vector, 1.0, -1.0, -1.0 )

    DEF_GLOBAL( Menu_window_width, int, 850 )
    DEF_GLOBAL( Menu_window_height, int, 260 )
    DEF_GLOBAL( Menu_window_font, int, 1 )
    DEF_GLOBAL( Menu_window_font_size, Real, 10.0 )
    DEF_GLOBAL( X_menu_origin, Real, 70.0 )
    DEF_GLOBAL( X_menu_dx, Real, 10.0 )
    DEF_GLOBAL( X_menu_dy, Real, 0.0 )
    DEF_GLOBAL( X_menu_text_offset, Real, 5.0 )
    DEF_GLOBAL( Y_menu_origin, Real, 10.0 )
    DEF_GLOBAL( Y_menu_dx, Real, -15.0 )
    DEF_GLOBAL( Y_menu_dy, Real, 10.0 )
    DEF_GLOBAL( Y_menu_text_offset, Real, 15.0 )
    DEF_GLOBAL( Menu_n_chars_per_entry, int, 10 )
    DEF_GLOBAL( Menu_n_lines_per_entry, int, 2 )
    DEF_GLOBAL( Menu_character_height, Real, 20.0 )
    DEF_GLOBAL( Menu_character_width, Real, 7.5 )
    DEF_GLOBAL_COLOUR( Menu_character_colour )
    DEF_GLOBAL_COLOUR( Menu_character_inactive_colour )
    DEF_GLOBAL_COLOUR( Menu_box_colour )
    DEF_GLOBAL_COLOUR( Menu_key_colour )
    DEF_GLOBAL( Menu_key_character_offset, Real, 2.0 )
    DEF_GLOBAL( Initial_2_sided_flag, BOOLEAN, FALSE )
    DEF_GLOBAL( Initial_backface_flag, BOOLEAN, TRUE )
    DEF_GLOBAL( Initial_line_curves_flag, BOOLEAN, FALSE )
    DEF_GLOBAL( Initial_n_curve_segments, int, 8 )

    DEF_GLOBAL( Segmenting_connectivity, int, 0 )

    DEF_GLOBAL( Visibility_on_input, BOOLEAN, TRUE )

    DEF_GLOBAL( Initial_x_scale, Real, 1.0 )
    DEF_GLOBAL( Initial_y_scale, Real, 1.0 )
    DEF_GLOBAL( Initial_z_scale, Real, 1.0 )

    DEF_GLOBAL3( Default_line_of_sight, Vector, 0.0, 0.0, -1.0 )
    DEF_GLOBAL3( Default_horizontal, Vector, 1.0, 0.0, 0.0 )

    DEF_GLOBAL( Monitor_widths_to_eye, Real, 1.2 )

    DEF_GLOBAL( Default_marker_type, int, 0 )
    DEF_GLOBAL( Default_marker_structure_id, int, 1 )
    DEF_GLOBAL( Default_marker_patient_id, int, 1 )
    DEF_GLOBAL( Default_marker_size, Real, 1.0 )
    DEF_GLOBAL_COLOUR( Default_marker_colour )
    DEF_GLOBAL_STRING( Default_marker_label, "Marker" )
    DEF_GLOBAL( Marker_pick_size, Real, 3.0 )

    DEF_GLOBAL( N_selected_displayed, int, 12 )
    DEF_GLOBAL( Selected_x_origin, Real, 650.0 )
    DEF_GLOBAL( Selected_y_origin, Real, 240.0 )
    DEF_GLOBAL_COLOUR( Selected_colour )
    DEF_GLOBAL_COLOUR( Visible_colour )
    DEF_GLOBAL_COLOUR( Invisible_colour )
    DEF_GLOBAL( Character_height_in_pixels, int, 10 )
    DEF_GLOBAL( Selected_box_x_offset, Real, 2.0 )
    DEF_GLOBAL( Selected_box_y_offset, Real, 2.0 )

    DEF_GLOBAL_COLOUR( Viewport_feedback_colour )
    DEF_GLOBAL( Viewport_min_x_size, Real, 0.1 )
    DEF_GLOBAL( Viewport_min_y_size, Real, 0.1 )

    DEF_GLOBAL( Slice_magnification_step, Real, 1.2 )
    DEF_GLOBAL( Slice_fit_oversize, Real, 0.05 )
    DEF_GLOBAL_COLOUR( Slice_divider_colour )
    DEF_GLOBAL( Slice_divider_left, int, 5 )
    DEF_GLOBAL( Slice_divider_right, int, 5 )
    DEF_GLOBAL( Slice_divider_top, int, 5 )
    DEF_GLOBAL( Slice_divider_bottom, int, 5 )
    DEF_GLOBAL( Slice_divider_x_position, Real, 0.5 )
    DEF_GLOBAL( Slice_divider_y_position, Real, 0.5 )
    DEF_GLOBAL( Left_panel_width, int, 90 )
    DEF_GLOBAL( Text_panel_height, int, 170 )
    DEF_GLOBAL_COLOUR( Slice_text_colour )
    DEF_GLOBAL( Slice_text_font, int, 0 )
    DEF_GLOBAL( Slice_text_font_size, Real, 18.0 )
    DEF_GLOBAL( Slice_readout_text_font, int, 1 )
    DEF_GLOBAL( Slice_readout_text_font_size, Real, 12.0 )
    DEF_GLOBAL3( Slice_index_offset, Point, 10.0, 5.0, 0.0 )
    DEF_GLOBAL( Readout_text_colour, int, 2 )
    DEF_GLOBAL_COLOUR( Readout_text_rgb_colour )
    DEF_GLOBAL_STRING( Slice_index_x_format, "X = %8g" )
    DEF_GLOBAL_STRING( Slice_index_y_format, "Y = %8g" )
    DEF_GLOBAL_STRING( Slice_index_z_format, "Z = %8g" )
    DEF_GLOBAL_STRING( Slice_probe_volume_index_format, "VI %2d" )
    DEF_GLOBAL_STRING( Slice_probe_x_voxel_format, "Xv %6.1f" )
    DEF_GLOBAL_STRING( Slice_probe_y_voxel_format, "Yv %6.1f" )
    DEF_GLOBAL_STRING( Slice_probe_z_voxel_format, "Zv %6.1f" )
    DEF_GLOBAL_STRING( Slice_probe_x_world_format, "Xw %6.1f" )
    DEF_GLOBAL_STRING( Slice_probe_y_world_format, "Yw %6.1f" )
    DEF_GLOBAL_STRING( Slice_probe_z_world_format, "Zw %6.1f" )
    DEF_GLOBAL_STRING( Slice_probe_voxel_format,   "Vx %6.4g" )
    DEF_GLOBAL_STRING( Slice_probe_val_format,     "Vl %6.4g" )
    DEF_GLOBAL_STRING( Slice_probe_label_format,   "Lb %6d" )
    DEF_GLOBAL( Probe_x_pos, int, 2 )
    DEF_GLOBAL( Probe_y_pos, int, 5 )
    DEF_GLOBAL( Probe_x_delta, int, 0 )
    DEF_GLOBAL( Probe_y_delta, int, 15 )
    DEF_GLOBAL( Slice_view1_axis1, int, 1 )
    DEF_GLOBAL( Slice_view1_axis2, int, 2 )

    DEF_GLOBAL( Slice_view2_axis1, int, 0 )
    DEF_GLOBAL( Slice_view2_axis2, int, 2 )

    DEF_GLOBAL( Slice_view3_axis1, int, 0 )
    DEF_GLOBAL( Slice_view3_axis2, int, 1 )

    DEF_GLOBAL3( Cursor_home, Point, 0.0, 0.0, 0.0 )
    DEF_GLOBAL_COLOUR( Cursor_colour )
    DEF_GLOBAL_COLOUR( Cursor_rgb_colour )
    DEF_GLOBAL( Show_cursor_contours, BOOLEAN, FALSE )
    DEF_GLOBAL( Cursor_contour_overlay_flag, BOOLEAN, FALSE )
    DEF_GLOBAL( Cursor_contour_thickness, Real, 3.0 )
    DEF_GLOBAL( Cursor_beep_on_surface, int, 0 )
    DEF_GLOBAL( Cursor_size_factor, Real, 0.05 )
    DEF_GLOBAL( Cursor_axis_size, Real, 50.0 )

    DEF_GLOBAL( Cursor_hor_start_0, Real, 3.0 )
    DEF_GLOBAL( Cursor_hor_end_0, Real, 10.0 )
    DEF_GLOBAL( Cursor_vert_start_0, Real, 3.0 )
    DEF_GLOBAL( Cursor_vert_end_0, Real, 10.0 )

    DEF_GLOBAL( Cursor_hor_start_1, Real, 3.0 )
    DEF_GLOBAL( Cursor_hor_end_1, Real, 10.0 )
    DEF_GLOBAL( Cursor_vert_start_1, Real, 3.0 )
    DEF_GLOBAL( Cursor_vert_end_1, Real, 10.0 )

    DEF_GLOBAL( Cursor_hor_start_2, Real, 3.0 )
    DEF_GLOBAL( Cursor_hor_end_2, Real, 30.0 )
    DEF_GLOBAL( Cursor_vert_start_2, Real, 3.0 )
    DEF_GLOBAL( Cursor_vert_end_2, Real, 30.0 )

    DEF_GLOBAL( Cursor_hor_start_3, Real, 10.0 )
    DEF_GLOBAL( Cursor_hor_end_3, Real, 30.0 )
    DEF_GLOBAL( Cursor_vert_start_3, Real, 10.0 )
    DEF_GLOBAL( Cursor_vert_end_3, Real, 30.0 )

    DEF_GLOBAL_COLOUR( Slice_cursor_colour1 )
    DEF_GLOBAL_COLOUR( Slice_cursor_colour2 )
    DEF_GLOBAL( Cursor_mouse_threshold, Real, 0.0 )
    DEF_GLOBAL( Max_cursor_angle, Real, 20.0 )
    DEF_GLOBAL( Min_cursor_angle_diff, Real, 5.0 )
    DEF_GLOBAL( Cursor_pick_distance, Real, 10.0 )

    DEF_GLOBAL_COLOUR( Slice_cross_section_colour )
    DEF_GLOBAL_COLOUR( Slice_crop_box_colour )

    DEF_GLOBAL_COLOUR( Extracted_surface_colour )
    DEF_GLOBAL5( Default_surface_property, Surfprop, 0.3, 0.3, 0.4, 10.0, 1.0 )

    DEF_GLOBAL( Min_voxels_per_update, int, 10 )
    DEF_GLOBAL( Max_voxels_per_update, int, 100000 )
    DEF_GLOBAL( Max_seconds_per_voxel_update, Real, 1.0 )
    DEF_GLOBAL( Max_surface_refinements, int, 0 )
    DEF_GLOBAL( Max_surface_error, Real, 0.02 )
    DEF_GLOBAL( Edge_point_threshold, Real, 0.25 )
    DEF_GLOBAL( Edge_point_new_density, Real, 0.125 )
    DEF_GLOBAL( Voxel_validity_if_mixed, BOOLEAN, FALSE )

    DEF_GLOBAL( Max_fast_colour_lookup, int, 100000 )
    DEF_GLOBAL( Colour_table_size, int, 1000 )
    DEF_GLOBAL_COLOUR( Colour_below )
    DEF_GLOBAL( User_defined_n_intervals, int, 4 )
    DEF_GLOBAL_COLOUR( User_defined_min_colour )
    DEF_GLOBAL_COLOUR( User_defined_max_colour )
    DEF_GLOBAL( User_defined_colour_coding_flip, BOOLEAN, TRUE )
    DEF_GLOBAL( Initial_low_limit_position, Real, 0.25 )
    DEF_GLOBAL( Initial_high_limit_position, Real, 0.75 )
    DEF_GLOBAL_COLOUR( Colour_above )
    DEF_GLOBAL( Initial_colour_coding_type, int, 1 )
    DEF_GLOBAL( User_defined_interpolation_space, int, 1 )
    DEF_GLOBAL( Default_filter_type, int, 0 )
    DEF_GLOBAL( Default_filter_width, int, 4.0 )

    DEF_GLOBAL( Colour_bar_resolution, int, 137 )
    DEF_GLOBAL( Colour_bar_top_offset, Real, 10.0 )
    DEF_GLOBAL( Colour_bar_bottom_offset, Real, 10.0 )
    DEF_GLOBAL( Colour_bar_left_offset, Real, 5.0 )
    DEF_GLOBAL( Colour_bar_width, Real, 10.0 )
    DEF_GLOBAL( Colour_bar_tick_width, Real, 3.0 )
    DEF_GLOBAL_COLOUR( Colour_bar_tick_colour )
    DEF_GLOBAL_COLOUR( Colour_bar_text_colour )
    DEF_GLOBAL( Colour_bar_text_font, int, 1 )
    DEF_GLOBAL( Colour_bar_text_size, Real, 10.0 )
    DEF_GLOBAL_COLOUR( Colour_bar_limit_colour )
    DEF_GLOBAL_COLOUR( Colour_bar_range_colour )
    DEF_GLOBAL( Colour_bar_desired_intervals, int, 5 )
    DEF_GLOBAL( Colour_bar_closest_text, Real, 10.0 )
    DEF_GLOBAL_STRING( Colour_bar_number_format, "%g" )

    DEF_GLOBAL( Normal_towards_lower, BOOLEAN, TRUE )

    DEF_GLOBAL( Initial_display_labels, BOOLEAN, TRUE )
    DEF_GLOBAL( Initial_share_labels, BOOLEAN, TRUE )
    DEF_GLOBAL( Initial_num_labels, int, 256 )
    DEF_GLOBAL( Label_colour_opacity, Real, 0.7 )

    DEF_GLOBAL( Use_cursor_origin, BOOLEAN, TRUE )

    DEF_GLOBAL( One_active_flag, BOOLEAN, FALSE )

    DEF_GLOBAL( Output_every, int, 0 )

    DEF_GLOBAL_STRING( Tmp_surface_name, "" )

    DEF_GLOBAL( Marching_cubes_method, int, 1 )
    DEF_GLOBAL( Default_x_voxel_max_distance, int, 100000 )
    DEF_GLOBAL( Default_y_voxel_max_distance, int, 100000 )
    DEF_GLOBAL( Default_z_voxel_max_distance, int, 100000 )

    DEF_GLOBAL( Compute_neighbours_on_input, BOOLEAN, FALSE )
    DEF_GLOBAL( Polygon_bintree_threshold, int, 100 )
    DEF_GLOBAL( Bintree_size_factor, Real, 0.3 )

    DEF_GLOBAL( Save_format, int, 1 )

    DEF_GLOBAL( Max_smoothing_distance, Real, 0.5 )
    DEF_GLOBAL( Smoothing_ratio, Real, 0.5 )
    DEF_GLOBAL( Smoothing_normal_ratio, Real, 0.0 )
    DEF_GLOBAL( Smoothing_threshold, Real, 0.01 )

    DEF_GLOBAL( Cursor_bitplanes, int, 0 )

    DEF_GLOBAL_COLOUR( Surface_curve_colour )
    DEF_GLOBAL( Surface_curve_overlay_flag, BOOLEAN, FALSE )
    DEF_GLOBAL( Surface_curve_thickness, Real, 3.0 )
    DEF_GLOBAL( Line_curvature_weight, Real, 0.0 )
    DEF_GLOBAL( Min_surface_curve_curvature, Real, 0.0 )
    DEF_GLOBAL( Max_surface_curve_curvature, Real, 0.0 )

    DEF_GLOBAL( N_painting_polygons, int, 100 )
    DEF_GLOBAL_COLOUR( Visible_segmenting_colour )
    DEF_GLOBAL_COLOUR( Invisible_segmenting_colour )
    DEF_GLOBAL( Snap_to_polygon_vertex, BOOLEAN, TRUE )

    DEF_GLOBAL( N_fitting_samples, int, 2000 )
    DEF_GLOBAL( Isovalue_factor, Real, 0.0 )
    DEF_GLOBAL( Fitting_isovalue, Real, 1000.0 )
    DEF_GLOBAL( Gradient_strength_factor, Real, 1.0 )
    DEF_GLOBAL( Gradient_strength_exponent, Real, 1.0 )
    DEF_GLOBAL( Curvature_factor, Real, 0.5 )
    DEF_GLOBAL( Surface_point_distance_threshold, Real, 1.0 )
    DEF_GLOBAL( Surface_point_distance_factor, Real, 1.0 )
    DEF_GLOBAL( Fitting_tolerance, Real, 1.0e-4 )
    DEF_GLOBAL( Max_fitting_evaluations, int, 50 )
    DEF_GLOBAL( Surface_model_resolution, int, 30 )
    DEF_GLOBAL( Minimization_method, int, 1 )
    DEF_GLOBAL( Max_parameter_delta, Real, 1.0 )

    DEF_GLOBAL( Max_voxel_scan_distance, Real, 2.0 )
    DEF_GLOBAL( Max_parametric_scan_distance, Real, 0.1 )
    DEF_GLOBAL( Max_polygon_scan_distance, Real, 2.0 )

    DEF_GLOBAL( Initial_atlas_opacity, Real, 1.0 )
    DEF_GLOBAL( Initial_atlas_transparent_threshold, int, 220 )
    DEF_GLOBAL( Initial_atlas_tolerance_x, Real, 1.0 )
    DEF_GLOBAL( Initial_atlas_tolerance_y, Real, 1.0 )
    DEF_GLOBAL( Initial_atlas_tolerance_z, Real, 1.0 )
    DEF_GLOBAL_STRING( Atlas_filename, "/avgbrain/atlas/talairach/obj/Talairach_atlas.list" )
    DEF_GLOBAL( Default_atlas_state, BOOLEAN, FALSE )

    DEF_GLOBAL( Marker_threshold, Real, 1.5 )
    DEF_GLOBAL( Marker_segment_id, int, 1000 )
    DEF_GLOBAL( Use_marker_distances, BOOLEAN, TRUE )

    DEF_GLOBAL( Default_x_brush_radius, Real, 3.0 )
    DEF_GLOBAL( Default_y_brush_radius, Real, 3.0 )
    DEF_GLOBAL( Default_z_brush_radius, Real, 0.0 )
    DEF_GLOBAL( Default_paint_label, int, 1 )
    DEF_GLOBAL( Initial_mouse_scale_factor, Real, 1.0 )
    DEF_GLOBAL( Draw_brush_outline, BOOLEAN, TRUE )
    DEF_GLOBAL_COLOUR( Brush_outline_colour )
    DEF_GLOBAL( Brush_outline_offset, int, 0 )
    DEF_GLOBAL( Snap_brush_to_centres, BOOLEAN, TRUE )
    DEF_GLOBAL( Undo_enabled, BOOLEAN, TRUE )

    DEF_GLOBAL( Volume_continuity, int, 0 )
    DEF_GLOBAL( Convert_volumes_to_byte, BOOLEAN, TRUE )

    DEF_GLOBAL( Move_slice_speed, Real, 0.25 )
    DEF_GLOBAL( Pixels_per_double_size, Real, 100.0 )

    DEF_GLOBAL( Clear_before_polygon_scan, BOOLEAN, FALSE )

    DEF_GLOBAL_COLOUR( Histogram_colour )
    DEF_GLOBAL( Histogram_x_scale, Real, 1.0 )
    DEF_GLOBAL( Histogram_smoothness_ratio, Real, 0.02 )

    DEF_GLOBAL_COLOUR( Menu_name_colour )
    DEF_GLOBAL( Menu_name_x, Real, 5.0 )
    DEF_GLOBAL( Menu_name_y, Real, 20.0 )
    DEF_GLOBAL( Menu_name_font, int, 1 )
    DEF_GLOBAL( Menu_name_font_size, Real, 12.0 )

    DEF_GLOBAL( Use_transparency_hardware, BOOLEAN, TRUE )

    DEF_GLOBAL_STRING( Crop_volume_command, "mincreshape %s %s -start %d,%d,%d -count %d,%d,%d" )

    DEF_GLOBAL( Crop_volumes_on_input, BOOLEAN, FALSE )
    DEF_GLOBAL( Crop_if_smaller, Real, 0.75 )

    DEF_GLOBAL( Initial_slice_continuity, int, -1 )

    DEF_GLOBAL_COLOUR( Cursor_pos_colour )
    DEF_GLOBAL( Cursor_pos_x_origin, int, 400 )
    DEF_GLOBAL( Cursor_pos_y_origin, int, 30 )
    DEF_GLOBAL_STRING( Cursor_pos_title, "Cursor Pos:" )
    DEF_GLOBAL_STRING( Cursor_pos_format, "%.1f %.1f %.1f" )

    DEF_GLOBAL( Crop_label_volumes_threshold, Real, 0.8 )

    DEF_GLOBAL_COLOUR( Unfinished_flag_colour )
    DEF_GLOBAL( Unfinished_flag_width, Real, 5.0 )

END_GLOBALS
