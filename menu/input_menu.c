#include  <display.h>

#define  FUNCTION_LIST \
MENU_FUNCTION(exit_program) \
MENU_FUNCTION(pop_menu) \
MENU_FUNCTION(push_menu) \
MENU_FUNCTION(load_file) \
MENU_FUNCTION(save_file) \
MENU_FUNCTION(create_model) \
MENU_FUNCTION(change_model_name) \
MENU_FUNCTION(reset_view) \
MENU_FUNCTION(save_image) \
MENU_FUNCTION(make_view_fit) \
MENU_FUNCTION(toggle_perspective) \
MENU_FUNCTION(toggle_render_mode) \
MENU_FUNCTION(toggle_shading) \
MENU_FUNCTION(toggle_lights) \
MENU_FUNCTION(toggle_two_sided) \
MENU_FUNCTION(toggle_backfacing) \
MENU_FUNCTION(toggle_line_curve_flag) \
MENU_FUNCTION(toggle_marker_label_flag) \
MENU_FUNCTION(toggle_double_buffer_threed) \
MENU_FUNCTION(toggle_double_buffer_slice) \
MENU_FUNCTION(set_n_curve_segments) \
MENU_FUNCTION(make_all_invisible) \
MENU_FUNCTION(make_all_visible) \
MENU_FUNCTION(advance_visible) \
MENU_FUNCTION(retreat_visible) \
MENU_FUNCTION(reverse_normals) \
MENU_FUNCTION(set_current_object_colour) \
MENU_FUNCTION(set_current_object_surfprop) \
MENU_FUNCTION(centre_view) \
MENU_FUNCTION(magnify_view) \
MENU_FUNCTION(translate_view) \
MENU_FUNCTION(rotate_view) \
MENU_FUNCTION(front_clipping) \
MENU_FUNCTION(back_clipping) \
MENU_FUNCTION(top_view) \
MENU_FUNCTION(bottom_view) \
MENU_FUNCTION(front_view) \
MENU_FUNCTION(back_view) \
MENU_FUNCTION(left_view) \
MENU_FUNCTION(right_view) \
MENU_FUNCTION(advance_selected) \
MENU_FUNCTION(retreat_selected) \
MENU_FUNCTION(descend_selected) \
MENU_FUNCTION(ascend_selected) \
                       MENU_FUNCTION(create_3d_slice) \
                       MENU_FUNCTION(move_slice_plus) \
                       MENU_FUNCTION(move_slice_minus) \
                       MENU_FUNCTION(menu_set_global_variable) \
                       MENU_FUNCTION(pick_view_rectangle) \
                       MENU_FUNCTION(toggle_object_visibility) \
                       MENU_FUNCTION(cut_object) \
                       MENU_FUNCTION(paste_object) \
                       MENU_FUNCTION(start_surface) \
                       MENU_FUNCTION(toggle_surface_extraction) \
                       MENU_FUNCTION(make_surface_permanent) \
                       MENU_FUNCTION(reset_surface) \
                       MENU_FUNCTION(set_surface_extract_x_max_distance) \
                       MENU_FUNCTION(set_surface_extract_y_max_distance) \
                       MENU_FUNCTION(set_surface_extract_z_max_distance) \
                       MENU_FUNCTION(get_labeled_boundary) \
                       MENU_FUNCTION(double_slice_voxels) \
                       MENU_FUNCTION(halve_slice_voxels) \
                       MENU_FUNCTION(reset_current_slice_view) \
                       MENU_FUNCTION(load_active_voxels) \
                       MENU_FUNCTION(save_active_voxels) \
                       MENU_FUNCTION(invert_activity) \
                       MENU_FUNCTION(clear_voxel) \
                       MENU_FUNCTION(label_voxel) \
                       MENU_FUNCTION(reset_activities) \
                       MENU_FUNCTION(clear_connected) \
                       MENU_FUNCTION(label_connected) \
                       MENU_FUNCTION(label_slice) \
                       MENU_FUNCTION(clear_slice) \
                       MENU_FUNCTION(label_connected_3d) \
                       MENU_FUNCTION(expand_labeled_3d) \
                       MENU_FUNCTION(set_colour_limits) \
                       MENU_FUNCTION(set_isovalue) \
                       MENU_FUNCTION(create_film_loop) \
                       MENU_FUNCTION(reset_polygon_visibility) \
                       MENU_FUNCTION(set_polygon_line_thickness) \
                       MENU_FUNCTION(set_visibility_from_colour) \
                       MENU_FUNCTION(remove_invisible_parts_of_polygon) \
                       MENU_FUNCTION(crop_above_plane) \
                       MENU_FUNCTION(crop_below_plane) \
                       MENU_FUNCTION(set_vis_to_invis_colour) \
                       MENU_FUNCTION(set_vis_to_vis_colour) \
                       MENU_FUNCTION(set_invis_colour_to_invis) \
                       MENU_FUNCTION(set_connected_invisible) \
                       MENU_FUNCTION(paint_invisible) \
                       MENU_FUNCTION(paint_visible) \
                       MENU_FUNCTION(set_connected_vis_colour) \
                       MENU_FUNCTION(set_connected_invis_colour) \
                       MENU_FUNCTION(paint_invis_colour) \
                       MENU_FUNCTION(paint_vis_colour) \
                       MENU_FUNCTION(set_vis_paint_colour) \
                       MENU_FUNCTION(set_invis_paint_colour) \
                       MENU_FUNCTION(set_n_paint_polygons) \
                       MENU_FUNCTION(input_polygons_bintree) \
                       MENU_FUNCTION(create_bintree_for_polygons) \
                       MENU_FUNCTION(create_normals_for_polygon) \
                       MENU_FUNCTION(reset_polygon_neighbours) \
                       MENU_FUNCTION(cut_polygon_neighbours) \
                       MENU_FUNCTION(make_polygon_sphere) \
                       MENU_FUNCTION(make_tetrahedral_sphere) \
                       MENU_FUNCTION(make_unit_sphere) \
                       MENU_FUNCTION(subdivide_current_polygon) \
                       MENU_FUNCTION(save_polygons_visibilities) \
                       MENU_FUNCTION(load_polygons_visibilities) \
                       MENU_FUNCTION(smooth_current_polygon) \
                       MENU_FUNCTION(smooth_current_polygon_with_volume) \
                       MENU_FUNCTION(start_deforming_object) \
                       MENU_FUNCTION(stop_deforming_object) \
                       MENU_FUNCTION(set_deformation_boundary) \
                       MENU_FUNCTION(set_deformation_model) \
                       MENU_FUNCTION(set_deformation_parameters) \
                       MENU_FUNCTION(set_deformation_original_positions) \
                       MENU_FUNCTION(show_deformation_model) \
                       MENU_FUNCTION(reset_deformation_model) \
                       MENU_FUNCTION(reverse_polygons_order) \
                       MENU_FUNCTION(smooth_current_lines) \
                       MENU_FUNCTION(make_current_line_tube) \
                       MENU_FUNCTION(convert_line_to_spline_points) \
                       MENU_FUNCTION(make_line_circle) \
                       MENU_FUNCTION(subdivide_current_lines) \
                       MENU_FUNCTION(save_polygons_bintree) \
                       MENU_FUNCTION(create_marker_at_cursor) \
                       MENU_FUNCTION(set_cursor_to_marker) \
                       MENU_FUNCTION(save_markers) \
                       MENU_FUNCTION(pick_marker_defaults) \
                       MENU_FUNCTION(move_cursor_to_home) \
                       MENU_FUNCTION(set_marker_segmentation_threshold) \
                       MENU_FUNCTION(set_default_marker_structure_id) \
                       MENU_FUNCTION(set_default_marker_patient_id) \
                       MENU_FUNCTION(set_default_marker_type) \
                       MENU_FUNCTION(set_default_marker_colour) \
                       MENU_FUNCTION(set_default_marker_label) \
                       MENU_FUNCTION(set_default_marker_size) \
                       MENU_FUNCTION(copy_defaults_to_marker) \
                       MENU_FUNCTION(copy_defaults_to_markers) \
                       MENU_FUNCTION(classify_markers) \
                       MENU_FUNCTION(change_marker_structure_id) \
                       MENU_FUNCTION(change_marker_patient_id) \
                       MENU_FUNCTION(change_marker_label) \
                       MENU_FUNCTION(change_marker_size) \
                       MENU_FUNCTION(change_marker_position) \
                       MENU_FUNCTION(change_marker_type) \
                       MENU_FUNCTION(toggle_lock_slice) \
                       MENU_FUNCTION(resample_slice_window_volume) \
                       MENU_FUNCTION(box_filter_slice_window_volume) \
                       MENU_FUNCTION(delete_current_object) \
                       MENU_FUNCTION(toggle_display_labels) \
                       MENU_FUNCTION(set_paint_x_brush_radius) \
                       MENU_FUNCTION(set_paint_y_brush_radius) \
                       MENU_FUNCTION(set_paint_z_brush_radius) \
                       MENU_FUNCTION(copy_labels_from_lower_slice) \
                       MENU_FUNCTION(copy_labels_from_higher_slice) \
                       MENU_FUNCTION(set_current_paint_label) \
                       MENU_FUNCTION(set_label_colour) \
                       MENU_FUNCTION(save_labels) \
                       MENU_FUNCTION(save_current_label) \
                       MENU_FUNCTION(load_labels) \
                       MENU_FUNCTION(reset_segmenting) \
                       MENU_FUNCTION(set_segmenting_threshold) \
                       MENU_FUNCTION(make_surface_line_permanent) \
                       MENU_FUNCTION(set_line_curvature_weight) \
                       MENU_FUNCTION(start_surface_line) \
                       MENU_FUNCTION(end_surface_line) \
                       MENU_FUNCTION(close_surface_line) \
                       MENU_FUNCTION(reset_surface_line) \
                       MENU_FUNCTION(colour_code_objects) \
                       MENU_FUNCTION(set_label_colour_ratio) \
                       MENU_FUNCTION(set_contour_colour_map) \
                       MENU_FUNCTION(set_hot_metal) \
                       MENU_FUNCTION(set_gray_scale) \
                       MENU_FUNCTION(set_spectral) \
                       MENU_FUNCTION(set_under_colour) \
                       MENU_FUNCTION(set_over_colour) \
                       MENU_FUNCTION(set_nearest_neighbour) \
                       MENU_FUNCTION(set_linear_interpolation) \
                       MENU_FUNCTION(set_box_filter) \
                       MENU_FUNCTION(set_triangle_filter) \
                       MENU_FUNCTION(set_gaussian_filter) \
                       MENU_FUNCTION(set_filter_half_width) \
                       MENU_FUNCTION(set_model_parameters) \
                       MENU_FUNCTION(convert_to_new_representation) \
                       MENU_FUNCTION(load_model_parameters) \
                       MENU_FUNCTION(save_model_parameters) \
                       MENU_FUNCTION(delete_all_surface_points) \
                       MENU_FUNCTION(add_surface_point) \
                       MENU_FUNCTION(delete_surface_point) \
                       MENU_FUNCTION(save_surface_points) \
                       MENU_FUNCTION(load_surface_points) \
                       MENU_FUNCTION(show_all_surface_points) \
                       MENU_FUNCTION(delete_surface_point) \
                       MENU_FUNCTION(delete_surface_point) \
                       MENU_FUNCTION(fit_surface) \
                       MENU_FUNCTION(create_surface_model) \
                       MENU_FUNCTION(scan_model_to_voxels) \
                       MENU_FUNCTION(scan_current_polygon_to_volume) \
                       MENU_FUNCTION(set_atlas_on_or_off) \
                       MENU_FUNCTION(set_atlas_opacity) \
                       MENU_FUNCTION(set_atlas_transparent_threshold) \
                       MENU_FUNCTION(flip_atlas_x) \
                       MENU_FUNCTION(flip_atlas_y) \
                       MENU_FUNCTION(flip_atlas_z) \
                       MENU_FUNCTION(set_atlas_tolerance_x) \
                       MENU_FUNCTION(set_atlas_tolerance_y) \
                       MENU_FUNCTION(set_atlas_tolerance_z) \
                       MENU_FUNCTION(mark_vertices) \
                

typedef  struct
{
    STRING                  action_name;
    menu_function_pointer   action;
    menu_update_pointer     update_action;
}
action_lookup_struct;

#define  MENU_FUNCTION(f)    DEF_MENU_FUNCTION(f); DEF_MENU_UPDATE(f);

FUNCTION_LIST

#undef   MENU_FUNCTION
#define  MENU_FUNCTION(f)    { CREATE_STRING(f), f, GLUE(menu_update_,f) },

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
