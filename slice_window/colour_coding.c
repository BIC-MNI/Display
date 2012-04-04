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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/slice_window/colour_coding.c,v 1.45 2001/05/27 00:19:53 stever Exp $";
#endif


#include  <display.h>

#define    MAX_LABEL_COLOUR_TABLE_SIZE    2000000

#define    DEFAULT_COLOUR_MAP_SUFFIX                    "map"

private  void  rebuild_colour_table(
    display_struct    *slice_window,
    int               volume_index );

private  BOOLEAN  is_shared_label_volume(
    slice_window_struct   *slice,
    int                   volume_index )
{
    int     i;

    for_less( i, 0, slice->n_volumes )
    {
        if( i != volume_index &&
            slice->volumes[i].labels == slice->volumes[volume_index].labels )
        {
            return( TRUE );
        }
    }

    return( FALSE );
}

private  void  delete_slice_labels(
    slice_window_struct   *slice,
    int                   volume_index )
{
    if( slice->volumes[volume_index].labels == (Volume) NULL )
        return;

    if( !is_shared_label_volume( slice, volume_index ) )
        delete_volume( slice->volumes[volume_index].labels );

    slice->volumes[volume_index].labels = NULL;
    delete_string( slice->volumes[volume_index].labels_filename );

    FREE( slice->volumes[volume_index].label_colour_table );
}

public  void  delete_slice_colour_coding(
    slice_window_struct   *slice,
    int                   volume_index )
{
    Colour   *ptr;

    if( slice->volumes[volume_index].labels == (Volume) NULL )
        return;

    delete_colour_coding( &slice->volumes[volume_index].colour_coding );
    delete_slice_labels( slice, volume_index );

    ptr = slice->volumes[volume_index].colour_table;

    if( ptr != NULL )
    {
        ptr += (int) slice->volumes[volume_index].offset;
        FREE( ptr );
    }
}

private  void  realloc_label_colour_table(
    display_struct    *slice_window,
    int               volume_index )
{
    int       n_labels, n_colours, n_around, n_up, u, a;
    Colour    col;
    Real      r, g, b, hue, sat;

    n_labels = get_num_labels( slice_window, volume_index );
    ALLOC( slice_window->slice.volumes[volume_index].label_colour_table,
           n_labels );

    n_colours = 0;
    set_colour_of_label( slice_window, volume_index, n_colours++,
                         make_rgba_Colour(0,0,0,0) );
    set_colour_of_label( slice_window, volume_index, n_colours++, RED );
    set_colour_of_label( slice_window, volume_index, n_colours++, GREEN );
    set_colour_of_label( slice_window, volume_index, n_colours++, BLUE );
    set_colour_of_label( slice_window, volume_index, n_colours++, CYAN );
    set_colour_of_label( slice_window, volume_index, n_colours++, MAGENTA );
    set_colour_of_label( slice_window, volume_index, n_colours++, YELLOW );
    set_colour_of_label( slice_window, volume_index, n_colours++, BLUE_VIOLET );
    set_colour_of_label( slice_window, volume_index, n_colours++, DEEP_PINK );
    set_colour_of_label( slice_window, volume_index, n_colours++, GREEN_YELLOW);
    set_colour_of_label( slice_window, volume_index, n_colours++,
                         LIGHT_SEA_GREEN );
    set_colour_of_label( slice_window, volume_index, n_colours++,
                         MEDIUM_TURQUOISE);
    set_colour_of_label( slice_window, volume_index, n_colours++, PURPLE );
    set_colour_of_label( slice_window, volume_index, n_colours++, WHITE );

    n_around = 12;
    n_up = 1;
    while( n_colours < n_labels )
    {
        for_less( u, 0, n_up )
        {
            if( (u % 2) == 1 )
                continue;

            for_less( a, 0, n_around )
            {
                hue = (Real) a / (Real) n_around;
                sat = 0.2 + (0.5 - 0.2) * ((Real) u / (Real) n_up);

                hsl_to_rgb( hue, 1.0, sat, &r, &g, &b );
                col = make_Colour_0_1( r, g, b );

                if( n_colours < n_labels )
                {
                    set_colour_of_label( slice_window, volume_index,
                                         n_colours, col );
                    ++n_colours;
                }
            }
        }
        n_up *= 2;
    }

    if( n_labels >= 256 )
        set_colour_of_label( slice_window, volume_index, 255, BLACK );
}

private  BOOLEAN  find_similar_labels(
    display_struct    *slice_window,
    int               volume_index,
    int               *orig_index )
{
    int                 i, k, m;
    int                 sizes[MAX_DIMENSIONS], this_size[MAX_DIMENSIONS];
    BOOLEAN             same;
    Volume              this_volume, volume;
    General_transform   *this_gen_transform, *gen_transform;
    Transform           *this_transform, *transform;

    this_volume = get_nth_volume( slice_window, volume_index );
    get_volume_sizes( this_volume, this_size );
    this_gen_transform = get_voxel_to_world_transform( this_volume );
    if( get_transform_type(this_gen_transform) != LINEAR )
        return( FALSE );
    this_transform = get_linear_transform_ptr( this_gen_transform );

    for_less( i, 0, get_n_volumes(slice_window) )
    {
        if( i == volume_index )
            continue;

        volume = get_nth_volume( slice_window, i );
        get_volume_sizes( volume, sizes );

        if( sizes[X] != this_size[X] ||
            sizes[Y] != this_size[Y] ||
            sizes[Z] != this_size[Z] )
            continue;

        gen_transform = get_voxel_to_world_transform( volume );
        if( get_transform_type(gen_transform) != LINEAR )
            continue;

        transform = get_linear_transform_ptr( gen_transform );

        same = TRUE;
        for_less( k, 0, 4 )
        for_less( m, 0, 4 )
        {
            if( Transform_elem(*this_transform,k,m) !=
                Transform_elem(*transform,k,m) )
                same = FALSE;
        }

        if( same )
            break;
    }

    if( i < get_n_volumes(slice_window) )
        *orig_index = i;

    return( i < get_n_volumes(slice_window) );
}
    

private  void  create_colour_coding(
    display_struct    *slice_window,
    int               volume_index )
{
    int                   orig_index;
    nc_type               type;
    slice_window_struct   *slice;

    slice = &slice_window->slice;

    if( slice->volumes[volume_index].n_labels <= (1 << 8) )
        type = NC_BYTE;
    else if( slice->volumes[volume_index].n_labels <= (1 << 16) )
        type = NC_SHORT;
    else
        type = NC_LONG;

    delete_string( slice->volumes[volume_index].labels_filename );

    /*--- this will break if you change the number of labels for a shared
          volume.  Later I will rewrite this code to handle this case */

    if( slice->share_labels_flag &&
        find_similar_labels( slice_window, volume_index, &orig_index ) )
    {
        slice->volumes[volume_index].labels = slice->volumes[orig_index].labels;
        slice->volumes[volume_index].labels_filename =
                    create_string( slice->volumes[orig_index].labels_filename );
    }
    else
    {
        slice->volumes[volume_index].labels = create_label_volume(
                          get_nth_volume( slice_window, volume_index ), type );

        slice->volumes[volume_index].labels_filename = create_string( NULL );

        set_volume_voxel_range( slice->volumes[volume_index].labels, 0.0,
                            (Real) slice->volumes[volume_index].n_labels-1.0 );
    }

    realloc_label_colour_table( slice_window, volume_index );
}

public  void  set_slice_window_number_labels(
    display_struct    *slice_window,
    int               volume_index,
    int               n_labels )
{
    delete_slice_labels( &slice_window->slice, volume_index );

    slice_window->slice.volumes[volume_index].n_labels = n_labels;

    create_colour_coding( slice_window, volume_index );
}

private  void  alloc_colour_table(
    display_struct    *slice_window,
    int               volume_index )
{
    Real        min_voxel, max_voxel;
    Colour      *ptr;

    if( is_an_rgb_volume(get_nth_volume(slice_window,volume_index)) )
    {
        slice_window->slice.volumes[volume_index].colour_table = NULL;
        return;
    }

    get_volume_voxel_range( get_nth_volume(slice_window,volume_index),
                            &min_voxel, &max_voxel );

    ALLOC( ptr, (int) max_voxel - (int) min_voxel + 1 );

    slice_window->slice.volumes[volume_index].offset = (int) min_voxel;
    slice_window->slice.volumes[volume_index].colour_table =
                                              ptr - (int) min_voxel;
}

public  void  initialize_slice_colour_coding(
    display_struct    *slice_window,
    int               volume_index )
{
    Real             low_limit, high_limit;
    histogram_struct   histogram;
    Volume             volume;
    Real               *histo_counts;
    Real               scale_factor, trans_factor;
    int                nbbins, axis_index, voxel_index;
    int                x, y, z, sizes[MAX_DIMENSIONS];
    int                start[MAX_DIMENSIONS], end[MAX_DIMENSIONS];
    int 			   sum_count, count, idx;
    Real               min_value, max_value, value;
    progress_struct    progress;
    BOOLEAN			   low_limit_done, high_limit_done;


    initialize_colour_coding(
           &slice_window->slice.volumes[volume_index].colour_coding,
           (Colour_coding_types) Initial_colour_coding_type,
           Colour_below, Colour_above, 0.0, 1.0 );

    slice_window->slice.volumes[volume_index].label_colour_opacity =
                                                        Label_colour_opacity;
    slice_window->slice.volumes[volume_index].n_labels = Initial_num_labels;

    slice_window->slice.volumes[volume_index].offset = 0;
    slice_window->slice.volumes[volume_index].colour_table = (Colour *) NULL;
    slice_window->slice.volumes[volume_index].label_colour_table =
                                                 (Colour *) NULL;
    slice_window->slice.volumes[volume_index].labels = (Volume) NULL;

    slice_window->slice.volumes[volume_index].labels_filename =
                                       create_string( NULL );

    alloc_colour_table( slice_window, volume_index );
    rebuild_colour_table( slice_window, volume_index );
    create_colour_coding( slice_window, volume_index );

    get_volume_real_range( get_nth_volume(slice_window,volume_index),
                           &min_value, &max_value );

    if( Initial_histogram_contrast ) {
        volume = get_nth_volume( slice_window, volume_index );
        get_volume_real_range( volume, &min_value, &max_value );
        get_volume_sizes( volume, sizes );

        initialize_histogram( &histogram, (max_value - min_value) / 1000.0, min_value );
        start[X] = 0;
        end[X] = sizes[X];
        start[Y] = 0;
        end[Y] = sizes[Y];
        start[Z] = 0;
        end[Z] = sizes[Z];

        axis_index = -1;
        voxel_index = 0;

        if( axis_index >= 0 && voxel_index >= 0 && voxel_index < sizes[axis_index] )
        {
            start[axis_index] = voxel_index;
            end[axis_index] = voxel_index+1;
        }

        if( axis_index < 0 )
        {
            initialize_progress_report( &progress, FALSE, sizes[X] * sizes[Y],
                                        "Histogramming" );
        }
        for_less( x, start[X], end[X] )
        {
            for_less( y, start[Y], end[Y] )
            {
                for_less( z, start[Z], end[Z] )
                {
                    {
                        value = get_volume_real_value( volume, x, y, z, 0, 0 );
                        add_to_histogram( &histogram, value );
                    }
                }

                if( axis_index < 0 )
                    update_progress_report( &progress, x * sizes[Y] + y + 1 );
            }
        }

        if( axis_index < 0 )
            terminate_progress_report( &progress );

        nbbins = get_histogram_counts( &histogram, &histo_counts,
        		Default_filter_width, &scale_factor, &trans_factor );

        sum_count = 0;
        for_less( idx, Initial_histogram_low_clip_index, nbbins )
			sum_count += histo_counts[idx];

		count = 0;
		low_limit_done = FALSE;
		high_limit_done = FALSE;
		for_less( idx, Initial_histogram_low_clip_index, nbbins )
		{
			if (!(low_limit_done) && (count / (Real)sum_count > Initial_histogram_low))
			{
				low_limit = idx * histogram.delta + histogram.offset;
				low_limit_done = TRUE;
			}

			if (count / (Real) sum_count >= Initial_histogram_high)
			{
				high_limit = idx * histogram.delta + histogram.offset;
				high_limit_done = TRUE;
				break;
			}
			count += histo_counts[idx];
		}

		if (!low_limit_done)
			low_limit = histogram.min_index * histogram.delta + histogram.offset ;

		if (!high_limit_done)
			high_limit = (histogram.max_index + 1) * histogram.delta + histogram.offset ;
        delete_histogram(&histogram);
    }
    else
    {
    	if (Initial_low_absolute_position >= 0)
			low_limit = Initial_low_absolute_position;
		else
			low_limit = min_value + Initial_low_limit_position * (max_value - min_value);

		if (Initial_high_absolute_position >= 0)
			high_limit = Initial_high_absolute_position;
		else
			high_limit = min_value + Initial_high_limit_position * (max_value - min_value);
    }
    change_colour_coding_range( slice_window, volume_index,
                                low_limit, high_limit );
}

public  Volume  get_nth_label_volume(
    display_struct   *display,
    int              volume_index )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        return( slice_window->slice.volumes[volume_index].labels );
    }
    else
        return( (Volume) NULL );
}

public  Volume  get_label_volume(
    display_struct   *display )
{
    return( get_nth_label_volume( display,
                                  get_current_volume_index(display)) );
}

public  BOOLEAN  label_volume_exists(
    display_struct   *display )
{
    Volume   label;

    label = get_label_volume( display );

    return( is_label_volume_initialized( label ) );
}

public  BOOLEAN  get_label_visibility(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index )
{
    Volume   label;

    label = get_nth_label_volume( slice_window, volume_index );

    return( get_slice_visibility( slice_window, volume_index, view_index ) &&
            slice_window->slice.volumes[volume_index].display_labels &&
            is_label_volume_initialized( label ) );
}

public  int  get_num_labels(
    display_struct   *display,
    int              volume_index )
{
    display_struct   *slice_window;

    if( !get_slice_window( display, &slice_window ) ||
        slice_window->slice.n_volumes == 0 )
        return( Initial_num_labels );
    else
        return( slice_window->slice.volumes[volume_index].n_labels );
}

private  Colour  apply_label_colour(
    display_struct    *slice_window,
    int               volume_index,
    Colour            col,
    int               label )
{
    Real      r1, g1, b1, a1, r2, g2, b2, a2;
    Real      r, g, b, a;
    Colour    label_col;

    if( label != 0 )
    {
        label_col = slice_window->slice.volumes[volume_index].
                                                 label_colour_table[label];

        r1 = get_Colour_r_0_1(col);
        g1 = get_Colour_g_0_1(col);
        b1 = get_Colour_b_0_1(col);
        a1 = get_Colour_a_0_1(col);

        r2 = get_Colour_r_0_1(label_col);
        g2 = get_Colour_g_0_1(label_col);
        b2 = get_Colour_b_0_1(label_col);
        a2 = get_Colour_a_0_1(label_col);

        r = r1 * (1.0 - a2) + a2 * r2;
        g = g1 * (1.0 - a2) + a2 * g2;
        b = b1 * (1.0 - a2) + a2 * b2;
        a = a1 * (1.0 - a2) + a2 * a2;

        col = make_rgba_Colour_0_1( r, g, b, a );
    }

    return( col );
}

private  Colour  get_slice_colour_coding(
    display_struct    *slice_window,
    int               volume_index,
    Real              value,
    int               label )
{
    Colour           col;

    col = get_colour_code( &slice_window->slice.volumes[volume_index].
                           colour_coding, value );

    if( label > 0 )
        col = apply_label_colour( slice_window, volume_index, col, label );

    return( col );
}

private  void  rebuild_colour_table(
    display_struct    *slice_window,
    int               volume_index )
{
    Volume           volume;
    int              voxel;
    Real             value, r, g, b, a, opacity;
    Colour           colour;
    Real             min_voxel, max_voxel;

    volume = get_nth_volume(slice_window,volume_index);

    if( is_an_rgb_volume(volume) )
        return;

    get_volume_voxel_range( volume, &min_voxel, &max_voxel );

    opacity = slice_window->slice.volumes[volume_index].opacity;

    for_inclusive( voxel, (int) min_voxel, (int) max_voxel )
    {
        value = convert_voxel_to_value( volume, (Real) voxel );
        colour = get_colour_code( &slice_window->slice.volumes[volume_index].
                                  colour_coding, value );

        r = get_Colour_r_0_1( colour );
        g = get_Colour_g_0_1( colour );
        b = get_Colour_b_0_1( colour );
        a = get_Colour_a_0_1( colour );

        slice_window->slice.volumes[volume_index].colour_table[voxel] =
                    make_rgba_Colour_0_1( r, g, b, a * opacity );
    }
}

public  void   set_colour_of_label(
    display_struct    *slice_window,
    int               volume_index,
    int               label,
    Colour            colour )
{
    Real  r, g, b;

    if( get_Colour_a(colour) == 255 )
    {
        r = get_Colour_r_0_1( colour );
        g = get_Colour_g_0_1( colour );
        b = get_Colour_b_0_1( colour );

        colour = make_rgba_Colour_0_1( r, g, b,
              slice_window->slice.volumes[volume_index].label_colour_opacity );
    }

    slice_window->slice.volumes[volume_index].label_colour_table[label] =colour;
}

public  Colour   get_colour_of_label(
    display_struct    *slice_window,
    int               volume_index,
    int               label )
{
    return( slice_window->slice.volumes[volume_index].
                                             label_colour_table[label] );
}

public  void   set_volume_opacity(
    display_struct   *slice_window,
    int              volume_index,
    Real             opacity )
{
    slice_window->slice.volumes[volume_index].opacity = opacity;

    colour_coding_has_changed( slice_window, volume_index, UPDATE_BOTH );
}

public  void   set_label_opacity(
    display_struct   *slice_window,
    int              volume_index,
    Real             opacity )
{
    int     i, n_labels;
    Real    r, g, b;
    Colour  *table;

    slice_window->slice.volumes[volume_index].label_colour_opacity = opacity;

    n_labels = get_num_labels(slice_window,volume_index);
    table = slice_window->slice.volumes[volume_index].label_colour_table;

    for_less( i, 1, n_labels )
    {
        r = get_Colour_r_0_1( table[i] );
        g = get_Colour_g_0_1( table[i] );
        b = get_Colour_b_0_1( table[i] );
        table[i] = make_rgba_Colour_0_1( r, g, b, opacity );
    }

    colour_coding_has_changed( slice_window, volume_index, UPDATE_LABELS );
}

public  void  colour_coding_has_changed(
    display_struct    *display,
    int               volume_index,
    Update_types      type )
{
    display_struct    *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        if( type == UPDATE_SLICE || type == UPDATE_BOTH )
        {
            rebuild_colour_table( slice_window, volume_index );

            if( volume_index == get_current_volume_index(slice_window) )
                set_colour_bar_update( slice_window );
        }

        set_slice_window_all_update( slice_window, volume_index, type );
    }
}

public  void  change_colour_coding_range(
    display_struct    *slice_window,
    int               volume_index,
    Real              min_value,
    Real              max_value )
{
    set_colour_coding_min_max( &slice_window->slice.volumes[volume_index].
                              colour_coding,
                               min_value, max_value );

    colour_coding_has_changed( slice_window, volume_index, UPDATE_SLICE );
}

private  void  colour_code_points(
    display_struct        *slice_window,
    int                   continuity,
    Colour_flags          *colour_flag,
    Colour                *colours[],
    int                   n_points,
    Point                 points[] )
{
    int      i, int_voxel[MAX_DIMENSIONS], label, volume_index, view_index;
    Real     val, voxel[MAX_DIMENSIONS];
    Volume   volume, label_volume;
    Colour   colour, volume_colour;

    if( *colour_flag != PER_VERTEX_COLOURS )
    {
        if( n_points > 0 )
        {
            REALLOC( *colours, n_points );
        }
        else
        {
            FREE( *colours );
        }
        *colour_flag = PER_VERTEX_COLOURS;
    }

    for_less( i, 0, n_points )
    {
        colour = make_rgba_Colour( 0, 0, 0, 0 );

        for_less( volume_index, 0, slice_window->slice.n_volumes )
        {
            if( slice_window->slice.volumes[volume_index].opacity == 0.0 )
                continue;

            for_less( view_index, 0, N_SLICE_VIEWS )
            {
                if( get_slice_visibility(slice_window,volume_index,view_index))
                    break;
            }

            if( view_index == N_SLICE_VIEWS )
                continue;

            volume = get_nth_volume( slice_window, volume_index );
            label_volume = get_nth_label_volume( slice_window, volume_index );

            convert_world_to_voxel( volume,
                                    (Real) Point_x(points[i]),
                                    (Real) Point_y(points[i]),
                                    (Real) Point_z(points[i]), voxel );


            if( is_an_rgb_volume( volume ) )
            {
                convert_real_to_int_voxel( get_volume_n_dimensions(volume),
                                           voxel, int_voxel );
                if( int_voxel_is_within_volume( volume, int_voxel ) )
                {
                    volume_colour = (Colour) get_volume_voxel_value(
                                  volume, int_voxel[0], int_voxel[1],
                                  int_voxel[2], 0, 0 );
                }
                else
                    volume_colour = get_colour_coding_under_colour(
                               &slice_window->slice.volumes[volume_index].
                                        colour_coding );
            }
            else
            {
                (void) evaluate_volume( volume, voxel, NULL,
                                        continuity, FALSE,
                                        get_volume_real_min(volume),
                                        &val, NULL, NULL );
            }

            label = 0;

            if( slice_window->slice.volumes[volume_index].display_labels &&
                is_label_volume_initialized( label_volume ) )
            {
                convert_real_to_int_voxel( get_volume_n_dimensions(volume),
                                           voxel, int_voxel );

                if( int_voxel_is_within_volume( volume, int_voxel ) )
                {
                    label = get_voxel_label( slice_window, volume_index,
                                             int_voxel[X], int_voxel[Y],
                                             int_voxel[Z] );
                }
            }

            if( is_an_rgb_volume( volume ) )
            {
                volume_colour = apply_label_colour( slice_window,
                                     volume_index, volume_colour, label );
            }
            else
            {
                volume_colour = get_slice_colour_coding( slice_window,
                                         volume_index, val, label );
            }

            COMPOSITE_COLOURS( colour, volume_colour, colour )
        }

        (*colours)[i] = colour;
    }
}

private  void  colour_code_object_points(
    display_struct         *slice_window,
    int                    continuity,
    object_struct          *object )
{
    Colour                  *colours;
    Colour_flags            colour_flag;
    polygons_struct         *polygons;
    quadmesh_struct         *quadmesh;
    lines_struct            *lines;
    marker_struct           *marker;

    switch( object->object_type )
    {
    case POLYGONS:
        polygons = get_polygons_ptr( object );
        colour_code_points( slice_window, continuity,
                            &polygons->colour_flag, &polygons->colours,
                            polygons->n_points, polygons->points );
        break;

    case QUADMESH:
        quadmesh = get_quadmesh_ptr( object );
        colour_code_points( slice_window, continuity,
                            &quadmesh->colour_flag, &quadmesh->colours,
                            quadmesh->m * quadmesh->n,
                            quadmesh->points );
        break;

    case LINES:
        lines = get_lines_ptr( object );
        colour_code_points( slice_window, continuity,
                            &lines->colour_flag, &lines->colours,
                            lines->n_points, lines->points );
        break;

    case MARKER:
        marker = get_marker_ptr( object );
        colour_flag = PER_VERTEX_COLOURS;
        colours = &marker->colour;
        colour_code_points( slice_window, continuity,
                            &colour_flag, &colours, 1, &marker->position );
        break;
    }
}

public  void  colour_code_an_object(
    display_struct   *display,
    object_struct    *object )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window) )
        colour_code_object_points( slice_window, Volume_continuity, object );
}

public  STRING    get_default_colour_map_suffix( void )
{
    return( DEFAULT_COLOUR_MAP_SUFFIX );
}

public  Status  load_label_colour_map(
    display_struct   *slice_window,
    STRING           filename )
{
    Status   status;
    FILE     *file;
    Colour   col;
    STRING   line;
    int      n_labels, index;

    if( open_file_with_default_suffix( filename,
                                       get_default_colour_map_suffix(),
                                       READ_FILE, ASCII_FORMAT, &file ) != OK )
        return( ERROR );

    n_labels = get_num_labels( slice_window,
                               get_current_volume_index(slice_window) );

    status = OK;
    while( input_int( file, &index ) == OK )
    {
        if( input_line( file, &line ) != OK )
        {
            print_error( "Error loading labels colour map.\n" );
            status = ERROR;
            break;
        }

        col = convert_string_to_colour( line );

        delete_string( line );

        if( index >= 1 && index < n_labels )
        {
            set_colour_of_label( slice_window,
                                 get_current_volume_index(slice_window),
                                 index, col );
        }
    }

    (void) close_file( file );

    return( status );
}

public  Status  save_label_colour_map(
    display_struct   *slice_window,
    STRING           filename )
{
    Status   status;
    FILE     *file;
    Real     red, green, blue;
    Colour   col;
    int      n_labels, index;

    if( open_file_with_default_suffix( filename,
                                       get_default_colour_map_suffix(),
                                       WRITE_FILE, ASCII_FORMAT, &file ) != OK )
        return( ERROR );

    n_labels = get_num_labels( slice_window,
                               get_current_volume_index(slice_window) );

    for_less( index, 1, n_labels )
    {
        col = get_colour_of_label( slice_window,
                                   get_current_volume_index(slice_window),
                                   index );

        red = get_Colour_r_0_1( col );
        green = get_Colour_g_0_1( col );
        blue = get_Colour_b_0_1( col );

        if( output_int( file, index ) != OK ||
            output_real( file, red ) != OK ||
            output_real( file, green ) != OK ||
            output_real( file, blue ) != OK ||
            output_newline( file ) != OK )
        {
            status = ERROR;
            break;
        }
    }

    (void) close_file( file );

    return( status );
}

public  void  clear_labels(
    display_struct   *display,
    int              volume_index )
{
    int    sizes[N_DIMENSIONS];

    get_volume_sizes( get_nth_label_volume(display,volume_index), sizes );

    tell_surface_extraction_label_changed( display, volume_index, 0, 0, 0 );
    tell_surface_extraction_label_changed( display, volume_index,
                                           sizes[X]-1, sizes[Y]-1, sizes[Z]-1 );
    set_all_volume_label_data( get_nth_label_volume(display,volume_index),
                               0 );
}

public  int  get_voxel_label(
    display_struct   *display,
    int              volume_index,
    int              x,
    int              y,
    int              z )
{
    return( get_3D_volume_label_data(
                     get_nth_label_volume(display,volume_index),
                     x, y, z ) );
}


public void update_label_tag(
	    display_struct   *display,
	    int              volume_index,
	    int              x,
	    int              y,
	    int              z,
	    int              label)
{
    Volume            label_volume;
    struct stack_list ** label_stack;
    struct stack_real *top_s;
    object_struct     *object;
    marker_struct     *marker;
    model_struct      *current_model;
    Real              *world_dyn;
    display_struct 	  *marker_window;
	display_struct 	  *three_d_window;
    Real		      world[MAX_DIMENSIONS];
    Real              voxel_real[MAX_DIMENSIONS];
    int			      voxel_int[MAX_DIMENSIONS];
    int			      value;

    marker_window = display->associated[MARKER_WINDOW];
    three_d_window = display->associated[THREE_D_WINDOW];
    label_stack = marker_window->label_stack;
    label_volume = get_nth_label_volume(display, volume_index);

    voxel_real[X] = x;
	voxel_real[Y] = y;
	voxel_real[Z] = z;
    convert_voxel_to_world( label_volume, voxel_real, &world[X], &world[Y], &world[Z] );
	if (label) /* add a voxel to a label region */
	{
		if (label_stack[label] == NULL)
		{
			label_stack[label] = stack_new();

			object = create_object( MARKER );
            marker = get_marker_ptr( object );
            fill_Point( marker->position, world[X], world[Y], world[Z]);
            marker->label = create_string( "" );
            marker->structure_id = -1;
            marker->patient_id = -1;
            marker->size = display->three_d.default_marker_size;
            marker->colour = display->three_d.default_marker_colour;
            marker->type = display->three_d.default_marker_type;

            current_model = get_current_model( three_d_window );
            add_object_to_list(&current_model->n_objects,
					&current_model->objects, object);
			rebuild_selected_list(three_d_window, marker_window);

		}
		ALLOC( world_dyn, MAX_DIMENSIONS );
		world_dyn[X] = world[X];
		world_dyn[Y] = world[Y];
		world_dyn[Z] = world[Z];
		label_stack[label] = push(label_stack[label], world_dyn);
	}

	/* remove a voxel from a label region */
	value = get_3D_volume_label_data(label_volume, x, y, z);
	if (value) /* only on not label voxel */
	{
		top_s = top(label_stack[value]);
		if ((top_s->cur[X] - world[X]) < 1e-10f
				&& (top_s->cur[Y] - world[Y]) < 1e-10f
				&& (top_s->cur[Z] - world[Z]) < 1e-10f)
		{
			do
			{
				top_s = top_s->next;
				pop(label_stack[value]);
				if (top_s == NULL)
				{
					FREE( label_stack[value] );
					label_stack[value] = NULL;
					voxel_int[X] = x;
					voxel_int[Y] = y;
					voxel_int[Z] = z;
					convert_int_to_real_voxel(MAX_DIMENSIONS, voxel_int,
							voxel_real);
					update_current_marker(marker_window, volume_index, voxel_real);
					get_current_object( three_d_window, &object );
					remove_current_object_from_hierarchy(three_d_window, &object);
					delete_object(object);
					rebuild_selected_list(three_d_window, marker_window);
					break;
				}
				convert_world_to_voxel(label_volume, top_s->cur[X],
						top_s->cur[Y], top_s->cur[Z], voxel_real);
				convert_real_to_int_voxel(MAX_DIMENSIONS, voxel_real, voxel_int);

			} while (get_3D_volume_label_data(label_volume, voxel_int[X],
					voxel_int[Y], voxel_int[Z]) != value);
		}
	}
}

public  void  set_voxel_label(
    display_struct   *display,
    int              volume_index,
    int              x,
    int              y,
    int              z,
    int              label )
{

    tell_surface_extraction_label_changed( display, volume_index, x, y, z );
    if( Tags_from_label )
    	update_label_tag(display, volume_index, x, y, z, label);

    set_volume_label_data_5d( get_nth_label_volume(display, volume_index),
    		x, y, z, 0, 0, label );

}

public  Status  load_user_defined_colour_coding(
    display_struct   *slice_window,
    STRING           filename )
{
    Status   status;

    status = input_user_defined_colour_coding( &slice_window->slice.volumes[
                         get_current_volume_index(slice_window)].colour_coding,
                         filename );

    return( status );
}

