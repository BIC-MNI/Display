
#include  <string.h>
#include  <def_graphics.h>
#include  <def_globals.h>

typedef  struct
{
    char   key;
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

public  Status  build_menu( menu_window )
    graphics_struct   *menu_window;
{
    Status   status;
    Status   create_menu_text();
    Status   create_menu_box();
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
    Status          create_object_struct();
    text_struct     *text;
    void            add_object_to_list();
    void            compute_origin();
    Status          update_menu_text();

    status = create_object_struct( &menu_entry->text, TEXT );

    if( status == OK )
    {
        menu_entry->text->visibility = FALSE;

        add_object_to_list( &menu_window->models[THREED_MODEL].objects,
                            menu_entry->text );

        text = menu_entry->text->ptr.text;

        compute_origin( menu_entry->key, &text->origin );
        Point_x(text->origin) += X_menu_text_offset;
        Point_y(text->origin) += Y_menu_text_offset;
        fill_Colour( text->colour, 1.0, 1.0, 1.0 );
        (void) strcpy( text->text, menu_entry->label );
        status = update_menu_text( menu_window, menu_entry );
    }

    return( status );
}

private  void   compute_origin( key, origin )
    char   key;
    Point  *origin;
{
    int      i;
    Boolean  found;

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
        PRINT( "Character %1s\n", &key );
        HANDLE_INTERNAL_ERROR( "Unrecognized menu key\n" );
    }
    else
    {
        fill_Point( *origin, X_menu_origin +
                             (Real) positions[i].x_pos * X_menu_dx +
                             (Real) positions[i].y_pos * Y_menu_dx,
                             Y_menu_origin +
                             (Real) positions[i].x_pos * X_menu_dy +
                             (Real) positions[i].y_pos * Y_menu_dy,
                             0.0 );
    }
}

private  Status   create_menu_box( menu_window, key )
    graphics_struct   *menu_window;
    char              key;
{
    Status          status;
    Status          create_object_struct();
    object_struct   *object;
    lines_struct    *lines;
    void            add_object_to_list();
    void            compute_origin();
    Point           origin;
    Real            x1, y1, x2, y2;

    status = create_object_struct( &object, LINES );

    if( status == OK )
    {
        add_object_to_list( &menu_window->models[THREED_MODEL].objects,
                            object );

        lines = object->ptr.lines;

        lines->n_points = 4;
        lines->n_items = 1;

        fill_Colour( lines->colour, 1.0, 1.0, 1.0 );

        CALLOC1( status, lines->points, lines->n_points, Point );
    }

    if( status == OK )
    {
        CALLOC1( status, lines->end_indices, lines->n_items, int );
    }

    if( status == OK )
    {
        lines->end_indices[0] = 5;

        CALLOC1( status, lines->indices, lines->end_indices[0], int );

        compute_origin( key, &origin );

        x1 = Point_x( origin );
        y1 = Point_y( origin );

        x2 = x1 + X_menu_box_size;
        y2 = y1 + Y_menu_box_size;

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
