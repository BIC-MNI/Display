
#include  <def_string.h>
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_files.h>

typedef  struct
{
    int       key;
    char      *key_name;
    Real      x_pos, y_pos, length;
    Boolean   in_slanted_part_of_keyboard;
} position_struct;

private   position_struct   positions[] = {
                           {(char) LEFT_ARROW_KEY, "<", 5.5, 0.0, 0.5, FALSE},
                           {(char) DOWN_ARROW_KEY, "v", 6.0, 0.0, 0.5, FALSE},
                           {(char) RIGHT_ARROW_KEY, ">", 6.5, 0.0, 0.5, FALSE},
                           {(char) UP_ARROW_KEY, "^", 6.0, 1.0, 0.5, FALSE},

                           {' ', "space", 1.0, 0.0, 3.0, FALSE },

                           {'z', "Z", 0.0, 1.0, 1.0, TRUE },
                           {'x', "X", 1.0, 1.0, 1.0, TRUE },
                           {'c', "C", 2.0, 1.0, 1.0, TRUE },
                           {'v', "V", 3.0, 1.0, 1.0, TRUE },
                           {'b', "B", 4.0, 1.0, 1.0, TRUE },
                           {'n', "N", 5.0, 1.0, 1.0, TRUE },

                           {'a', "A", 0.0, 2.0, 1.0, TRUE },
                           {'s', "S", 1.0, 2.0, 1.0, TRUE },
                           {'d', "D", 2.0, 2.0, 1.0, TRUE },
                           {'f', "F", 3.0, 2.0, 1.0, TRUE },
                           {'g', "G", 4.0, 2.0, 1.0, TRUE },
                           {'h', "H", 5.0, 2.0, 1.0, TRUE },

                           {'q', "Q", 0.0, 3.0, 1.0, TRUE },
                           {'w', "W", 1.0, 3.0, 1.0, TRUE },
                           {'e', "E", 2.0, 3.0, 1.0, TRUE },
                           {'r', "R", 3.0, 3.0, 1.0, TRUE },
                           {'t', "T", 4.0, 3.0, 1.0, TRUE },
                           {'y', "Y", 5.0, 3.0, 1.0, TRUE },

                           {'1', "1", 0.0, 4.0, 1.0, TRUE },
                           {'2', "2", 1.0, 4.0, 1.0, TRUE },
                           {'3', "3", 2.0, 4.0, 1.0, TRUE },
                           {'4', "4", 3.0, 4.0, 1.0, TRUE },
                           {'5', "5", 4.0, 4.0, 1.0, TRUE },
                           {'6', "6", 5.0, 4.0, 1.0, TRUE },
                           {'7', "7", 6.0, 4.0, 1.0, TRUE }
                         };

static    Status   create_menu_text();
static    Status   create_menu_box();
static    Status   create_menu_character();
static    void     compute_origin();

public  Status  build_menu( menu_window )
    graphics_struct   *menu_window;
{
    Status   status;
    int      i;

    for_less( i, 0, SIZEOF_STATIC_ARRAY(positions) )
    {
        status = create_menu_box( menu_window, positions[i].key );

        if( status == OK )
            status = create_menu_character( menu_window, positions[i].key,
                                            positions[i].key_name );

        if( status != OK )
            break;
    }

    for_less( i, 1, menu_window->menu.n_entries )
    {
        status = create_menu_text( menu_window, &menu_window->menu.entries[i] );

        if( status != OK )
        {
            break;
        }
    }

    return( status );
}

private  Status   create_menu_text( menu_window, menu_entry )
    graphics_struct   *menu_window;
    menu_entry_struct *menu_entry;
{
    Status          status;
    Real            x, y, length;
    int             i;
    Status          create_object();
    text_struct     *text;
    Status          add_object_to_model();
    void            set_menu_text();
    Status          update_menu_text();
    model_struct    *model;
    model_struct    *get_graphics_model();

    ALLOC( status, menu_entry->text_list, menu_window->menu.n_lines_in_entry );

    for_less( i, 0, menu_window->menu.n_lines_in_entry )
    {
        status = create_object( &menu_entry->text_list[i], TEXT );

        if( status == OK )
        {
            menu_entry->text_list[i]->visibility = FALSE;

            model = get_graphics_model( menu_window, MENU_BUTTONS_MODEL );

            status = add_object_to_model( model, menu_entry->text_list[i] );
        }

        if( status == OK )
        {
            text = menu_entry->text_list[i]->ptr.text;

            text->font = Menu_window_font;
            text->size = Menu_window_font_size;
            text->colour = Menu_character_colour;

            compute_origin( &menu_window->menu, menu_entry->key, &x, &y,
                            &length );
            x += X_menu_text_offset;
            y += (menu_window->menu.n_lines_in_entry - i) *
                 menu_window->menu.character_height -
                 Y_menu_text_offset;

            if( i == 0 )
                x += Menu_key_character_offset * text->size;

            fill_Point( text->origin, x, y, 0.0 );

            menu_entry->n_chars_across = (int)
                   ( length * menu_window->menu.n_chars_per_unit_across );
        }
    }

    if( status == OK )
    {
        set_menu_text( menu_window, menu_entry, menu_entry->label );

        status = update_menu_text( menu_window, menu_entry );
    }

    return( status );
}

private  void   compute_origin( menu, key, x, y, length )
    menu_window_struct   *menu;
    int                  key;
    Real                 *x;
    Real                 *y;
    Real                 *length;
{
    int      i;
    Boolean  found;
    Real     x_dx, y_dy;

    found = FALSE;

    for_less( i, 0, SIZEOF_STATIC_ARRAY(positions) )
    {
        if( positions[i].key == key )
        {
            found = TRUE;
            break;
        }
    }

    if( !found )
    {
        PRINT( "Character %c\n", key );
        HANDLE_INTERNAL_ERROR( "Unrecognized menu key\n" );
    }
    else
    {
        x_dx = menu->x_dx +
               menu->n_chars_per_unit_across *
               menu->character_width;
        y_dy = menu->y_dy +
               menu->n_lines_in_entry *
               menu->character_height;

        *x = X_menu_origin + (Real) positions[i].x_pos * x_dx;
        *y = Y_menu_origin + (Real) positions[i].y_pos * y_dy;

        if( positions[i].in_slanted_part_of_keyboard )
        {
            *x += (Real) positions[i].y_pos * menu->y_dx;
            *y += (Real) positions[i].x_pos * menu->x_dy;
        }

        *length = positions[i].length;
    }
}

private  Status   create_menu_box( menu_window, key )
    graphics_struct   *menu_window;
    int               key;
{
    Status          status;
    Status          create_object();
    object_struct   *object;
    lines_struct    *lines;
    Status          add_object_to_model();
    Real            x1, y1, x2, y2, length;
    model_struct    *model;
    model_struct    *get_graphics_model();

    status = create_object( &object, LINES );

    if( status == OK )
    {
        model = get_graphics_model( menu_window, MENU_BUTTONS_MODEL );

        status = add_object_to_model( model, object );
    }

    if( status == OK )
    {
        lines = object->ptr.lines;

        lines->line_thickness = 1;
        lines->n_points = 4;
        lines->n_items = 1;

        lines->colour_flag = ONE_COLOUR;

        ALLOC( status, lines->colours, 1 );
    }

    if( status == OK )
    {
        lines->colours[0] = Menu_box_colour;

        ALLOC( status, lines->points, lines->n_points );
    }

    if( status == OK )
        ALLOC( status, lines->end_indices, lines->n_items );

    if( status == OK )
    {
        lines->end_indices[0] = 5;

        ALLOC( status, lines->indices, lines->end_indices[0] );

        compute_origin( &menu_window->menu, key, &x1, &y1, &length );

        x2 = x1 + length * menu_window->menu.n_chars_per_unit_across *
                  menu_window->menu.character_width;
        y2 = y1 + menu_window->menu.n_lines_in_entry *
                  menu_window->menu.character_height;

        fill_Point( lines->points[0], x1, y1, 0.0 );
        fill_Point( lines->points[1], x2, y1, 0.0 );
        fill_Point( lines->points[2], x2, y2, 0.0 );
        fill_Point( lines->points[3], x1, y2, 0.0 );

        lines->indices[0] = 0;
        lines->indices[1] = 1;
        lines->indices[2] = 2;
        lines->indices[3] = 3;
        lines->indices[4] = 0;
    }

    return( status );
}

private  Status   create_menu_character( menu_window, key, key_name )
    graphics_struct   *menu_window;
    int               key;
    char              key_name[];
{
    Status          status;
    Real            x, y, length;
    Status          create_object();
    object_struct   *object;
    text_struct     *text;
    Status          add_object_to_model();
    model_struct    *model;
    model_struct    *get_graphics_model();

    status = create_object( &object, TEXT );

    if( status == OK )
    {
        text = object->ptr.text;

        text->font = Menu_window_font;
        text->size = Menu_window_font_size;
        text->colour = Menu_key_colour;

        (void) sprintf( text->text, "%s-", key_name );

        compute_origin( &menu_window->menu, key, &x, &y, &length );

        fill_Point( text->origin,
                    x + X_menu_text_offset,
                    y + menu_window->menu.n_lines_in_entry *
                        menu_window->menu.character_height -
                        Y_menu_text_offset,
                    0.0 );

        model = get_graphics_model( menu_window, MENU_BUTTONS_MODEL );

        status = add_object_to_model( model, object );
    }

    return( status );
}

private  Boolean   point_within_menu_key_entry( menu_window, key, x, y )
    graphics_struct  *menu_window;
    int              key;
    Real             x;
    Real             y;
{
    Real      x1, y1, x2, y2, length;

    compute_origin( &menu_window->menu, key, &x1, &y1, &length );

    x2 = x1 + length * menu_window->menu.n_chars_per_unit_across *
              menu_window->menu.character_width;
    y2 = y1 + menu_window->menu.n_lines_in_entry *
              menu_window->menu.character_height;

    return( x >= x1 && x <= x2 && y >= y1 && y <= y2 );
}

public  Boolean   lookup_key_for_mouse_position( menu_window, x, y, key )
    graphics_struct  *menu_window;
    Real             x;
    Real             y;
    int              *key;
{
    int      i;
    Boolean  found;

    found = FALSE;

    for_less( i, 0, SIZEOF_STATIC_ARRAY(positions) )
    {
        if( point_within_menu_key_entry( menu_window, positions[i].key, x, y ) )
        {
            *key = positions[i].key;
            found = TRUE;
            break;
        }
    }

    return( found );
}
