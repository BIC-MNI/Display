/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/menu/input_menu.c,v 1.142 2005-04-03 03:39:58 stever Exp $";
#endif

#include  <display.h>

#define  MENU1 \
MENU_F(exit_program) \
MENU_F(show_memory) \
MENU_F(pop_menu) \
MENU_F(push_menu) \
MENU_F(toggle_shift_key) \
MENU_F(load_file) \
MENU_F(save_file) \
MENU_F(create_model) \
MENU_F(change_model_name) \
MENU_F(reset_view) \
MENU_F(print_view) \
MENU_F(type_in_view_origin) \
MENU_F(type_in_view_line_of_sight) \
MENU_F(type_in_view_up_dir) \
MENU_F(type_in_view_window_width) \
MENU_F(type_in_view_perspective_distance) \
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
MENU_F(toggle_slice_interpolation) \
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
MENU_F(type_in_3D_origin) \
MENU_F(advance_selected) \
MENU_F(retreat_selected) \
MENU_F(descend_selected) \
MENU_F(ascend_selected) \
MENU_F(change_background_colour) \
MENU_F(create_3d_slice) \
MENU_F(create_3d_slice_profile) \
MENU_F(save_slice_image) \
MENU_F(save_slice_window) \
MENU_F(toggle_slice_visibility) \
MENU_F(toggle_cross_section_visibility) \
MENU_F(toggle_slice_cross_section_visibility) \
MENU_F(set_current_arbitrary_view) \
MENU_F(rotate_slice_axes)


#define  MENU2 \
MENU_F(move_slice_plus) \
MENU_F(move_slice_minus) \
MENU_F(menu_set_global_variable) \
MENU_F(pick_view_rectangle) \
MENU_F(toggle_object_visibility) \
MENU_F(cut_object) \
MENU_F(paste_object) \
MENU_F(set_surface_invalid_label_range) \
MENU_F(start_volume_isosurface) \
MENU_F(start_volume_binary_isosurface) \
MENU_F(start_label_binary_isosurface) \
MENU_F(get_voxelated_surface) \
MENU_F(get_voxelated_label_surface) \
MENU_F(toggle_surface_extraction) \
MENU_F(make_surface_permanent) \
MENU_F(reset_surface) \
MENU_F(reset_slice_crop) \
MENU_F(toggle_slice_crop_visibility) \
MENU_F(next_volume_visible) \
MENU_F(prev_volume_visible) \
MENU_F(prev_current_volume) \
MENU_F(pick_crop_box_edge) \
MENU_F(set_crop_box_filename) \
MENU_F(load_cropped_volume) \
MENU_F(crop_volume_to_file) \
MENU_F(reset_current_slice_view) \
MENU_F(toggle_share_labels) \
MENU_F(load_label_data) \
MENU_F(save_label_data) \
MENU_F(toggle_crop_labels_on_output) \
MENU_F(clear_voxel) \
MENU_F(label_voxel) \
MENU_F(clear_connected) \
MENU_F(toggle_connectivity) \
MENU_F(label_connected) \
MENU_F(label_connected_no_threshold) \
MENU_F(label_slice) \
MENU_F(clear_slice) \
MENU_F(label_connected_3d) \
MENU_F(dilate_labels) \
MENU_F(erode_labels) \
MENU_F(set_colour_limits) \
MENU_F(save_colour_map) \
MENU_F(load_colour_map) \
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
MENU_F(coalesce_current_polygons) \
MENU_F(separate_current_polygons)

#define MENU3 \
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
MENU_F(reverse_polygons_order) \
MENU_F(print_polygons_surface_area) \
MENU_F(smooth_current_lines) \
MENU_F(set_line_widths) \
MENU_F(convert_markers_to_lines) \
MENU_F(convert_markers_to_closed_lines) \
MENU_F(make_current_line_tube) \
MENU_F(convert_line_to_spline_points) \
MENU_F(make_line_circle) \
MENU_F(subdivide_current_lines) \
MENU_F(save_polygons_bintree) \
MENU_F(create_marker_at_cursor) \
MENU_F(set_cursor_to_marker) \
MENU_F(save_markers) \
MENU_F(move_cursor_to_home) \
MENU_F(type_in_voxel_origin) \
MENU_F(type_in_slice_plane) \
MENU_F(print_voxel_origin) \
MENU_F(print_slice_plane) \
MENU_F(set_default_marker_structure_id) \
MENU_F(set_default_marker_patient_id) \
MENU_F(set_default_marker_type) \
MENU_F(set_default_marker_colour) \
MENU_F(set_default_marker_label) \
MENU_F(set_default_marker_size) \
MENU_F(copy_defaults_to_marker) \
MENU_F(copy_defaults_to_markers) \
MENU_F(change_marker_structure_id) \
MENU_F(change_marker_patient_id) \
MENU_F(change_marker_label) \
MENU_F(change_marker_size) \
MENU_F(change_marker_position) \
MENU_F(change_marker_type) \
MENU_F(set_current_volume_opacity) \
MENU_F(resample_slice_window_volume) \
MENU_F(box_filter_slice_window_volume) \
MENU_F(reset_volume_transform) \
MENU_F(toggle_incremental_slice_update) \
MENU_F(transform_current_volume) \
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
MENU_F(translate_labels_arbitrary)

#define  MENU4 \
MENU_F(calculate_volume) \
MENU_F(toggle_current_volume) \
MENU_F(delete_current_volume) \
MENU_F(toggle_fast_update) \
MENU_F(toggle_cursor_follows_paintbrush) \
MENU_F(toggle_cursor_visibility) \
MENU_F(set_paint_xy_brush_radius) \
MENU_F(set_paint_z_brush_radius) \
MENU_F(copy_labels_from_lower_slice) \
MENU_F(copy_labels_from_higher_slice) \
MENU_F(set_current_paint_label) \
MENU_F(set_current_erase_label) \
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
MENU_F(set_red) \
MENU_F(set_green) \
MENU_F(set_blue) \
MENU_F(set_arbitrary_colour_scale) \
MENU_F(set_user_defined_colour_scale) \
MENU_F(load_user_defined_colour_scale) \
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
MENU_F(flip_object) \
MENU_F(translate_volume_plus_x) \
MENU_F(translate_volume_minus_x) \
MENU_F(translate_volume_plus_y) \
MENU_F(translate_volume_minus_y) \
MENU_F(translate_volume_plus_z) \
MENU_F(translate_volume_minus_z) \
MENU_F(rotate_volume_plus_x) \
MENU_F(rotate_volume_minus_x) \
MENU_F(rotate_volume_plus_y) \
MENU_F(rotate_volume_minus_y) \
MENU_F(rotate_volume_plus_z) \
MENU_F(rotate_volume_minus_z) \
MENU_F(magnify_volume) \
MENU_F(shrink_volume) \
MENU_F(set_volume_rotation_step) \
MENU_F(set_volume_translation_step) \
MENU_F(set_volume_scale_step) \
MENU_F(insert_volume_as_labels) \
MENU_F(save_current_volume_transform)

#define MENU_SEAL \
MENU_F(sulci_menu1) \
MENU_F(sulci_menu2) \
MENU_F(sulci_menu3) \
MENU_F(sulci_menu4) \
MENU_F(sulci_menu5) \
MENU_F(sulci_menu6) \
MENU_F(sulci_menu7) \
MENU_F(sulci_menu8) \
MENU_F(sulci_menu9) \
MENU_F(choose_invisible) \
MENU_F(sulci_menu10) \
MENU_F(smalls_invisible) \

#if USE_SEAL
#  define  FUNCTION_LIST  MENU1 MENU2 MENU3 MENU4 MENU_SEAL
#else
#  define  FUNCTION_LIST  MENU1 MENU2 MENU3 MENU4
#endif                

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

private  int  translate_key_name(
    STRING    key_name );
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
    STRING                    menu_name,
    int                       n_menus,
    menu_definition_struct    menus[] );
private  BOOLEAN  lookup_menu_action(
    STRING                 action_name,
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
    menus = NULL;

    while( status == OK &&
           input_string( file, &menu_entry.menu_name, ' ' ) == OK )
    {
        status = input_menu_entry( file, &menu_entry );
        if( status == OK )
        {
            ADD_ELEMENT_TO_ARRAY( menus, n_menus, menu_entry, 10 );
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
    Status         status;
    STRING         key_name;

    status = input_quoted_string( file, &key_name );

    if( status == OK )
    {
        action->key = translate_key_name( key_name );

        delete_string( key_name );
    }

    if( status == OK )
        status = input_possibly_quoted_string( file, &action->action_name );

    if( status == OK )
        status = input_quoted_string( file, &action->label );

    return( status );
}

private  int  translate_key_name(
    STRING    str )
{
    int     key;

    if( equal_strings( str, "left" ) || equal_strings( str, "\\left" ) )
        key = LEFT_ARROW_KEY;
    else if( equal_strings( str, "right" ) || equal_strings( str, "\\right" ) )
        key = RIGHT_ARROW_KEY;
    else if( equal_strings( str, "up" ) || equal_strings( str, "\\up" ) )
        key = UP_ARROW_KEY;
    else if( equal_strings( str, "down" ) || equal_strings( str, "\\down" ) )
        key = DOWN_ARROW_KEY;
    else if( string_length( str ) == 1 )
    {
        key = (int) str[0];
        if( key < 0 )
            key += 128;
    }
    else
    {
        print( "Error in key name: \\%s\n", str );
        key = (int) "]";
    }

    return( key );
}

private  Status  input_menu_entry(
    FILE                     *file,
    menu_definition_struct   *menu_entry )
{
    Status              status;
    BOOLEAN             found_brace;
    STRING              permanent_string;
    BOOLEAN             permanent_flag;
    key_action_struct   entry;

    status = skip_input_until( file, '{' );

    if( status == OK )
    {
        found_brace = FALSE;

        menu_entry->n_entries = 0;

        do
        {
            status = input_string( file, &permanent_string, ' ' );

            if( status == OK )
            {
                if( equal_strings( permanent_string, "}" ) )
                {
                    found_brace = TRUE;
                }
                else if( equal_strings( permanent_string, "permanent" ) )
                {
                    permanent_flag = TRUE;
                }
                else if( equal_strings( permanent_string, "transient" ) )
                {
                    permanent_flag = FALSE;
                }
                else
                {
                    print( "Expected permanent or transient.\n" );
                    status = ERROR;
                }
            }

            delete_string( permanent_string );

            if( status == OK && !found_brace )
            {
                entry.permanent_flag = permanent_flag;

                status = input_key_action( file, &entry );

                if( status == OK )
                {
                    ADD_ELEMENT_TO_ARRAY( menu_entry->entries,
                                          menu_entry->n_entries,
                                          entry, 1 );
                }

                status = OK;
            }

        } while( status == OK && !found_brace );
    }

    return( status );
}

private  void  free_input_menu(
    int                      n_menus,
    menu_definition_struct   *menus )
{
    int      i, j;

    if( n_menus > 0 )
    {
        for_less( i, 0, n_menus )
        {
            delete_string( menus[i].menu_name );
            if( menus[i].n_entries > 0 )
            {
                for_less( j, 0, menus[i].n_entries )
                {
                    delete_string( menus[i].entries[j].action_name );
                    delete_string( menus[i].entries[j].label );
                }

                FREE( menus[i].entries );
            }
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
            menu->entries[entry_index].label = create_string(
                                                 menus[i].entries[c].label );
            ++entry_index;
        }
    }

    menu->entries[0].n_children = menus[0].n_entries;

    ALLOC( menu->entries[0].children, menu->entries[0].n_children );

    for_less( c, 0, menus[0].n_entries )
        menu->entries[0].children[c] = menus[0].entries[c].menu_entry;
    menu->entries[0].label = create_string( "Top Level Display" );

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
                {
                    menu_entry->action = push_menu;
                    menu_entry->update_action = menu_update_push_menu;
                }
            }
        }
    }
}

private  int  lookup_menu_name(
    STRING                    menu_name,
    int                       n_menus,
    menu_definition_struct    menus[] )
{
    int  i;

    for_less( i, 0, n_menus )
    {
        if( equal_strings( menu_name, menus[i].menu_name ) )
            break;
    }

    if( i >= n_menus )
    {
        i = -1;
    }

    return( i );
}

private  BOOLEAN  lookup_menu_action(
    STRING                 action_name,
    menu_function_pointer  *action,
    menu_update_pointer    *update_action )
{
    BOOLEAN  found;
    int      i;
    STRING   table_name;

    found = FALSE;

    for_less( i, 0, SIZEOF_STATIC_ARRAY(actions) )
    {
        table_name = actions[i].action_name;
        while( *table_name == ' ' )
        {
            ++table_name;
        }

        if( strncmp( action_name, table_name,
                     (size_t) string_length(action_name) ) == 0)
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
    delete_string( entry->label );

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
