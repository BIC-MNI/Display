
#include  <def_display.h>

private  void  get_slice_scale(
    display_struct    *display,
    int               view_index,
    Real              *x_scale,
    Real              *y_scale );

public  void  initialize_slice_window(
    display_struct    *slice_window )
{
    int     c, label;

    slice_window->slice.volume_present = FALSE;

    slice_window->slice.temporary_indices_alloced = 0;

    initialize_slice_window_events( slice_window );

    for_less( c, 0, N_DIMENSIONS )
        slice_window->slice.slice_views[c].update_flag = TRUE;

    slice_window->slice.next_to_update = X;

    slice_window->slice.slice_views[0].axis_map[0]  = Slice_view1_axis1;
    slice_window->slice.slice_views[0].axis_flip[0] = Slice_view1_flip1;
    slice_window->slice.slice_views[0].axis_map[1]  = Slice_view1_axis2;
    slice_window->slice.slice_views[0].axis_flip[1] = Slice_view1_flip2;
    slice_window->slice.slice_views[0].axis_map[2]  = Slice_view1_axis3;
    slice_window->slice.slice_views[0].axis_flip[2] = Slice_view1_flip3;

    slice_window->slice.slice_views[1].axis_map[0]  = Slice_view2_axis1;
    slice_window->slice.slice_views[1].axis_flip[0] = Slice_view2_flip1;
    slice_window->slice.slice_views[1].axis_map[1]  = Slice_view2_axis2;
    slice_window->slice.slice_views[1].axis_flip[1] = Slice_view2_flip2;
    slice_window->slice.slice_views[1].axis_map[2]  = Slice_view2_axis3;
    slice_window->slice.slice_views[1].axis_flip[2] = Slice_view2_flip3;

    slice_window->slice.slice_views[2].axis_map[0]  = Slice_view3_axis1;
    slice_window->slice.slice_views[2].axis_flip[0] = Slice_view3_flip1;
    slice_window->slice.slice_views[2].axis_map[1]  = Slice_view3_axis2;
    slice_window->slice.slice_views[2].axis_flip[1] = Slice_view3_flip2;
    slice_window->slice.slice_views[2].axis_map[2]  = Slice_view3_axis3;
    slice_window->slice.slice_views[2].axis_flip[2] = Slice_view3_flip3;

    initialize_slice_models( slice_window );

    initialize_colour_coding( &slice_window->slice.colour_coding,
                              GRAY_SCALE, Colour_below, Colour_above,
                              0.0, 1.0 );

    initialize_colour_bar( slice_window );

    slice_window->slice.label_colour_ratio = Label_colour_display_ratio;
    slice_window->slice.fast_lookup_present = FALSE;

    for_less( label, 0, NUM_LABELS )
    {
        slice_window->slice.fast_lookup[label] = (Colour *) 0;
        slice_window->slice.label_colours_used[label] = FALSE;
    }

    add_new_label( slice_window, ACTIVE_BIT, WHITE );

    add_new_label( slice_window, ACTIVE_BIT | LABEL_BIT, Labeled_voxel_colour);

    add_new_label( slice_window, LABEL_BIT, Inactive_and_labeled_voxel_colour);

    add_new_label( slice_window, 0, Inactive_voxel_colour );

    initialize_segmenting( &slice_window->slice.segmenting );
    initialize_atlas( &slice_window->slice.atlas );
}

public  void  set_slice_window_volume(
    display_struct    *slice_window,
    volume_struct     *volume )
{
    int              c, x_index, y_index, num_entries;
    int              size[N_DIMENSIONS];
    Real             factor, min_thickness, max_thickness;
    Real             thickness[N_DIMENSIONS];

    slice_window->slice.volume_present = TRUE;
    slice_window->slice.volume = *volume;

    get_volume_size( volume, &size[X], &size[Y], &size[Z] );
    get_volume_slice_thickness( volume, &thickness[X], &thickness[Y],
                                        &thickness[Z] );

    for_less( c, 0, N_DIMENSIONS )
    {
        slice_window->slice.slice_views[c].x_offset = 0;
        slice_window->slice.slice_views[c].y_offset = 0;
        slice_window->slice.slice_index[c] = (int) (size[c] / 2);
        slice_window->slice.slice_locked[c] = FALSE;
    }

    slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[X] =
                          thickness[X];
    slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Y] =
                          thickness[Y];
    slice_window->associated[THREE_D_WINDOW]->three_d.cursor.box_size[Z] =
                          thickness[Z];

    update_cursor_size( slice_window->associated[THREE_D_WINDOW] );

    min_thickness = thickness[X];
    max_thickness = thickness[X];

    for_less( c, 1, N_DIMENSIONS )
    {
        if( min_thickness > thickness[c] )
            min_thickness = thickness[c];
        if( max_thickness < thickness[c] )
            max_thickness = thickness[c];
    }

    factor = 1.0 / min_thickness;

    for_less( c, 0, N_DIMENSIONS )
    {
        x_index = slice_window->slice.slice_views[c].axis_map[X];
        y_index = slice_window->slice.slice_views[c].axis_map[Y];

        slice_window->slice.slice_views[c].x_scale = factor * thickness[x_index];

        slice_window->slice.slice_views[c].y_scale = factor * thickness[y_index];
    }

    num_entries = volume->max_value - volume->min_value + 1;

    slice_window->slice.fast_lookup_present =
                  (num_entries <= Max_fast_colour_lookup);

    if( slice_window->slice.fast_lookup_present )
    {
        create_fast_lookup( slice_window, 0 );
        create_fast_lookup( slice_window, ACTIVE_BIT );
        create_fast_lookup( slice_window, LABEL_BIT );
        create_fast_lookup( slice_window, ACTIVE_BIT | LABEL_BIT );
    }

    change_colour_coding_range( slice_window,
                                (Real) volume->min_value,
                                (Real) volume->max_value );

    initialize_voxel_flags( &slice_window->associated[THREE_D_WINDOW]
                            ->three_d.surface_extraction.voxels_queued,
                            get_n_voxels(volume) );

    initialize_voxel_done_flags( &slice_window->associated[THREE_D_WINDOW]
                                  ->three_d.surface_extraction.voxel_done_flags,
                                  get_n_voxels(volume) );

    set_atlas_state( slice_window, Default_atlas_state );
    rebuild_colour_bar( slice_window );
}

public  void  change_colour_coding_range(
    display_struct    *slice_window,
    Real              min_value,
    Real              max_value )
{
    set_colour_coding_min_max( &slice_window->slice.colour_coding,
                               min_value, max_value );

    colour_coding_has_changed( slice_window );
}

public  Colour  get_slice_colour_coding(
    display_struct    *slice_window,
    int               value,
    int               label )
{
    Colour           col, tmp_col, mult, scaled_col;

    col = get_colour_code( &slice_window->slice.colour_coding, (Real) value );

    if( label != ACTIVE_BIT )
    {
        tmp_col = slice_window->slice.label_colours[label];
        MULT_COLOURS( mult, tmp_col, col );
        mult = SCALE_COLOUR( mult, 1.0-slice_window->slice.label_colour_ratio);
        scaled_col = SCALE_COLOUR( col, slice_window->slice.label_colour_ratio);
        ADD_COLOURS( col, mult, scaled_col );
    }

    return( col );
}

public  void  create_fast_lookup(
    display_struct    *slice_window,
    int               label )
{
    ALLOC( slice_window->slice.fast_lookup[label], 
           slice_window->slice.volume.max_value -
           slice_window->slice.volume.min_value + 1 );
}

public  void   add_new_label(
    display_struct    *slice_window,
    int               label,
    Colour            colour )
{
    slice_window->slice.label_colours[label] = colour;
    slice_window->slice.label_colours_used[label] = TRUE;

    if( slice_window->slice.fast_lookup_present )
    {
        if( slice_window->slice.fast_lookup[label] == (Colour *) 0 )
            create_fast_lookup( slice_window, label );

        rebuild_fast_lookup_for_label( slice_window, label );
    }
}

public  int  lookup_label_colour(
    display_struct    *slice_window,
    Colour            colour )
{
    Boolean   found_colour, found_empty;
    int       i, first_empty, label;

    found_colour = FALSE;
    found_empty = FALSE;

    for_inclusive( i, 1, LOWER_AUXILIARY_BITS )
    {
        label = i | ACTIVE_BIT;

        if( slice_window->slice.label_colours_used[label] )
        {
            if( equal_colours( slice_window->slice.label_colours[label],
                               colour ) )
            {
                found_colour = TRUE;
                break;
            }
        }
        else if( !found_empty )
        {
            found_empty = TRUE;
            first_empty = label;
        }
    }

    if( !found_colour )
    {
        if( found_empty )
        {
            label = first_empty;
            (void) add_new_label( slice_window, label, colour );
        }
        else
            label = (ACTIVE_BIT | LABEL_BIT);
    }

    return( label );
}

public  void  rebuild_fast_lookup_for_label(
    display_struct    *slice_window,
    int               label )
{
    int              val, min_val, max_val;
    Colour           colour;

    min_val = slice_window->slice.volume.min_value;
    max_val = slice_window->slice.volume.max_value;
   
    for_inclusive( val, min_val, max_val )
    {
        colour = get_slice_colour_coding( slice_window, val, label );

        slice_window->slice.fast_lookup[label][val-min_val] = colour;
    }
}

public  void  rebuild_fast_lookup(
    display_struct    *slice_window )
{
    int              label;

    if( slice_window->slice.fast_lookup_present )
    {
        for_less( label, 0, NUM_LABELS )
        {
            if( slice_window->slice.fast_lookup[label] != (Colour *) 0 )
                rebuild_fast_lookup_for_label( slice_window, label );
        }
    }
}

public  Boolean   get_slice_window_volume(
    display_struct   *display,
    volume_struct    **volume )
{
    Boolean  volume_set;

    volume_set = FALSE;

    if( display->associated[SLICE_WINDOW] != (display_struct  *) 0 &&
        display->associated[SLICE_WINDOW]->slice.volume_present )
    {
        *volume = &display->associated[SLICE_WINDOW]->slice.volume;
        volume_set = TRUE;
    }
    else
        *volume = (volume_struct *) 0;

    return( volume_set );
}

public  Boolean  get_slice_window(
    display_struct   *display,
    display_struct   **slice_window )
{
    *slice_window = display->associated[SLICE_WINDOW];

    return( *slice_window != (display_struct  *) 0 );
}

public  void  delete_slice_window(
    slice_window_struct   *slice_window )
{
    int      i;

    if( slice_window->temporary_indices_alloced > 0 )
    {
        FREE( slice_window->temporary_indices );
    }

    if( slice_window->fast_lookup_present )
    {
        for_less( i, 0, NUM_LABELS )
        {
            if( slice_window->fast_lookup[i] != (Colour *) 0 )
                FREE( slice_window->fast_lookup[i] );
        }
    }
}

public  Boolean  slice_window_exists(
    display_struct    *display )
{
    return( display != (display_struct  *) 0 &&
            display->associated[SLICE_WINDOW] != (display_struct  *) 0 );
}

public  Boolean  find_slice_view_mouse_is_in(
    display_struct    *display,
    int               x_pixel,
    int               y_pixel,
    int               *view_index )
{
    Boolean  found;
    int      c;
    int      x_min, x_max, y_min, y_max;

    found = FALSE;

    if( slice_window_exists(display) )
    {
        for_less( c, 0, N_DIMENSIONS )
        {
            get_slice_viewport( display, c, &x_min, &x_max, &y_min, &y_max );

            if( x_pixel >= x_min && x_pixel <= x_max &&
                y_pixel >= y_min && y_pixel <= y_max )
            {
                *view_index = c;
                found = TRUE;

                break;
            }
        }
    }

    return( found );
}

public  Boolean  convert_pixel_to_voxel(
    display_struct    *display,
    int               x_pixel,
    int               y_pixel,
    int               *x,
    int               *y,
    int               *z,
    int               *view_index )
{
    Boolean  found;
    Real     x_scale, y_scale;
    int      x_index, y_index, axis_index;
    int      start_indices[N_DIMENSIONS];
    int      voxel_indices[N_DIMENSIONS];
    int      x_pixel_start, x_pixel_end, y_pixel_start, y_pixel_end;

    found = FALSE;

    if( find_slice_view_mouse_is_in( display, x_pixel, y_pixel, view_index ) )
    {
        get_slice_view( display, *view_index, &x_scale, &y_scale,
                        &x_pixel_start, &y_pixel_start,
                        &x_pixel_end, &y_pixel_end,
                        start_indices );

        if( x_pixel >= x_pixel_start && x_pixel <= x_pixel_end &&
            y_pixel >= y_pixel_start && y_pixel <= y_pixel_end )
        {
            axis_index =
                display->slice.slice_views[*view_index].axis_map[Z];
            x_index =
                display->slice.slice_views[*view_index].axis_map[X];
            y_index =
                display->slice.slice_views[*view_index].axis_map[Y];

            voxel_indices[axis_index] = start_indices[axis_index];

            voxel_indices[x_index] = start_indices[x_index] +
                                        (x_pixel - x_pixel_start) / x_scale;
            voxel_indices[y_index] = start_indices[y_index] +
                                        (y_pixel - y_pixel_start) / y_scale;

            *x = voxel_indices[X];
            *y = voxel_indices[Y];
            *z = voxel_indices[Z];

            found = TRUE;
        }
    }

    return( found );
}

private  int  voxel_to_pixel(
    int   x_min,
    int   x_offset,
    Real  x_scale,
    int   voxel )
{
    return( x_min + x_offset + (Real) voxel * x_scale );
}

public  void  convert_voxel_to_pixel(
    display_struct    *display,
    int               view_index,
    int               x_voxel,
    int               y_voxel,
    int               *x_pixel,
    int               *y_pixel )
{
    int      x_index, y_index;
    int      x_min, x_max, y_min, y_max;
    int      size[N_DIMENSIONS];
    Real     x_scale, y_scale;

    get_slice_viewport( display, view_index, &x_min, &x_max, &y_min, &y_max );

    get_slice_scale( display, view_index, &x_scale, &y_scale );

    x_index = display->slice.slice_views[view_index].axis_map[X];
    y_index = display->slice.slice_views[view_index].axis_map[Y];

    get_volume_size( &display->slice.volume,
                     &size[X], &size[Y], &size[Z] );

    if( display->slice.slice_views[view_index].axis_flip[X] )
        x_voxel = size[x_index] - 1 - x_voxel;

    if( display->slice.slice_views[view_index].axis_flip[Y] )
        y_voxel = size[y_index] - 1 - y_voxel;

    *x_pixel = voxel_to_pixel( x_min,
                               display->slice.slice_views[view_index].x_offset,
                               x_scale, x_voxel );

    *y_pixel = voxel_to_pixel( y_min,
                               display->slice.slice_views[view_index].y_offset,
                               y_scale, y_voxel );
}

public  Boolean  get_voxel_corresponding_to_point(
    display_struct    *display,
    Point             *point,
    Real              *x,
    Real              *y,
    Real              *z )
{
    volume_struct   *volume;
    Boolean         converted;

    converted = FALSE;

    if( get_slice_window_volume( display, &volume ) )
    {
        convert_world_to_voxel( volume,
                            Point_x(*point), Point_y(*point), Point_z(*point),
                            x, y, z );

        converted = voxel_is_within_volume( volume, *x, *y, *z );
    }

    return( converted );
}

private  void  get_slice_scale(
    display_struct    *display,
    int               view_index,
    Real              *x_scale,
    Real              *y_scale )
{
    *x_scale = display->slice.slice_views[view_index].x_scale;
    *y_scale = display->slice.slice_views[view_index].y_scale;
}

public  void  get_slice_viewport(
    display_struct    *display,
    int               view_index,
    int               *x_min,
    int               *x_max,
    int               *y_min,
    int               *y_max )
{
    int  x_size, y_size;

    G_get_window_size( display->window, &x_size, &y_size );

    switch( view_index )
    {
    case 0:
        *x_min = Slice_divider_left;
        *x_max = display->slice.x_split-1-Slice_divider_right;
        *y_min = display->slice.y_split+1+Slice_divider_bottom;
        *y_max = y_size-Slice_divider_top;
        break;

    case 1:
        *x_min = display->slice.x_split+1+Slice_divider_left;
        *x_max = x_size-Slice_divider_right;
        *y_min = display->slice.y_split+1+Slice_divider_bottom;
        *y_max = y_size-Slice_divider_top;
        break;

    case 2:
        *x_min = Slice_divider_left;
        *x_max = display->slice.x_split-1-Slice_divider_right;
        *y_min = Slice_divider_bottom;
        *y_max = display->slice.y_split-1-Slice_divider_top;
        break;

    default:
        *x_min = display->slice.x_split+1+Slice_divider_left;
        *x_max = x_size-Slice_divider_right;
        *y_min = Slice_divider_bottom;
        *y_max = display->slice.y_split-1-Slice_divider_top;
    }
}

public  void  get_slice_view(
    display_struct   *display,
    int              view_index,
    Real             *x_scale,
    Real             *y_scale,
    int              *x_pixel,
    int              *y_pixel,
    int              *x_pixel_end,
    int              *y_pixel_end,
    int              indices[N_DIMENSIONS] )
{
    int   x_axis_index, y_axis_index, axis_index;
    int   x_offset, y_offset;
    int   x_size, y_size;
    int   x_min, x_max, y_min, y_max;
    int   size[N_DIMENSIONS];
    Real  start_offset;

    axis_index = display->slice.slice_views[view_index].axis_map[Z];
    x_axis_index = display->slice.slice_views[view_index].axis_map[X];
    y_axis_index = display->slice.slice_views[view_index].axis_map[Y];

    indices[axis_index] = display->slice.slice_index[axis_index];

    x_offset = display->slice.slice_views[view_index].x_offset;
    y_offset = display->slice.slice_views[view_index].y_offset;

    get_slice_scale( display, view_index, x_scale, y_scale );

    get_volume_size( &display->slice.volume,
                     &size[X], &size[Y], &size[Z] );

    x_size = size[x_axis_index];
    y_size = size[y_axis_index];

    get_slice_viewport( display, view_index, &x_min, &x_max, &y_min, &y_max );

    *x_pixel = voxel_to_pixel( x_min, x_offset, *x_scale, 0 );
    indices[x_axis_index] = 0;

    *x_pixel_end = voxel_to_pixel( x_min, x_offset, *x_scale, x_size ) - 1;
    if( *x_pixel_end > x_max )
    {
        *x_pixel_end = x_max;
    }

    if( *x_pixel < x_min )
    {
        start_offset = -(Real) x_offset / *x_scale;
        indices[x_axis_index] = (int) start_offset;

        if( start_offset != (Real) indices[x_axis_index] )
        {
            ++indices[x_axis_index];
            *x_pixel = voxel_to_pixel( x_min, x_offset, *x_scale,
                                       indices[x_axis_index] );
        }
        else
        {
            *x_pixel = x_min;
        }

        if( indices[x_axis_index] >= x_size )
        {
            *x_pixel = x_max + 1;
        }
    }

    *y_pixel = voxel_to_pixel( y_min, y_offset, *y_scale, 0 );
    indices[y_axis_index] = 0;

    *y_pixel_end = voxel_to_pixel( y_min, y_offset, *y_scale, y_size ) - 1;
    if( *y_pixel_end > y_max )
    {
        *y_pixel_end = y_max;
    }

    if( *y_pixel < y_min )
    {
        start_offset = -(Real) y_offset / *y_scale;
        indices[y_axis_index] = (int) start_offset;

        if( start_offset != (Real) indices[y_axis_index] )
        {
            ++indices[y_axis_index];
            *y_pixel = voxel_to_pixel( y_min, y_offset, *y_scale,
                                       indices[y_axis_index] );
        }
        else
        {
            *y_pixel = y_min;
        }

        if( indices[y_axis_index] >= y_size )
        {
            *y_pixel = y_max + 1;
        }
    }

    if( display->slice.slice_views[view_index].axis_flip[X] )
    {
        indices[x_axis_index] = x_size - 1 - indices[x_axis_index];
        *x_scale = -(*x_scale);
    }

    if( display->slice.slice_views[view_index].axis_flip[Y] )
    {
        indices[y_axis_index] = y_size - 1 - indices[y_axis_index];
        *y_scale = -(*y_scale);
    }
}

public  Boolean  get_voxel_in_slice_window(
    display_struct    *display,
    int               *x,
    int               *y,
    int               *z,
    int               *view_index )
{
    display_struct    *slice_window;
    int               x_mouse, y_mouse;
    Boolean           found;

    slice_window = display->associated[SLICE_WINDOW];

    G_get_mouse_position( slice_window->window, &x_mouse, &y_mouse );

    found = convert_pixel_to_voxel( slice_window, x_mouse, y_mouse, x, y, z,
                                    view_index );

    return( found );
}

public  Boolean  get_voxel_in_three_d_window(
    display_struct    *display,
    int               *x,
    int               *y,
    int               *z )
{
    Boolean          found;
    object_struct    *object;
    int              object_index;
    Point            intersection_point;
    display_struct   *slice_window;
    Real             xr, yr, zr;

    found = FALSE;

    if( get_mouse_scene_intersection( display, &object, &object_index,
                                      &intersection_point ) )
    {
        slice_window = display->associated[SLICE_WINDOW];

        if( slice_window != (display_struct  *) 0 )
        {
            if( get_voxel_corresponding_to_point( slice_window,
                                                  &intersection_point,
                                                  &xr, &yr, &zr ) )
            {
                *x = ROUND( xr );
                *y = ROUND( yr );
                *z = ROUND( zr );
                found = TRUE;
            }
        }
    }

    return( found );
}

public  Boolean  get_voxel_under_mouse(
    display_struct    *display,
    int               *x,
    int               *y,
    int               *z,
    int               *view_index )
{
    display_struct    *three_d, *slice_window;
    Boolean           found;

    three_d = display->associated[THREE_D_WINDOW];
    slice_window = display->associated[SLICE_WINDOW];

    if( G_is_mouse_in_window( slice_window->window ) )
    {
        found = get_voxel_in_slice_window( display, x, y, z, view_index );
    }
    else if( G_is_mouse_in_window( three_d->window ) )
    {
        found = get_voxel_in_three_d_window( three_d, x, y, z );
        *view_index = 2;
    }
    else
    {
        found = FALSE;
    }

    return( found );
}

public  void  get_current_voxel(
    display_struct    *slice_window,
    int               *x,
    int               *y,
    int               *z )
{
    *x = slice_window->slice.slice_index[X];
    *y = slice_window->slice.slice_index[Y];
    *z = slice_window->slice.slice_index[Z];
}

public  Boolean  set_current_voxel(
    display_struct    *slice_window,
    int               x,
    int               y,
    int               z )
{
    Boolean           changed;
    int               i, j, axis_index, indices[N_DIMENSIONS];

    indices[X] = x;
    indices[Y] = y;
    indices[Z] = z;

    changed = FALSE;

    for_less( i, 0, N_DIMENSIONS )
    {
        axis_index = slice_window->slice.slice_views[i].axis_map[Z];

        if( indices[axis_index] != slice_window->slice.slice_index[axis_index] )
        {
            slice_window->slice.slice_index[axis_index] = indices[axis_index];

            for_less( j, i, N_DIMENSIONS )
            {
                if( slice_window->slice.slice_views[j].axis_map[Z] ==
                    axis_index)
                {
                    set_slice_window_update( slice_window, j );
                }
            }

            changed = TRUE;
        }
    }

    return( changed );
}

private  void  set_cursor_colour(
    display_struct   *slice_window )
{
    int       x, y, z;
    Real      value;

    if( get_isosurface_value( slice_window->associated[THREE_D_WINDOW], &value))
    {
        get_current_voxel( slice_window, &x, &y, &z );

        if( cube_is_within_volume( &slice_window->slice.volume, x, y, z ) &&
            voxel_contains_value( &slice_window->slice.volume, x, y, z, value ))
        {
            update_cursor_colour( slice_window->associated[THREE_D_WINDOW],
                                  &Cursor_colour_on_surface );
        }
        else
        {
            update_cursor_colour( slice_window->associated[THREE_D_WINDOW],
                                  &Cursor_colour_off_surface );
        }
    }
}

public  Boolean  update_cursor_from_voxel(
    display_struct    *slice_window )
{
    int               x, y, z;
    Real              x_w, y_w, z_w;
    Boolean           changed;
    Point             new_origin;
    display_struct    *display;

    display = slice_window->associated[THREE_D_WINDOW];

    get_current_voxel( slice_window, &x, &y, &z );

    convert_voxel_to_world( &slice_window->slice.volume,
                            (Real) x, (Real) y, (Real) z, &x_w, &y_w, &z_w );
    fill_Point( new_origin, x_w, y_w, z_w );

    if( !EQUAL_POINTS( new_origin, display->three_d.cursor.origin ) )
    {
        display->three_d.cursor.origin = new_origin;

        set_cursor_colour( slice_window );

        update_cursor( display );

        changed = TRUE;
    }
    else
    {
        changed = FALSE;
    }

    return( changed );
}

public  Boolean  update_voxel_from_cursor(
    display_struct    *slice_window )
{
    Real              x, y, z;
    Boolean           changed;
    display_struct    *display;

    changed = FALSE;

    if( slice_window_exists(slice_window) )
    {
        display = slice_window->associated[THREE_D_WINDOW];

        if( get_voxel_corresponding_to_point( slice_window,
                                    &display->three_d.cursor.origin,
                                    &x, &y, &z ) )
        {
            changed = set_current_voxel( slice_window,
                                         ROUND( x ), ROUND( y ), ROUND( z ) );

            set_cursor_colour( slice_window );
        }
    }

    return( changed );
}

public  void  set_slice_window_update(
    display_struct   *display,
    int              view_index )
{
#ifndef  BUG
    if( display != (display_struct  *) 0 )
    {
#endif
        display->slice.slice_views[view_index].update_flag = TRUE;
        set_update_required( display, NORMAL_PLANES );
#ifndef  BUG
    }
#endif
}

public  void  update_slice_window(
    display_struct   *display )
{
    int   c;

    for_less( c, 0, 3 )
    {
        if( display->slice.slice_views[c].update_flag )
        {
            rebuild_slice_pixels( display, c );
            display->slice.slice_views[c].update_flag = FALSE;
        }
    }

}

public  void  create_slice_window(
    display_struct   *display,
    volume_struct    *volume )
{
    display_struct   *slice_window, *menu_window;
    int              nx, ny, nz;
    String           title;

    get_volume_size( volume, &nx, &ny, &nz );

    (void) sprintf( title, "%s [%d * %d * %d]", volume->filename,
                    nx, ny, nz );

    (void) create_graphics_window( SLICE_WINDOW, &slice_window, title, 0, 0 );

    menu_window = display->associated[MENU_WINDOW];

    slice_window->associated[THREE_D_WINDOW] = display;
    slice_window->associated[MENU_WINDOW] = menu_window;
    slice_window->associated[SLICE_WINDOW] = slice_window;
    display->associated[SLICE_WINDOW] = slice_window;
    menu_window->associated[SLICE_WINDOW] = slice_window;

    set_slice_window_volume( slice_window, volume );

    set_slice_window_update( slice_window, 0 );
    set_slice_window_update( slice_window, 1 );
    set_slice_window_update( slice_window, 2 );
}

public  void  colour_coding_has_changed(
    display_struct    *display )
{
    display_struct    *slice_window;

    slice_window = display->associated[SLICE_WINDOW];

    if( slice_window != (display_struct  *) 0 )
    {
        rebuild_fast_lookup( slice_window );

        rebuild_colour_bar( slice_window );
        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }
}

