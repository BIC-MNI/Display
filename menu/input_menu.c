#include  <def_string.h>
#include  <def_graphics.h>
#include  <def_alloc.h>

#define  FUNCTION_LIST \
                       MENU_FUNCTION(exit_program) \
                       MENU_FUNCTION(pop_menu) \
                       MENU_FUNCTION(push_menu) \
                       MENU_FUNCTION(load_file) \
                       MENU_FUNCTION(save_file) \
                       MENU_FUNCTION(create_model) \
                       MENU_FUNCTION(reset_view) \
                       MENU_FUNCTION(make_view_fit) \
                       MENU_FUNCTION(toggle_perspective) \
                       MENU_FUNCTION(toggle_render_mode) \
                       MENU_FUNCTION(toggle_shading) \
                       MENU_FUNCTION(toggle_lights) \
                       MENU_FUNCTION(toggle_two_sided) \
                       MENU_FUNCTION(toggle_backfacing) \
                       MENU_FUNCTION(toggle_line_curve_flag) \
                       MENU_FUNCTION(set_n_curve_segments) \
                       MENU_FUNCTION(make_all_invisible) \
                       MENU_FUNCTION(make_all_visible) \
                       MENU_FUNCTION(advance_visible) \
                       MENU_FUNCTION(retreat_visible) \
                       MENU_FUNCTION(reverse_normals) \
                       MENU_FUNCTION(set_current_object_colour) \
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
                       MENU_FUNCTION(advance_slice) \
                       MENU_FUNCTION(retreat_slice) \
                       MENU_FUNCTION(prev_marked_slice) \
                       MENU_FUNCTION(next_marked_slice) \
                       MENU_FUNCTION(toggle_marked_slice) \
                       MENU_FUNCTION(set_rotating_slice_mode) \
                       MENU_FUNCTION(set_translating_slice_mode) \
                       MENU_FUNCTION(reset_slice_transform) \
                       MENU_FUNCTION(menu_set_global_variable) \
                       MENU_FUNCTION(pick_view_rectangle) \
                       MENU_FUNCTION(toggle_object_visibility) \
                       MENU_FUNCTION(output_slice_transforms) \
                       MENU_FUNCTION(open_slice_window) \
                       MENU_FUNCTION(start_surface) \
                       MENU_FUNCTION(toggle_surface_extraction) \
                       MENU_FUNCTION(make_surface_permanent) \
                       MENU_FUNCTION(reset_surface) \
                       MENU_FUNCTION(double_slice_voxels) \
                       MENU_FUNCTION(halve_slice_voxels) \
                       MENU_FUNCTION(load_active_voxels) \
                       MENU_FUNCTION(save_active_voxels) \
                       MENU_FUNCTION(load_labeled_voxels) \
                       MENU_FUNCTION(save_labeled_voxels) \
                       MENU_FUNCTION(reset_activities) \
                       MENU_FUNCTION(set_colour_limits) \
                       MENU_FUNCTION(set_isovalue) \
                       MENU_FUNCTION(create_film_loop) \
                       MENU_FUNCTION(set_edited_surface) \
                       MENU_FUNCTION(start_segmenting_surface) \
                       MENU_FUNCTION(reset_polygon_visibility) \
                       MENU_FUNCTION(delete_connected_surface) \
                       MENU_FUNCTION(input_polygons_bintree) \
                       MENU_FUNCTION(create_bintree_for_polygons) \
                       MENU_FUNCTION(create_normals_for_polygon) \
                       MENU_FUNCTION(smooth_current_polygon) \
                       MENU_FUNCTION(smooth_current_lines) \
                       MENU_FUNCTION(make_current_line_tube) \
                       MENU_FUNCTION(convert_line_to_spline_points) \
                       MENU_FUNCTION(save_polygons_bintree) \
                       MENU_FUNCTION(create_marker_at_cursor) \
                       MENU_FUNCTION(set_cursor_to_marker) \
                       MENU_FUNCTION(save_markers) \
                       MENU_FUNCTION(toggle_lock_slice) \
                       MENU_FUNCTION(delete_current_object) \
                       MENU_FUNCTION(label_point) \
                       MENU_FUNCTION(generate_regions) \
                       MENU_FUNCTION(reset_segmenting) \
                       MENU_FUNCTION(set_segmenting_threshold) \
                       MENU_FUNCTION(make_surface_line_permanent) \
                       MENU_FUNCTION(start_surface_line) \
                       MENU_FUNCTION(end_surface_line) \
                       MENU_FUNCTION(reset_surface_line) \
                       MENU_FUNCTION(paint_invisible) \
                       MENU_FUNCTION(paint_visible) \
                       MENU_FUNCTION(remove_invisible_parts_of_polygon) \
                

typedef  struct
{
    String                  action_name;
    menu_function_pointer   action;
    menu_update_pointer     update_action;
}
action_lookup_struct;

#define  MENU_FUNCTION(f)    Status f(), menu_update_##f();

FUNCTION_LIST

#undef   MENU_FUNCTION
#define  MENU_FUNCTION(f)    { #f, f, menu_update_##f },

static  action_lookup_struct   actions[] = {
                                               FUNCTION_LIST
                                           };

typedef  struct
{
    Boolean            permanent_flag;
    char               key;
    String             action_name;
    String             label;
    menu_entry_struct  *menu_entry;
}  key_action_struct;

typedef  struct
{
    String              menu_name;
    int                 n_entries;
    key_action_struct   *entries;
} menu_definition_struct;

static    Status   input_menu();
static    Status   create_menu();
static    Status   free_input_menu();
static    Status   input_menu_entry();
static    Status   lookup_menu_action();
static    int      lookup_menu_name();

public  Status  read_menu( menu, file )
    menu_window_struct   *menu;
    FILE                 *file;
{
    Status                   status;
    int                      n_menus;
    menu_definition_struct   *menus;

    status = input_menu( file, &n_menus, &menus );

    if( status == OK )
    {
        status = create_menu( menu, n_menus, menus );
    }

    if( status == OK )
    {
        status = free_input_menu( n_menus, menus );
    }

    return( status );
}

private  Status  input_menu( file, n_menus_ptr, menus_ptr )
    FILE                     *file;
    int                      *n_menus_ptr;
    menu_definition_struct   **menus_ptr;
{
    Status                   status;
    Status                   input_string();
    int                      n_menus;
    menu_definition_struct   *menus;
    menu_definition_struct   menu_entry;

    status = OK;

    n_menus = 0;

    while( status == OK &&
           input_string( file, menu_entry.menu_name, ' ' ) == OK )
    {
        status = input_menu_entry( file, &menu_entry );
        if( status == OK )
        {
            CHECK_ALLOC1( status, menus, n_menus, n_menus+1,
                          menu_definition_struct, 10 );
        }

        if( status == OK )
        {
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

private  Status  input_key_action( file, action )
    FILE                *file;
    key_action_struct   *action;
{
    Status    status;
    char      ch;
    Status    input_string();
    Status    input_character();
    Status    skip_input_until();

    status = skip_input_until( file, '\'' );

    if( status == OK )
    {
        status = input_character( file, &ch );
    }

    if( status == OK )
    {
        action->key = ch;

        status = input_character( file, &ch );
    }

    if( status == OK )
    {
        if( ch != '\'' )
        {
            PRINT_ERROR( "Expected '.\n" );;
            status = ERROR;
        }
    }

    if( status == OK )
    {
        status = input_string( file, action->action_name, ' ' );
    }

    if( status == OK )
    {
        status = skip_input_until( file, '"' );
    }

    if( status == OK )
    {
        status = input_string( file, action->label, '"' );
    }

    return( status );
}

private  Status  input_menu_entry( file, menu_entry )
    FILE                     *file;
    menu_definition_struct   *menu_entry;
{
    Status   status;
    Status   skip_input_until();
    Status   input_string();
    Boolean  found_brace;
    String   permanent_string;
    Boolean  permanent_flag;

    status = skip_input_until( file, '{' );

    if( status == OK )
    {
        found_brace = FALSE;

        menu_entry->n_entries = 0;

        do
        {
            status = input_string( file, permanent_string, ' ' );

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
                    PRINT_ERROR( "Expected permanent or transient.\n" );
                    status = ERROR;
                }
            }

            if( status == OK && !found_brace )
            {
                CHECK_ALLOC1( status, menu_entry->entries,
                              menu_entry->n_entries,
                              menu_entry->n_entries+1,
                              key_action_struct, 1 );

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

private  Status  free_input_menu( n_menus, menus )
    int                      n_menus;
    menu_definition_struct   *menus;
{
    int      i;
    Status   status;

    status = OK;

    if( n_menus > 0 )
    {
        for_less( i, 0, n_menus )
        {
            if( status == OK && menus[i].n_entries > 0 )
            {
                FREE1( status, menus[i].entries );
            }
        }

        FREE1( status, menus );
    }

    return( status );
}

private  Status  create_menu( menu, n_menus, menus )
    menu_window_struct       *menu;
    int                      n_menus;
    menu_definition_struct   *menus;
{
    Status              status;
    int                 i, c, child, n_entries, entry_index, menu_index;
    Status              create_menu_entry();
    Status              create_key_entry();
    void                turn_on_menu_entry();
    menu_entry_struct   *menu_entry;

    status = OK;

    n_entries = 1;

    for_less( i, 0, n_menus )
    {
        n_entries += menus[i].n_entries;
    }

    menu->n_entries = n_entries;

    ALLOC1( status, menu->entries, menu->n_entries, menu_entry_struct );

    if( status == OK )
    {
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

        ALLOC1( status,
                 menu->entries[0].children,
                 menu->entries[0].n_children, menu_entry_struct * );
    }

    if( status == OK )
    {
        for_less( c, 0, menus[0].n_entries )
        {
            menu->entries[0].children[c] = menus[0].entries[c].menu_entry;
        }

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
                    ALLOC1( status,
                             menu_entry->children,
                             menu_entry->n_children,
                             menu_entry_struct * );

                    if( status == OK )
                    {
                        for_less( child, 0, menus[menu_index].n_entries )
                        {
                            menu_entry->children[child] =
                                    menus[menu_index].entries[child].menu_entry;
                        }
                        menu_entry->action = push_menu;
                        menu_entry->update_action = menu_update_push_menu;
                    }
                }
                else
                {
                    menu_entry->n_children = 0;
                    status = lookup_menu_action( 
                                       menus[i].entries[c].action_name,
                                       &menu_entry->action,
                                       &menu_entry->update_action );

                    if( status != OK )
                    {
                        break;
                    }
                }
            }

            if( status != OK )
            {
                break;
            }
        }
    }

    return( status );
}

private  int  lookup_menu_name( menu_name, n_menus, menus )
    char                      menu_name[];
    int                       n_menus;
    menu_definition_struct  menus[];
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

private  Status  lookup_menu_action( action_name, action, update_action )
    char                   action_name[];
    menu_function_pointer  *action;
    menu_update_pointer    *update_action;
{
    Status   status;
    int      i;
    char     *table_name;

    status = ERROR;

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
            status = OK;
            break;
        }
    }

    if( status != OK )
    {
        PRINT_ERROR( "Menu function undefined " );
        PRINT( "%s\n", action_name );
    }

    return( status );
}

private  Status  delete_menu_entry( top_flag, entry )
    Boolean             top_flag;
    menu_entry_struct   *entry;
{
    Status  status;

    status = OK;

    if( !top_flag )
        FREE1( status, entry->text_list );

    if( status == OK && entry->n_children > 0 )
    {
        FREE1( status, entry->children );
    }

    return( status );
}

public  Status  delete_menu( menu )
    menu_window_struct  *menu;
{
    int      i;
    Status   status;

    status = OK;

    for_less( i, 0, menu->n_entries )
    {
        if( status == OK )
        {
            status = delete_menu_entry( i == 0, &menu->entries[i] );
        }
    }

    if( status == OK )
    {
        FREE1( status,  menu->entries );
    }

    return( status );
}
