
#include  <def_string.h>
#include  <def_graphics.h>
#include  <def_globals.h>

typedef  struct
{
    int    key;
    int    x_pos, y_pos;
} position_struct;

private   position_struct   positions[] = {
                                             {' ', 3, 0 },

                                             {'z', 0, 1 },
                                             {'x', 1, 1 },
                                             {'c', 2, 1 },
                                             {'v', 3, 1 },
                                             {'b', 4, 1 },

                                             {'a', 0, 2 },
                                             {'s', 1, 2 },
                                             {'d', 2, 2 },
                                             {'f', 3, 2 },
                                             {'g', 4, 2 },

                                             {'q', 0, 3 },
                                             {'w', 1, 3 },
                                             {'e', 2, 3 },
                                             {'r', 3, 3 },
                                             {'t', 4, 3 },

                                             {'1', 0, 4 },
                                             {'2', 1, 4 },
                                             {'3', 2, 4 },
                                             {'4', 3, 4 },
                                             {'5', 4, 4 },
                                          };

static    Status   create_menu_text();
static    Status   create_menu_box();
static    void     compute_origin();

public  Status  build_menu( menu_window )
    graphics_struct   *menu_window;
{
    Status   status;
    int      i;

    for_less( i, 0, SIZEOF_STATIC_ARRAY(positions) )
    {
        status = create_menu_box( menu_window, positions[i].key );

        if( status != OK )
        {
            break;
        }
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
    int             i;
    Status          create_object();
    text_struct     *text;
    Status          add_object_to_model();
    void            set_menu_text();
    Status          update_menu_text();
    model_struct    *model;
    model_struct    *get_graphics_model();

    ALLOC1( status, menu_entry->text_list, menu_window->menu.n_lines_in_entry,
            object_struct * );

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

            compute_origin( &menu_window->menu, menu_entry->key, &text->origin);
            Point_x(text->origin) += X_menu_text_offset;
            Point_y(text->origin) += (menu_window->menu.n_lines_in_entry - i) *
                                     menu_window->menu.character_height -
                                     Y_menu_text_offset;
            fill_Colour( text->colour, 1.0, 1.0, 1.0 );
        }
    }

    if( status == OK )
    {
        set_menu_text( menu_window, menu_entry, menu_entry->label );

        status = update_menu_text( menu_window, menu_entry );
    }

    return( status );
}

private  void   compute_origin( menu_window, key, origin )
    menu_window_struct   *menu_window;
    int                  key;
    Point                *origin;
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
        x_dx = menu_window->x_dx +
               menu_window->n_chars_across_entry *
               menu_window->character_width;
        y_dy = menu_window->y_dy +
               menu_window->n_lines_in_entry *
               menu_window->character_height;

        fill_Point( *origin, X_menu_origin +
                             (Real) positions[i].x_pos * x_dx +
                             (Real) positions[i].y_pos * menu_window->y_dx,
                             Y_menu_origin +
                             (Real) positions[i].x_pos * menu_window->x_dy +
                             (Real) positions[i].y_pos * y_dy,
                             0.0 );
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
    Point           origin;
    Real            x1, y1, x2, y2;
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

        ALLOC1( status, lines->colours, 1, Colour );

        fill_Colour( lines->colours[0], 1.0, 1.0, 1.0 );

        ALLOC1( status, lines->points, lines->n_points, Point );
    }

    if( status == OK )
    {
        ALLOC1( status, lines->end_indices, lines->n_items, int );
    }

    if( status == OK )
    {
        lines->end_indices[0] = 5;

        ALLOC1( status, lines->indices, lines->end_indices[0], int );

        compute_origin( &menu_window->menu, key, &origin );

        x1 = Point_x( origin );
        y1 = Point_y( origin );

        x2 = x1 + menu_window->menu.n_chars_across_entry *
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
