#ifndef  DEF_display_prototypes
#define  DEF_display_prototypes

  VIO_Status  change_global_variable(
    VIO_STR   str,
    VIO_STR   *variable_name,
    VIO_STR   *new_value );

  VIO_Status  set_global_variable_value(
    VIO_STR   variable_name,
    VIO_STR   new_value );

VIO_Status get_user_input(const char *prompt, const char *format, ...);

VIO_Status get_user_file(const char *prompt, VIO_BOOL saving,
                         char *extension, VIO_STR *filename);

  void  display_objects(
    Gwindow                     window,
    object_struct               *object,
    update_interrupted_struct   *interrupt,
    Bitplane_types              bitplanes,
    VIO_BOOL                     *past_last_object );

  void  initialize_window_callbacks(
    display_struct    *display_window );

  VIO_Status   main_event_loop( void );

  VIO_BOOL  window_is_up_to_date(
    display_struct   *display );

  VIO_BOOL  is_shift_key_pressed( void );

  VIO_BOOL  is_ctrl_key_pressed( void );

  /* main/graphics.c */
  int  get_list_of_windows(
    display_struct  ***display );

  display_struct  *lookup_window(
    Gwindow   window );

  void  initialize_graphics( void );

  void  terminate_graphics( void );

  void print_graphics_state(FILE *);

  VIO_Status  create_graphics_window(
    window_types      window_type,
    VIO_BOOL           double_buffering,
    display_struct    **display,
    VIO_STR            title,
    int               x,
    int               y,
    int               width,
    int               height );

  model_struct  *get_graphics_model(
    display_struct    *display,
    int               model_index );

  model_info_struct  *get_model_info(
    model_struct   *model );

  Bitplane_types  get_model_bitplanes(
    model_struct   *model );

  void  create_model_after_current(
    display_struct   *display );

  void  initialize_model_info(
    model_struct   *model );

  void  initialize_3D_model_info(
    model_struct   *model );

  void  initialize_display_model(
    model_struct   *model );

  void  terminate_display_model(
    model_struct   *model );

  Bitplane_types  get_cursor_bitplanes( void );

  void  set_update_required(
    display_struct   *display,
    Bitplane_types   which_bitplanes );

  VIO_BOOL  graphics_normal_planes_update_required(
    display_struct   *display );

  VIO_BOOL  graphics_update_required(
    display_struct   *display );

  void  graphics_models_have_changed(
    display_struct  *display );

  void  update_graphics(
    display_struct               *display,
    update_interrupted_struct    *interrupt );

  void  delete_graphics_window(
    display_struct   *display );

  void  update_view(
    display_struct  *display );

  void  fit_view_to_visible_models(
    display_struct   *display );

  void  reset_view_parameters(
    display_struct   *display,
    VIO_Vector           *line_of_sight,
    VIO_Vector           *horizontal );

  VIO_Real  size_of_domain(
    display_struct   *display );

  void  initialize_three_d_window(
    display_struct   *display );

void show_three_d_window( display_struct *three_d,
                          display_struct *markers );

void attach_vertex_data(display_struct *display,
                        object_struct *object,
                        vertex_data_struct *vtxd_ptr);

  void  delete_three_d(
    display_struct  *display );

  void  add_object_to_current_model(
    display_struct   *display,
    object_struct     *object );

  display_struct  *get_three_d_window(
    display_struct  *display );

  void  convert_transform_to_view_space(
    display_struct    *display,
    VIO_Transform         *transform,
    VIO_Transform         *view_space_transform );

  void  apply_transform_in_view_space(
    display_struct    *display,
    VIO_Transform         *transform );

  void  transform_model(
    display_struct    *display,
    VIO_Transform         *transform );

  void  get_inverse_model_transform(
    display_struct    *display,
    VIO_Transform         *inverse_transform );

  void  initialize_atlas(
    atlas_struct   *atlas );

  void  delete_atlas(
    atlas_struct   *atlas );

  void  regenerate_atlas_lookup(
    display_struct    *slice_window );

  void  set_atlas_state(
    display_struct    *slice_window,
    VIO_BOOL           state );

  VIO_BOOL  is_atlas_loaded(
    display_struct  *display );

  VIO_BOOL  render_atlas_slice_to_pixels(
    atlas_struct  *atlas,
    VIO_Colour        image[],
    int           image_x_size,
    int           image_y_size,
    VIO_Real          world_start[],
    VIO_Real          world_x_axis[],
    VIO_Real          world_y_axis[] );

  VIO_Status  load_graphics_file( 
    display_struct   *display,
    VIO_STR           filename,
    VIO_BOOL          is_label_file );

  VIO_Status   input_volume_file(
    VIO_STR         filename,
    VIO_Volume         *volume_ptr );

/*** input_files/vertex_data.c ***/

vertex_data_struct *input_vertex_data( const char *filename );

  /** \defgroup Commands Menu command functions
   *
   * Top-level command functions callable directly from the menu.
   *
   * @{
   */
  DEF_MENU_FUNCTION( set_atlas_on_or_off );

  DEF_MENU_UPDATE(set_atlas_on_or_off );

  DEF_MENU_FUNCTION( set_atlas_opacity );

  DEF_MENU_UPDATE(set_atlas_opacity );

  DEF_MENU_FUNCTION( set_atlas_transparent_threshold );

  DEF_MENU_UPDATE(set_atlas_transparent_threshold );

  DEF_MENU_FUNCTION( flip_atlas_x );

  DEF_MENU_UPDATE(flip_atlas_x );

  DEF_MENU_FUNCTION( flip_atlas_y );

  DEF_MENU_UPDATE(flip_atlas_y );

  DEF_MENU_FUNCTION( flip_atlas_z );

  DEF_MENU_UPDATE(flip_atlas_z );

  DEF_MENU_FUNCTION( set_atlas_tolerance_x );

  DEF_MENU_UPDATE(set_atlas_tolerance_x );

  DEF_MENU_FUNCTION( set_atlas_tolerance_y );

  DEF_MENU_UPDATE(set_atlas_tolerance_y );

  DEF_MENU_FUNCTION( set_atlas_tolerance_z );

  DEF_MENU_UPDATE(set_atlas_tolerance_z );

  DEF_MENU_FUNCTION( menu_set_global_variable );

  DEF_MENU_UPDATE(menu_set_global_variable );

  DEF_MENU_FUNCTION( show_memory );

  DEF_MENU_UPDATE(show_memory );

  DEF_MENU_FUNCTION(set_colour_limits );

  DEF_MENU_UPDATE(set_colour_limits );

  DEF_MENU_FUNCTION(set_contour_colour_map );

  DEF_MENU_UPDATE(set_contour_colour_map );

  DEF_MENU_FUNCTION(set_hot_metal );

  DEF_MENU_UPDATE(set_hot_metal );

  DEF_MENU_FUNCTION(set_gray_scale );

  DEF_MENU_UPDATE(set_gray_scale );

  DEF_MENU_FUNCTION(set_red );

  DEF_MENU_UPDATE(set_red );

  DEF_MENU_FUNCTION(set_green );

  DEF_MENU_UPDATE(set_green );

  DEF_MENU_FUNCTION(set_blue );

  DEF_MENU_UPDATE(set_blue );

  DEF_MENU_FUNCTION(set_spectral );

  DEF_MENU_UPDATE(set_spectral );

  DEF_MENU_FUNCTION(set_arbitrary_colour_scale );

  DEF_MENU_UPDATE(set_arbitrary_colour_scale );

  DEF_MENU_FUNCTION(set_user_defined_colour_scale );

  DEF_MENU_UPDATE(set_user_defined_colour_scale );

  DEF_MENU_FUNCTION(set_under_colour );

  DEF_MENU_UPDATE(set_under_colour );

  DEF_MENU_FUNCTION(set_over_colour );

  DEF_MENU_UPDATE(set_over_colour );

  DEF_MENU_FUNCTION(set_label_colour_ratio );

  DEF_MENU_UPDATE(set_label_colour_ratio );

  DEF_MENU_FUNCTION(set_nearest_neighbour );

  DEF_MENU_UPDATE(set_nearest_neighbour );

  DEF_MENU_FUNCTION(set_linear_interpolation );

  DEF_MENU_UPDATE(set_linear_interpolation );

  DEF_MENU_FUNCTION(set_box_filter );

  DEF_MENU_UPDATE(set_box_filter );

  DEF_MENU_FUNCTION(set_triangle_filter );

  DEF_MENU_UPDATE(set_triangle_filter );

  DEF_MENU_FUNCTION(set_gaussian_filter );

  DEF_MENU_UPDATE(set_gaussian_filter );

  DEF_MENU_FUNCTION(set_filter_half_width );

  DEF_MENU_UPDATE(set_filter_half_width );

  DEF_MENU_FUNCTION(set_slice_window_n_labels );

  DEF_MENU_UPDATE(set_slice_window_n_labels );

  DEF_MENU_FUNCTION(toggle_share_labels );

  DEF_MENU_UPDATE(toggle_share_labels );

  DEF_MENU_FUNCTION(save_colour_map );

  DEF_MENU_UPDATE(save_colour_map );

  DEF_MENU_FUNCTION(load_colour_map );

  DEF_MENU_UPDATE(load_colour_map );

  DEF_MENU_FUNCTION(load_user_defined_colour_scale );

  DEF_MENU_UPDATE(load_user_defined_colour_scale );

DEF_MENU_FUNCTION( load_vertex_data );

DEF_MENU_UPDATE( load_vertex_data );

  DEF_MENU_FUNCTION( load_file );

  DEF_MENU_UPDATE(load_file );

  DEF_MENU_FUNCTION( save_file );

  DEF_MENU_UPDATE(save_file );

  DEF_MENU_FUNCTION( save_oblique_plane );

  DEF_MENU_UPDATE( save_oblique_plane );

  DEF_MENU_FUNCTION( load_oblique_plane );

  DEF_MENU_UPDATE( load_oblique_plane );

  DEF_MENU_FUNCTION( smooth_current_lines );

  DEF_MENU_UPDATE(smooth_current_lines );

  DEF_MENU_FUNCTION( make_current_line_tube );

  DEF_MENU_UPDATE(make_current_line_tube );

  DEF_MENU_FUNCTION( convert_line_to_spline_points );

  DEF_MENU_UPDATE(convert_line_to_spline_points );

  DEF_MENU_FUNCTION( make_line_circle );

  DEF_MENU_UPDATE(make_line_circle );

  DEF_MENU_FUNCTION( subdivide_current_lines );

  DEF_MENU_UPDATE(subdivide_current_lines );

  DEF_MENU_FUNCTION( convert_markers_to_lines );

  DEF_MENU_UPDATE(convert_markers_to_lines );

  DEF_MENU_FUNCTION( convert_markers_to_closed_lines );

  DEF_MENU_UPDATE(convert_markers_to_closed_lines );

  DEF_MENU_FUNCTION( set_line_widths );

  DEF_MENU_UPDATE(set_line_widths );

  DEF_MENU_FUNCTION( reverse_normals );

  DEF_MENU_UPDATE(reverse_normals );

  DEF_MENU_FUNCTION( advance_visible );

  DEF_MENU_UPDATE(advance_visible );

  DEF_MENU_FUNCTION( retreat_visible );

  DEF_MENU_UPDATE(retreat_visible );

  DEF_MENU_FUNCTION( make_all_invisible );

  DEF_MENU_UPDATE(make_all_invisible );

  DEF_MENU_FUNCTION( make_all_visible );

  DEF_MENU_UPDATE(make_all_visible );

  DEF_MENU_FUNCTION( advance_selected );

  DEF_MENU_UPDATE(advance_selected );

  DEF_MENU_FUNCTION( retreat_selected );

  DEF_MENU_UPDATE(retreat_selected );

  DEF_MENU_FUNCTION( descend_selected );

  DEF_MENU_UPDATE(descend_selected );

  DEF_MENU_FUNCTION( ascend_selected );

  DEF_MENU_UPDATE(ascend_selected );

  DEF_MENU_FUNCTION( toggle_object_visibility );

  DEF_MENU_UPDATE(toggle_object_visibility );

  DEF_MENU_FUNCTION( create_model );

  DEF_MENU_UPDATE(create_model );

  DEF_MENU_FUNCTION( change_model_name );

  DEF_MENU_UPDATE(change_model_name );

  DEF_MENU_FUNCTION( delete_current_object );

  DEF_MENU_UPDATE(delete_current_object );

  DEF_MENU_FUNCTION( set_current_object_colour );

  DEF_MENU_UPDATE(set_current_object_colour );

  DEF_MENU_FUNCTION( set_current_object_surfprop );

  DEF_MENU_UPDATE(set_current_object_surfprop );

  DEF_MENU_FUNCTION( cut_object );

  DEF_MENU_UPDATE(cut_object );

  DEF_MENU_FUNCTION( paste_object );

  DEF_MENU_UPDATE(paste_object );

  DEF_MENU_FUNCTION( mark_vertices );

  DEF_MENU_UPDATE(mark_vertices );

  DEF_MENU_FUNCTION( flip_object );

  DEF_MENU_UPDATE(flip_object );

  DEF_MENU_FUNCTION( scan_current_object_to_volume );

  DEF_MENU_UPDATE(scan_current_object_to_volume );
/** @} */
  void  set_marker_to_defaults(
    display_struct  *display,
    marker_struct   *marker );

  void  create_marker_at_position(
    display_struct    *display,
    VIO_Point             *position,
    VIO_STR            label );

  /** \addtogroup Commands
   * @{
   */
  DEF_MENU_FUNCTION( create_marker_at_cursor );

  DEF_MENU_UPDATE(create_marker_at_cursor );

  DEF_MENU_FUNCTION( set_cursor_to_marker );

  DEF_MENU_UPDATE(set_cursor_to_marker );

  DEF_MENU_FUNCTION( save_markers );

  DEF_MENU_UPDATE(save_markers );

  DEF_MENU_FUNCTION( set_default_marker_structure_id );

  DEF_MENU_UPDATE(set_default_marker_structure_id );

  DEF_MENU_FUNCTION( set_default_marker_patient_id );

  DEF_MENU_UPDATE(set_default_marker_patient_id );

  DEF_MENU_FUNCTION( set_default_marker_size );

  DEF_MENU_UPDATE(set_default_marker_size );

  DEF_MENU_FUNCTION( set_default_marker_colour );

  DEF_MENU_UPDATE(set_default_marker_colour );

  DEF_MENU_FUNCTION( set_default_marker_type );

  DEF_MENU_UPDATE(set_default_marker_type );

  DEF_MENU_FUNCTION( set_default_marker_label );

  DEF_MENU_UPDATE(set_default_marker_label );

  DEF_MENU_FUNCTION( change_marker_structure_id );

  DEF_MENU_UPDATE(change_marker_structure_id );

  DEF_MENU_FUNCTION( change_marker_patient_id );

  DEF_MENU_UPDATE(change_marker_patient_id );

  DEF_MENU_FUNCTION( change_marker_type );

  DEF_MENU_UPDATE(change_marker_type );

  DEF_MENU_FUNCTION( change_marker_size );

  DEF_MENU_UPDATE(change_marker_size );

  DEF_MENU_FUNCTION( change_marker_position );

  DEF_MENU_UPDATE(change_marker_position );

  DEF_MENU_FUNCTION( change_marker_label );

  DEF_MENU_UPDATE(change_marker_label );

  DEF_MENU_FUNCTION( copy_defaults_to_marker );

  DEF_MENU_UPDATE(copy_defaults_to_marker );

  DEF_MENU_FUNCTION( copy_defaults_to_markers );

  DEF_MENU_UPDATE(copy_defaults_to_markers );

  DEF_MENU_FUNCTION( move_cursor_to_home );

  DEF_MENU_UPDATE(move_cursor_to_home );
  /** @} */

  VIO_BOOL  get_current_polygons(
    display_struct      *display,
    polygons_struct     **polygons );

  /** \addtogroup Commands
   * @{
   */

  DEF_MENU_FUNCTION( input_polygons_bintree );

  DEF_MENU_UPDATE(input_polygons_bintree );

  DEF_MENU_FUNCTION( save_polygons_bintree );

  DEF_MENU_UPDATE(save_polygons_bintree );

  DEF_MENU_FUNCTION( create_bintree_for_polygons );

  DEF_MENU_UPDATE(create_bintree_for_polygons );

  DEF_MENU_FUNCTION( create_normals_for_polygon );

  DEF_MENU_UPDATE(create_normals_for_polygon );

  DEF_MENU_FUNCTION( average_normals_for_polygon );

  DEF_MENU_UPDATE(average_normals_for_polygon );

  DEF_MENU_FUNCTION( smooth_current_polygon );

  DEF_MENU_UPDATE(smooth_current_polygon );

  DEF_MENU_FUNCTION( reverse_polygons_order );

  DEF_MENU_UPDATE(reverse_polygons_order );

  DEF_MENU_FUNCTION( make_polygon_sphere );

  DEF_MENU_UPDATE(make_polygon_sphere );

  DEF_MENU_FUNCTION( make_tetrahedral_sphere );

  DEF_MENU_UPDATE(make_tetrahedral_sphere );

  DEF_MENU_FUNCTION( subdivide_current_polygon );

  DEF_MENU_UPDATE(subdivide_current_polygon );

  DEF_MENU_FUNCTION( reset_polygon_neighbours );

  DEF_MENU_UPDATE(reset_polygon_neighbours );

  DEF_MENU_FUNCTION( cut_polygon_neighbours );

  DEF_MENU_UPDATE(cut_polygon_neighbours );

  DEF_MENU_FUNCTION( set_polygon_line_thickness );

  DEF_MENU_UPDATE(set_polygon_line_thickness );

  DEF_MENU_FUNCTION( print_polygons_surface_area );

  DEF_MENU_UPDATE(print_polygons_surface_area );

  DEF_MENU_FUNCTION( coalesce_current_polygons );

  DEF_MENU_UPDATE(coalesce_current_polygons );

  DEF_MENU_FUNCTION( separate_current_polygons );

  DEF_MENU_UPDATE(separate_current_polygons );

  DEF_MENU_FUNCTION( exit_program );

  DEF_MENU_UPDATE(exit_program );

DEF_MENU_FUNCTION(save_window_state);

DEF_MENU_UPDATE(save_window_state);

  DEF_MENU_FUNCTION( set_paint_xy_brush_radius );

  DEF_MENU_UPDATE(set_paint_xy_brush_radius );

  DEF_MENU_FUNCTION( set_paint_z_brush_radius );

  DEF_MENU_UPDATE(set_paint_z_brush_radius );

  DEF_MENU_FUNCTION( set_current_paint_label );

  DEF_MENU_UPDATE(set_current_paint_label );

  DEF_MENU_FUNCTION( set_current_erase_label );

  DEF_MENU_UPDATE(set_current_erase_label );

  DEF_MENU_FUNCTION( set_label_colour );

  DEF_MENU_UPDATE(set_label_colour );

  DEF_MENU_FUNCTION( copy_labels_from_lower_slice );

  DEF_MENU_UPDATE(copy_labels_from_lower_slice );

  DEF_MENU_FUNCTION( copy_labels_from_higher_slice );

  DEF_MENU_UPDATE(copy_labels_from_higher_slice );

  DEF_MENU_FUNCTION( toggle_display_labels );

  DEF_MENU_UPDATE(toggle_display_labels );

  DEF_MENU_FUNCTION( change_labels_in_range );

  DEF_MENU_UPDATE(change_labels_in_range );

  DEF_MENU_FUNCTION( calculate_volume );

  DEF_MENU_UPDATE(calculate_volume );

  DEF_MENU_FUNCTION( flip_labels_in_x );

  DEF_MENU_UPDATE(flip_labels_in_x );

  DEF_MENU_FUNCTION( translate_labels_up );

  DEF_MENU_UPDATE(translate_labels_up );

  DEF_MENU_FUNCTION( translate_labels_down );

  DEF_MENU_UPDATE(translate_labels_down );

  DEF_MENU_FUNCTION( translate_labels_left );

  DEF_MENU_UPDATE(translate_labels_left );

  DEF_MENU_FUNCTION( translate_labels_right );

  DEF_MENU_UPDATE(translate_labels_right );

  DEF_MENU_FUNCTION( undo_slice_labels );

  DEF_MENU_UPDATE(undo_slice_labels );

  DEF_MENU_FUNCTION( translate_labels_arbitrary );

  DEF_MENU_UPDATE(translate_labels_arbitrary );

  DEF_MENU_FUNCTION( toggle_fast_update );

  DEF_MENU_UPDATE(toggle_fast_update );

  DEF_MENU_FUNCTION( toggle_cursor_follows_paintbrush );

  DEF_MENU_UPDATE(toggle_cursor_follows_paintbrush );

  DEF_MENU_FUNCTION( toggle_render_mode );

  DEF_MENU_UPDATE(toggle_render_mode );

  DEF_MENU_FUNCTION( toggle_shading );

  DEF_MENU_UPDATE(toggle_shading );

  DEF_MENU_FUNCTION( toggle_lights );

  DEF_MENU_UPDATE(toggle_lights );

  DEF_MENU_FUNCTION( toggle_two_sided );

  DEF_MENU_UPDATE(toggle_two_sided );

  DEF_MENU_FUNCTION( toggle_backfacing );

  DEF_MENU_UPDATE(toggle_backfacing );

  DEF_MENU_FUNCTION( toggle_line_curve_flag );

  DEF_MENU_UPDATE(toggle_line_curve_flag );

  DEF_MENU_FUNCTION( toggle_marker_label_flag );

  DEF_MENU_UPDATE(toggle_marker_label_flag );

  DEF_MENU_FUNCTION( set_n_curve_segments );

  DEF_MENU_UPDATE(set_n_curve_segments );

  DEF_MENU_FUNCTION( toggle_double_buffer_threed );

  DEF_MENU_UPDATE(toggle_double_buffer_threed );

  DEF_MENU_FUNCTION( toggle_double_buffer_slice );

  DEF_MENU_UPDATE(toggle_double_buffer_slice );

  DEF_MENU_FUNCTION( change_background_colour );

  DEF_MENU_UPDATE(change_background_colour );
 
  /* from callbacks/segmenting.c */

  DEF_MENU_FUNCTION( label_voxel );

  DEF_MENU_UPDATE(label_voxel );

  DEF_MENU_FUNCTION( clear_voxel );

  DEF_MENU_UPDATE(clear_voxel );

  DEF_MENU_FUNCTION( reset_segmenting );

  DEF_MENU_UPDATE(reset_segmenting );

  DEF_MENU_FUNCTION( set_segmenting_threshold );

  DEF_MENU_UPDATE(set_segmenting_threshold );
  /** @} */

  VIO_Status  input_label_volume_file(
    display_struct   *display,
    VIO_STR           filename );

  /** \addtogroup Commands
   * @{
   */
  DEF_MENU_FUNCTION(load_label_data);

  DEF_MENU_UPDATE(load_label_data );

  DEF_MENU_FUNCTION(save_label_data);

  DEF_MENU_UPDATE(save_label_data );

  DEF_MENU_FUNCTION( load_labels );

  DEF_MENU_UPDATE(load_labels );

  DEF_MENU_FUNCTION( save_labels );

  DEF_MENU_UPDATE(save_labels );

  DEF_MENU_FUNCTION( save_current_label );

  DEF_MENU_UPDATE(save_current_label );

  DEF_MENU_FUNCTION(label_slice);

  DEF_MENU_UPDATE(label_slice );

  DEF_MENU_FUNCTION(clear_slice);

  DEF_MENU_UPDATE(clear_slice );

  DEF_MENU_FUNCTION(clear_connected);

  DEF_MENU_UPDATE(clear_connected );

  DEF_MENU_FUNCTION(label_connected);

  DEF_MENU_UPDATE(label_connected );

  DEF_MENU_FUNCTION(label_connected_no_threshold);

  DEF_MENU_UPDATE(label_connected_no_threshold );

  DEF_MENU_FUNCTION(label_connected_3d);

  DEF_MENU_UPDATE(label_connected_3d );

  DEF_MENU_FUNCTION(clear_label_connected_3d);

  DEF_MENU_UPDATE(clear_label_connected_3d );

  DEF_MENU_FUNCTION(dilate_labels);

  DEF_MENU_UPDATE(dilate_labels );

  DEF_MENU_FUNCTION(erode_labels);

  DEF_MENU_UPDATE(erode_labels );

  DEF_MENU_FUNCTION(toggle_connectivity);

  DEF_MENU_UPDATE(toggle_connectivity );

  DEF_MENU_FUNCTION(toggle_crop_labels_on_output);

  DEF_MENU_UPDATE(toggle_crop_labels_on_output);

DEF_MENU_FUNCTION(toggle_secondary_brush);
DEF_MENU_UPDATE(toggle_secondary_brush);

  /** @} */

  VIO_Status input_tag_label_file(
    display_struct   *display,
    VIO_STR           filename );

  VIO_Status   input_tag_objects_label(
    display_struct* display,
    int            *n_objects,
    object_struct  **object_list[]);

  /** \addtogroup Commands
   * @{
   */

  DEF_MENU_FUNCTION( reset_polygon_visibility );

  DEF_MENU_UPDATE(reset_polygon_visibility );

  DEF_MENU_FUNCTION( remove_invisible_parts_of_polygon );

  DEF_MENU_UPDATE(remove_invisible_parts_of_polygon);

  DEF_MENU_FUNCTION( set_n_paint_polygons );

  DEF_MENU_UPDATE(set_n_paint_polygons);

  DEF_MENU_FUNCTION( set_vis_paint_colour );

  DEF_MENU_UPDATE(set_vis_paint_colour);

  DEF_MENU_FUNCTION( set_invis_paint_colour );

  DEF_MENU_UPDATE(set_invis_paint_colour);

  DEF_MENU_FUNCTION( set_connected_invisible );

  DEF_MENU_UPDATE(set_connected_invisible );

  DEF_MENU_FUNCTION( paint_invisible );

  DEF_MENU_UPDATE(paint_invisible);

  DEF_MENU_FUNCTION( paint_visible );

  DEF_MENU_UPDATE(paint_visible);

  DEF_MENU_FUNCTION( set_connected_vis_colour );

  DEF_MENU_UPDATE(set_connected_vis_colour );

  DEF_MENU_FUNCTION( set_connected_invis_colour );

  DEF_MENU_UPDATE(set_connected_invis_colour );

  DEF_MENU_FUNCTION( paint_invis_colour );

  DEF_MENU_UPDATE(paint_invis_colour);

  DEF_MENU_FUNCTION( paint_vis_colour );

  DEF_MENU_UPDATE(paint_vis_colour);

  DEF_MENU_FUNCTION( set_visibility_from_colour );

  DEF_MENU_UPDATE(set_visibility_from_colour);

  DEF_MENU_FUNCTION( set_invis_colour_to_invis );

  DEF_MENU_UPDATE(set_invis_colour_to_invis);

  DEF_MENU_FUNCTION( set_vis_to_invis_colour );

  DEF_MENU_UPDATE(set_vis_to_invis_colour);

  DEF_MENU_FUNCTION( set_vis_to_vis_colour );

  DEF_MENU_UPDATE(set_vis_to_vis_colour);

  DEF_MENU_FUNCTION( crop_above_plane );

  DEF_MENU_UPDATE(crop_above_plane);

  DEF_MENU_FUNCTION( crop_below_plane );

  DEF_MENU_UPDATE(crop_below_plane);

  DEF_MENU_FUNCTION( save_polygons_visibilities );

  DEF_MENU_UPDATE(save_polygons_visibilities);

  DEF_MENU_FUNCTION( load_polygons_visibilities );

  DEF_MENU_UPDATE(load_polygons_visibilities);

  DEF_MENU_FUNCTION( start_surface_line );

  DEF_MENU_UPDATE(start_surface_line);

  DEF_MENU_FUNCTION( end_surface_line );

  DEF_MENU_UPDATE(end_surface_line);

  DEF_MENU_FUNCTION( close_surface_line );

  DEF_MENU_UPDATE(close_surface_line);

  DEF_MENU_FUNCTION( reset_surface_line );

  DEF_MENU_UPDATE(reset_surface_line);

  DEF_MENU_FUNCTION( make_surface_line_permanent );

  DEF_MENU_UPDATE(make_surface_line_permanent);

  DEF_MENU_FUNCTION( set_line_curvature_weight );

  DEF_MENU_UPDATE(set_line_curvature_weight );

  DEF_MENU_FUNCTION( set_surface_curve_curvature );

  DEF_MENU_UPDATE(set_surface_curve_curvature );

  DEF_MENU_FUNCTION( pick_surface_point_on_line );

  DEF_MENU_UPDATE(pick_surface_point_on_line );

  DEF_MENU_FUNCTION(start_volume_isosurface );

  DEF_MENU_UPDATE(start_volume_isosurface );

  DEF_MENU_FUNCTION(start_volume_binary_isosurface );

  DEF_MENU_UPDATE(start_volume_binary_isosurface );

  DEF_MENU_FUNCTION(start_label_binary_isosurface );

  DEF_MENU_UPDATE(start_label_binary_isosurface );

  DEF_MENU_FUNCTION(toggle_surface_extraction);

  DEF_MENU_UPDATE(toggle_surface_extraction );

  DEF_MENU_FUNCTION(reset_surface);

  DEF_MENU_UPDATE(reset_surface );

  DEF_MENU_FUNCTION(make_surface_permanent);

  DEF_MENU_UPDATE(make_surface_permanent );

  DEF_MENU_FUNCTION(get_voxelated_label_surface);

  DEF_MENU_UPDATE(get_voxelated_label_surface );

  DEF_MENU_FUNCTION(get_voxelated_surface);

  DEF_MENU_UPDATE(get_voxelated_surface );

  DEF_MENU_FUNCTION( set_surface_invalid_label_range );

  DEF_MENU_UPDATE(set_surface_invalid_label_range );

  DEF_MENU_FUNCTION( make_view_fit );

  DEF_MENU_UPDATE(make_view_fit );

  DEF_MENU_FUNCTION( reset_view );

  DEF_MENU_UPDATE(reset_view );

  DEF_MENU_FUNCTION( right_tilted_view );

  DEF_MENU_UPDATE(right_tilted_view );

  DEF_MENU_FUNCTION( left_tilted_view );

  DEF_MENU_UPDATE(left_tilted_view );

  DEF_MENU_FUNCTION( top_view );

  DEF_MENU_UPDATE(top_view );

  DEF_MENU_FUNCTION( bottom_view );

  DEF_MENU_UPDATE(bottom_view );

  DEF_MENU_FUNCTION( front_view );

  DEF_MENU_UPDATE(front_view );

  DEF_MENU_FUNCTION( back_view );

  DEF_MENU_UPDATE(back_view );

  DEF_MENU_FUNCTION( left_view );

  DEF_MENU_UPDATE(left_view );

  DEF_MENU_FUNCTION( right_view );

  DEF_MENU_UPDATE(right_view );

  DEF_MENU_FUNCTION( toggle_perspective );

  DEF_MENU_UPDATE(toggle_perspective );

  DEF_MENU_FUNCTION( front_clipping );

  DEF_MENU_UPDATE(front_clipping );

  DEF_MENU_FUNCTION( back_clipping );

  DEF_MENU_UPDATE(back_clipping );

  DEF_MENU_FUNCTION( pick_view_rectangle );

  DEF_MENU_UPDATE(pick_view_rectangle );

  DEF_MENU_FUNCTION( create_film_loop );

  DEF_MENU_UPDATE(create_film_loop );

  DEF_MENU_FUNCTION( save_image );

  DEF_MENU_UPDATE(save_image );

  DEF_MENU_FUNCTION( toggle_stereo_mode );

  DEF_MENU_UPDATE(toggle_stereo_mode );

  DEF_MENU_FUNCTION( set_eye_separation );

  DEF_MENU_UPDATE(set_eye_separation );

  DEF_MENU_FUNCTION( print_view );

  DEF_MENU_UPDATE(print_view );

  DEF_MENU_FUNCTION(type_in_3D_origin);

  DEF_MENU_UPDATE(type_in_3D_origin);

  DEF_MENU_FUNCTION(type_in_view_origin);

  DEF_MENU_UPDATE(type_in_view_origin);

  DEF_MENU_FUNCTION(type_in_view_line_of_sight);

  DEF_MENU_UPDATE(type_in_view_line_of_sight);

  DEF_MENU_FUNCTION(type_in_view_up_dir);

  DEF_MENU_UPDATE(type_in_view_up_dir);

  DEF_MENU_FUNCTION(type_in_view_window_width);

  DEF_MENU_UPDATE(type_in_view_window_width);

  DEF_MENU_FUNCTION(type_in_view_perspective_distance);

  DEF_MENU_UPDATE(type_in_view_perspective_distance);

  DEF_MENU_FUNCTION(move_slice_plus);

  DEF_MENU_UPDATE(move_slice_plus );

  DEF_MENU_FUNCTION(move_slice_minus);

  DEF_MENU_UPDATE(move_slice_minus );

  DEF_MENU_FUNCTION(move_time_plus);

  DEF_MENU_UPDATE(move_time_plus );

  DEF_MENU_FUNCTION(move_time_minus);

  DEF_MENU_UPDATE(move_time_minus );

  DEF_MENU_FUNCTION(toggle_slice_visibility);

  DEF_MENU_UPDATE(toggle_slice_visibility );

  DEF_MENU_FUNCTION(toggle_cross_section_visibility);

  DEF_MENU_UPDATE(toggle_cross_section_visibility );

  DEF_MENU_FUNCTION(reset_current_slice_view);

  DEF_MENU_UPDATE(reset_current_slice_view );

  DEF_MENU_FUNCTION(colour_code_objects );

  DEF_MENU_UPDATE(colour_code_objects );

  DEF_MENU_FUNCTION(create_3d_slice);

  DEF_MENU_UPDATE(create_3d_slice);

  DEF_MENU_FUNCTION(create_3d_slice_profile);

  DEF_MENU_UPDATE(create_3d_slice_profile);

  DEF_MENU_FUNCTION(resample_slice_window_volume);

  DEF_MENU_UPDATE(resample_slice_window_volume);

  DEF_MENU_FUNCTION(box_filter_slice_window_volume);

  DEF_MENU_UPDATE(box_filter_slice_window_volume);

  DEF_MENU_FUNCTION(pick_slice_angle_point);

  DEF_MENU_UPDATE(pick_slice_angle_point);

  DEF_MENU_FUNCTION( rotate_slice_axes );

  DEF_MENU_UPDATE(rotate_slice_axes );

  DEF_MENU_FUNCTION(reset_slice_crop);

  DEF_MENU_UPDATE(reset_slice_crop);

  DEF_MENU_FUNCTION(toggle_slice_crop_visibility);

  DEF_MENU_UPDATE(toggle_slice_crop_visibility);

  DEF_MENU_FUNCTION(pick_crop_box_edge);

  DEF_MENU_UPDATE(pick_crop_box_edge);

  DEF_MENU_FUNCTION(set_crop_box_filename);

  DEF_MENU_UPDATE(set_crop_box_filename);

  DEF_MENU_FUNCTION(load_cropped_volume);

  DEF_MENU_UPDATE(load_cropped_volume);

  DEF_MENU_FUNCTION(crop_volume_to_file);

  DEF_MENU_UPDATE(crop_volume_to_file);

  DEF_MENU_FUNCTION(redo_histogram);

  DEF_MENU_UPDATE(redo_histogram);

  DEF_MENU_FUNCTION(redo_histogram_labeled);

  DEF_MENU_UPDATE(redo_histogram_labeled);

  DEF_MENU_FUNCTION(print_voxel_origin);

  DEF_MENU_UPDATE(print_voxel_origin);

  DEF_MENU_FUNCTION(print_slice_plane);

  DEF_MENU_UPDATE(print_slice_plane);

  DEF_MENU_FUNCTION(type_in_voxel_origin);

  DEF_MENU_UPDATE(type_in_voxel_origin);

  DEF_MENU_FUNCTION(type_in_slice_plane);

  DEF_MENU_UPDATE(type_in_slice_plane);

  DEF_MENU_FUNCTION(toggle_slice_cross_section_visibility);

  DEF_MENU_UPDATE(toggle_slice_cross_section_visibility);

  DEF_MENU_FUNCTION(set_current_arbitrary_view);

  DEF_MENU_UPDATE(set_current_arbitrary_view);

  DEF_MENU_FUNCTION(toggle_slice_anchor);

  DEF_MENU_UPDATE(toggle_slice_anchor);

  DEF_MENU_FUNCTION(delete_current_volume);

  DEF_MENU_UPDATE(delete_current_volume);

  DEF_MENU_FUNCTION(toggle_current_volume);

  DEF_MENU_UPDATE(toggle_current_volume);

  DEF_MENU_FUNCTION(prev_current_volume);

  DEF_MENU_UPDATE(prev_current_volume);

  DEF_MENU_FUNCTION(set_current_volume_opacity);

  DEF_MENU_UPDATE(set_current_volume_opacity);

  DEF_MENU_FUNCTION(next_volume_visible);

  DEF_MENU_UPDATE(next_volume_visible);

  DEF_MENU_FUNCTION(prev_volume_visible);

  DEF_MENU_UPDATE(prev_volume_visible);

  DEF_MENU_FUNCTION(toggle_slice_interpolation);

  DEF_MENU_UPDATE(toggle_slice_interpolation );

  DEF_MENU_FUNCTION( save_slice_image );

  DEF_MENU_UPDATE(save_slice_image );

  DEF_MENU_FUNCTION( save_slice_window );

  DEF_MENU_UPDATE(save_slice_window );

  DEF_MENU_FUNCTION(toggle_incremental_slice_update);

  DEF_MENU_UPDATE(toggle_incremental_slice_update);

  DEF_MENU_FUNCTION( toggle_shift_key );

  DEF_MENU_UPDATE(toggle_shift_key );

  DEF_MENU_FUNCTION(toggle_cursor_visibility);

  DEF_MENU_UPDATE(toggle_cursor_visibility );

  DEF_MENU_FUNCTION(insert_volume_as_labels);

  DEF_MENU_UPDATE(insert_volume_as_labels );

DEF_MENU_FUNCTION(reset_interactions);
DEF_MENU_UPDATE(reset_interactions);

  DEF_MENU_FUNCTION( transform_current_volume );

  DEF_MENU_UPDATE(transform_current_volume );

  DEF_MENU_FUNCTION( reset_volume_transform );

  DEF_MENU_UPDATE(reset_volume_transform );

  DEF_MENU_FUNCTION( translate_volume_plus_x);

  DEF_MENU_UPDATE(translate_volume_plus_x );

  DEF_MENU_FUNCTION( translate_volume_minus_x);

  DEF_MENU_UPDATE(translate_volume_minus_x );

  DEF_MENU_FUNCTION( translate_volume_plus_y);

  DEF_MENU_UPDATE(translate_volume_plus_y );

  DEF_MENU_FUNCTION( translate_volume_minus_y);

  DEF_MENU_UPDATE(translate_volume_minus_y );

  DEF_MENU_FUNCTION( translate_volume_plus_z);

  DEF_MENU_UPDATE(translate_volume_plus_z );

  DEF_MENU_FUNCTION( translate_volume_minus_z);

  DEF_MENU_UPDATE(translate_volume_minus_z );

  DEF_MENU_FUNCTION( magnify_volume);

  DEF_MENU_UPDATE(magnify_volume );

  DEF_MENU_FUNCTION( shrink_volume);

  DEF_MENU_UPDATE(shrink_volume );

  DEF_MENU_FUNCTION( rotate_volume_plus_x);

  DEF_MENU_UPDATE(rotate_volume_plus_x );

  DEF_MENU_FUNCTION( rotate_volume_minus_x);

  DEF_MENU_UPDATE(rotate_volume_minus_x );

  DEF_MENU_FUNCTION( rotate_volume_plus_y);

  DEF_MENU_UPDATE(rotate_volume_plus_y );

  DEF_MENU_FUNCTION( rotate_volume_minus_y);

  DEF_MENU_UPDATE(rotate_volume_minus_y );

  DEF_MENU_FUNCTION( rotate_volume_plus_z);

  DEF_MENU_UPDATE(rotate_volume_plus_z );

  DEF_MENU_FUNCTION( rotate_volume_minus_z);

  DEF_MENU_UPDATE(rotate_volume_minus_z );

  DEF_MENU_FUNCTION( set_volume_rotation_step);

  DEF_MENU_UPDATE(set_volume_rotation_step );

  DEF_MENU_FUNCTION( set_volume_scale_step);

  DEF_MENU_UPDATE(set_volume_scale_step );

  DEF_MENU_FUNCTION( set_volume_translation_step);

  DEF_MENU_UPDATE(set_volume_translation_step );

  DEF_MENU_FUNCTION( save_current_volume_transform );

  DEF_MENU_UPDATE(save_current_volume_transform );
  /** @} */

  void  advance_current_object(
    display_struct    *display );

  void  retreat_current_object(
    display_struct    *display );

  object_struct  *get_current_model_object(
    display_struct    *display );

  model_struct  *get_current_model(
    display_struct    *display );

  int  get_current_object_index(
    display_struct    *display );

  void  set_current_object(
    display_struct    *display,
    object_struct     *object );

int get_object_index(display_struct *display, object_struct *object_ptr );

  void  set_current_object_index(
    display_struct    *display,
    int               index );

  VIO_BOOL  get_current_object(
    display_struct    *display,
    object_struct     **current_object );

  void  initialize_current_object(
    display_struct    *display );

  void  terminate_current_object(
    selection_struct   *current_object );

  void  push_current_object(
    display_struct    *display );

  VIO_BOOL  current_object_is_top_level(
    display_struct    *display );

  void  pop_current_object(
    display_struct    *display );

  VIO_BOOL  current_object_is_this_type(
    display_struct    *display,
    Object_types      type );

  VIO_BOOL  current_object_exists(
    display_struct    *display );

  VIO_Status   save_window_to_file(
    display_struct  *display,
    VIO_STR          filename,
    int             x_min,
    int             x_max,
    int             y_min,
    int             y_max );

/* markers/markers.c */
VIO_BOOL  update_current_marker(
    display_struct   *display,
    int              volume_index,
    VIO_Real             voxel[] );

VIO_Status  initialize_marker_window(display_struct    *marker_window);

/* surface_extraction/boundary_extraction.c */  
  void  read_voxellation_block(
    surface_extraction_struct   *surf );

  VIO_BOOL  extract_voxel_boundary_surface(
    VIO_Volume                      volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel[] );

  void   initialize_voxel_queue(
    voxel_queue_struct  *voxel_queue );

  void   insert_in_voxel_queue(
    voxel_queue_struct  *voxel_queue,
    int                 voxel[] );

  void   get_next_voxel_from_queue(
    voxel_queue_struct    *voxel_queue,
    int                   voxel[] );

  VIO_BOOL  voxels_remaining(
    voxel_queue_struct  *voxel_queue );

  void  delete_voxel_queue(
    voxel_queue_struct   *voxel_queue );

  void  initialize_voxel_flags(
    bitlist_3d_struct  *voxel_flags,
    int                min_limits[],
    int                max_limits[] );

  void  delete_voxel_flags(
    bitlist_3d_struct  *voxel_flags );

  void  clear_voxel_flags(
    bitlist_3d_struct  *voxel_flags );

  VIO_BOOL  get_voxel_flag(
    bitlist_3d_struct   *voxel_flags,
    int                 min_limits[],
    int                 voxel[] );

  void  set_voxel_flag(
    bitlist_3d_struct      *voxel_flags,
    int                    min_limits[],
    int                    voxel[] );

  void  reset_voxel_flag(
    bitlist_3d_struct      *voxel_flags,
    int                    min_limits[],
    int                    voxel[] );

  void  initialize_voxel_done_flags(
    VIO_UCHAR   **voxel_done_flags,
    int             min_limits[],
    int             max_limits[] );

  void  delete_voxel_done_flags(
    VIO_UCHAR  voxel_done_flags[] );

  void  clear_voxel_done_flags(
    VIO_UCHAR   voxel_done_flags[],
    int             min_limits[],
    int             max_limits[] );

  VIO_UCHAR  get_voxel_done_flag(
    int                 min_limits[],
    int                 max_limits[],
    VIO_UCHAR       voxel_done_flags[],
    int                 voxel[] );

  void  set_voxel_done_flag(
    int                 min_limits[],
    int                 max_limits[],
    VIO_UCHAR       voxel_done_flags[],
    int                 voxel[],
    VIO_UCHAR       flag );

  void  initialize_edge_points(
    hash_table_struct  *hash_table );

  void  delete_edge_points(
    hash_table_struct  *hash_table );

  VIO_BOOL  lookup_edge_point_id(
    int                 sizes[],
    hash_table_struct   *hash_table,
    int                 x,
    int                 y,
    int                 z,
    int                 edge_intersected,
    int                 *edge_point_id );

  void  record_edge_point_id(
    int                 sizes[],
    hash_table_struct   *hash_table,
    int                 x,
    int                 y,
    int                 z,
    int                 edge_intersected,
    int                 edge_point_id );

  void  remove_edge_point(
    int                 sizes[],
    hash_table_struct   *hash_table,
    int                 x,
    int                 y,
    int                 z,
    int                 edge_intersected );

  void  initialize_surface_extraction(
    display_struct     *display );

  void  delete_surface_extraction(
    display_struct    *display );

  void  reset_surface_extraction(
    display_struct    *display );

  void  tell_surface_extraction_volume_deleted(
    display_struct    *display,
    VIO_Volume            volume,
    VIO_Volume            label_volume );

  void  start_surface_extraction(
    display_struct     *display );

  void  stop_surface_extraction(
    display_struct     *display );

  void  set_invalid_label_range_for_surface_extraction(
    display_struct  *display,
    int             min_label,
    int             max_label );

  VIO_BOOL  voxel_contains_surface(
    VIO_Volume                      volume,
    VIO_Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel_index[] );

  VIO_BOOL  extract_voxel_surface(
    VIO_Volume                      volume,
    VIO_Volume                      label_volume,
    surface_extraction_struct   *surface_extraction,
    int                         voxel[],
    VIO_BOOL                     first_voxel );

  void  start_surface_extraction_at_point(
    display_struct     *display,
    VIO_Volume             volume,
    VIO_Volume             label_volume,
    VIO_BOOL            binary_flag,
    VIO_BOOL            voxellate_flag,
    VIO_Real               min_value,
    VIO_Real               max_value,
    int                x,
    int                y,
    int                z );

  VIO_BOOL  some_voxels_remaining_to_do(
    surface_extraction_struct   *surface_extraction );

  VIO_BOOL  extract_more_surface(
    display_struct    *display );

  void  tell_surface_extraction_range_of_labels_changed(
    display_struct    *display,
    int               volume_index,
    int               range[2][VIO_N_DIMENSIONS] );

  void  tell_surface_extraction_label_changed(
    display_struct    *display,
    int               volume_index,
    int               x,
    int               y,
    int               z );

  void  remove_empty_polygons(
    polygons_struct  *polygons );

  void  install_surface_extraction(
    display_struct     *display );

  void  uninstall_surface_extraction(
    display_struct     *display );

  void  scan_object_to_current_volume(
    display_struct   *slice_window,
    object_struct    *object );

  void  start_picking_markers(
    display_struct   *display );

  void  initialize_front_clipping(
    display_struct   *display );

  void  initialize_back_clipping(
    display_struct   *display );

  VIO_Status  start_film_loop(
    display_struct   *display,
    VIO_STR           base_filename,
    int              axis_index,
    int              n_steps );

/* From events/magnify.c */
  void  initialize_magnification(
    display_struct   *display );
  void  initialize_scroll_magnification(
    display_struct   *display );

DEF_EVENT_FUNCTION(start_translation);

  VIO_BOOL  mouse_moved(
    display_struct   *display,
    VIO_Real             *new_x,
    VIO_Real             *new_y,
    VIO_Real             *old_x,
    VIO_Real             *old_y );

  VIO_BOOL  pixel_mouse_moved(
    display_struct   *display,
    int              *new_x,
    int              *new_y,
    int              *old_x,
    int              *old_y );

  void  record_mouse_position(
    display_struct   *display );

  void  record_mouse_pixel_position(
    display_struct   *display );

  /* from events/mouse_trans.c */

  void  mouse_translation_update(
    display_struct   *display );

  void  initialize_picking_object(
    display_struct    *display );

  VIO_BOOL  get_mouse_scene_intersection(
    display_struct    *display,
    Object_types      desired_object_type,
    object_struct     **object,
    int               *object_index,
    VIO_Point             *intersection );

VIO_BOOL get_cursor_scene_intersection(
    display_struct    *display,
    Object_types      desired_object_type,
    object_struct     **object,
    int               *object_index,
    VIO_Point         *intersection );

  VIO_BOOL  get_polygon_under_mouse(
    display_struct    *display,
    polygons_struct   **polygons,
    int               *poly_index,
    VIO_Point             *intersection );

  void  start_picking_viewport(
    display_struct   *display );

  void  initialize_rotating_slice(
    display_struct   *display );

  VIO_BOOL  get_spaceball_transform(
    display_struct   *display,
    VIO_Real             x1,
    VIO_Real             y1,
    VIO_Real             x2,
    VIO_Real             y2,
    VIO_Transform        *transform );

  void  initialize_virtual_spaceball(
    display_struct   *display );

  void  initialize_resize_events(
    display_struct   *display );

  void   terminate_any_interactions(
    display_struct   *display );

  void  draw_2d_line(
    display_struct    *display,
    View_types        view_type,
    VIO_Colour            colour,
    VIO_Real              x1,
    VIO_Real              y1,
    VIO_Real              x2,
    VIO_Real              y2 );

  void  draw_2d_rectangle(
    display_struct    *display,
    View_types        view_type,
    VIO_Colour            colour,
    VIO_Real              x1,
    VIO_Real              y1,
    VIO_Real              x2,
    VIO_Real              y2 );

  void  draw_polygons(
    display_struct    *display,
    polygons_struct   *polygons );

  render_struct  *get_main_render(
    display_struct    *display );

  void  draw_text_3d(
    display_struct    *display,
    VIO_Point             *origin,
    VIO_Colour            colour,
    VIO_STR            str );

  VIO_BOOL  intersect_ray_with_objects_hierarchy(
    display_struct    *display,
    VIO_Point             *ray_origin,
    VIO_Vector            *ray_direction,
    Object_types      desired_object_type,
    object_struct     **object,
    int               *closest_object_index,
    VIO_Point             *intersection_point );

  void  intersect_plane_with_polygons(
    display_struct    *display,
    VIO_Vector            *plane_normal,
    VIO_Real              plane_constant,
    lines_struct      *lines,
    int               *n_points_alloced,
    int               *n_indices_alloced,
    int               *n_end_indices_alloced );

  VIO_BOOL  intersect_plane_one_polygon(
    VIO_Vector            *plane_normal,
    VIO_Real              plane_constant,
    polygons_struct   *polygons,
    int               poly,
    lines_struct      *lines,
    int               *n_points_alloced,
    int               *n_indices_alloced,
    int               *n_end_indices_alloced );

  void  initialize_cursor(
    display_struct    *display );

  void  reset_cursor(
    display_struct    *display );

  void  update_cursor_size(
    display_struct    *display );

  void  update_cursor(
    display_struct    *display );

  void  set_cursor_origin(
    display_struct   *display,
    const VIO_Point  *origin );

  void  get_cursor_origin(
    display_struct   *display,
    VIO_Point            *origin );

  void  get_cursor_origin(
    display_struct   *display,
    VIO_Point            *origin );

  void  rebuild_cursor_icon(
    display_struct    *display );

  void  update_cursor_colour(
    display_struct   *display,
    VIO_Colour           colour );

  void  build_menu(
    display_struct    *menu_window );

  void  rebuild_menu(
    display_struct    *menu_window );

  VIO_Real  get_size_of_menu_text_area(
    display_struct   *menu_window,
    int              key,
    int              line_number );

  VIO_BOOL   lookup_key_for_mouse_position(
    display_struct   *menu_window,
    VIO_Real             x,
    VIO_Real             y,
    int              *key );

  void  rebuild_cursor_position_model(
    display_struct    *display );

  /* menu/menu.c */

  VIO_Status  initialize_menu(
    display_struct    *menu_window,
    VIO_STR            default_directory1,
    VIO_STR            default_directory2,
    VIO_STR            default_directory3,
    VIO_STR            default_directory4,
    VIO_STR            menu_filename );

  void  initialize_menu_actions(
    display_struct    *menu_window );

  void  initialize_menu_window(
    display_struct    *menu_window );

  void  update_menu_text(
    display_struct      *display,
    menu_entry_struct   *menu_entry );

  DEF_MENU_FUNCTION( push_menu );

  DEF_MENU_UPDATE(push_menu );

  DEF_MENU_FUNCTION( pop_menu );

  DEF_MENU_UPDATE(pop_menu );

  void  pop_menu_one_level(
    display_struct   *menu_window );

  void   set_menu_text(
    display_struct      *menu_window,
    menu_entry_struct   *menu_entry,
    VIO_STR              text );

  void  update_all_menu_text(
    display_struct   *display );

  /* menu/input_menu.c */
  VIO_Status  read_menu(
    menu_window_struct   *menu,
    FILE                 *file );

  void  delete_menu(
    menu_window_struct  *menu );

  /* menu/selected.c */
  void  rebuild_selected_list(
    display_struct    *display,
    display_struct    *marker_window );

  VIO_BOOL  mouse_is_on_object_name(
    display_struct    *display,
    int               x,
    int               y,
    object_struct     **object_under_mouse );

  /* menu/text.c */
  void  set_menu_text_real(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    VIO_Real               value );

  void  set_menu_text_int(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    int                value );

  void  set_menu_text_boolean(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    VIO_BOOL            value,
    VIO_STR             off_str,
    VIO_STR             on_str );

  void  set_menu_text_on_off(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    VIO_BOOL            value );

  void  set_menu_text_with_colour(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    VIO_Colour             colour );

  void  set_menu_text_string(
    display_struct     *menu_window,
    menu_entry_struct  *menu_entry,
    VIO_STR             str );

/* cursor_contours/contours.c */
  void  initialize_cursor_plane_outline(
    display_struct    *display );

  void  cut_polygon_neighbours_from_lines(
    display_struct     *display,
    polygons_struct    *polygons );

/* from segmenting/painting.c */
int  get_current_paint_label(display_struct *display);
int  get_current_erase_label(display_struct *display);

void set_painting_mode(display_struct *display, VIO_BOOL freestyle);
VIO_BOOL get_painting_mode(display_struct *display);

void  initialize_voxel_labeling(display_struct *slice_window);

void  delete_voxel_labeling(slice_window_struct *slice);

void  flip_labels_around_zero(display_struct *slice_window);

void  translate_labels(
    display_struct   *slice_window,
    int              volume_index,
    int              delta[] );

void set_voxel_label_with_undo(display_struct *slice_window, int volume_index,
                               int voxel[], int label);

void  copy_labels_slice_to_slice(
    display_struct   *slice_window,
    int              volume_index,
    int              axis,
    int              src_voxel,
    int              dest_voxel,
    VIO_Real             min_threshold,
    VIO_Real             max_threshold );

/* from segmenting/segmenting.c */

  void  initialize_segmenting(
    segmenting_struct  *segmenting );

  void  clear_all_labels(
    display_struct    *display );

  void  set_labels_on_slice(
    display_struct  *slice_window,
    int             volume_index,
    int             axis_index,
    int             position,
    int             label );

  void  set_connected_voxels_labels(
    display_struct    *slice_window,
    int               volume_index,
    int               axis_index,
    int               position[],
    VIO_Real              min_threshold,
    VIO_Real              max_threshold,
    int               label_min_threshold,
    int               label_max_threshold,
    Neighbour_types   connectivity,
    int               label );

  /* from segmenting/segment_polygons.c */
  void  initialize_surface_edit(
    surface_edit_struct   *surface_edit );

  void  set_visibility_around_poly(
    polygons_struct  *polygons,
    int              poly,
    int              max_polys_to_do,
    VIO_BOOL          set_visibility_flag,
    VIO_BOOL          new_visibility,
    VIO_BOOL          set_colour_flag,
    VIO_Colour           colour );

  void  crop_polygons_visibilities(
    polygons_struct  *polygons,
    int              axis_index,
    VIO_Real             position,
    VIO_BOOL          cropping_above );

  void  initialize_colour_bar(
    display_struct    *slice_window );

  void  rebuild_colour_bar(
    display_struct   *slice_window );

  int  get_colour_bar_y_pos(
    display_struct      *slice_window,
    VIO_Real                value );

  VIO_BOOL  mouse_within_colour_bar(
    display_struct      *slice_window,
    VIO_Real                x,
    VIO_Real                y,
    VIO_Real                *ratio );

  void  get_histogram_space(
    display_struct      *slice_window,
    int                 *x1,
    int                 *x2 );

  void  delete_slice_colour_coding(
    slice_window_struct   *slice,
    int                   volume_index );

  void  set_slice_window_number_labels(
    display_struct    *slice_window,
    int               volume_index,
    int               n_labels );

  void  initialize_slice_colour_coding(
    display_struct    *slice_window,
    int               volume_index );

  VIO_Volume  get_nth_label_volume(
    display_struct   *display,
    int              volume_index );

  VIO_Volume  get_label_volume(
    display_struct   *display );

  VIO_BOOL  label_volume_exists(
    display_struct   *display );

  VIO_BOOL  get_label_visibility(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index );

  int  get_num_labels(
    display_struct   *display,
    int              volume_index );

  void   set_colour_of_label(
    display_struct    *slice_window,
    int               volume_index,
    int               label,
    VIO_Colour            colour );

  VIO_Colour   get_colour_of_label(
    display_struct    *slice_window,
    int               volume_index,
    int               label );

  void   set_volume_opacity(
    display_struct   *slice_window,
    int              volume_index,
    VIO_Real             opacity );

  void   set_label_opacity(
    display_struct   *slice_window,
    int              volume_index,
    VIO_Real             opacity );

  void  colour_coding_has_changed(
    display_struct    *display,
    int               volume_index,
    Update_types      type );

  void  change_colour_coding_range(
    display_struct    *slice_window,
    int               volume_index,
    VIO_Real              min_value,
    VIO_Real              max_value );

  void  colour_code_an_object(
    display_struct   *display,
    object_struct    *object );

  VIO_STR    get_default_colour_map_suffix( void );

  VIO_Status  load_label_colour_map(
    display_struct   *slice_window,
    VIO_STR           filename );

  VIO_Status  save_label_colour_map(
    display_struct   *slice_window,
    VIO_STR           filename );

  void  clear_labels(
    display_struct   *display,
    int              volume_index );

  int  get_voxel_label(
    display_struct   *display,
    int              volume_index,
    int              x,
    int              y,
    int              z );

  void  set_voxel_label(
    display_struct   *display,
    int              volume_index,
    int              x,
    int              y,
    int              z,
    int              label );

  VIO_Status  load_user_defined_colour_coding(
    display_struct   *slice_window,
    VIO_STR           filename );

  void  initialize_crop_box(
    display_struct   *slice_window );

  void  delete_crop_box(
    display_struct   *slice_window );

  void  set_crop_filename(
    display_struct   *slice_window,
    VIO_STR           filename );

  VIO_Status  create_cropped_volume_to_file(
    display_struct   *slice_window,
    VIO_STR           cropped_filename );

  void  crop_and_load_volume(
    display_struct   *slice_window );

  void  toggle_slice_crop_box_visibility(
    display_struct   *slice_window );

  void  reset_crop_box_position(
    display_struct   *display );

  void  start_picking_crop_box(
    display_struct    *slice_window );

  void  get_volume_crop_limits(
    display_struct    *display,
    int               min_voxel[],
    int               max_voxel[] );

  void  initialize_slice_models(
    display_struct    *slice_window );

  void  initialize_slice_models_for_volume(
    display_struct    *slice_window,
    int               volume_index );

  void  delete_slice_models_for_volume(
    display_struct    *slice_window,
    int               volume_index );

  void  rebuild_slice_divider(
    display_struct    *slice_window );

  Bitplane_types  get_slice_readout_bitplanes( void );

  void  rebuild_probe(
    display_struct    *slice_window );

  void  get_slice_cross_section_direction(
    display_struct    *slice_window,
    int               view_index,
    int               section_index,
    VIO_Vector            *in_plane_axis );

  void  rebuild_slice_unfinished_flag(
    display_struct    *slice_window,
    int               view_index );

  VIO_BOOL  get_slice_unfinished_flag_visibility(
    display_struct    *slice_window,
    int               view_index );

  void  set_slice_unfinished_flag_visibility(
    display_struct    *slice_window,
    int               view_index,
    VIO_BOOL           state );

  void  rebuild_slice_cross_section(
    display_struct    *slice_window,
    int               view_index );

  void  rebuild_slice_crop_box(
    display_struct    *slice_window,
    int               view_index );

  void  rebuild_slice_cursor(
    display_struct    *slice_window,
    int               view_index );

  object_struct  *get_slice_pixels_object(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index );

  object_struct  *get_label_slice_pixels_object(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index );

  object_struct  *get_composite_slice_pixels_object(
    display_struct    *slice_window,
    int               view_index );

  int  rebuild_slice_pixels_for_volume(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    VIO_BOOL           incremental_flag,
    VIO_BOOL           interrupted,
    VIO_BOOL           continuing_flag,
    VIO_BOOL           *finished );

void rebuild_slice_field_of_view(display_struct *slice_window, int view_index);

  void  rebuild_slice_text(
    display_struct    *slice_window,
    int               view_index );

  void  rebuild_atlas_slice_pixels(
    display_struct    *slice_window,
    int               view_index );

  void  composite_volume_and_labels(
    display_struct        *slice_window,
    int                   view_index );

  int  rebuild_label_slice_pixels_for_volume(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    VIO_BOOL           incremental_flag,
    VIO_BOOL           interrupted,
    VIO_BOOL           continuing_flag,
    VIO_BOOL           *finished );

  void  update_slice_pixel_visibilities(
    display_struct    *slice_window,
    int               view );

  void  initialize_slice_histogram(
    display_struct   *slice_window );

  void  delete_slice_histogram(
    slice_window_struct   *slice );

  void  clear_histogram(
    display_struct   *slice_window );

  void  resize_histogram(
    display_struct   *slice_window );

  void  compute_histogram(
    display_struct   *slice_window,
    int              axis_index,
    int              voxel_index,
    VIO_BOOL          labeled_only );

  void  start_picking_slice_angle(
    display_struct    *slice_window );

  object_struct   *create_3d_slice_quadmesh(
    VIO_Volume         volume,
    int            axis_index,
    VIO_Real           voxel_position );

  void  create_slice_window(
    display_struct   *display,
    VIO_STR           filename,
    VIO_Volume           volume );

  void  update_all_slice_models(
    display_struct   *slice_window );

  void  delete_slice_window_volume(
    display_struct   *slice_window,
    int              volume_index );

  void  delete_slice_window(
    display_struct   *slice_window );

  VIO_STR  get_volume_filename(
    display_struct    *slice_window,
    int               volume_index );

  void  add_slice_window_volume(
    display_struct    *display,
    VIO_STR            filename,
    VIO_Volume            volume );

  void  set_current_volume_index(
    display_struct  *slice_window,
    int             volume_index );

  int  get_n_volumes(
    display_struct  *display );

  int   get_current_volume_index(
    display_struct   *display );

  VIO_BOOL   get_slice_window_volume(
    display_struct   *display,
    VIO_Volume           *volume );

  VIO_Volume  get_nth_volume(
    display_struct   *display,
    int              volume_index );

  VIO_Volume   get_volume(
    display_struct   *display );

  VIO_BOOL  slice_window_exists(
    display_struct   *display );

  VIO_BOOL  get_slice_window(
    display_struct   *display,
    display_struct   **slice_window );

  VIO_BOOL  get_range_of_volumes(
    display_struct   *display,
    VIO_Point            *min_limit,
    VIO_Point            *max_limit );

  void  set_slice_cursor_update(
    display_struct   *slice_window,
    int              view_index );

  void  set_slice_text_update(
    display_struct   *slice_window,
    int              view_index );

  void  set_slice_cross_section_update(
    display_struct   *slice_window,
    int              view_index );

  void  set_crop_box_update(
    display_struct   *slice_window,
    int              view_index );

  void  set_slice_dividers_update(
    display_struct   *slice_window );

  void  set_probe_update(
    display_struct   *slice_window );

  void  set_colour_bar_update(
    display_struct   *slice_window );

  void  set_atlas_update(
    display_struct   *slice_window,
    int              view_index );

  void  set_slice_window_update(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    Update_types     type );

  void  set_slice_window_all_update(
    display_struct   *slice_window,
    int              volume_index,
    Update_types     type );

  void  set_slice_viewport_update(
    display_struct   *slice_window,
    int              model_number );

  void set_slice_outline_update(
    display_struct   *slice_window,
    int              view_index );

  void  update_slice_window(
    display_struct   *slice_window );

  void  set_slice_composite_update(
    display_struct   *slice_window,
    int              view_index,
    int              x_min,
    int              x_max,
    int              y_min,
    int              y_max );

  VIO_BOOL  get_slice_subviewport(
    display_struct   *slice_window,
    int              view_index,
    int              *x_min,
    int              *x_max,
    int              *y_min,
    int              *y_max );

  void  initialize_volume_cross_section(
    display_struct    *display );

  void  rebuild_volume_outline(
    display_struct    *slice_window );

  void  rebuild_volume_cross_section(
    display_struct    *display );

  void  set_volume_cross_section_visibility(
    display_struct    *display,
    VIO_BOOL           state );

  VIO_BOOL  get_volume_cross_section_visibility(
    display_struct    *display );

  void  initialize_slice_window_events(
    display_struct    *slice_window );

  void  set_voxel_cursor_from_mouse_position(
    display_struct    *slice_window );

  void  initialize_slice_undo(
    volume_undo_struct  *undo );

  void  delete_slice_undo(
    display_struct     *slice_window,
    int                volume_index );

  VIO_BOOL  slice_labels_to_undo(
    display_struct  *display );

  int  undo_slice_labels_if_any(
    display_struct  *display );

  void undo_finish(display_struct *slice_window, int volume_index);
  void undo_start(display_struct *slice_window, int volume_index);
  void undo_save(display_struct *slice_window, int volume_index,
                 const int voxel[], int label);

/*
 * slice_window/view.c 
 */
  void  initialize_slice_window_view(
    display_struct    *slice_window,
    int               volume_index );

  void  set_slice_visibility(
    display_struct    *slice_window,
    int               volume_index,
    int               view,
    VIO_BOOL           visibility );

  VIO_BOOL  get_slice_visibility(
    display_struct    *slice_window,
    int               volume_index,
    int               view );

  void  reset_slice_view(
    display_struct    *slice_window,
    int               view );

  void  resize_slice_view(
    display_struct    *slice_window,
    int               view );

  void  scale_slice_view(
    display_struct    *slice_window,
    int               view,
    VIO_Real              scale_factor );

  void  translate_slice_view(
    display_struct    *slice_window,
    int               view,
    VIO_Real              dx,
    VIO_Real              dy );

  VIO_BOOL  find_slice_view_mouse_is_in(
    display_struct    *display,
    int               x_pixel,
    int               y_pixel,
    int               *view_index );

  VIO_BOOL  convert_pixel_to_voxel(
    display_struct    *display,
    int               volume_index,
    int               x_pixel,
    int               y_pixel,
    VIO_Real              voxel[],
    int               *view_index );

  void  convert_voxel_to_pixel(
    display_struct    *display,
    int               volume_index,
    int               view_index,
    VIO_Real          voxel[],
    VIO_Real          *x_pixel,
    VIO_Real          *y_pixel );

  void  get_voxel_to_pixel_transform(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index,
    int               *x_index,
    int               *y_index,
    VIO_Real              *x_scale,
    VIO_Real              *x_trans,
    VIO_Real              *y_scale,
    VIO_Real              *y_trans );

  VIO_BOOL  get_voxel_corresponding_to_point(
    display_struct  *display,
    const VIO_Point *point,
    VIO_Real        voxel[] );

  void   get_slice_window_partitions(
    display_struct    *slice_window,
    int               *left_panel_width,
    int               *left_slice_width,
    int               *right_slice_width,
    int               *bottom_slice_height,
    int               *top_slice_height,
    int               *text_panel_height,
    int               *colour_bar_panel_height );

  void  get_slice_viewport(
    display_struct    *slice_window,
    int               view_index,
    int               *x_min,
    int               *x_max,
    int               *y_min,
    int               *y_max );

  void  get_colour_bar_viewport(
    display_struct    *slice_window,
    int               *x_min,
    int               *x_max,
    int               *y_min,
    int               *y_max );

  void  get_text_display_viewport(
    display_struct    *slice_window,
    int               *x_min,
    int               *x_max,
    int               *y_min,
    int               *y_max );

  void  get_slice_divider_intersection(
    display_struct    *slice_window,
    int               *x,
    int               *y );

  void  set_slice_divider_intersection(
    display_struct    *slice_window,
    int               x,
    int               y );

  VIO_BOOL  get_volume_corresponding_to_pixel(
    display_struct    *slice_window,
    int               x,
    int               y,
    int               *volume_index,
    int               *view_index,
    VIO_Real              voxel[] );

  VIO_BOOL  get_voxel_in_slice_window(
    display_struct    *display,
    VIO_Real              voxel[],
    int               *volume_index,
    int               *view_index );

  VIO_BOOL  get_voxel_under_mouse(
    display_struct    *display,
    int               *volume_index,
    int               *view_index,
    VIO_Real              voxel[] );

  void  get_current_voxel(
    display_struct    *display,
    int               volume_index,
    VIO_Real          voxel[] );

  VIO_BOOL  set_current_voxel(
    display_struct    *slice_window,
    int               ref_volume_index,
    VIO_Real          voxel[] );

  void  get_slice_perp_axis(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    VIO_Real         perp_axis[VIO_N_DIMENSIONS] );

  void  set_slice_plane_perp_axis(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    const VIO_Real   voxel_perp[] );

  void  set_slice_plane(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    const VIO_Real   x_axis[],
    const VIO_Real   y_axis[] );

  void  get_slice_plane(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    VIO_Real             origin[],
    VIO_Real             x_axis[],
    VIO_Real             y_axis[] );

  VIO_BOOL  get_slice_view_index_under_mouse(
    display_struct   *display,
    int              *view_index );

  VIO_BOOL  get_axis_index_under_mouse(
    display_struct   *display,
    int              *volume_index,
    int              *axis_index );

  VIO_BOOL  slice_has_ortho_axes(
    display_struct   *slice_window,
    int              volume_index,
    int              view_index,
    int              *x_index,
    int              *y_index,
    int              *axis_index );

  int  get_arbitrary_view_index(
    display_struct   *display );

  void  get_slice_model_viewport(
    display_struct   *slice_window,
    int              model,
    int              *x_min,
    int              *x_max,
    int              *y_min,
    int              *y_max );

  VIO_BOOL  update_cursor_from_voxel(
    display_struct    *slice_window );

  VIO_BOOL  update_voxel_from_cursor(
    display_struct    *slice_window );

  void  update_all_slice_axes_views(
    display_struct    *slice_window,
    int               volume_index );

  void  set_volume_transform(
    display_struct     *display,
    int                volume_index,
    VIO_General_transform  *transform );

  void  concat_transform_to_volume(
    display_struct     *display,
    int                volume_index,
    VIO_General_transform  *transform );

  void  transform_current_volume_from_file(
    display_struct   *display,
    VIO_STR           filename );

  void  reset_current_volume_transform(
    display_struct   *display );

/* from closest_line.c */
  VIO_BOOL  find_closest_line_point_to_point(
    display_struct    *display,
    VIO_Point             *point,
    VIO_Point             *closest_line_point );

  void  initialize_surface_curve(
    display_struct     *display );

  void  start_surface_curve(
    display_struct     *display );

  void  end_surface_curve(
    display_struct     *display );

  void  close_surface_curve(
    display_struct     *display );

  void  pick_surface_point_near_a_line(
    display_struct   *display );

  void  reset_surface_curve(
    display_struct     *display );

  void  make_surface_curve_permanent(
    display_struct     *display );

  VIO_BOOL  distance_along_polygons(
    polygons_struct   *polygons,
    VIO_Real              curvature_weight,
    VIO_Real              min_curvature,
    VIO_Real              max_curvature,
    VIO_Point             *p1,
    int               poly1,
    VIO_Point             *p2,
    int               poly2,
    VIO_Real              *dist,
    lines_struct      *lines );

  void  find_polygon_vertex_nearest_point(
    polygons_struct  *polygons,
    int              poly,
    VIO_Point            *point,
    VIO_Point            *closest_vertex );

  void  convert_lines_to_tubes_objects(
    display_struct    *display,
    lines_struct      *lines,
    int               n_around,
    VIO_Real              radius );

  void  add_action_table_function(
    action_table_struct   *action_table,
    Event_types           event_type,
    event_function_type   function );

  void  remove_action_table_function(
    action_table_struct   *action_table,
    Event_types           event_type,
    event_function_type   function );

  void  push_action_table(
    action_table_struct   *action_table,
    Event_types           event_type );

  void  pop_action_table(
    action_table_struct   *action_table,
    Event_types           event_type );

  int  get_event_actions(
    action_table_struct   *action_table,
    Event_types           event_type,
    event_function_type   *actions_list[] );

  void  initialize_action_table(
    action_table_struct   *action_table );

  void  fit_view_to_domain(
    view_struct   *view,
    VIO_Point         *min_limit,
    VIO_Point         *max_limit );

  void  initialize_lights(
    light_struct  *lights );

  void  initialize_render(
    render_struct  *render );

  void  initialize_render_3D(
    render_struct  *render );

  void  set_render_info(
    window_struct  *window,
    render_struct  *render );

  void  initialize_view(
    view_struct  *view,
    VIO_Vector       *line_of_sight,
    VIO_Vector       *horizontal );

  void  assign_view_direction(
    view_struct    *view,
    VIO_Vector         *line_of_sight,
    VIO_Vector         *hor );

  void  get_view_z_axis(
    view_struct   *view,
    VIO_Vector        *z_axis );

  void  get_view_centre(
    view_struct   *view,
    VIO_Point         *centre );

  void  get_screen_axes(
    view_struct   *view,
    VIO_Vector        *hor,
    VIO_Vector        *vert );

  void  adjust_view_for_aspect(
    view_struct    *view,
    window_struct  *window );

  void  transform_point_to_world(
    view_struct   *view,
    VIO_Point         *p,
    VIO_Point         *transformed_point );

  void  transform_world_to_model(
    view_struct   *view,
    VIO_Point         *p,
    VIO_Point         *transformed_point );

  void  transform_world_to_model_vector(
    view_struct   *view,
    VIO_Vector        *v,
    VIO_Vector        *transformed_vector );

  void  transform_point_to_view_space(
    view_struct   *view,
    VIO_Point         *p,
    VIO_Point         *transformed_point );

  void  transform_point_to_screen(
    view_struct   *view,
    VIO_Point         *p,
    VIO_Point         *transformed_point );

  void  set_model_scale(
    view_struct   *view,
    VIO_Real          sx,
    VIO_Real          sy,
    VIO_Real          sz );

  void  convert_screen_to_ray(
    view_struct   *view,
    VIO_Real          x_screen,
    VIO_Real          y_screen,
    VIO_Point         *origin,
    VIO_Vector        *direction );

  void  magnify_view_size(
    view_struct  *view,
    VIO_Real         factor );

  void  set_view_rectangle(
    view_struct   *view,
    VIO_Real          x_min,
    VIO_Real          x_max,
    VIO_Real          y_min,
    VIO_Real          y_max );

  void  transform_screen_to_pixels(
    window_struct  *window,
    VIO_Point          *screen,
    VIO_Point          *pixels );

/* from slice_view/outline.c */
void rebuild_slice_object_outline(display_struct *slice_window, int view_index);
void initialize_slice_object_outline(display_struct *display);

/* from callbacks/object_ops.c */
VIO_BOOL  remove_current_object_from_hierarchy(
    display_struct   *display,
    object_struct    **object );
#endif
