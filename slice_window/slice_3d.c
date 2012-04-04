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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/slice_window/slice_3d.c,v 1.9 2001/05/27 00:19:54 stever Exp $";
#endif


#include  <display.h>

public  void  initialize_volume_cross_section(
    display_struct    *display )
{
    model_struct   *model;

    model = get_graphics_model( display, MISCELLANEOUS_MODEL );

    display->three_d.volume_outline = create_object( LINES );
    initialize_lines( get_lines_ptr( display->three_d.volume_outline ),
                      Volume_outline_colour );
    set_object_visibility( display->three_d.volume_outline, OFF );
    add_object_to_model( model, display->three_d.volume_outline );

    display->three_d.cross_section = create_object( POLYGONS );
    initialize_polygons( get_polygons_ptr( display->three_d.cross_section ),
                         Cross_section_colour, &Cross_section_spr );
    set_object_visibility( display->three_d.cross_section, OFF );
    add_object_to_model( model, display->three_d.cross_section );
}

private  void   create_box(
    Volume         volume,
    object_struct  *object )
{
    int            i, c, sizes[MAX_DIMENSIONS];
    Real           voxel[MAX_DIMENSIONS], x, y, z;
    lines_struct   *lines;

    lines = get_lines_ptr( object );
    delete_lines( lines );
    initialize_lines( lines, Volume_outline_colour );

    ALLOC( lines->points, 8 );
    ALLOC( lines->end_indices, 4 );
    ALLOC( lines->indices, 16 );

    lines->n_points = 8;
    lines->n_items = 4;

    lines->end_indices[0] = 4;
    lines->end_indices[1] = 8;
    lines->end_indices[2] = 12;
    lines->end_indices[3] = 16;

    lines->indices[0] = 0;
    lines->indices[1] = 1;
    lines->indices[2] = 5;
    lines->indices[3] = 4;

    lines->indices[4] = 1;
    lines->indices[5] = 3;
    lines->indices[6] = 7;
    lines->indices[7] = 5;

    lines->indices[8] = 3;
    lines->indices[9] = 2;
    lines->indices[10] = 6;
    lines->indices[11] = 7;

    lines->indices[12] = 2;
    lines->indices[13] = 0;
    lines->indices[14] = 4;
    lines->indices[15] = 6;

    get_volume_sizes( volume, sizes );

    for_less( i, 0, lines->n_points )
    {
        for_less( c, 0, get_volume_n_dimensions(volume) )
        {
            if( (i & (1 << c)) == 0 )
                voxel[c] = -0.5;
            else
                voxel[c] = (Real) sizes[c] - 0.5;
        }

        convert_voxel_to_world( volume, voxel, &x, &y, &z );
        fill_Point( lines->points[i], x, y, z );
    }
}

public  void  rebuild_volume_outline(
    display_struct    *slice_window )
{
    display_struct    *display;
    Volume            volume;

    display = get_three_d_window( slice_window );

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        create_box( volume, display->three_d.volume_outline );
        set_update_required( display, NORMAL_PLANES );
    }
    else
        set_object_visibility( display->three_d.volume_outline, FALSE );
}

private  void   create_cross_section(
    Volume         volume,
    object_struct  *object,
    Real           origin[],
    Real           x_axis[],
    Real           y_axis[],
    Real           z_axis[] )
{
    int               i, n_points;
    Real              voxels[2*MAX_DIMENSIONS][MAX_DIMENSIONS];
    Real              x, y, z, nx, ny, nz;
    Vector            normal;
    polygons_struct   *polygons;

    polygons = get_polygons_ptr( object );
    delete_polygons( polygons );
    initialize_polygons( polygons, Cross_section_colour, &Cross_section_spr );

    n_points = get_volume_cross_section( volume, origin, x_axis, y_axis,
                                         voxels );

    if( n_points == 0 )
        return;

    ALLOC( polygons->points, n_points );
    ALLOC( polygons->normals, n_points );
    ALLOC( polygons->end_indices, 1 );
    ALLOC( polygons->indices, n_points );

    polygons->n_items = 1;
    polygons->n_points = n_points;
    polygons->end_indices[0] = n_points;

    convert_voxel_vector_to_world( volume, z_axis, &nx, &ny, &nz );

    fill_Vector( normal, nx, ny, nz );
    NORMALIZE_VECTOR( normal, normal );

    for_less( i, 0, n_points )
    {
        convert_voxel_to_world( volume, voxels[i], &x, &y, &z );
        fill_Point( polygons->points[i], x, y, z );
        polygons->normals[i] = normal;
        polygons->indices[i] = i;
    }
}

public  void  rebuild_volume_cross_section(
    display_struct    *display )
{
    Real            origin[MAX_DIMENSIONS];
    Real            x_axis[MAX_DIMENSIONS], y_axis[MAX_DIMENSIONS];
    Real            z_axis[MAX_DIMENSIONS];
    display_struct  *slice_window;
    Volume          volume;

    display = get_three_d_window( display );

    if( get_slice_window( display, &slice_window ) &&
        get_slice_window_volume( slice_window, &volume ) &&
        get_object_visibility( display->three_d.cross_section ) )
    {
        get_slice_plane( slice_window,
                         get_current_volume_index(slice_window),
                         get_arbitrary_view_index(slice_window),
                         origin, x_axis, y_axis );
        get_slice_perp_axis( slice_window,
                             get_current_volume_index(slice_window),
                             get_arbitrary_view_index(slice_window), z_axis );

        create_cross_section( volume, display->three_d.cross_section,
                              origin, x_axis, y_axis, z_axis );

        set_update_required( display, NORMAL_PLANES );
    }
    else
        set_object_visibility( display->three_d.cross_section, FALSE );
}

public  void  set_volume_cross_section_visibility(
    display_struct    *display,
    BOOLEAN           state )
{
    display_struct  *slice_window;

    display = get_three_d_window( display );
    state = state && get_n_volumes(display) > 0;

    if( get_slice_window( display, &slice_window ) )
    {
        set_object_visibility( display->three_d.volume_outline, state );
        set_object_visibility( display->three_d.cross_section, state );

        if( state )
            rebuild_volume_cross_section( slice_window );

        set_update_required( display, NORMAL_PLANES );
    }
}

public  BOOLEAN  get_volume_cross_section_visibility(
    display_struct    *display )
{
    BOOLEAN         state;

    state = get_object_visibility(
                   get_three_d_window(display)->three_d.volume_outline );

    return( state );
}
