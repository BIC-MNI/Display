
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_math.h>
#include  <def_stdio.h>

public  DEF_MENU_FUNCTION( reset_polygon_visibility )   /* ARGSUSED */
{
    int              i;
    polygons_struct  *polygons;
    void             set_update_required();
    void             set_polygons_visibilities();

    if( get_current_polygons(graphics,&polygons) )
    {
        set_polygons_visibilities( polygons, TRUE );

        if( polygons->colour_flag == PER_ITEM_COLOURS )
        {
            for_less( i, 0, polygons->n_items )
                polygons->colours[i] = Visible_segmenting_colour;
        }

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_polygon_visibility )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( remove_invisible_parts_of_polygon )   /* ARGSUSED */
{
    Status           status;
    Status           remove_invisible_polygons();
    polygons_struct  *polygons;
    void             set_update_required();

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
    {
        status = remove_invisible_polygons( polygons );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(remove_invisible_parts_of_polygon)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_n_paint_polygons )   /* ARGSUSED */
{
    int   n;

    PRINT( "Enter # paint polygons: " );
    if( scanf( "%d", &n ) == 1 && n >= 0 )
    {
        graphics->three_d.surface_edit.n_paint_polygons = n;
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_n_paint_polygons)   /* ARGSUSED */
{
    String   text;
    void     set_menu_text();

    (void) sprintf( text, label,
                    graphics->three_d.surface_edit.n_paint_polygons );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_connected_invisible )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            intersection_point;
    void             set_update_required();
    Status           status;
    Status           set_visibility_around_poly();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &intersection_point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                polygons->n_items,
                TRUE, OFF, TRUE, &Invisible_segmenting_colour );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_connected_invisible )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_invisible )   /* ARGSUSED */
{
    Status           status;
    Status           set_visibility_around_poly();
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;
    void             set_update_required();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                       graphics->three_d.surface_edit.n_paint_polygons,
                       TRUE, OFF,
                       TRUE, &Invisible_segmenting_colour );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(paint_invisible)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_visible )   /* ARGSUSED */
{
    Status           status;
    Status           set_visibility_around_poly();
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;
    void             set_update_required();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                         graphics->three_d.surface_edit.n_paint_polygons,
                         TRUE, TRUE,
                         TRUE, &Visible_segmenting_colour );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(paint_visible)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_connected_vis_colour )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            intersection_point;
    void             set_update_required();
    Status           status;
    Status           set_visibility_around_poly();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &intersection_point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                 polygons->n_items,
                 FALSE, OFF, TRUE, &Visible_segmenting_colour );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_connected_vis_colour )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_connected_invis_colour )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            intersection_point;
    void             set_update_required();
    Status           status;
    Status           set_visibility_around_poly();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &intersection_point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
               polygons->n_items,
               FALSE, OFF, TRUE, &Invisible_segmenting_colour );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_connected_invis_colour )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_invis_colour )   /* ARGSUSED */
{
    Status           status;
    Status           set_visibility_around_poly();
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;
    void             set_update_required();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                       graphics->three_d.surface_edit.n_paint_polygons,
                       FALSE, OFF,
                       TRUE, &Invisible_segmenting_colour );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(paint_invis_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_vis_colour )   /* ARGSUSED */
{
    Status           status;
    Status           set_visibility_around_poly();
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;
    void             set_update_required();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                         graphics->three_d.surface_edit.n_paint_polygons,
                         FALSE, OFF,
                         TRUE, &Visible_segmenting_colour );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(paint_vis_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_visibility_from_colour )   /* ARGSUSED */
{
    int              i;
    Status           status;
    Status           create_polygons_visibilities();
    polygons_struct  *polygons;
    void             set_update_required();

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
    {
        status = create_polygons_visibilities( polygons );

        for_less( i, 0, polygons->n_items )
        {
            polygons->visibilities[i] =
                (polygons->colour_flag != PER_ITEM_COLOURS ||
                 !equal_colours(&Invisible_segmenting_colour,
                                &polygons->colours[i]) );
        }

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_visibility_from_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_invis_colour_to_invis )   /* ARGSUSED */
{
    int              i;
    Status           status;
    Status           create_polygons_visibilities();
    polygons_struct  *polygons;
    void             set_update_required();

    status = OK;

    if( get_current_polygons(graphics,&polygons) && 
        polygons->colour_flag == PER_ITEM_COLOURS )
    {
        status = create_polygons_visibilities( polygons );

        for_less( i, 0, polygons->n_items )
        {
            if( equal_colours(&Invisible_segmenting_colour,
                              &polygons->colours[i]) )
            {
                polygons->visibilities[i] = FALSE;
            }
        }

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_invis_colour_to_invis)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_vis_to_invis_colour )   /* ARGSUSED */
{
    int              i;
    Status           status;
    Status           set_polygon_per_item_colours();
    polygons_struct  *polygons;
    void             set_update_required();

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
    {
        status = set_polygon_per_item_colours( polygons );

        for_less( i, 0, polygons->n_items )
        {
            if( polygons->visibilities == (Smallest_int *) 0 ||
                polygons->visibilities[i] )
            {
                polygons->colours[i] = Invisible_segmenting_colour;
            }
        }

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_vis_to_invis_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_vis_to_vis_colour )   /* ARGSUSED */
{
    int              i;
    Status           status;
    Status           set_polygon_per_item_colours();
    polygons_struct  *polygons;
    void             set_update_required();

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
    {
        status = set_polygon_per_item_colours( polygons );

        for_less( i, 0, polygons->n_items )
        {
            if( polygons->visibilities == (Smallest_int *) 0 ||
                polygons->visibilities[i] )
            {
                polygons->colours[i] = Visible_segmenting_colour;
            }
        }

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_vis_to_vis_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( crop_above_plane )   /* ARGSUSED */
{
    void   crop_surface();

    crop_surface( graphics, TRUE );

    return( OK );
}

public  DEF_MENU_UPDATE(crop_above_plane)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( crop_below_plane )   /* ARGSUSED */
{
    void   crop_surface();

    crop_surface( graphics, FALSE );

    return( OK );
}

public  DEF_MENU_UPDATE(crop_below_plane)   /* ARGSUSED */
{
    return( OK );
}

private  void  crop_surface( graphics, above_flag )
    graphics_struct  *graphics;
{
    int              ch, axis_index;
    Real             pos;
    Point            position;
    polygons_struct  *polygons;
    graphics_struct  *slice_window;
    Boolean          get_axis_view_index_under_mouse();
    void             set_update_required();
    void             crop_polygons_visibilities();
    void             convert_voxel_to_point();

    if( get_current_polygons(graphics,&polygons) )
    {
        if( get_axis_view_index_under_mouse( graphics, &axis_index ) )
        {
            slice_window = graphics->associated[SLICE_WINDOW];
            fill_Point( position, 0.0, 0.0, 0.0 );
            Point_coord(position,axis_index) =
                      (Real) slice_window->slice.slice_index[axis_index];
            convert_voxel_to_point( slice_window->slice.volume,
                                    Point_x(position),
                                    Point_y(position),
                                    Point_z(position),
                                    &position );
            pos = Point_coord(position,axis_index);
        }
        else
        {
            PRINT( "Specify an axis: " );
            while( (ch = getchar()) != '\n' && (ch < 'x' || ch > 'z') )
            {}

            if( ch != '\n' )
                while( getchar() != '\n' )
                {}

            axis_index = ch - 'x';
            if( axis_index < 0 || axis_index > 2 )
                return;

            pos = Point_coord(graphics->three_d.cursor.origin,axis_index);
        }

        crop_polygons_visibilities( polygons, axis_index, pos, above_flag );
        set_update_required( graphics, NORMAL_PLANES );
    }
}

public  DEF_MENU_FUNCTION( save_polygons_visibilities )   /* ARGSUSED */
{
    Status           status;
    Status           io_polygons_visibilities();
    polygons_struct  *polygons;

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
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
    Status           io_polygons_visibilities();
    polygons_struct  *polygons;
    void             set_update_required();

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
    {
        status = io_polygons_visibilities( polygons, READ_FILE );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(load_polygons_visibilities)   /* ARGSUSED */
{
    return( OK );
}

private  Status  io_polygons_visibilities( polygons, io_flag )
    polygons_struct  *polygons;
    IO_types         io_flag;
{
    Status           status;
    Status           open_file();
    Status           io_binary_data();
    Status           close_file();
    Status           create_polygons_visibilities();
    Status           input_string();
    String           filename;
    FILE             *file;

    status = create_polygons_visibilities( polygons );

    if( status == OK )
    {
        PRINT( "Enter filename: " );

        status = input_string( stdin, filename, ' ' );
    }

    if( status == OK )
        status = open_file( filename, io_flag, BINARY_FORMAT, &file );

    if( status == OK )
        status = io_binary_data( file, io_flag, polygons->visibilities,
                                 sizeof(polygons->visibilities[0]),
                                 polygons->n_items );

    if( status == OK )
        status = close_file( file );

    return( status );
}
