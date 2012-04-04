#ifndef  DEF_display_prototypes
#define  DEF_display_prototypes

public  Status  change_global_variable(
    STRING   str,
    STRING   *variable_name,
    STRING   *new_value );

public  Status  set_global_variable_value(
    STRING   variable_name,
    STRING   new_value );

public  void  display_objects(
    Gwindow                     window,
    object_struct               *object,
    update_interrupted_struct   *interrupt,
    Bitplane_types              bitplanes,
    BOOLEAN                     *past_last_object );

public  void  initialize_window_callbacks(
    display_struct    *display_window );

public  Status   main_event_loop( void );

public  BOOLEAN  window_is_up_to_date(
    display_struct   *display );

public  BOOLEAN  is_shift_key_pressed( void );

public  int  get_list_of_windows(
    display_struct  ***display );

public  display_struct  *lookup_window(
    Gwindow   window );

public  display_struct  *get_main_window( void );

public  void  initialize_graphics( void );

public  void  terminate_graphics( void );

public  Status  create_graphics_window(
    window_types      window_type,
    BOOLEAN           double_buffering,
    display_struct    **display,
    STRING            title,
    int               width,
    int               height );

public  model_struct  *get_graphics_model(
    display_struct    *display,
    int               model_index );

public  model_info_struct  *get_model_info(
    model_struct   *model );

public  Bitplane_types  get_model_bitplanes(
    model_struct   *model );

public  void  create_model_after_current(
    display_struct   *display );

public  void  initialize_model_info(
    model_struct   *model );

public  void  initialize_3D_model_info(
    model_struct   *model );

public  void  initialize_display_model(
    model_struct   *model );

public  void  terminate_display_model(
    model_struct   *model );

public  Bitplane_types  get_cursor_bitplanes( void );

public  void  set_update_required(
    display_struct   *display,
    Bitplane_types   which_bitplanes );

public  BOOLEAN  graphics_normal_planes_update_required(
    display_struct   *display );

public  BOOLEAN  graphics_update_required(
    display_struct   *display );

public  void  graphics_models_have_changed(
    display_struct  *display );

public  void  update_graphics(
    display_struct               *display,
    update_interrupted_struct    *interrupt );

public  void  delete_graphics_window(
    display_struct   *display );

public  void  update_view(
    display_struct  *display );

public  void  fit_view_to_visible_models(
    display_struct   *display );

public  void  reset_view_parameters(
    display_struct   *display,
    Vector           *line_of_sight,
    Vector           *horizontal );

public  Real  size_of_domain(
    display_struct   *display );

public  void  initialize_three_d_window(
    display_struct   *display );

public  void  define_lights(
    display_struct   *display );

public  void  delete_three_d(
    display_struct  *display );

public  void  add_object_to_current_model(
    display_struct   *display,
    object_struct     *object );

public  display_struct  *get_three_d_window(
    display_struct  *display );

public  void  convert_transform_to_view_space(
    display_struct    *display,
    Transform         *transform,
    Transform         *view_space_transform );

public  void  apply_transform_in_view_space(
    display_struct    *display,
    Transform         *transform );

public  void  transform_model(
    display_struct    *display,
    Transform         *transform );

public  void  get_inverse_model_transform(
    display_struct    *display,
    Transform         *inverse_transform );

public  void  initialize_atlas(
    atlas_struct   *atlas );

public  void  delete_atlas(
    atlas_struct   *atlas );

public  void  regenerate_atlas_lookup(
    display_struct    *slice_window );

public  void  set_atlas_state(
    display_struct    *slice_window,
    BOOLEAN           state );

public  BOOLEAN  is_atlas_loaded(
    display_struct  *display );

public  BOOLEAN  render_atlas_slice_to_pixels(
    atlas_struct  *atlas,
    Colour        image[],
    int           image_x_size,
    int           image_y_size,
    Real          world_start[],
    Real          world_x_axis[],
    Real          world_y_axis[] );

public  Status  load_graphics_file( 
    display_struct   *display,
    STRING           filename,
    BOOLEAN          is_label_file );

public  Status   input_volume_file(
    STRING         filename,
    Volume         *volume_ptr );

public  DEF_MENU_FUNCTION( set_atlas_on_or_off );

public  DEF_MENU_UPDATE(set_atlas_on_or_off );

public  DEF_MENU_FUNCTION( set_atlas_opacity );

public  DEF_MENU_UPDATE(set_atlas_opacity );

public  DEF_MENU_FUNCTION( set_atlas_transparent_threshold );

public  DEF_MENU_UPDATE(set_atlas_transparent_threshold );

public  DEF_MENU_FUNCTION( flip_atlas_x );

public  DEF_MENU_UPDATE(flip_atlas_x );

public  DEF_MENU_FUNCTION( flip_atlas_y );

public  DEF_MENU_UPDATE(flip_atlas_y );

public  DEF_MENU_FUNCTION( flip_atlas_z );

public  DEF_MENU_UPDATE(flip_atlas_z );

public  DEF_MENU_FUNCTION( set_atlas_tolerance_x );

public  DEF_MENU_UPDATE(set_atlas_tolerance_x );

public  DEF_MENU_FUNCTION( set_atlas_tolerance_y );

public  DEF_MENU_UPDATE(set_atlas_tolerance_y );

public  DEF_MENU_FUNCTION( set_atlas_tolerance_z );

public  DEF_MENU_UPDATE(set_atlas_tolerance_z );

public  DEF_MENU_FUNCTION( menu_set_global_variable );

public  DEF_MENU_UPDATE(menu_set_global_variable );

public  DEF_MENU_FUNCTION( show_memory );

public  DEF_MENU_UPDATE(show_memory );

public  DEF_MENU_FUNCTION(set_colour_limits );

public  DEF_MENU_UPDATE(set_colour_limits );

public  DEF_MENU_FUNCTION(set_contour_colour_map );

public  DEF_MENU_UPDATE(set_contour_colour_map );

public  DEF_MENU_FUNCTION(set_hot_metal );

public  DEF_MENU_UPDATE(set_hot_metal );

public  DEF_MENU_FUNCTION(set_gray_scale );

public  DEF_MENU_UPDATE(set_gray_scale );

public  DEF_MENU_FUNCTION(set_red );

public  DEF_MENU_UPDATE(set_red );

public  DEF_MENU_FUNCTION(set_green );

public  DEF_MENU_UPDATE(set_green );

public  DEF_MENU_FUNCTION(set_blue );

public  DEF_MENU_UPDATE(set_blue );

public  DEF_MENU_FUNCTION(set_spectral );

public  DEF_MENU_UPDATE(set_spectral );

public  DEF_MENU_FUNCTION(set_arbitrary_colour_scale );

public  DEF_MENU_UPDATE(set_arbitrary_colour_scale );

public  DEF_MENU_FUNCTION(set_user_defined_colour_scale );

public  DEF_MENU_UPDATE(set_user_defined_colour_scale );

public  DEF_MENU_FUNCTION(set_under_colour );

public  DEF_MENU_UPDATE(set_under_colour );

public  DEF_MENU_FUNCTION(set_over_colour );

public  DEF_MENU_UPDATE(set_over_colour );

public  DEF_MENU_FUNCTION(set_label_colour_ratio );

public  DEF_MENU_UPDATE(set_label_colour_ratio );

public  DEF_MENU_FUNCTION(set_nearest_neighbour );

public  DEF_MENU_UPDATE(set_nearest_neighbour );

public  DEF_MENU_FUNCTION(set_linear_interpolation );

public  DEF_MENU_UPDATE(set_linear_interpolation );

public  DEF_MENU_FUNCTION(set_box_filter );

public  DEF_MENU_UPDATE(set_box_filter );

public  DEF_MENU_FUNCTION(set_triangle_filter );

public  DEF_MENU_UPDATE(set_triangle_filter );

public  DEF_MENU_FUNCTION(set_gaussian_filter );

public  DEF_MENU_UPDATE(set_gaussian_filter );

public  DEF_MENU_FUNCTION(set_filter_half_width );

public  DEF_MENU_UPDATE(set_filter_half_width );

public  DEF_MENU_FUNCTION(set_slice_window_n_labels );

public  DEF_MENU_UPDATE(set_slice_window_n_labels );

public  DEF_MENU_FUNCTION(toggle_share_labels );

public  DEF_MENU_UPDATE(toggle_share_labels );

public  DEF_MENU_FUNCTION(save_colour_map );

public  DEF_MENU_UPDATE(save_colour_map );

public  DEF_MENU_FUNCTION(load_colour_map );

public  DEF_MENU_UPDATE(load_colour_map );

public  DEF_MENU_FUNCTION(load_user_defined_colour_scale );

public  DEF_MENU_UPDATE(load_user_defined_colour_scale );

public  DEF_MENU_FUNCTION( load_file );

public  DEF_MENU_UPDATE(load_file );

public  DEF_MENU_FUNCTION( save_file );

public  DEF_MENU_UPDATE(save_file );

public  DEF_MENU_FUNCTION( georges_menu1 );

public  DEF_MENU_UPDATE(georges_menu1 );

public  DEF_MENU_FUNCTION( georges_menu2 );

public  DEF_MENU_UPDATE(georges_menu2 );

public  DEF_MENU_FUNCTION( georges_menu3 );

public  DEF_MENU_UPDATE(georges_menu3 );

public  DEF_MENU_FUNCTION( georges_menu4 );

public  DEF_MENU_UPDATE(georges_menu4 );

public  DEF_MENU_FUNCTION( smooth_current_lines );

public  DEF_MENU_UPDATE(smooth_current_lines );

public  DEF_MENU_FUNCTION( make_current_line_tube );

public  DEF_MENU_UPDATE(make_current_line_tube );

public  DEF_MENU_FUNCTION( convert_line_to_spline_points );

public  DEF_MENU_UPDATE(convert_line_to_spline_points );

public  DEF_MENU_FUNCTION( make_line_circle );

public  DEF_MENU_UPDATE(make_line_circle );

public  DEF_MENU_FUNCTION( subdivide_current_lines );

public  DEF_MENU_UPDATE(subdivide_current_lines );

public  DEF_MENU_FUNCTION( convert_markers_to_lines );

public  DEF_MENU_UPDATE(convert_markers_to_lines );

public  DEF_MENU_FUNCTION( convert_markers_to_closed_lines );

public  DEF_MENU_UPDATE(convert_markers_to_closed_lines );

public  DEF_MENU_FUNCTION( set_line_widths );

public  DEF_MENU_UPDATE(set_line_widths );

public  DEF_MENU_FUNCTION( reverse_normals );

public  DEF_MENU_UPDATE(reverse_normals );

public  DEF_MENU_FUNCTION( advance_visible );

public  DEF_MENU_UPDATE(advance_visible );

public  DEF_MENU_FUNCTION( retreat_visible );

public  DEF_MENU_UPDATE(retreat_visible );

public  DEF_MENU_FUNCTION( make_all_invisible );

public  DEF_MENU_UPDATE(make_all_invisible );

public  DEF_MENU_FUNCTION( make_all_visible );

public  DEF_MENU_UPDATE(make_all_visible );

public  DEF_MENU_FUNCTION( advance_selected );

public  DEF_MENU_UPDATE(advance_selected );

public  DEF_MENU_FUNCTION( retreat_selected );

public  DEF_MENU_UPDATE(retreat_selected );

public  DEF_MENU_FUNCTION( descend_selected );

public  DEF_MENU_UPDATE(descend_selected );

public  DEF_MENU_FUNCTION( ascend_selected );

public  DEF_MENU_UPDATE(ascend_selected );

public  DEF_MENU_FUNCTION( toggle_object_visibility );

public  DEF_MENU_UPDATE(toggle_object_visibility );

public  DEF_MENU_FUNCTION( create_model );

public  DEF_MENU_UPDATE(create_model );

public  DEF_MENU_FUNCTION( change_model_name );

public  DEF_MENU_UPDATE(change_model_name );

public  DEF_MENU_FUNCTION( delete_current_object );

public  DEF_MENU_UPDATE(delete_current_object );

public  DEF_MENU_FUNCTION( set_current_object_colour );

public  DEF_MENU_UPDATE(set_current_object_colour );

public  DEF_MENU_FUNCTION( set_current_object_surfprop );

public  DEF_MENU_UPDATE(set_current_object_surfprop );

public  DEF_MENU_FUNCTION( cut_object );

public  DEF_MENU_UPDATE(cut_object );

public  DEF_MENU_FUNCTION( paste_object );

public  DEF_MENU_UPDATE(paste_object );

public  DEF_MENU_FUNCTION( mark_vertices );

public  DEF_MENU_UPDATE(mark_vertices );

public  DEF_MENU_FUNCTION( flip_object );

public  DEF_MENU_UPDATE(flip_object );

public  DEF_MENU_FUNCTION( scan_current_object_to_volume );

public  DEF_MENU_UPDATE(scan_current_object_to_volume );

public  void  set_marker_to_defaults(
    display_struct  *display,
    marker_struct   *marker );

public  void  create_marker_at_position(
    display_struct    *display,
    Point             *position,
    STRING            label );

public  DEF_MENU_FUNCTION( create_marker_at_cursor );

public  DEF_MENU_UPDATE(create_marker_at_cursor );

public  DEF_MENU_FUNCTION( set_cursor_to_marker );

public  DEF_MENU_UPDATE(set_cursor_to_marker );

public  DEF_MENU_FUNCTION( save_markers );

public  DEF_MENU_UPDATE(save_markers );

public  DEF_MENU_FUNCTION( set_default_marker_structure_id );

public  DEF_MENU_UPDATE(set_default_marker_structure_id );

public  DEF_MENU_FUNCTION( set_default_marker_patient_id );

public  DEF_MENU_UPDATE(set_default_marker_patient_id );

public  DEF_MENU_FUNCTION( set_default_marker_size );

public  DEF_MENU_UPDATE(set_default_marker_size );

public  DEF_MENU_FUNCTION( set_default_marker_colour );

public  DEF_MENU_UPDATE(set_default_marker_colour );

public  DEF_MENU_FUNCTION( set_default_marker_type );

public  DEF_MENU_UPDATE(set_default_marker_type );

public  DEF_MENU_FUNCTION( set_default_marker_label );

public  DEF_MENU_UPDATE(set_default_marker_label );

public  DEF_MENU_FUNCTION( change_marker_structure_id );

public  DEF_MENU_UPDATE(change_marker_structure_id );

public  DEF_MENU_FUNCTION( change_marker_patient_id );

public  DEF_MENU_UPDATE(change_marker_patient_id );

public  DEF_MENU_FUNCTION( change_marker_type );

public  DEF_MENU_UPDATE(change_marker_type );

public  DEF_MENU_FUNCTION( change_marker_size );

public  DEF_MENU_UPDATE(change_marker_size );

public  DEF_MENU_FUNCTION( change_marker_position );

public  DEF_MENU_UPDATE(change_marker_position );

public  DEF_MENU_FUNCTION( change_marker_label );

public  DEF_MENU_UPDATE(change_marker_label );

public  DEF_MENU_FUNCTION( copy_defaults_to_marker );

public  DEF_MENU_UPDATE(copy_defaults_to_marker );

public  DEF_MENU_FUNCTION( copy_defaults_to_markers );

public  DEF_MENU_UPDATE(copy_defaults_to_markers );

public  DEF_MENU_FUNCTION( move_cursor_to_home );

public  DEF_MENU_UPDATE(move_cursor_to_home );

public  BOOLEAN  get_current_polygons(
    display_struct      *display,
    polygons_struct     **polygons );

public  DEF_MENU_FUNCTION( input_polygons_bintree );

public  DEF_MENU_UPDATE(input_polygons_bintree );

public  DEF_MENU_FUNCTION( save_polygons_bintree );

public  DEF_MENU_UPDATE(save_polygons_bintree );

public  DEF_MENU_FUNCTION( create_bintree_for_polygons );

public  DEF_MENU_UPDATE(create_bintree_for_polygons );

public  DEF_MENU_FUNCTION( create_normals_for_polygon );

public  DEF_MENU_UPDATE(create_normals_for_polygon );

public  DEF_MENU_FUNCTION( average_normals_for_polygon );

public  DEF_MENU_UPDATE(average_normals_for_polygon );

public  DEF_MENU_FUNCTION( smooth_current_polygon );

public  DEF_MENU_UPDATE(smooth_current_polygon );

public  DEF_MENU_FUNCTION( reverse_polygons_order );

public  DEF_MENU_UPDATE(reverse_polygons_order );

public  DEF_MENU_FUNCTION( make_polygon_sphere );

public  DEF_MENU_UPDATE(make_polygon_sphere );

public  DEF_MENU_FUNCTION( make_tetrahedral_sphere );

public  DEF_MENU_UPDATE(make_tetrahedral_sphere );

public  DEF_MENU_FUNCTION( subdivide_current_polygon );

public  DEF_MENU_UPDATE(subdivide_current_polygon );

public  DEF_MENU_FUNCTION( reset_polygon_neighbours );

public  DEF_MENU_UPDATE(reset_polygon_neighbours );

public  DEF_MENU_FUNCTION( cut_polygon_neighbours );

public  DEF_MENU_UPDATE(cut_polygon_neighbours );

public  DEF_MENU_FUNCTION( set_polygon_line_thickness );

public  DEF_MENU_UPDATE(set_polygon_line_thickness );

public  DEF_MENU_FUNCTION( print_polygons_surface_area );

public  DEF_MENU_UPDATE(print_polygons_surface_area );

public  DEF_MENU_FUNCTION( coalesce_current_polygons );

public  DEF_MENU_UPDATE(coalesce_current_polygons );

public  DEF_MENU_FUNCTION( separate_current_polygons );

public  DEF_MENU_UPDATE(separate_current_polygons );

public  DEF_MENU_FUNCTION( exit_program );

public  DEF_MENU_UPDATE(exit_program );

public  DEF_MENU_FUNCTION( set_paint_xy_brush_radius );

public  DEF_MENU_UPDATE(set_paint_xy_brush_radius );

public  DEF_MENU_FUNCTION( set_paint_z_brush_radius );

public  DEF_MENU_UPDATE(set_paint_z_brush_radius );

public  DEF_MENU_FUNCTION( set_current_paint_label );

public  DEF_MENU_UPDATE(set_current_paint_label );

public  DEF_MENU_FUNCTION( set_current_erase_label );

public  DEF_MENU_UPDATE(set_current_erase_label );

public  DEF_MENU_FUNCTION( set_label_colour );

public  DEF_MENU_UPDATE(set_label_colour );

public  DEF_MENU_FUNCTION( copy_labels_from_lower_slice );

public  DEF_MENU_UPDATE(copy_labels_from_lower_slice );

public  DEF_MENU_FUNCTION( copy_labels_from_higher_slice );

public  DEF_MENU_UPDATE(copy_labels_from_higher_slice );

public  DEF_MENU_FUNCTION( toggle_display_labels );

public  DEF_MENU_UPDATE(toggle_display_labels );

public  DEF_MENU_FUNCTION( change_labels_in_range );

public  DEF_MENU_UPDATE(change_labels_in_range );

public  DEF_MENU_FUNCTION( calculate_volume );

public  DEF_MENU_UPDATE(calculate_volume );

public  DEF_MENU_FUNCTION( flip_labels_in_x );

public  DEF_MENU_UPDATE(flip_labels_in_x );

public  DEF_MENU_FUNCTION( translate_labels_up );

public  DEF_MENU_UPDATE(translate_labels_up );

public  DEF_MENU_FUNCTION( translate_labels_down );

public  DEF_MENU_UPDATE(translate_labels_down );

public  DEF_MENU_FUNCTION( translate_labels_left );

public  DEF_MENU_UPDATE(translate_labels_left );

public  DEF_MENU_FUNCTION( translate_labels_right );

public  DEF_MENU_UPDATE(translate_labels_right );

public  DEF_MENU_FUNCTION( undo_slice_labels );

public  DEF_MENU_UPDATE(undo_slice_labels );

public  DEF_MENU_FUNCTION( translate_labels_arbitrary );

public  DEF_MENU_UPDATE(translate_labels_arbitrary );

public  DEF_MENU_FUNCTION( toggle_fast_update );

public  DEF_MENU_UPDATE(toggle_fast_update );

public  DEF_MENU_FUNCTION( toggle_cursor_follows_paintbrush );

public  DEF_MENU_UPDATE(toggle_cursor_follows_paintbrush );

public  DEF_MENU_FUNCTION( toggle_render_mode );

public  DEF_MENU_UPDATE(toggle_render_mode );

public  DEF_MENU_FUNCTION( toggle_shading );

public  DEF_MENU_UPDATE(toggle_shading );

public  DEF_MENU_FUNCTION( toggle_lights );

public  DEF_MENU_UPDATE(toggle_lights );

public  DEF_MENU_FUNCTION( toggle_two_sided );

public  DEF_MENU_UPDATE(toggle_two_sided );

public  DEF_MENU_FUNCTION( toggle_backfacing );

public  DEF_MENU_UPDATE(toggle_backfacing );

public  DEF_MENU_FUNCTION( toggle_line_curve_flag );

public  DEF_MENU_UPDATE(toggle_line_curve_flag );

public  DEF_MENU_FUNCTION( toggle_marker_label_flag );

public  DEF_MENU_UPDATE(toggle_marker_label_flag );

public  DEF_MENU_FUNCTION( set_n_curve_segments );

public  DEF_MENU_UPDATE(set_n_curve_segments );

public  DEF_MENU_FUNCTION( toggle_double_buffer_threed );

public  DEF_MENU_UPDATE(toggle_double_buffer_threed );

public  DEF_MENU_FUNCTION( toggle_double_buffer_slice );

public  DEF_MENU_UPDATE(toggle_double_buffer_slice );

public  DEF_MENU_FUNCTION( change_background_colour );

public  DEF_MENU_UPDATE(change_background_colour );

public  DEF_MENU_FUNCTION( label_voxel );

public  DEF_MENU_UPDATE(label_voxel );

public  DEF_MENU_FUNCTION( clear_voxel );

public  DEF_MENU_UPDATE(clear_voxel );

public  DEF_MENU_FUNCTION( reset_segmenting );

public  DEF_MENU_UPDATE(reset_segmenting );

public  DEF_MENU_FUNCTION( set_segmenting_threshold );

public  DEF_MENU_UPDATE(set_segmenting_threshold );

public  Status  input_label_volume_file(
    display_struct   *display,
    STRING           filename );

public  DEF_MENU_FUNCTION(load_label_data);

public  DEF_MENU_UPDATE(load_label_data );

public  DEF_MENU_FUNCTION(save_label_data);

public  DEF_MENU_UPDATE(save_label_data );

public  Status input_tag_label_file(
    display_struct   *display,
    STRING           filename );

public  DEF_MENU_FUNCTION( load_labels );

public  DEF_MENU_UPDATE(load_labels );

public  DEF_MENU_FUNCTION( save_labels );

public  DEF_MENU_UPDATE(save_labels );

public  DEF_MENU_FUNCTION( save_current_label );

public  DEF_MENU_UPDATE(save_current_label );

public  DEF_MENU_FUNCTION(label_slice);

public  DEF_MENU_UPDATE(label_slice );

public  DEF_MENU_FUNCTION(clear_slice);

public  DEF_MENU_UPDATE(clear_slice );

public  DEF_MENU_FUNCTION(clear_connected);

public  DEF_MENU_UPDATE(clear_connected );

public  DEF_MENU_FUNCTION(label_connected);

public  DEF_MENU_UPDATE(label_connected );

public  DEF_MENU_FUNCTION(label_connected_no_threshold);

public  DEF_MENU_UPDATE(label_connected_no_threshold );

public  DEF_MENU_FUNCTION(label_connected_3d);

public  DEF_MENU_UPDATE(label_connected_3d );

public  DEF_MENU_FUNCTION(clear_label_connected_3d);

public  DEF_MENU_UPDATE(clear_label_connected_3d );

public  DEF_MENU_FUNCTION(dilate_labels);

public  DEF_MENU_UPDATE(dilate_labels );

public  DEF_MENU_FUNCTION(erode_labels);

public  DEF_MENU_UPDATE(erode_labels );

public  DEF_MENU_FUNCTION(toggle_connectivity);

public  DEF_MENU_UPDATE(toggle_connectivity );

public  DEF_MENU_FUNCTION(toggle_crop_labels_on_output);

public  DEF_MENU_UPDATE(toggle_crop_labels_on_output);

public  DEF_MENU_FUNCTION( reset_polygon_visibility );

public  DEF_MENU_UPDATE(reset_polygon_visibility );

public  DEF_MENU_FUNCTION( remove_invisible_parts_of_polygon );

public  DEF_MENU_UPDATE(remove_invisible_parts_of_polygon);

public  DEF_MENU_FUNCTION( set_n_paint_polygons );

public  DEF_MENU_UPDATE(set_n_paint_polygons);

public  DEF_MENU_FUNCTION( set_vis_paint_colour );

public  DEF_MENU_UPDATE(set_vis_paint_colour);

public  DEF_MENU_FUNCTION( set_invis_paint_colour );

public  DEF_MENU_UPDATE(set_invis_paint_colour);

public  DEF_MENU_FUNCTION( set_connected_invisible );

public  DEF_MENU_UPDATE(set_connected_invisible );

public  DEF_MENU_FUNCTION( paint_invisible );

public  DEF_MENU_UPDATE(paint_invisible);

public  DEF_MENU_FUNCTION( paint_visible );

public  DEF_MENU_UPDATE(paint_visible);

public  DEF_MENU_FUNCTION( set_connected_vis_colour );

public  DEF_MENU_UPDATE(set_connected_vis_colour );

public  DEF_MENU_FUNCTION( set_connected_invis_colour );

public  DEF_MENU_UPDATE(set_connected_invis_colour );

public  DEF_MENU_FUNCTION( paint_invis_colour );

public  DEF_MENU_UPDATE(paint_invis_colour);

public  DEF_MENU_FUNCTION( paint_vis_colour );

public  DEF_MENU_UPDATE(paint_vis_colour);

public  DEF_MENU_FUNCTION( set_visibility_from_colour );

public  DEF_MENU_UPDATE(set_visibility_from_colour);

public  DEF_MENU_FUNCTION( set_invis_colour_to_invis );

public  DEF_MENU_UPDATE(set_invis_colour_to_invis);

public  DEF_MENU_FUNCTION( set_vis_to_invis_colour );

public  DEF_MENU_UPDATE(set_vis_to_invis_colour);

public  DEF_MENU_FUNCTION( set_vis_to_vis_colour );

public  DEF_MENU_UPDATE(set_vis_to_vis_colour);

public  DEF_MENU_FUNCTION( crop_above_plane );

public  DEF_MENU_UPDATE(crop_above_plane);

public  DEF_MENU_FUNCTION( crop_below_plane );

public  DEF_MENU_UPDATE(crop_below_plane);

public  DEF_MENU_FUNCTION( save_polygons_visibilities );

public  DEF_MENU_UPDATE(save_polygons_visibilities);

public  DEF_MENU_FUNCTION( load_polygons_visibilities );

public  DEF_MENU_UPDATE(load_polygons_visibilities);

public  DEF_MENU_FUNCTION( start_surface_line );

public  DEF_MENU_UPDATE(start_surface_line);

public  DEF_MENU_FUNCTION( end_surface_line );

public  DEF_MENU_UPDATE(end_surface_line);

public  DEF_MENU_FUNCTION( close_surface_line );

public  DEF_MENU_UPDATE(close_surface_line);

public  DEF_MENU_FUNCTION( reset_surface_line );

public  DEF_MENU_UPDATE(reset_surface_line);

public  DEF_MENU_FUNCTION( make_surface_line_permanent );

public  DEF_MENU_UPDATE(make_surface_line_permanent);

public  DEF_MENU_FUNCTION( set_line_curvature_weight );

public  DEF_MENU_UPDATE(set_line_curvature_weight );

public  DEF_MENU_FUNCTION( set_surface_curve_curvature );

public  DEF_MENU_UPDATE(set_surface_curve_curvature );

public  DEF_MENU_FUNCTION( pick_surface_point_on_line );

public  DEF_MENU_UPDATE(pick_surface_point_on_line );

public  DEF_MENU_FUNCTION(start_volume_isosurface );

public  DEF_MENU_UPDATE(start_volume_isosurface );

public  DEF_MENU_FUNCTION(start_volume_binary_isosurface );

public  DEF_MENU_UPDATE(start_volume_binary_isosurface );

public  DEF_MENU_FUNCTION(start_label_binary_isosurface );

public  DEF_MENU_UPDATE(start_label_binary_isosurface );

public  DEF_MENU_FUNCTION(toggle_surface_extraction);

public  DEF_MENU_UPDATE(toggle_surface_extraction );

public  DEF_MENU_FUNCTION(reset_surface);

public  DEF_MENU_UPDATE(reset_surface );

public  DEF_MENU_FUNCTION(make_surface_permanent);

public  DEF_MENU_UPDATE(make_surface_permanent );

public  DEF_MENU_FUNCTION(get_voxelated_label_surface);

public  DEF_MENU_UPDATE(get_voxelated_label_surface );

public  DEF_MENU_FUNCTION(get_voxelated_surface);

public  DEF_MENU_UPDATE(get_voxelated_surface );

public  DEF_MENU_FUNCTION( set_surface_invalid_label_range );

public  DEF_MENU_UPDATE(set_surface_invalid_label_range );

public  DEF_MENU_FUNCTION( make_view_fit );

public  DEF_MENU_UPDATE(make_view_fit );

public  DEF_MENU_FUNCTION( reset_view );

public  DEF_MENU_UPDATE(reset_view );

public  DEF_MENU_FUNCTION( right_tilted_view );

public  DEF_MENU_UPDATE(right_tilted_view );

public  DEF_MENU_FUNCTION( left_tilted_view );

public  DEF_MENU_UPDATE(left_tilted_view );

public  DEF_MENU_FUNCTION( top_view );

public  DEF_MENU_UPDATE(top_view );

public  DEF_MENU_FUNCTION( bottom_view );

public  DEF_MENU_UPDATE(bottom_view );

public  DEF_MENU_FUNCTION( front_view );

public  DEF_MENU_UPDATE(front_view );

public  DEF_MENU_FUNCTION( back_view );

public  DEF_MENU_UPDATE(back_view );

public  DEF_MENU_FUNCTION( left_view );

public  DEF_MENU_UPDATE(left_view );

public  DEF_MENU_FUNCTION( right_view );

public  DEF_MENU_UPDATE(right_view );

public  DEF_MENU_FUNCTION( toggle_perspective );

public  DEF_MENU_UPDATE(toggle_perspective );

public  DEF_MENU_FUNCTION( magnify_view );

public  DEF_MENU_UPDATE(magnify_view );

public  DEF_MENU_FUNCTION( translate_view );

public  DEF_MENU_UPDATE(translate_view );

public  DEF_MENU_FUNCTION( rotate_view );

public  DEF_MENU_UPDATE(rotate_view );

public  DEF_MENU_FUNCTION( front_clipping );

public  DEF_MENU_UPDATE(front_clipping );

public  DEF_MENU_FUNCTION( back_clipping );

public  DEF_MENU_UPDATE(back_clipping );

public  DEF_MENU_FUNCTION( pick_view_rectangle );

public  DEF_MENU_UPDATE(pick_view_rectangle );

public  DEF_MENU_FUNCTION( create_film_loop );

public  DEF_MENU_UPDATE(create_film_loop );

public  DEF_MENU_FUNCTION( save_image );

public  DEF_MENU_UPDATE(save_image );

public  DEF_MENU_FUNCTION( toggle_stereo_mode );

public  DEF_MENU_UPDATE(toggle_stereo_mode );

public  DEF_MENU_FUNCTION( set_eye_separation );

public  DEF_MENU_UPDATE(set_eye_separation );

public  DEF_MENU_FUNCTION( print_view );

public  DEF_MENU_UPDATE(print_view );

public  DEF_MENU_FUNCTION(type_in_3D_origin);

public  DEF_MENU_UPDATE(type_in_3D_origin);

public  DEF_MENU_FUNCTION(type_in_view_origin);

public  DEF_MENU_UPDATE(type_in_view_origin);

public  DEF_MENU_FUNCTION(type_in_view_line_of_sight);

public  DEF_MENU_UPDATE(type_in_view_line_of_sight);

public  DEF_MENU_FUNCTION(type_in_view_up_dir);

public  DEF_MENU_UPDATE(type_in_view_up_dir);

public  DEF_MENU_FUNCTION(type_in_view_window_width);

public  DEF_MENU_UPDATE(type_in_view_window_width);

public  DEF_MENU_FUNCTION(type_in_view_perspective_distance);

public  DEF_MENU_UPDATE(type_in_view_perspective_distance);

public  DEF_MENU_FUNCTION(move_slice_plus);

public  DEF_MENU_UPDATE(move_slice_plus );

public  DEF_MENU_FUNCTION(move_slice_minus);

public  DEF_MENU_UPDATE(move_slice_minus );

public  DEF_MENU_FUNCTION(toggle_slice_visibility);

public  DEF_MENU_UPDATE(toggle_slice_visibility );

public  DEF_MENU_FUNCTION(toggle_cross_section_visibility);

public  DEF_MENU_UPDATE(toggle_cross_section_visibility );

public  DEF_MENU_FUNCTION(reset_current_slice_view);

public  DEF_MENU_UPDATE(reset_current_slice_view );

public  DEF_MENU_FUNCTION(colour_code_objects );

public  DEF_MENU_UPDATE(colour_code_objects );

public  DEF_MENU_FUNCTION(create_3d_slice);

public  DEF_MENU_UPDATE(create_3d_slice);

public  DEF_MENU_FUNCTION(create_3d_slice_profile);

public  DEF_MENU_UPDATE(create_3d_slice_profile);

public  DEF_MENU_FUNCTION(resample_slice_window_volume);

public  DEF_MENU_UPDATE(resample_slice_window_volume);

public  DEF_MENU_FUNCTION(box_filter_slice_window_volume);

public  DEF_MENU_UPDATE(box_filter_slice_window_volume);

public  DEF_MENU_FUNCTION(pick_slice_angle_point);

public  DEF_MENU_UPDATE(pick_slice_angle_point);

public  DEF_MENU_FUNCTION( rotate_slice_axes );

public  DEF_MENU_UPDATE(rotate_slice_axes );

public  DEF_MENU_FUNCTION(reset_slice_crop);

public  DEF_MENU_UPDATE(reset_slice_crop);

public  DEF_MENU_FUNCTION(toggle_slice_crop_visibility);

public  DEF_MENU_UPDATE(toggle_slice_crop_visibility);

public  DEF_MENU_FUNCTION(pick_crop_box_edge);

public  DEF_MENU_UPDATE(pick_crop_box_edge);

public  DEF_MENU_FUNCTION(set_crop_box_filename);

public  DEF_MENU_UPDATE(set_crop_box_filename);

public  DEF_MENU_FUNCTION(load_cropped_volume);

public  DEF_MENU_UPDATE(load_cropped_volume);

public  DEF_MENU_FUNCTION(crop_volume_to_file);

public  DEF_MENU_UPDATE(crop_volume_to_file);

public  DEF_MENU_FUNCTION(redo_histogram);

public  DEF_MENU_UPDATE(redo_histogram);

public  DEF_MENU_FUNCTION(redo_histogram_labeled);

public  DEF_MENU_UPDATE(redo_histogram_labeled);

public  DEF_MENU_FUNCTION(print_voxel_origin);

public  DEF_MENU_UPDATE(print_voxel_origin);

public  DEF_MENU_FUNCTION(print_slice_plane);

public  DEF_MENU_UPDATE(print_slice_plane);

public  DEF_MENU_FUNCTION(type_in_voxel_origin);

public  DEF_MENU_UPDATE(type_in_voxel_origin);

public  DEF_MENU_FUNCTION(type_in_slice_plane);

public  DEF_MENU_UPDATE(type_in_slice_plane);

public  DEF_MENU_FUNCTION(toggle_slice_cross_section_visibility);

public  DEF_MENU_UPDATE(toggle_slice_cross_section_visibility);

public  DEF_MENU_FUNCTION(set_current_arbitrary_view);

public  DEF_MENU_UPDATE(set_current_arbitrary_view);

public  DEF_MENU_FUNCTION(toggle_slice_anchor);

public  DEF_MENU_UPDATE(toggle_slice_anchor);

public  DEF_MENU_FUNCTION(delete_current_volume);

public  DEF_MENU_UPDATE(delete_current_volume);

public  DEF_MENU_FUNCTION(toggle_current_volume);

public  DEF_MENU_UPDATE(toggle_current_volume);

public  DEF_MENU_FUNCTION(prev_current_volume);

public  DEF_MENU_UPDATE(prev_current_volume);

public  DEF_MENU_FUNCTION(set_current_volume_opacity);

public  DEF_MENU_UPDATE(set_current_volume_opacity);

public  DEF_MENU_FUNCTION(next_volume_visible);

public  DEF_MENU_UPDATE(next_volume_visible);

public  DEF_MENU_FUNCTION(prev_volume_visible);

public  DEF_MENU_UPDATE(prev_volume_visible);

public  DEF_MENU_FUNCTION(toggle_slice_interpolation);

public  DEF_MENU_UPDATE(toggle_slice_interpolation );

public  DEF_MENU_FUNCTION( save_slice_image );

public  DEF_MENU_UPDATE(save_slice_image );

public  DEF_MENU_FUNCTION( save_slice_window );

public  DEF_MENU_UPDATE(save_slice_window );

public  DEF_MENU_FUNCTION(toggle_incremental_slice_update);

public  DEF_MENU_UPDATE(toggle_incremental_slice_update);

public  DEF_MENU_FUNCTION( toggle_shift_key );

public  DEF_MENU_UPDATE(toggle_shift_key );

public  DEF_MENU_FUNCTION(toggle_cursor_visibility);

public  DEF_MENU_UPDATE(toggle_cursor_visibility );

public  DEF_MENU_FUNCTION(insert_volume_as_labels);

public  DEF_MENU_UPDATE(insert_volume_as_labels );

public  DEF_MENU_FUNCTION( transform_current_volume );

public  DEF_MENU_UPDATE(transform_current_volume );

public  DEF_MENU_FUNCTION( reset_volume_transform );

public  DEF_MENU_UPDATE(reset_volume_transform );

public  DEF_MENU_FUNCTION( translate_volume_plus_x);

public  DEF_MENU_UPDATE(translate_volume_plus_x );

public  DEF_MENU_FUNCTION( translate_volume_minus_x);

public  DEF_MENU_UPDATE(translate_volume_minus_x );

public  DEF_MENU_FUNCTION( translate_volume_plus_y);

public  DEF_MENU_UPDATE(translate_volume_plus_y );

public  DEF_MENU_FUNCTION( translate_volume_minus_y);

public  DEF_MENU_UPDATE(translate_volume_minus_y );

public  DEF_MENU_FUNCTION( translate_volume_plus_z);

public  DEF_MENU_UPDATE(translate_volume_plus_z );

public  DEF_MENU_FUNCTION( translate_volume_minus_z);

public  DEF_MENU_UPDATE(translate_volume_minus_z );

public  DEF_MENU_FUNCTION( magnify_volume);

public  DEF_MENU_UPDATE(magnify_volume );

public  DEF_MENU_FUNCTION( shrink_volume);

public  DEF_MENU_UPDATE(shrink_volume );

public  DEF_MENU_FUNCTION( rotate_volume_plus_x);

public  DEF_MENU_UPDATE(rotate_volume_plus_x );

public  DEF_MENU_FUNCTION( rotate_volume_minus_x);

public  DEF_MENU_UPDATE(rotate_volume_minus_x );

public  DEF_MENU_FUNCTION( rotate_volume_plus_y);

public  DEF_MENU_UPDATE(rotate_volume_plus_y );

public  DEF_MENU_FUNCTION( rotate_volume_minus_y);

public  DEF_MENU_UPDATE(rotate_volume_minus_y );

public  DEF_MENU_FUNCTION( rotate_volume_plus_z);

public  DEF_MENU_UPDATE(rotate_volume_plus_z );

public  DEF_MENU_FUNCTION( rotate_volume_minus_z);

public  DEF_MENU_UPDATE(rotate_volume_minus_z );

public  DEF_MENU_FUNCTION( set_volume_rotation_step);

public  DEF_MENU_UPDATE(set_volume_rotation_step );

public  DEF_MENU_FUNCTION( set_volume_scale_step);

public  DEF_MENU_UPDATE(set_volume_scale_step );

public  DEF_MENU_FUNCTION( set_volume_translation_step);

public  DEF_MENU_UPDATE(set_volume_translation_step );

public  DEF_MENU_FUNCTION( save_current_volume_transform );

public  DEF_MENU_UPDATE(save_current_volume_transform );

public  void  advance_current_object(
    display_struct    *display );

public  void  retreat_current_object(
    display_struct    *display );

public  object_struct  *get_current_model_object(
    display_struct    *display );

public  model_struct  *get_current_model(
    display_struct    *display );

public  int  get_current_object_index(
    display_struct    *display );

public  void  set_current_object(
    display_struct    *display,
    object_struct     *object );

public  void  set_current_object_index(
    display_struct    *display,
    int               index );

public  BOOLEAN  get_current_object(
    display_struct    *display,
    object_struct     **current_object );

public  void  initialize_current_object(
    display_struct    *display );

public  void  terminate_current_object(
    selection_struct   *current_object );

public  void  push_current_object(
    display_struct    *display );

public  BOOLEAN  current_object_is_top_level(
    display_struct    *display );

public  void  pop_current_object(
    display_struct    *display );

public  BOOLEAN  current_object_is_this_type(
    display_struct    *display,
    Object_types      type );

public  BOOLEAN  current_object_exists(
    display_struct    *display );

public  void  turn_off_connected_polygons(
    display_struct   *display );

public  void  initialize_surface_edit(
    surface_edit_struct   *surface_edit );

public  void  delete_surface_edit(
    surface_edit_struct   *surface_edit );

public  void  set_edited_polygons(
    surface_edit_struct   *surface_edit,
    polygons_struct       *polygons );

public  BOOLEAN  get_edited_polygons(
    surface_edit_struct   *surface_edit,
    polygons_struct       **polygons );

public  void  reset_edited_polygons(
    surface_edit_struct   *surface_edit );

public  void  start_segmenting_polygons(
    display_struct   *display );

public  Status   save_window_to_file(
    display_struct  *display,
    STRING          filename,
    int             x_min,
    int             x_max,
    int             y_min,
    int             y_max );

public  BOOLEAN  update_current_marker(
    display_struct   *display,
    int              volume_index,
    Real             voxel[] );

public  void  read_voxellation_block(
    surface_extraction_struct   *surf );

public  BOOLEAN  extract_voxel_boundary_surface(
    Volume                      volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel[] );

public  void   initialize_voxel_queue(
    voxel_queue_struct  *voxel_queue );

public  void   insert_in_voxel_queue(
    voxel_queue_struct  *voxel_queue,
    int                 voxel[] );

public  void   get_next_voxel_from_queue(
    voxel_queue_struct    *voxel_queue,
    int                   voxel[] );

public  BOOLEAN  voxels_remaining(
    voxel_queue_struct  *voxel_queue );

public  void  delete_voxel_queue(
    voxel_queue_struct   *voxel_queue );

public  void  initialize_voxel_flags(
    bitlist_3d_struct  *voxel_flags,
    int                min_limits[],
    int                max_limits[] );

public  void  delete_voxel_flags(
    bitlist_3d_struct  *voxel_flags );

public  void  clear_voxel_flags(
    bitlist_3d_struct  *voxel_flags );

public  BOOLEAN  get_voxel_flag(
    bitlist_3d_struct   *voxel_flags,
    int                 min_limits[],
    int                 voxel[] );

public  void  set_voxel_flag(
    bitlist_3d_struct      *voxel_flags,
    int                    min_limits[],
    int                    voxel[] );

public  void  reset_voxel_flag(
    bitlist_3d_struct      *voxel_flags,
    int                    min_limits[],
    int                    voxel[] );

public  void  initialize_voxel_done_flags(
    unsigned_byte   **voxel_done_flags,
    int             min_limits[],
    int             max_limits[] );

public  void  delete_voxel_done_flags(
    unsigned_byte  voxel_done_flags[] );

public  void  clear_voxel_done_flags(
    unsigned_byte   voxel_done_flags[],
    int             min_limits[],
    int             max_limits[] );

public  unsigned_byte  get_voxel_done_flag(
    int                 min_limits[],
    int                 max_limits[],
    unsigned_byte       voxel_done_flags[],
    int                 voxel[] );

public  void  set_voxel_done_flag(
    int                 min_limits[],
    int                 max_limits[],
    unsigned_byte       voxel_done_flags[],
    int                 voxel[],
    unsigned_byte       flag );

public  void  initialize_edge_points(
    hash_table_struct  *hash_table );

public  void  delete_edge_points(
    hash_table_struct  *hash_table );

public  BOOLEAN  lookup_edge_point_id(
    int                 sizes[],
    hash_table_struct   *hash_table,
    int                 x,
    int                 y,
    int                 z,
    int                 edge_intersected,
    int                 *edge_point_id );

public  void  record_edge_point_id(
    int                 sizes[],
    hash_table_struct   *hash_table,
    int                 x,
    int                 y,
    int                 z,
    int                 edge_intersected,
    int                 edge_point_id );

public  void  remove_edge_point(
    int                 sizes[],
    hash_table_struct   *hash_table,
    int                 x,
    int                 y,
    int                 z,
    int                 edge_intersected );

public  void  initialize_surface_extraction(
    display_struct     *display );

public  void  delete_surface_extraction(
    display_struct    *display );

public  void  reset_surface_extraction(
    display_struct    *display );

public  void  tell_surface_extraction_volume_deleted(
    display_struct    *display,
    Volume            volume,
    Volume            label_volume );

public  void  start_surface_extraction(
    display_struct     *display );

public  void  stop_surface_extraction(
    display_struct     *display );

public  int  get_n_voxels(
    Volume            volume );

public  void  set_invalid_label_range_for_surface_extraction(
    display_struct  *display,
    int             min_label,
    int             max_label );

public  BOOLEAN  voxel_contains_surface(
    Volume                      volume,
    Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel_index[] );

public  BOOLEAN  extract_voxel_surface(
    Volume                      volume,
    Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel[],
    BOOLEAN                     first_voxel );

public  void  start_surface_extraction_at_point(
    display_struct     *display,
    Volume             volume,
    Volume             label_volume,
    BOOLEAN            binary_flag,
    BOOLEAN            voxellate_flag,
    Real               min_value,
    Real               max_value,
    int                x,
    int                y,
    int                z );

public  BOOLEAN  some_voxels_remaining_to_do(
    surface_extraction_struct   *surface_extraction );

public  BOOLEAN  extract_more_surface(
    display_struct    *display );

public  void  tell_surface_extraction_range_of_labels_changed(
    display_struct    *display,
    int               volume_index,
    int               range[2][N_DIMENSIONS] );

public  void  tell_surface_extraction_label_changed(
    display_struct    *display,
    int               volume_index,
    int               x,
    int               y,
    int               z );

public  void  remove_empty_polygons(
    polygons_struct  *polygons );

public  void  install_surface_extraction(
    display_struct     *display );

public  void  uninstall_surface_extraction(
    display_struct     *display );

public  void  scan_object_to_current_volume(
    display_struct   *slice_window,
    object_struct    *object );

public  void  start_picking_markers(
    display_struct   *display );

public  void  initialize_front_clipping(
    display_struct   *display );

public  void  initialize_back_clipping(
    display_struct   *display );

public  Status  start_film_loop(
    display_struct   *display,
    STRING           base_filename,
    int              axis_index,
    int              n_steps );

public  void  initialize_magnification(
    display_struct   *display );

public  BOOLEAN  mouse_moved(
    display_struct   *display,
    Real             *new_x,
    Real             *new_y,
    Real             *old_x,
    Real             *old_y );

public  BOOLEAN  pixel_mouse_moved(
    display_struct   *display,
    int              *new_x,
    int              *new_y,
    int              *old_x,
    int              *old_y );

public  void  record_mouse_position(
    display_struct   *display );

public  void  record_mouse_pixel_position(
    display_struct   *display );

public  void  initialize_translation(
    display_struct   *display );

public  void  initialize_picking_object(
    display_struct    *display );

public  BOOLEAN  get_mouse_scene_intersection(
    display_struct    *display,
    Object_types      desired_object_type,
    object_struct     **object,
    int               *object_index,
    Point             *intersection );

public  BOOLEAN  get_polygon_under_mouse(
    display_struct    *display,
    polygons_struct   **polygons,
    int               *poly_index,
    Point             *intersection );

public  void  start_picking_viewport(
    display_struct   *display );

public  void  initialize_rotating_slice(
    display_struct   *display );

public  BOOLEAN  get_spaceball_transform(
    display_struct   *display,
    Real             x1,
    Real             y1,
    Real             x2,
    Real             y2,
    Transform        *transform );

public  void  initialize_virtual_spaceball(
    display_struct   *display );

public  void  initialize_resize_events(
    display_struct   *display );

public  void   terminate_any_interactions(
    display_struct   *display );

public  void  draw_2d_line(
    display_struct    *display,
    View_types        view_type,
    Colour            colour,
    Real              x1,
    Real              y1,
    Real              x2,
    Real              y2 );

public  void  draw_2d_rectangle(
    display_struct    *display,
    View_types        view_type,
    Colour            colour,
    Real              x1,
    Real              y1,
    Real              x2,
    Real              y2 );

public  void  draw_polygons(
    display_struct    *display,
    polygons_struct   *polygons );

public  render_struct  *get_main_render(
    display_struct    *display );

public  void  draw_text_3d(
    display_struct    *display,
    Point             *origin,
    Colour            colour,
    STRING            str );

public  BOOLEAN  intersect_ray_with_objects_hierarchy(
    display_struct    *display,
    Point             *ray_origin,
    Vector            *ray_direction,
    Object_types      desired_object_type,
    object_struct     **object,
    int               *closest_object_index,
    Point             *intersection_point );

public  void  intersect_plane_with_polygons(
    display_struct    *display,
    Vector            *plane_normal,
    Real              plane_constant,
    lines_struct      *lines,
    int               *n_points_alloced,
    int               *n_indices_alloced,
    int               *n_end_indices_alloced );

public  BOOLEAN  intersect_plane_one_polygon(
    Vector            *plane_normal,
    Real              plane_constant,
    polygons_struct   *polygons,
    int               poly,
    lines_struct      *lines,
    int               *n_points_alloced,
    int               *n_indices_alloced,
    int               *n_end_indices_alloced );

public  void  initialize_cursor(
    display_struct    *display );

public  void  reset_cursor(
    display_struct    *display );

public  void  update_cursor_size(
    display_struct    *display );

public  void  update_cursor(
    display_struct    *display );

public  void  get_cursor_origin(
    display_struct   *display,
    Point            *origin );

public  void  rebuild_cursor_icon(
    display_struct    *display );

public  void  update_cursor_colour(
    display_struct   *display,
    Colour           colour );

public  void  build_menu(
    display_struct    *menu_window );

public  void  rebuild_menu(
    display_struct    *menu_window );

public  Real  get_size_of_menu_text_area(
    display_struct   *menu_window,
    int              key,
    int              line_number );

public  BOOLEAN   lookup_key_for_mouse_position(
    display_struct   *menu_window,
    Real             x,
    Real             y,
    int              *key );

public  void  rebuild_cursor_position_model(
    display_struct    *display );

public  Status  initialize_menu(
    display_struct    *menu_window,
    STRING            default_directory1,
    STRING            default_directory2,
    STRING            default_directory3,
    STRING            default_directory4,
    STRING            menu_filename );

public  void  initialize_menu_actions(
    display_struct    *menu_window );

public  void  initialize_menu_window(
    display_struct    *menu_window );

public  void  update_menu_text(
    display_struct      *display,
    menu_entry_struct   *menu_entry );

public  DEF_MENU_FUNCTION( push_menu );

public  DEF_MENU_UPDATE(push_menu );

public  DEF_MENU_FUNCTION( pop_menu );

public  DEF_MENU_UPDATE(pop_menu );

public  void  pop_menu_one_level(
    display_struct   *menu_window );

public  void   set_menu_text(
    display_struct      *menu_window,
    menu_entry_struct   *menu_entry,
    STRING              text );

public  void  update_all_menu_text(
    display_struct   *display );

public  Status  read_menu(
    menu_window_struct   *menu,
    FILE                 *file );

public  void  delete_menu(
    menu_window_struct  *menu );

public  void  rebuild_selected_list(
    display_struct    *display,
    display_struct    *menu_window );

public  BOOLEAN  mouse_is_on_object_name(
    display_struct    *display,
    int               x,
    int               y,
    object_struct     **object_under_mouse );

public  void  set_menu_text_real(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    Real               value );

public  void  set_menu_text_int(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    int                value );

public  void  set_menu_text_boolean(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    BOOLEAN            value,
    STRING             off_str,
    STRING             on_str );

public  void  set_menu_text_on_off(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    BOOLEAN            value );

public  void  set_menu_text_with_colour(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    Colour             colour );

public  void  set_menu_text_string(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    STRING             str );

public  void  initialize_cursor_plane_outline(
    display_struct    *display );

public  void  delete_cursor_plane_outline(
    display_struct    *display );

public  void  cut_polygon_neighbours_from_lines(
    display_struct     *display,
    polygons_struct    *polygons );

public  int  get_current_paint_label(
    display_struct    *display );

public  void  initialize_voxel_labeling(
    display_struct    *slice_window );

public  void  delete_voxel_labeling(
    slice_window_struct    *slice );

public  void  flip_labels_around_zero(
    display_struct  *slice_window );

public  void  translate_labels(
    display_struct   *slice_window,
    int              volume_index,
    int              delta[] );

public  void  copy_labels_slice_to_slice(
    display_struct   *slice_window,
    int              volume_index,
    int              axis,
    int              src_voxel,
    int              dest_voxel,
    Real             min_threshold,
    Real             max_threshold );

public  void  initialize_segmenting(
    segmenting_struct  *segmenting );

public  void  clear_all_labels(
    display_struct    *display );

public  void  set_labels_on_slice(
    display_struct  *slice_window,
    int             volume_index,
    int             axis_index,
    int             position,
    int             label );

public  void  set_connected_voxels_labels(
    display_struct    *slice_window,
    int               volume_index,
    int               axis_index,
    int               position[],
    Real              min_threshold,
    Real              max_threshold,
    int               label_min_threshold,
    int               label_max_threshold,
    Neighbour_types   connectivity,
    int               label );

public  void  set_visibility_around_poly(
    polygons_struct  *polygons,
    int              poly,
    int              max_polys_to_do,
    BOOLEAN          set_visibility_flag,
    BOOLEAN          new_visibility,
    BOOLEAN          set_colour_flag,
    Colour           colour );

public  void  crop_polygons_visibilities(
    polygons_struct  *polygons,
    int              axis_index,
    Real             position,
    BOOLEAN          cropping_above );

public  void  initialize_colour_bar(
    display_struct    *slice_window );

public  void  rebuild_colour_bar(
    display_struct   *slice_window );

public  int  get_colour_bar_y_pos(
    display_struct      *slice_window,
    Real                value );

public  BOOLEAN  mouse_within_colour_bar(
    display_struct      *slice_window,
    Real                x,
    Real                y,
    Real                *ratio );

public  void  get_histogram_space(
    display_struct      *slice_window,
    int                 *x1,
    int                 *x2 );

public  void  delete_slice_colour_coding(
    slice_window_struct   *slice,
    int                   volume_index );

public  void  set_slice_window_number_labels(
    display_struct    *slice_window,
    int               volume_index,
    int               n_labels );

public  void  initialize_slice_colour_coding(
    display_struct    *slice_window,
    int               volume_index );

public  Volume  get_nth_label_volume(
    display_struct   *display,
    int              volume_index );

public  Volume  get_label_volume(
    display_struct   *display );

public  BOOLEAN  label_volume_exists(
    display_struct   *display );

public  BOOLEAN  get_label_visibility(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index );

public  int  get_num_labels(
    display_struct   *display,
    int              volume_index );

public  void   set_colour_of_label(
    display_struct    *slice_window,
    int               volume_index,
    int               label,
    Colour            colour );

public  Colour   get_colour_of_label(
    display_struct    *slice_window,
    int               volume_index,
    int               label );

public  void   set_volume_opacity(
    display_struct   *slice_window,
    int              volume_index,
    Real             opacity );

public  void   set_label_opacity(
    display_struct   *slice_window,
    int              volume_index,
    Real             opacity );

public  void  colour_coding_has_changed(
    display_struct    *display,
    int               volume_index,
    Update_types      type );

public  void  change_colour_coding_range(
    display_struct    *slice_window,
    int               volume_index,
    Real              min_value,
    Real              max_value );

public  void  colour_code_an_object(
    display_struct   *display,
    object_struct    *object );

public  STRING    get_default_colour_map_suffix( void );

public  Status  load_label_colour_map(
    display_struct   *slice_window,
    STRING           filename );

public  Status  save_label_colour_map(
    display_struct   *slice_window,
    STRING           filename );

public  void  clear_labels(
    display_struct   *display,
    int              volume_index );

public  int  get_voxel_label(
    display_struct   *display,
    int              volume_index,
    int              x,
    int              y,
    int              z );

public  void  set_voxel_label(
    display_struct   *display,
    int              volume_index,
    int              x,
    int              y,
    int              z,
    int              label );

public  Status  load_user_defined_colour_coding(
    display_struct   *slice_window,
    STRING           filename );

public  void  initialize_crop_box(
    display_struct   *slice_window );

public  void  delete_crop_box(
    display_struct   *slice_window );

public  void  set_crop_filename(
    display_struct   *slice_window,
    STRING           filename );

public  Status  create_cropped_volume_to_file(
    display_struct   *slice_window,
    STRING           cropped_filename );

public  void  crop_and_load_volume(
    display_struct   *slice_window );

public  void  toggle_slice_crop_box_visibility(
    display_struct   *slice_window );

public  void  reset_crop_box_position(
    display_struct   *display );

public  void  start_picking_crop_box(
    display_struct    *slice_window );

public  void  get_volume_crop_limits(
    display_struct    *display,
    int               min_voxel[],
    int               max_voxel[] );

public  void  initialize_slice_models(
    display_struct    *slice_window );

public  void  initialize_slice_models_for_volume(
    display_struct    *slice_window,
    int               volume_index );

public  void  delete_slice_models_for_volume(
    display_struct    *slice_window,
    int               volume_index );

public  void  rebuild_slice_divider(
    display_struct    *slice_window );

public  Bitplane_types  get_slice_readout_bitplanes( void );

public  void  rebuild_probe(
    display_struct    *slice_window );

public  void  get_slice_cross_section_direction(
    display_struct    *slice_window,
    int               view_index,
    int               section_index,
    Vector            *in_plane_axis );

public  void  rebuild_slice_unfinished_flag(
    display_struct    *slice_window,
    int               view_index );

public  BOOLEAN  get_slice_unfinished_flag_visibility(
    display_struct    *slice_window,
    int               view_index );

public  void  set_slice_unfinished_flag_visibility(
    display_struct    *slice_window,
    int               view_index,
    BOOLEAN           state );

public  void  rebuild_slice_cross_section(
    display_struct    *slice_window,
    int               view_index );

public  void  rebuild_slice_crop_box(
    display_struct    *slice_window,
    int               view_index );

public  void  rebuild_slice_cursor(
    display_struct    *slice_window,
    int               view_index );

public  object_struct  *get_slice_pixels_object(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index );

public  object_struct  *get_label_slice_pixels_object(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index );

public  object_struct  *get_composite_slice_pixels_object(
    display_struct    *slice_window,
    int               view_index );

public  int  rebuild_slice_pixels_for_volume(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    BOOLEAN           incremental_flag,
    BOOLEAN           interrupted,
    BOOLEAN           continuing_flag,
    BOOLEAN           *finished );

public  void  rebuild_slice_text(
    display_struct    *slice_window,
    int               view_index );

public  void  rebuild_atlas_slice_pixels(
    display_struct    *slice_window,
    int               view_index );

public  void  composite_volume_and_labels(
    display_struct        *slice_window,
    int                   view_index );

public  int  rebuild_label_slice_pixels_for_volume(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    BOOLEAN           incremental_flag,
    BOOLEAN           interrupted,
    BOOLEAN           continuing_flag,
    BOOLEAN           *finished );

public  void  update_slice_pixel_visibilities(
    display_struct    *slice_window,
    int               view );

public  void  initialize_slice_histogram(
    display_struct   *slice_window );

public  void  delete_slice_histogram(
    slice_window_struct   *slice );

public  void  clear_histogram(
    display_struct   *slice_window );

public  void  resize_histogram(
    display_struct   *slice_window );

public  void  compute_histogram(
    display_struct   *slice_window,
    int              axis_index,
    int              voxel_index,
    BOOLEAN          labeled_only );

public  void  start_picking_slice_angle(
    display_struct    *slice_window );

public  object_struct   *create_3d_slice_quadmesh(
    Volume         volume,
    int            axis_index,
    Real           voxel_position );

public  void  create_slice_window(
    display_struct   *display,
    STRING           filename,
    Volume           volume );

public  void  update_all_slice_models(
    display_struct   *slice_window );

public  void  delete_slice_window_volume(
    display_struct   *slice_window,
    int              volume_index );

public  void  delete_slice_window(
    display_struct   *slice_window );

public  STRING  get_volume_filename(
    display_struct    *slice_window,
    int               volume_index );

public  void  add_slice_window_volume(
    display_struct    *display,
    STRING            filename,
    Volume            volume );

public  void  set_current_volume_index(
    display_struct  *slice_window,
    int             volume_index );

public  int  get_n_volumes(
    display_struct  *display );

public  int   get_current_volume_index(
    display_struct   *display );

public  BOOLEAN   get_slice_window_volume(
    display_struct   *display,
    Volume           *volume );

public  Volume  get_nth_volume(
    display_struct   *display,
    int              volume_index );

public  Volume   get_volume(
    display_struct   *display );

public  BOOLEAN  slice_window_exists(
    display_struct   *display );

public  BOOLEAN  get_slice_window(
    display_struct   *display,
    display_struct   **slice_window );

public  BOOLEAN  get_range_of_volumes(
    display_struct   *display,
    Point            *min_limit,
    Point            *max_limit );

public  void  set_slice_cursor_update(
    display_struct   *slice_window,
    int              view_index );

public  void  set_slice_text_update(
    display_struct   *slice_window,
    int              view_index );

public  void  set_slice_cross_section_update(
    display_struct   *slice_window,
    int              view_index );

public  void  set_crop_box_update(
    display_struct   *slice_window,
    int              view_index );

public  void  set_slice_dividers_update(
    display_struct   *slice_window );

public  void  set_probe_update(
    display_struct   *slice_window );

public  void  set_colour_bar_update(
    display_struct   *slice_window );

public  void  set_atlas_update(
    display_struct   *slice_window,
    int              view_index );

public  void  set_slice_window_update(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    Update_types     type );

public  void  set_slice_window_all_update(
    display_struct   *slice_window,
    int              volume_index,
    Update_types     type );

public  void  set_slice_viewport_update(
    display_struct   *slice_window,
    int              model_number );

public  void  update_slice_window(
    display_struct   *slice_window );

public  void  set_slice_composite_update(
    display_struct   *slice_window,
    int              view_index,
    int              x_min,
    int              x_max,
    int              y_min,
    int              y_max );

public  BOOLEAN  get_slice_subviewport(
    display_struct   *slice_window,
    int              view_index,
    int              *x_min,
    int              *x_max,
    int              *y_min,
    int              *y_max );

public  void  initialize_volume_cross_section(
    display_struct    *display );

public  void  rebuild_volume_outline(
    display_struct    *slice_window );

public  void  rebuild_volume_cross_section(
    display_struct    *display );

public  void  set_volume_cross_section_visibility(
    display_struct    *display,
    BOOLEAN           state );

public  BOOLEAN  get_volume_cross_section_visibility(
    display_struct    *display );

public  void  initialize_slice_window_events(
    display_struct    *slice_window );

public  void  set_voxel_cursor_from_mouse_position(
    display_struct    *slice_window );

public  void  initialize_slice_undo(
    slice_undo_struct  *undo );

public  void  delete_slice_undo(
    slice_undo_struct  *undo,
    int                volume_index );

public  void  record_slice_labels(
    display_struct  *display,
    int             volume_index,
    int             axis_index,
    int             slice_index );

public  void  record_slice_under_mouse(
    display_struct  *display,
    int             volume_index );

public  BOOLEAN  slice_labels_to_undo(
    display_struct  *display );

public  int  undo_slice_labels_if_any(
    display_struct  *display );

public  void  initialize_slice_window_view(
    display_struct    *slice_window,
    int               volume_index );

public  void  set_slice_visibility(
    display_struct    *slice_window,
    int               volume_index,
    int               view,
    BOOLEAN           visibility );

public  BOOLEAN  get_slice_visibility(
    display_struct    *slice_window,
    int               volume_index,
    int               view );

public  void  reset_slice_view(
    display_struct    *slice_window,
    int               view );

public  void  resize_slice_view(
    display_struct    *slice_window,
    int               view );

public  void  scale_slice_view(
    display_struct    *slice_window,
    int               view,
    Real              scale_factor );

public  void  translate_slice_view(
    display_struct    *slice_window,
    int               view,
    Real              dx,
    Real              dy );

public  BOOLEAN  find_slice_view_mouse_is_in(
    display_struct    *display,
    int               x_pixel,
    int               y_pixel,
    int               *view_index );

public  BOOLEAN  convert_pixel_to_voxel(
    display_struct    *display,
    int               volume_index,
    int               x_pixel,
    int               y_pixel,
    Real              voxel[],
    int               *view_index );

public  void  convert_voxel_to_pixel(
    display_struct    *display,
    int               volume_index,
    int               view_index,
    Real              voxel[],
    Real              *x_pixel,
    Real              *y_pixel );

public  void  get_voxel_to_pixel_transform(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    int               *x_index,
    int               *y_index,
    Real              *x_scale,
    Real              *x_trans,
    Real              *y_scale,
    Real              *y_trans );

public  BOOLEAN  get_voxel_corresponding_to_point(
    display_struct    *display,
    Point             *point,
    Real              voxel[] );

public  void   get_slice_window_partitions(
    display_struct    *slice_window,
    int               *left_panel_width,
    int               *left_slice_width,
    int               *right_slice_width,
    int               *bottom_slice_height,
    int               *top_slice_height,
    int               *text_panel_height,
    int               *colour_bar_panel_height );

public  void  get_slice_viewport(
    display_struct    *slice_window,
    int               view_index,
    int               *x_min,
    int               *x_max,
    int               *y_min,
    int               *y_max );

public  void  get_colour_bar_viewport(
    display_struct    *slice_window,
    int               *x_min,
    int               *x_max,
    int               *y_min,
    int               *y_max );

public  void  get_text_display_viewport(
    display_struct    *slice_window,
    int               *x_min,
    int               *x_max,
    int               *y_min,
    int               *y_max );

public  void  get_slice_divider_intersection(
    display_struct    *slice_window,
    int               *x,
    int               *y );

public  void  set_slice_divider_position(
    display_struct    *slice_window,
    int               x,
    int               y );

public  BOOLEAN  get_volume_corresponding_to_pixel(
    display_struct    *slice_window,
    int               x,
    int               y,
    int               *volume_index,
    int               *view_index,
    Real              voxel[] );

public  BOOLEAN  get_voxel_in_slice_window(
    display_struct    *display,
    Real              voxel[],
    int               *volume_index,
    int               *view_index );

public  BOOLEAN  get_voxel_in_three_d_window(
    display_struct    *display,
    Real              voxel[] );

public  BOOLEAN  get_voxel_under_mouse(
    display_struct    *display,
    int               *volume_index,
    int               *view_index,
    Real              voxel[] );

public  void  get_current_voxel(
    display_struct    *display,
    int               volume_index,
    Real              voxel[] );

public  BOOLEAN  set_current_voxel(
    display_struct    *slice_window,
    int               this_volume_index,
    Real              voxel[] );

public  void  get_slice_perp_axis(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    Real             perp_axis[N_DIMENSIONS] );

public  void  set_slice_plane_perp_axis(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    Real             voxel_perp[] );

public  void  set_slice_plane(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    Real             x_axis[],
    Real             y_axis[] );

public  void  get_slice_plane(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    Real             origin[],
    Real             x_axis[],
    Real             y_axis[] );

public  BOOLEAN  get_slice_view_index_under_mouse(
    display_struct   *display,
    int              *view_index );

public  BOOLEAN  get_axis_index_under_mouse(
    display_struct   *display,
    int              *volume_index,
    int              *axis_index );

public  BOOLEAN  slice_has_ortho_axes(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    int              *x_index,
    int              *y_index,
    int              *axis_index );

public  int  get_arbitrary_view_index(
    display_struct   *display );

public  void  get_slice_model_viewport(
    display_struct   *slice_window,
    int              model,
    int              *x_min,
    int              *x_max,
    int              *y_min,
    int              *y_max );

public  BOOLEAN  update_cursor_from_voxel(
    display_struct    *slice_window );

public  BOOLEAN  update_voxel_from_cursor(
    display_struct    *slice_window );

public  void  update_all_slice_axes_views(
    display_struct    *slice_window,
    int               volume_index );

public  void  slice_view_has_changed(
    display_struct   *display,
    int              view );

public  void  set_volume_transform(
    display_struct     *display,
    int                volume_index,
    General_transform  *transform );

public  void  concat_transform_to_volume(
    display_struct     *display,
    int                volume_index,
    General_transform  *transform );

public  void  transform_current_volume_from_file(
    display_struct   *display,
    STRING           filename );

public  void  reset_current_volume_transform(
    display_struct   *display );

public  BOOLEAN  find_closest_line_point_to_point(
    display_struct    *display,
    Point             *point,
    Point             *closest_line_point );

public  void  initialize_surface_curve(
    display_struct     *display );

public  void  start_surface_curve(
    display_struct     *display );

public  void  end_surface_curve(
    display_struct     *display );

public  void  close_surface_curve(
    display_struct     *display );

public  void  pick_surface_point_near_a_line(
    display_struct   *display );

public  void  reset_surface_curve(
    display_struct     *display );

public  void  make_surface_curve_permanent(
    display_struct     *display );

public  BOOLEAN  distance_along_polygons(
    polygons_struct   *polygons,
    Real              curvature_weight,
    Real              min_curvature,
    Real              max_curvature,
    Point             *p1,
    int               poly1,
    Point             *p2,
    int               poly2,
    Real              *dist,
    lines_struct      *lines );

public  void  find_polygon_vertex_nearest_point(
    polygons_struct  *polygons,
    int              poly,
    Point            *point,
    Point            *closest_vertex );

public  void  convert_lines_to_tubes_objects(
    display_struct    *display,
    lines_struct      *lines,
    int               n_around,
    Real              radius );

public  void  add_action_table_function(
    action_table_struct   *action_table,
    Event_types           event_type,
    event_function_type   function );

public  void  remove_action_table_function(
    action_table_struct   *action_table,
    Event_types           event_type,
    event_function_type   function );

public  void  push_action_table(
    action_table_struct   *action_table,
    Event_types           event_type );

public  void  pop_action_table(
    action_table_struct   *action_table,
    Event_types           event_type );

public  int  get_event_actions(
    action_table_struct   *action_table,
    Event_types           event_type,
    event_function_type   *actions_list[] );

public  void  initialize_action_table(
    action_table_struct   *action_table );

public  void  fit_view_to_domain(
    view_struct   *view,
    Point         *min_limit,
    Point         *max_limit );

public  void  initialize_lights(
    light_struct  *lights );

public  void  initialize_render(
    render_struct  *render );

public  void  initialize_render_3D(
    render_struct  *render );

public  void  set_render_info(
    window_struct  *window,
    render_struct  *render );

public  void  initialize_view(
    view_struct  *view,
    Vector       *line_of_sight,
    Vector       *horizontal );

public  void  assign_view_direction(
    view_struct    *view,
    Vector         *line_of_sight,
    Vector         *hor );

public  void  get_view_z_axis(
    view_struct   *view,
    Vector        *z_axis );

public  void  get_view_centre(
    view_struct   *view,
    Point         *centre );

public  void  get_screen_centre(
    view_struct   *view,
    Point         *centre );

public  void  get_screen_axes(
    view_struct   *view,
    Vector        *hor,
    Vector        *vert );

public  void  get_screen_point(
    view_struct  *view,
    Real         x,
    Real         y,
    Point        *point );

public  void  adjust_view_for_aspect(
    view_struct    *view,
    window_struct  *window );

public  void  convert_point_from_coordinate_system(
    Point    *origin,
    Vector   *x_axis,
    Vector   *y_axis,
    Vector   *z_axis,
    Point    *point,
    Point    *transformed_point );

public  void  transform_point_to_world(
    view_struct   *view,
    Point         *p,
    Point         *transformed_point );

public  void  transform_world_to_model(
    view_struct   *view,
    Point         *p,
    Point         *transformed_point );

public  void  transform_world_to_model_vector(
    view_struct   *view,
    Vector        *v,
    Vector        *transformed_vector );

public  void  transform_world_to_screen(
    view_struct   *view,
    Point         *p,
    Point         *transformed_point );

public  void  transform_point_to_view_space(
    view_struct   *view,
    Point         *p,
    Point         *transformed_point );

public  void  transform_point_to_screen(
    view_struct   *view,
    Point         *p,
    Point         *transformed_point );

public  void  set_model_scale(
    view_struct   *view,
    Real          sx,
    Real          sy,
    Real          sz );

public  void  convert_screen_to_ray(
    view_struct   *view,
    Real          x_screen,
    Real          y_screen,
    Point         *origin,
    Vector        *direction );

public  void  magnify_view_size(
    view_struct  *view,
    Real         factor );

public  void  set_view_rectangle(
    view_struct   *view,
    Real          x_min,
    Real          x_max,
    Real          y_min,
    Real          y_max );

public  void  transform_screen_to_pixels(
    window_struct  *window,
    Point          *screen,
    Point          *pixels );
#endif
