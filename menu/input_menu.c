#include  <display.h>

#define  FUNCTION_LIST \
MENU_F(exit_program) \
MENU_F(show_memory) \
MENU_F(pop_menu) \
MENU_F(push_menu) \
MENU_F(load_file) \
MENU_F(save_file) \
MENU_F(create_model) \
MENU_F(change_model_name) \
MENU_F(reset_view) \
MENU_F(print_view) \
MENU_F(toggle_stereo_mode) \
MENU_F(set_eye_separation) \
MENU_F(save_image) \
MENU_F(make_view_fit) \
MENU_F(toggle_perspective) \
MENU_F(toggle_render_mode) \
MENU_F(toggle_shading) \
MENU_F(toggle_lights) \
MENU_F(toggle_two_sided) \
MENU_F(toggle_backfacing) \
MENU_F(toggle_line_curve_flag) \
MENU_F(toggle_marker_label_flag) \
MENU_F(toggle_double_buffer_threed) \
MENU_F(toggle_double_buffer_slice) \
MENU_F(set_n_curve_segments) \
MENU_F(make_all_invisible) \
MENU_F(make_all_visible) \
MENU_F(advance_visible) \
MENU_F(retreat_visible) \
MENU_F(reverse_normals) \
MENU_F(set_current_object_colour) \
MENU_F(set_current_object_surfprop) \
MENU_F(magnify_view) \
MENU_F(translate_view) \
MENU_F(rotate_view) \
MENU_F(front_clipping) \
MENU_F(back_clipping) \
MENU_F(right_tilted_view) \
MENU_F(left_tilted_view) \
MENU_F(top_view) \
MENU_F(bottom_view) \
MENU_F(front_view) \
MENU_F(back_view) \
MENU_F(left_view) \
MENU_F(right_view) \
MENU_F(advance_selected) \
MENU_F(retreat_selected) \
MENU_F(descend_selected) \
MENU_F(ascend_selected) \
MENU_F(change_background_colour) \
MENU_F(create_3d_slice) \
MENU_F(toggle_slice_visibility) \
MENU_F(toggle_cross_section_visibility) \
MENU_F(toggle_slice_cross_section_visibility) \
MENU_F(set_current_arbitrary_view) \
MENU_F(rotate_slice_axes) \
MENU_F(move_slice_plus) \
MENU_F(move_slice_minus) \
MENU_F(menu_set_global_variable) \
MENU_F(pick_view_rectangle) \
MENU_F(toggle_object_visibility) \
MENU_F(cut_object) \
MENU_F(paste_object) \
MENU_F(set_surface_valid_range) \
MENU_F(set_surface_valid_out_range) \
MENU_F(start_volume_isosurface) \
MENU_F(start_volume_binary_isosurface) \
MENU_F(start_label_binary_isosurface) \
MENU_F(get_voxelated_surface) \
MENU_F(get_voxelated_label_surface) \
MENU_F(toggle_surface_extraction) \
MENU_F(make_surface_permanent) \
MENU_F(reset_surface) \
MENU_F(set_surface_extract_x_max_distance) \
MENU_F(set_surface_extract_y_max_distance) \
MENU_F(set_surface_extract_z_max_distance) \
MENU_F(reset_current_slice_view) \
MENU_F(load_label_data) \
MENU_F(save_label_data) \
MENU_F(clear_voxel) \
MENU_F(label_voxel) \
MENU_F(clear_connected) \
MENU_F(toggle_connectivity) \
MENU_F(label_connected) \
MENU_F(label_connected_no_threshold) \
MENU_F(label_slice) \
MENU_F(clear_slice) \
MENU_F(label_connected_3d) \
MENU_F(expand_labeled_3d) \
MENU_F(set_colour_limits) \
MENU_F(create_film_loop) \
MENU_F(reset_polygon_visibility) \
MENU_F(set_polygon_line_thickness) \
MENU_F(set_visibility_from_colour) \
MENU_F(remove_invisible_parts_of_polygon) \
MENU_F(crop_above_plane) \
MENU_F(crop_below_plane) \
MENU_F(set_vis_to_invis_colour) \
MENU_F(set_vis_to_vis_colour) \
MENU_F(set_invis_colour_to_invis) \
MENU_F(set_connected_invisible) \
MENU_F(paint_invisible) \
MENU_F(paint_visible) \
MENU_F(set_connected_vis_colour) \
MENU_F(set_connected_invis_colour) \
MENU_F(paint_invis_colour) \
MENU_F(paint_vis_colour) \
MENU_F(set_vis_paint_colour) \
MENU_F(set_invis_paint_colour) \
MENU_F(set_n_paint_polygons) \
MENU_F(input_polygons_bintree) \
MENU_F(create_bintree_for_polygons) \
MENU_F(create_normals_for_polygon) \
MENU_F(average_normals_for_polygon) \
MENU_F(reset_polygon_neighbours) \
MENU_F(cut_polygon_neighbours) \
MENU_F(make_polygon_sphere) \
MENU_F(make_tetrahedral_sphere) \
MENU_F(subdivide_current_polygon) \
MENU_F(save_polygons_visibilities) \
MENU_F(load_polygons_visibilities) \
MENU_F(smooth_current_polygon) \
MENU_F(start_deforming_object) \
MENU_F(stop_deforming_object) \
MENU_F(set_deformation_boundary) \
MENU_F(set_deformation_model) \
MENU_F(set_deformation_parameters) \
MENU_F(set_deformation_original_positions) \
MENU_F(show_deformation_model) \
MENU_F(reset_deformation_model) \
MENU_F(reverse_polygons_order) \
MENU_F(print_polygons_surface_area) \
MENU_F(smooth_current_lines) \
MENU_F(set_line_widths) \
MENU_F(convert_markers_to_lines) \
MENU_F(make_current_line_tube) \
MENU_F(convert_line_to_spline_points) \
MENU_F(make_line_circle) \
MENU_F(subdivide_current_lines) \
MENU_F(save_polygons_bintree) \
MENU_F(create_marker_at_cursor) \
MENU_F(set_cursor_to_marker) \
MENU_F(save_markers) \
MENU_F(pick_marker_defaults) \
MENU_F(move_cursor_to_home) \
MENU_F(type_in_voxel_origin) \
MENU_F(type_in_slice_plane) \
MENU_F(print_voxel_origin) \
MENU_F(print_slice_plane) \
MENU_F(set_marker_segmentation_threshold) \
MENU_F(set_default_marker_structure_id) \
MENU_F(set_default_marker_patient_id) \
MENU_F(set_default_marker_type) \
MENU_F(set_default_marker_colour) \
MENU_F(set_default_marker_label) \
MENU_F(set_default_marker_size) \
MENU_F(copy_defaults_to_marker) \
MENU_F(copy_defaults_to_markers) \
MENU_F(classify_markers) \
MENU_F(change_marker_structure_id) \
MENU_F(change_marker_patient_id) \
MENU_F(change_marker_label) \
MENU_F(change_marker_size) \
MENU_F(change_marker_position) \
MENU_F(change_marker_type) \
MENU_F(resample_slice_window_volume) \
MENU_F(box_filter_slice_window_volume) \
MENU_F(redo_histogram) \
MENU_F(redo_histogram_labeled) \
MENU_F(pick_slice_angle_point) \
MENU_F(toggle_slice_anchor) \
MENU_F(delete_current_object) \
MENU_F(toggle_display_labels) \
MENU_F(undo_slice_labels) \
MENU_F(translate_labels_up) \
MENU_F(translate_labels_down) \
MENU_F(translate_labels_left) \
MENU_F(translate_labels_right) \
MENU_F(translate_labels_arbitrary) \
MENU_F(calculate_volume) \
MENU_F(set_paint_xy_brush_radius) \
MENU_F(set_paint_z_brush_radius) \
MENU_F(copy_labels_from_lower_slice) \
MENU_F(copy_labels_from_higher_slice) \
MENU_F(set_current_paint_label) \
MENU_F(set_label_colour) \
MENU_F(set_slice_window_n_labels) \
MENU_F(save_labels) \
MENU_F(save_current_label) \
MENU_F(load_labels) \
MENU_F(reset_segmenting) \
MENU_F(set_segmenting_threshold) \
MENU_F(make_surface_line_permanent) \
MENU_F(set_line_curvature_weight) \
MENU_F(start_surface_line) \
MENU_F(end_surface_line) \
MENU_F(close_surface_line) \
MENU_F(reset_surface_line) \
MENU_F(set_surface_curve_curvature) \
MENU_F(pick_surface_point_on_line) \
MENU_F(colour_code_objects) \
MENU_F(set_label_colour_ratio) \
MENU_F(set_contour_colour_map) \
MENU_F(set_hot_metal) \
MENU_F(set_gray_scale) \
MENU_F(set_spectral) \
MENU_F(set_under_colour) \
MENU_F(set_over_colour) \
MENU_F(set_nearest_neighbour) \
MENU_F(set_linear_interpolation) \
MENU_F(set_box_filter) \
MENU_F(set_triangle_filter) \
MENU_F(set_gaussian_filter) \
MENU_F(set_filter_half_width) \
MENU_F(change_labels_in_range) \
MENU_F(scan_current_object_to_volume) \
MENU_F(set_atlas_on_or_off) \
MENU_F(set_atlas_opacity) \
MENU_F(set_atlas_transparent_threshold) \
MENU_F(flip_atlas_x) \
MENU_F(flip_atlas_y) \
MENU_F(flip_atlas_z) \
MENU_F(set_atlas_tolerance_x) \
MENU_F(set_atlas_tolerance_y) \
MENU_F(set_atlas_tolerance_z) \
MENU_F(mark_vertices) \
MENU_F(flip_object)
                

typedef  struct
{
    STRING                  action_name;
    menu_function_pointer   action;
    menu_update_pointer     update_action;
}
action_lookup_struct;

#define  MENU_F(f) DEF_MENU_FUNCTION(f); DEF_MENU_UPDATE(f);

FUNCTION_LIST

#undef   MENU_F
#define  MENU_F(f) {CREATE_STRING(f),f,GLUE(menu_update_,f)},

static  action_lookup_struct   actions[] = {
                                               FUNCTION_LIST
                                           };

typedef  struct
{
    BOOLEAN            permanent_flag;
    int                key;
    STRING             action_name;
    STRING             label;
    menu_entry_struct  *menu_entry;
}  key_action_struct;

typedef  struct
{
    STRING              menu_name;
    int                 n_entries;
    key_action_struct   *entries;
} menu_definition_struct;

private  Status  input_special_character(
    FILE   *file,
    int    *ch );
private  Status  input_menu_entry(
    FILE                     *file,
    menu_definition_struct   *menu_entry );
private  Status  input_menu(
    FILE                     *file,
    int                      *n_menus_ptr,
    menu_definition_struct   **menus_ptr );
private  void  free_input_menu(
    int                      n_menus,
    menu_definition_struct   *menus );
private  void  create_menu(
    menu_window_struct       *menu,
    int                      n_menus,
    menu_definition_struct   *menus );
private  int  lookup_menu_name(
    char                      menu_name[],
    int                       n_menus,
    menu_definition_struct    menus[] );
private  BOOLEAN  lookup_menu_action(
    char                   action_name[],
    menu_function_pointer  *action,
    menu_update_pointer    *update_action );

public  Status  read_menu(
    menu_window_struct   *menu,
    FILE                 *file )
{
    Status                   status;
    int                      n_menus;
    menu_definition_struct   *menus;

    status = input_menu( file, &n_menus, &menus );

    if( status == OK )
    {
        create_menu( menu, n_menus, menus );

        free_input_menu( n_menus, menus );
    }

    return( status );
}

private  Status  input_menu(
    FILE                     *file,
    int                      *n_menus_ptr,
    menu_definition_struct   **menus_ptr )
{
    Status                   status;
    int                      n_menus;
    menu_definition_struct   *menus;
    menu_definition_struct   menu_entry;

    status = OK;

    n_menus = 0;

    while( status == OK &&
           input_string( file, menu_entry.menu_name, MAX_STRING_LENGTH, ' ' )
           == OK )
    {
        status = input_menu_entry( file, &menu_entry );
        if( status == OK )
        {
            SET_ARRAY_SIZE( menus, n_menus, n_menus+1, 10 );
            menus[n_menus] = menu_entry;
            ++n_menus;
        }
    }

    if( status == OK )
    {
        *n_menus_ptr = n_menus;
        *menus_ptr = menus;
    }

    return( status );
}

private  Status  input_key_action(
    FILE                *file,
    key_action_struct   *action )
{
    Status    status;
    char      ch;

    status = skip_input_until( file, '\'' );

    if( status == OK )
        status = input_character( file, &ch );

    if( status == OK )
    {
        if( ch == '\\' )
        {
            status = input_special_character( file, &action->key );
        }
        else
        {
            action->key = ch;
            if( action->key < 0 )
                action->key += 128;

            status = input_character( file, &ch );

            if( status == OK && ch != '\'' )
            {
                print( "Expected '.\n" );
                status = ERROR;
            }
        }
    }

    if( status == OK )
    {
        status = input_string( file, action->action_name, MAX_STRING_LENGTH,
                               ' ' );
    }

    if( status == OK )
        status = skip_input_until( file, '"' );

    if( status == OK )
        status = input_string( file, action->label, MAX_STRING_LENGTH, '"' );

    return( status );
}

private  Status  input_special_character(
    FILE   *file,
    int    *ch )
{
    STRING  str;
    Status  status;

    status = input_string( file, str, MAX_STRING_LENGTH, '\'' );

    if( status == OK )
    {
        if( strcmp( str, "left" ) == 0 )
            *ch = LEFT_ARROW_KEY;
        else if( strcmp( str, "right" ) == 0 )
            *ch = RIGHT_ARROW_KEY;
        else if( strcmp( str, "up" ) == 0 )
            *ch = UP_ARROW_KEY;
        else if( strcmp( str, "down" ) == 0 )
            *ch = DOWN_ARROW_KEY;
        else if( sscanf( str, "%d", ch ) != 1 )
        {
            print( "Error in reading special character: \\%s\n", str );
            status = ERROR;
        }
    }

    return( status );
}

private  Status  input_menu_entry(
    FILE                     *file,
    menu_definition_struct   *menu_entry )
{
    Status   status;
    BOOLEAN  found_brace;
    STRING   permanent_string;
    BOOLEAN  permanent_flag;

    status = skip_input_until( file, '{' );

    if( status == OK )
    {
        found_brace = FALSE;

        menu_entry->n_entries = 0;

        do
        {
            status = input_string( file, permanent_string, MAX_STRING_LENGTH,
                                   ' ' );

            if( status == OK )
            {
                if( strcmp( permanent_string, "}" ) == 0 )
                {
                    found_brace = TRUE;
                }
                else if( strcmp( permanent_string, "permanent" ) == 0 )
                {
                    permanent_flag = TRUE;
                }
                else if( strcmp( permanent_string, "transient" ) == 0 )
                {
                    permanent_flag = FALSE;
                }
                else
                {
                    print( "Expected permanent or transient.\n" );
                    status = ERROR;
                }
            }

            if( status == OK && !found_brace )
            {
                SET_ARRAY_SIZE( menu_entry->entries,
                                menu_entry->n_entries,
                                menu_entry->n_entries+1, 1 );

                if( status == OK )
                {
                    menu_entry->entries[menu_entry->n_entries].permanent_flag =
                             permanent_flag;
                    status = input_key_action( file,
                        &menu_entry->entries[menu_entry->n_entries] );
                    ++menu_entry->n_entries;
                }
            }

        } while( status == OK && !found_brace );
    }

    return( status );
}

private  void  free_input_menu(
    int                      n_menus,
    menu_definition_struct   *menus )
{
    int      i;
    if( n_menus > 0 )
    {
        for_less( i, 0, n_menus )
        {
            if( menus[i].n_entries > 0 )
                FREE( menus[i].entries );
        }

        FREE( menus );
    }
}

private  void  create_menu(
    menu_window_struct       *menu,
    int                      n_menus,
    menu_definition_struct   *menus )
{
    int                 i, c, child, n_entries, entry_index, menu_index;
    menu_entry_struct   *menu_entry;

    n_entries = 1;

    for_less( i, 0, n_menus )
        n_entries += menus[i].n_entries;

    menu->n_entries = n_entries;

    ALLOC( menu->entries, menu->n_entries );

    entry_index = 1;

    for_less( i, 0, n_menus )
    {
        for_less( c, 0, menus[i].n_entries )
        {
            menus[i].entries[c].menu_entry = &menu->entries[entry_index];
            menu->entries[entry_index].permanent_flag =
                          menus[i].entries[c].permanent_flag;
            menu->entries[entry_index].key = menus[i].entries[c].key;
            (void) strcpy( menu->entries[entry_index].label,
                           menus[i].entries[c].label );
            ++entry_index;
        }
    }

    menu->entries[0].n_children = menus[0].n_entries;

    ALLOC( menu->entries[0].children, menu->entries[0].n_children );

    for_less( c, 0, menus[0].n_entries )
        menu->entries[0].children[c] = menus[0].entries[c].menu_entry;
    (void) strcpy( menu->entries[0].label, "Top Level Display" );

    for_less( i, 0, n_menus )
    {
        for_less( c, 0, menus[i].n_entries )
        {
            menu_entry = menus[i].entries[c].menu_entry;

            menu_index = lookup_menu_name( menus[i].entries[c].action_name,
                                           n_menus, menus );

            if( menu_index >= 0 )
            {
                menu_entry->n_children = menus[menu_index].n_entries;
                ALLOC( menu_entry->children, menu_entry->n_children );

                for_less( child, 0, menus[menu_index].n_entries )
                {
                    menu_entry->children[child] =
                            menus[menu_index].entries[child].menu_entry;
                }

                menu_entry->action = push_menu;
                menu_entry->update_action = menu_update_push_menu;
            }
            else
            {
                menu_entry->n_children = 0;
                if( !lookup_menu_action( menus[i].entries[c].action_name,
                                         &menu_entry->action,
                                         &menu_entry->update_action ) )
                    break;
            }
        }
    }
}

private  int  lookup_menu_name(
    char                      menu_name[],
    int                       n_menus,
    menu_definition_struct    menus[] )
{
    int  i;

    for_less( i, 0, n_menus )
    {
        if( strcmp( menu_name, menus[i].menu_name ) == 0 )
        {
            break;
        }
    }

    if( i >= n_menus )
    {
        i = -1;
    }

    return( i );
}

private  BOOLEAN  lookup_menu_action(
    char                   action_name[],
    menu_function_pointer  *action,
    menu_update_pointer    *update_action )
{
    BOOLEAN  found;
    int      i;
    char     *table_name;

    found = FALSE;

    for_less( i, 0, SIZEOF_STATIC_ARRAY(actions) )
    {
        table_name = actions[i].action_name;
        while( *table_name == ' ' )
        {
            ++table_name;
        }

        if( strncmp( action_name, table_name, strlen(action_name) ) == 0 )
        {
            *action = actions[i].action;
            *update_action = actions[i].update_action;
            found = TRUE;
            break;
        }
    }

    if( !found )
    {
        print( "Menu function undefined " );
        print( "%s\n", action_name );
    }

    return( found );
}

private  void  delete_menu_entry(
    BOOLEAN             top_flag,
    menu_entry_struct   *entry )
{
    if( !top_flag )
        FREE( entry->text_list );

    if( entry->n_children > 0 )
        FREE( entry->children );
}

public  void  delete_menu(
    menu_window_struct  *menu )
{
    int      i;

    for_less( i, 0, menu->n_entries )
    {
        delete_menu_entry( i == 0, &menu->entries[i] );
    }

    FREE( menu->entries );
}
