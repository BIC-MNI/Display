
#include  <def_display.h>

private  void  crop_surface(
    display_struct   *display,
    Boolean          above_flag );
private  Status  io_polygons_visibilities(
    polygons_struct  *polygons,
    IO_types         io_flag );

public  DEF_MENU_FUNCTION( reset_polygon_visibility )   /* ARGSUSED */
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        set_polygons_visibilities( polygons, TRUE );

        if( polygons->colour_flag == PER_ITEM_COLOURS )
        {
            for_less( i, 0, polygons->n_items )
                polygons->colours[i] = Visible_segmenting_colour;
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_polygon_visibility )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( remove_invisible_parts_of_polygon )   /* ARGSUSED */
{
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        remove_invisible_polygons( polygons );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(remove_invisible_parts_of_polygon)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_n_paint_polygons )   /* ARGSUSED */
{
    int   n;

    print( "Enter # paint polygons: " );

    if( input_int( stdin, &n ) == OK && n >= 0 )
        display->three_d.surface_edit.n_paint_polygons = n;

    (void) input_newline( stdin );

    return( OK );
}

public  DEF_MENU_UPDATE(set_n_paint_polygons)   /* ARGSUSED */
{
    String   text;

    (void) sprintf( text, label,
                    display->three_d.surface_edit.n_paint_polygons );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_connected_invisible )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index, polygons->n_items,
                                    TRUE, OFF, TRUE,
                                    &Invisible_segmenting_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_connected_invisible )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_invisible )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index,
                       display->three_d.surface_edit.n_paint_polygons,
                       TRUE, OFF,
                       TRUE, &Invisible_segmenting_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(paint_invisible)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_visible )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index,
                         display->three_d.surface_edit.n_paint_polygons,
                         TRUE, TRUE,
                         TRUE, &Visible_segmenting_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(paint_visible)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_connected_vis_colour )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index, polygons->n_items,
                                    FALSE, OFF, TRUE,
                                    &Visible_segmenting_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_connected_vis_colour )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_connected_invis_colour )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index, polygons->n_items,
                                    FALSE, OFF, TRUE,
                                    &Invisible_segmenting_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_connected_invis_colour )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_invis_colour )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index,
                       display->three_d.surface_edit.n_paint_polygons,
                       FALSE, OFF, TRUE, &Invisible_segmenting_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(paint_invis_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_vis_colour )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;

    if( get_polygon_under_mouse( display, &polygons, &poly_index, &point ) )
    {
        set_visibility_around_poly( polygons, poly_index,
                         display->three_d.surface_edit.n_paint_polygons,
                         FALSE, OFF, TRUE, &Visible_segmenting_colour );

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(paint_vis_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_visibility_from_colour )   /* ARGSUSED */
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        create_polygons_visibilities( polygons );

        for_less( i, 0, polygons->n_items )
        {
            polygons->visibilities[i] =
                (polygons->colour_flag != PER_ITEM_COLOURS ||
                 !equal_colours(Invisible_segmenting_colour,
                                polygons->colours[i]) );
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_visibility_from_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_invis_colour_to_invis )   /* ARGSUSED */
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) && 
        polygons->colour_flag == PER_ITEM_COLOURS )
    {
        create_polygons_visibilities( polygons );

        for_less( i, 0, polygons->n_items )
        {
            if( equal_colours(Invisible_segmenting_colour,
                              polygons->colours[i]) )
            {
                polygons->visibilities[i] = FALSE;
            }
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_invis_colour_to_invis)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_vis_to_invis_colour )   /* ARGSUSED */
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        set_polygon_per_item_colours( polygons );

        for_less( i, 0, polygons->n_items )
        {
            if( polygons->visibilities == (Smallest_int *) 0 ||
                polygons->visibilities[i] )
            {
                polygons->colours[i] = Invisible_segmenting_colour;
            }
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_vis_to_invis_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_vis_to_vis_colour )   /* ARGSUSED */
{
    int              i;
    polygons_struct  *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        set_polygon_per_item_colours( polygons );

        for_less( i, 0, polygons->n_items )
        {
            if( polygons->visibilities == (Smallest_int *) 0 ||
                polygons->visibilities[i] )
            {
                polygons->colours[i] = Visible_segmenting_colour;
            }
        }

        set_update_required( display, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_vis_to_vis_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( crop_above_plane )   /* ARGSUSED */
{
    crop_surface( display, TRUE );

    return( OK );
}

public  DEF_MENU_UPDATE(crop_above_plane)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( crop_below_plane )   /* ARGSUSED */
{
    crop_surface( display, FALSE );

    return( OK );
}

public  DEF_MENU_UPDATE(crop_below_plane)   /* ARGSUSED */
{
    return( OK );
}

private  void  crop_surface(
    display_struct   *display,
    Boolean          above_flag )
{
    char             ch;
    int              axis_index;
    Real             pos;
    Real             x, y, z;
    Point            position;
    polygons_struct  *polygons;
    display_struct   *slice_window;

    if( get_current_polygons(display,&polygons) )
    {
        if( get_axis_index_under_mouse( display, &axis_index ) )
        {
            slice_window = display->associated[SLICE_WINDOW];
            fill_Point( position, 0.0, 0.0, 0.0 );
            Point_coord(position,axis_index) =
                      (Real) slice_window->slice.slice_index[axis_index];
            convert_voxel_to_world( get_volume(slice_window),
                                    Point_x(position),
                                    Point_y(position),
                                    Point_z(position), &x, &y, &z );
            fill_Point( position, x, y, z );
            pos = Point_coord(position,axis_index);
        }
        else
        {
            print( "Specify an axis: " );
            while( input_nonwhite_character( stdin, &ch ) == OK &&
                   ch != '\n' && (ch < 'x' || ch > 'z') )
            {}

            if( ch != '\n' )
                (void) input_newline( stdin );

            axis_index = ch - 'x';
            if( axis_index < 0 || axis_index > 2 )
                return;

            pos = Point_coord(display->three_d.cursor.origin,axis_index);
        }

        crop_polygons_visibilities( polygons, axis_index, pos, above_flag );
        set_update_required( display, NORMAL_PLANES );
    }
}

public  DEF_MENU_FUNCTION( save_polygons_visibilities )   /* ARGSUSED */
{
    Status           status;
    polygons_struct  *polygons;

    status = OK;

    if( get_current_polygons(display,&polygons) )
    {
        status = io_polygons_visibilities( polygons, WRITE_FILE );
    }

    return( status );
}

public  DEF_MENU_UPDATE(save_polygons_visibilities)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( load_polygons_visibilities )   /* ARGSUSED */
{
    Status           status;
    polygons_struct  *polygons;

    status = OK;

    if( get_current_polygons(display,&polygons) )
    {
        status = io_polygons_visibilities( polygons, READ_FILE );

        set_update_required( display, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(load_polygons_visibilities)   /* ARGSUSED */
{
    return( OK );
}

private  Status  io_polygons_visibilities(
    polygons_struct  *polygons,
    IO_types         io_flag )
{
    Status           status;
    String           filename;
    FILE             *file;

    create_polygons_visibilities( polygons );

    print( "Enter filename: " );

    status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

    (void) input_newline( stdin );

    if( status == OK )
        status = open_file_with_default_suffix( filename, "vis", io_flag,
                                                BINARY_FORMAT, &file );

    if( status == OK )
        status = io_binary_data( file, io_flag, (void *) polygons->visibilities,
                                 sizeof(polygons->visibilities[0]),
                                 polygons->n_items );

    if( status == OK )
        status = close_file( file );

    return( status );
}
