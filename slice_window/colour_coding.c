/**
 * \file slice_window/colour_coding.c
 * \brief Functions to handle colour coding and label volumes for the
 * slice window.
 *
 * \copyright
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
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>
#include  <assert.h>

/* Define the minimum and maximum colour table sizes we use. We don't
 * allow colour tables to be much greater than the 16M or so colours
 * possible with standard 24-bit colour depth. However, we also want
 * to guard against using the color table if it is too small, for example,
 * if we are loading an image with voxel values in the 0-1 range.
 */
#define    MAX_COLOUR_TABLE_SIZE    16777216
#define    MIN_COLOUR_TABLE_SIZE    2

#define    DEFAULT_COLOUR_MAP_SUFFIX                    "map"

static  void  rebuild_colour_table(
    display_struct    *slice_window,
    int               volume_index );

/**
 * Returns TRUE if the current volume shares its label volume with another volume.
 */
static  VIO_BOOL  is_shared_label_volume(
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

/**
 * Delete the current volume's labels if they both exist and are not
 * shared with any other volume.
 */
static  void  delete_volume_labels(
    slice_window_struct   *slice,
    int                   volume_index )
{
    if( slice->volumes[volume_index].labels == (VIO_Volume) NULL )
        return;

    if( !is_shared_label_volume( slice, volume_index ) )
        delete_volume( slice->volumes[volume_index].labels );

    slice->volumes[volume_index].labels = NULL;
    if (slice->volumes[volume_index].labels_filename != NULL)
    {
        delete_string( slice->volumes[volume_index].labels_filename );
        slice->volumes[volume_index].labels_filename = NULL;
    }
    FREE( slice->volumes[volume_index].label_colour_table );
}

  void  delete_slice_colour_coding(
    slice_window_struct   *slice,
    int                   volume_index )
{
    VIO_Colour   *ptr;

    delete_colour_coding( &slice->volumes[volume_index].colour_coding );
    delete_volume_labels( slice, volume_index );

    ptr = slice->volumes[volume_index].colour_table;
    if( ptr != NULL )
    {
        ptr += (int) slice->volumes[volume_index].offset;
        FREE( ptr );
        slice->volumes[volume_index].colour_table = NULL;
    }
}

static  void  realloc_label_colour_table(
    display_struct    *slice_window,
    int               volume_index )
{
    int       n_labels, n_colours, n_around, n_up, u, a;
    VIO_Colour    col;
    VIO_Real      r, g, b, hue, sat;

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
                hue = (VIO_Real) a / (VIO_Real) n_around;
                sat = 0.2 + (0.5 - 0.2) * ((VIO_Real) u / (VIO_Real) n_up);

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

/**
 * Checks currently loaded volumes for a label volume that would be
 * considered similar to the new volume, and can therefore be shared
 * with it.
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The selected volume index.
 * \returns The index of a similar volume, or -1 if none found.
 */
static int find_similar_labels(
    display_struct    *slice_window,
    int               volume_index)
{
    int                 i, k, m;
    int                 sizes[VIO_MAX_DIMENSIONS], this_size[VIO_MAX_DIMENSIONS];
    VIO_BOOL             same;
    VIO_Volume              this_volume, volume;
    VIO_General_transform   *this_gen_transform, *gen_transform;
    VIO_Transform           *this_transform, *transform;

    this_volume = get_nth_volume( slice_window, volume_index );
    get_volume_sizes( this_volume, this_size );
    this_gen_transform = get_voxel_to_world_transform( this_volume );
    if( get_transform_type(this_gen_transform) != LINEAR )
        return( -1 );
    this_transform = get_linear_transform_ptr( this_gen_transform );

    for_less( i, 0, get_n_volumes(slice_window) )
    {
        if( i == volume_index )
            continue;

        volume = get_nth_volume( slice_window, i );
        get_volume_sizes( volume, sizes );

        if( sizes[VIO_X] != this_size[VIO_X] ||
            sizes[VIO_Y] != this_size[VIO_Y] ||
            sizes[VIO_Z] != this_size[VIO_Z] )
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
            return( i );
    }
    return( -1 );
}


/**
 * Creates a new, empty label volume to associate with the given volume.
 * Resets the label filename, if one is set.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The selected volume index.
 */
static  void  create_colour_coding(
    display_struct    *slice_window,
    int               volume_index )
{
    int                   orig_index;
    nc_type               type;
    slice_window_struct   *slice;
    int                   n_labels;

    slice = &slice_window->slice;

    n_labels = get_num_labels( slice_window, volume_index );

    if( n_labels <= (1 << 8) )
        type = NC_BYTE;
    else if( n_labels <= (1 << 16) )
        type = NC_SHORT;
    else
        type = NC_LONG;

    if (slice->volumes[volume_index].labels_filename != NULL)
    {
        delete_string( slice->volumes[volume_index].labels_filename );
        slice->volumes[volume_index].labels_filename = NULL;
    }

    if( slice->share_labels_flag &&
        (orig_index = find_similar_labels( slice_window, volume_index )) >= 0 )
    {
        printf("Volume %d is sharing labels with volume %d.\n",
               volume_index, orig_index);
        slice->volumes[volume_index].labels = slice->volumes[orig_index].labels;
        slice->volumes[volume_index].labels_filename =
                    create_string( slice->volumes[orig_index].labels_filename );
    }
    else
    {
        VIO_Volume labels = create_label_volume(
                          get_nth_volume( slice_window, volume_index ), type );

        /* Constrain the label volume to only three dimensions.
         * TODO: Is this really the right thing to do?
         */
        if (get_volume_n_dimensions(labels) > 3)
        {
            set_volume_n_dimensions(labels, 3);
        }

        slice->volumes[volume_index].labels = labels;
        slice->volumes[volume_index].labels_filename = create_string( NULL );

        set_volume_voxel_range( labels, 0.0, n_labels - 1.0 );
        set_volume_real_range( labels, 0.0, n_labels - 1.0 );
    }

    realloc_label_colour_table( slice_window, volume_index );
}

/**
 * Sets the number of labels associated with a given volume. If the
 * labels are shared amongs several volumes, updates each volume to
 * reflect the new labels and ranges.
 *
 * \param slice_window A pointer to the slice windows' display_struct.
 * \param volume_index The index (zero-based) of the volume.
 * \param n_labels The new number of labels.
 */
void set_slice_window_number_labels(
    display_struct    *slice_window,
    int               volume_index,
    int               n_labels )
{
    VIO_Volume          label_volume;
    nc_type             new_type;
    nc_type             old_type;
    VIO_BOOL            signed_flag;
    slice_window_struct *slice;
    int                 i;

    slice = &slice_window->slice;

    label_volume = get_nth_label_volume( slice_window, volume_index );
    if (label_volume == NULL)
    {
        return;
    }

    if( n_labels <= (1 << 8) )
        new_type = NC_BYTE;
    else if( n_labels <= (1 << 16) )
        new_type = NC_SHORT;
    else
        new_type = NC_LONG;

    old_type = get_volume_nc_data_type( label_volume, &signed_flag );

    if (nctypelen(new_type) > nctypelen(old_type))
    {
        VIO_Volume new_label_volume = copy_volume_new_type( label_volume,
                                                            new_type,
                                                            FALSE );
        /* If we can't make a copy of the new label volume,
         * we can't set the new number of labels.
         */
        if ( new_label_volume == NULL )
        {
            return;
        }

        /* Now substitute the new labels for each of the volumes that
         * shared the original label volume.
         */
        for_less( i, 0, slice->n_volumes )
        {
            if (slice->volumes[i].labels == label_volume)
            {
                slice->volumes[i].labels = new_label_volume;
            }
        }

        delete_volume( label_volume );

        label_volume = new_label_volume;
    }

    set_volume_voxel_range( label_volume, 0.0, n_labels - 1.0 );
    set_volume_real_range( label_volume, 0.0, n_labels - 1.0 );

    for_less( i, 0, slice->n_volumes )
    {
        if (slice->volumes[i].labels == label_volume)
        {
            FREE( slice->volumes[i].label_colour_table );

            slice->volumes[i].n_labels = n_labels;

            realloc_label_colour_table( slice_window, i );
        }
    }
}

static  void  alloc_colour_table(
    display_struct    *slice_window,
    int               volume_index )
{
    VIO_Real        min_voxel, max_voxel;
    VIO_Colour      *ptr;
    VIO_Volume      volume = get_nth_volume(slice_window, volume_index);
    VIO_Real        colour_table_size;

    if( is_an_rgb_volume(volume) )
    {
        slice_window->slice.volumes[volume_index].colour_table = NULL;
        return;
    }

    get_volume_voxel_range( volume,  &min_voxel, &max_voxel );

    colour_table_size = max_voxel - min_voxel + 1;
    if ( colour_table_size > MAX_COLOUR_TABLE_SIZE ||
         colour_table_size < MIN_COLOUR_TABLE_SIZE)
    {
        slice_window->slice.volumes[volume_index].colour_table = NULL;
        return;
    }

    ALLOC( ptr, (int) max_voxel - (int) min_voxel + 1 );
    if ( ptr == NULL )
    {
        print_error("Failed to allocate colour table!\n");
        return;
    }

    slice_window->slice.volumes[volume_index].offset = (int) min_voxel;
    slice_window->slice.volumes[volume_index].colour_table =
                                              ptr - (int) min_voxel;
}


/**
 * Sets the initial volume color coding contrast by calculating a
 * histogram over the whole volume. This takes a while for larger
 * volumes, so it should be automatically disabled if the volume
 * is larger than some threshold.
 */
static VIO_BOOL
calculate_contrast_from_histogram(VIO_Volume volume,
                                  VIO_Real *low_limit,
                                  VIO_Real *high_limit)
{
  VIO_Real            min_value, max_value;
  VIO_BOOL            low_limit_done, high_limit_done;
  VIO_Real            delta;
  VIO_Real            *histo_counts;
  int                 bin_count;
  int                 x, y, z;
  int                 sizes[VIO_MAX_DIMENSIONS];
  VIO_Real            sum_count;
  size_t              count;
  int                 idx;
  histogram_struct    histogram;
  VIO_progress_struct progress;
  VIO_Real            scale_factor, trans_factor;
  size_t              n_voxels;

  get_volume_real_range( volume, &min_value, &max_value );
  get_volume_sizes( volume, sizes );

  /*
   * Check the number of voxels in the volume. Don't bother to perform
   * this elaborate histogram-based calculation for very large volumes.
   */
  n_voxels = sizes[0];
  for (idx = 1; idx < get_volume_n_dimensions(volume); idx++)
  {
    n_voxels *= sizes[idx];
  }
  if (n_voxels > 500000000)
  {
    return FALSE;
  }

  delta = fabs( (max_value - min_value) / 1000.0 );

  if (delta < 1e-6)
    delta = 1e-6;

  initialize_histogram( &histogram, delta, min_value );

  initialize_progress_report( &progress, FALSE, sizes[VIO_X] * sizes[VIO_Y],
                              "Histogramming" );

  for_less( x, 0, sizes[VIO_X] )
  {
    for_less( y, 0, sizes[VIO_Y] )
    {
      for_less( z, 0, sizes[VIO_Z] )
      {
        add_to_histogram( &histogram,
                          get_volume_real_value( volume, x, y, z, 0, 0 ) );
      }
    }
    update_progress_report( &progress, x * sizes[VIO_Y] + y + 1 );
  }

  terminate_progress_report( &progress );

  bin_count = get_histogram_counts( &histogram, &histo_counts,
                                    Default_filter_width,
                                    &scale_factor, &trans_factor );

  sum_count = 0.0;
  for_less( idx, Initial_histogram_low_clip_index, bin_count )
    sum_count += (VIO_Real) histo_counts[idx];

  count = 0;
  low_limit_done = FALSE;
  high_limit_done = FALSE;
  for_less( idx, Initial_histogram_low_clip_index, bin_count )
  {
    if (!low_limit_done && (count / sum_count > Initial_histogram_low))
    {
      *low_limit = idx * histogram.delta + histogram.offset;
      low_limit_done = TRUE;
    }

    if (count / sum_count >= Initial_histogram_high)
    {
      *high_limit = idx * histogram.delta + histogram.offset;
      high_limit_done = TRUE;
      break;
    }
    count += histo_counts[idx];
  }

  FREE(histo_counts);

  if (!low_limit_done)
    *low_limit = histogram.min_index * histogram.delta + histogram.offset;

  if (!high_limit_done)
    *high_limit = (histogram.max_index + 1) * histogram.delta + histogram.offset;
  /**
   * Correct lower and upper limits here in case of an extremely skewed
   * distribution. This helps do a better job of displaying images with
   * a white background, such as the BigBrain.
   */
  if (*low_limit > (max_value - min_value) / 2.0)
  {
    *low_limit = min_value + (max_value - min_value) * Initial_histogram_low;
  }

  delete_histogram(&histogram);
  return TRUE;
}

/**
 * Simple way to compute the initial contrast, without
 * going to all the trouble to calculate a histogram.
 */
static void
calculate_contrast_from_range(VIO_Volume volume,
                              VIO_Real *low_limit,
                              VIO_Real *high_limit)
{
  VIO_Real min_value, max_value;

  get_volume_real_range( volume, &min_value, &max_value );

  if (Initial_coding_range_absolute)
  {
      *low_limit = Initial_coding_range_low;
      *high_limit = Initial_coding_range_high;
  }
  else
  {
      *low_limit = min_value + Initial_coding_range_low * (max_value - min_value);
      *high_limit = min_value + Initial_coding_range_high * (max_value - min_value);
  }
}

/**
 * Set up the colour coding parameters for the slice view. Allocates and
 * rebuilds the "colour table" that provides rapid access to the mapping
 * from voxel values to colours. The colour table is derived from the
 * abstract colour map.
 */
void  initialize_slice_colour_coding(
    display_struct    *slice_window,
    int               volume_index )
{
    VIO_Real           low_limit, high_limit;
    Colour_coding_types colour_coding_type = Current_colour_coding_type;
    loaded_volume_struct *loaded_volume_ptr;
    VIO_Volume         volume;

    /* For volumes after the first, adopt a different color coding
     * scheme than the default.
     */
    if (volume_index > 0 &&
        colour_coding_type == (Colour_coding_types) Initial_colour_coding_type)
    {
        colour_coding_type = SPECTRAL;
    }

    loaded_volume_ptr = &slice_window->slice.volumes[volume_index];

    initialize_colour_coding(&loaded_volume_ptr->colour_coding,
                             colour_coding_type,
                             Colour_below, Colour_above, 0.0, 1.0 );

    loaded_volume_ptr->label_colour_opacity = Label_colour_opacity;
    loaded_volume_ptr->n_labels = Initial_num_labels;
    loaded_volume_ptr->offset = 0;
    loaded_volume_ptr->colour_table = NULL;
    loaded_volume_ptr->label_colour_table = NULL;
    loaded_volume_ptr->labels = NULL;
    loaded_volume_ptr->labels_filename = create_string( NULL );

    alloc_colour_table( slice_window, volume_index );
    rebuild_colour_table( slice_window, volume_index );
    create_colour_coding( slice_window, volume_index );

    volume = get_nth_volume(slice_window, volume_index);

    if ( !Initial_histogram_contrast ||
         !calculate_contrast_from_histogram(volume, &low_limit, &high_limit))
    {
        calculate_contrast_from_range(volume, &low_limit, &high_limit);
    }

    change_colour_coding_range( slice_window, volume_index,
                                low_limit, high_limit );
}

  VIO_Volume  get_nth_label_volume(
    display_struct   *display,
    int              volume_index )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        return( slice_window->slice.volumes[volume_index].labels );
    }
    else
        return( (VIO_Volume) NULL );
}

  VIO_Volume  get_label_volume(
    display_struct   *display )
{
    return( get_nth_label_volume( display,
                                  get_current_volume_index(display)) );
}

  VIO_BOOL  label_volume_exists(
    display_struct   *display )
{
    VIO_Volume   label;

    label = get_label_volume( display );

    return( is_label_volume_initialized( label ) );
}

  VIO_BOOL  get_label_visibility(
    display_struct    *slice_window,
    int               volume_index,
    int               view_index )
{
    VIO_Volume   label;

    label = get_nth_label_volume( slice_window, volume_index );

    return( get_slice_visibility( slice_window, volume_index, view_index ) &&
            slice_window->slice.volumes[volume_index].display_labels &&
            is_label_volume_initialized( label ) );
}

  int  get_num_labels(
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

static  VIO_Colour  apply_label_colour(
    display_struct    *slice_window,
    int               volume_index,
    VIO_Colour            col,
    int               label )
{
    VIO_Real      r1, g1, b1, a1, r2, g2, b2, a2;
    VIO_Real      r, g, b, a;
    VIO_Colour    label_col;

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

static  VIO_Colour  get_slice_colour_coding(
    display_struct    *slice_window,
    int               volume_index,
    VIO_Real              value,
    int               label )
{
    VIO_Colour           col;

    col = get_colour_code( &slice_window->slice.volumes[volume_index].
                           colour_coding, value );

    if( label > 0 )
        col = apply_label_colour( slice_window, volume_index, col, label );

    return( col );
}

/**
 * For each value in the voxel range of the data, create an entry in the
 * colour table corresponding to the appropriate colour for that value.
 * The resulting colour table is used in render_slice_to_pixels() to
 * provide fast access to the colour map.
 */
static  void  rebuild_colour_table(
    display_struct    *slice_window,
    int               volume_index )
{
    VIO_Volume       volume;
    int              voxel;
    VIO_Real         value, r, g, b, a, opacity;
    VIO_Colour       colour;
    VIO_Real         min_voxel, max_voxel;

    volume = get_nth_volume(slice_window,volume_index);

    if( is_an_rgb_volume(volume) ||
        slice_window->slice.volumes[volume_index].colour_table == NULL)
        return;

    get_volume_voxel_range( volume, &min_voxel, &max_voxel );

    opacity = slice_window->slice.volumes[volume_index].opacity;

    for_inclusive( voxel, (int) min_voxel, (int) max_voxel )
    {
        value = convert_voxel_to_value( volume, (VIO_Real) voxel );
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

  void   set_colour_of_label(
    display_struct    *slice_window,
    int               volume_index,
    int               label,
    VIO_Colour            colour )
{
    VIO_Real  r, g, b;

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

  VIO_Colour   get_colour_of_label(
    display_struct    *slice_window,
    int               volume_index,
    int               label )
{
    return( slice_window->slice.volumes[volume_index].
                                             label_colour_table[label] );
}

  void   set_volume_opacity(
    display_struct   *slice_window,
    int              volume_index,
    VIO_Real             opacity )
{
    slice_window->slice.volumes[volume_index].opacity = opacity;

    colour_coding_has_changed( slice_window, volume_index, UPDATE_BOTH );
}

  void   set_label_opacity(
    display_struct   *slice_window,
    int              volume_index,
    VIO_Real             opacity )
{
    int     i, n_labels;
    VIO_Real    r, g, b;
    VIO_Colour  *table;

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

  void  colour_coding_has_changed(
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

  void  change_colour_coding_range(
    display_struct    *slice_window,
    int               volume_index,
    VIO_Real              min_value,
    VIO_Real              max_value )
{
    set_colour_coding_min_max( &slice_window->slice.volumes[volume_index].
                              colour_coding,
                               min_value, max_value );

    colour_coding_has_changed( slice_window, volume_index, UPDATE_SLICE );
}

/**
 * Colour code each of the points associated with an object, by copying
 * the encoded colours from an associated volume.
 */
static  void  colour_code_points(
    display_struct        *slice_window,
    int                   continuity,
    Colour_flags          *colour_flag,
    VIO_Colour            *colours[],
    int                   n_points,
    VIO_Point             points[] )
{
    int      i, int_voxel[VIO_MAX_DIMENSIONS], label, volume_index, view_index;
    VIO_Real     val, voxel[VIO_MAX_DIMENSIONS];
    VIO_Volume   volume, label_volume;
    VIO_Colour   colour, volume_colour;

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
                                    (VIO_Real) Point_x(points[i]),
                                    (VIO_Real) Point_y(points[i]),
                                    (VIO_Real) Point_z(points[i]), voxel );


            if( is_an_rgb_volume( volume ) )
            {
                convert_real_to_int_voxel( get_volume_n_dimensions(volume),
                                           voxel, int_voxel );
                if( int_voxel_is_within_volume( volume, int_voxel ) )
                {
                    volume_colour = (VIO_Colour) get_volume_voxel_value(
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
                                             int_voxel[VIO_X], int_voxel[VIO_Y],
                                             int_voxel[VIO_Z] );
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

static  void  colour_code_object_points(
    display_struct         *slice_window,
    int                    continuity,
    object_struct          *object )
{
    VIO_Colour              *colours;
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

    default:
        /* Do not apply colour coding to a MODEL, PIXELS,
         * or TEXT object.
         */
        break;
    }
}

  void  colour_code_an_object(
    display_struct   *display,
    object_struct    *object )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window) )
        colour_code_object_points( slice_window, Volume_continuity, object );
}

  VIO_STR    get_default_colour_map_suffix( void )
{
    return( DEFAULT_COLOUR_MAP_SUFFIX );
}

  VIO_Status  load_label_colour_map(
    display_struct   *slice_window,
    VIO_STR           filename )
{
    VIO_Status   status;
    FILE     *file;
    VIO_Colour   col;
    VIO_STR   line;
    int      n_labels, index;

    if( open_file_with_default_suffix( filename,
                                       get_default_colour_map_suffix(),
                                       READ_FILE, ASCII_FORMAT, &file ) != VIO_OK )
        return( VIO_ERROR );

    n_labels = get_num_labels( slice_window,
                               get_current_volume_index(slice_window) );

    status = VIO_OK;
    while( input_int( file, &index ) == VIO_OK )
    {
        if( input_line( file, &line ) != VIO_OK )
        {
            print_error( "Error loading labels colour map.\n" );
            status = VIO_ERROR;
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

  VIO_Status  save_label_colour_map(
    display_struct   *slice_window,
    VIO_STR           filename )
{
    VIO_Status   status;
    FILE     *file;
    VIO_Real     red, green, blue;
    VIO_Colour   col;
    int      n_labels, index;

    if( open_file_with_default_suffix( filename,
                                       get_default_colour_map_suffix(),
                                       WRITE_FILE, ASCII_FORMAT, &file ) != VIO_OK )
        return( VIO_ERROR );

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

        if( output_int( file, index ) != VIO_OK ||
            output_real( file, red ) != VIO_OK ||
            output_real( file, green ) != VIO_OK ||
            output_real( file, blue ) != VIO_OK ||
            output_newline( file ) != VIO_OK )
        {
            status = VIO_ERROR;
            break;
        }
    }

    (void) close_file( file );

    return( status );
}

  void  clear_labels(
    display_struct   *display,
    int              volume_index )
{
    int    sizes[VIO_MAX_DIMENSIONS];

    get_volume_sizes( get_nth_label_volume(display,volume_index), sizes );

    tell_surface_extraction_label_changed( display, volume_index, 0, 0, 0 );
    tell_surface_extraction_label_changed( display, volume_index,
                                           sizes[VIO_X]-1, sizes[VIO_Y]-1, sizes[VIO_Z]-1 );
    set_all_volume_label_data( get_nth_label_volume(display,volume_index),
                               0 );
}

  int  get_voxel_label(
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


/**
 * If we are auto-generating tags from the labels, this function will
 * update the tags list to reflect the new labels. It is therefore called
 * after each label operation.
 * \param slice_window A pointer to the display_struct for the slice window.
 * \param volume_index The desired volume index.
 * \param x The voxel x coordinate
 * \param y The voxel y coordinate
 * \param z The voxel z coordinate
 * \param label The newly applied label.
 */
void update_label_tag(
        display_struct   *slice_window,
        int              volume_index,
        int              x,
        int              y,
        int              z,
        int              label)
{
    VIO_Volume        label_volume;
    struct stack_list ** label_stack;
    object_struct     *object;
    marker_struct     *marker;
    model_struct      *current_model;
    VIO_Real          *world_dyn;
    display_struct    *marker_window;
    display_struct    *three_d_window;
    VIO_Real          world[VIO_MAX_DIMENSIONS];
    VIO_Real          voxel_real[VIO_MAX_DIMENSIONS];
    int               value;
    int               n_labels;
    loaded_volume_struct *volume_ptr;

    marker_window = slice_window->associated[MARKER_WINDOW];
    three_d_window = slice_window->associated[THREE_D_WINDOW];
    label_volume = get_nth_label_volume(slice_window, volume_index);
    n_labels = get_num_labels( slice_window, volume_index );
    volume_ptr = &slice_window->slice.volumes[volume_index];
    label_stack = volume_ptr->label_stack;

    voxel_real[VIO_X] = x;
    voxel_real[VIO_Y] = y;
    voxel_real[VIO_Z] = z;
    convert_voxel_to_world( label_volume, voxel_real,
                            &world[VIO_X], &world[VIO_Y], &world[VIO_Z] );
    if ( label > 0 && label < n_labels ) /* add a voxel to a label region */
    {
        volume_ptr->label_count[label]++;
        if (label_stack[label] == NULL)
        {
            label_stack[label] = stack_new();

            object = create_object( MARKER );
            marker = get_marker_ptr( object );
            fill_Point( marker->position, world[VIO_X], world[VIO_Y], world[VIO_Z]);
            marker->label = create_string( "" );
            marker->structure_id = label;
            marker->patient_id = -1;
            marker->size = three_d_window->three_d.default_marker_size;
            marker->colour = get_colour_of_label( slice_window, volume_index,
                                                  label );
            marker->type = three_d_window->three_d.default_marker_type;

            current_model = get_current_model( three_d_window );
            add_object_to_list(&current_model->n_objects,
                               &current_model->objects, object);
            rebuild_selected_list(three_d_window, marker_window);
        }
        ALLOC( world_dyn, VIO_N_DIMENSIONS );
        world_dyn[VIO_X] = x;
        world_dyn[VIO_Y] = y;
        world_dyn[VIO_Z] = z;
        label_stack[label] = push(label_stack[label], world_dyn);
    }

    /* Get the current value of this voxel, if any. If we
     * are about to change the value, we want to update the
     * stack of the old label appropriately.
     */
    value = get_3D_volume_label_data(label_volume, x, y, z);

    if (value != label && value > 0 && value < n_labels )
    {
        struct stack_real *top_s;
        struct stack_real *prv_s;

        if (volume_ptr->label_count[value] != 0)
          volume_ptr->label_count[value]--;

        for (prv_s = NULL, top_s = top(label_stack[value]);
             top_s != NULL;
             prv_s = top_s, top_s = top_s->next)
        {
            if (fabs(top_s->cur[VIO_X] - x) < 1e-10 &&
                fabs(top_s->cur[VIO_Y] - y) < 1e-10 &&
                fabs(top_s->cur[VIO_Z] - z) < 1e-10)
            {
                if (top_s == label_stack[value]->head)
                {
                    label_stack[value]->head = top_s->next;
                }
                else
                {
                    prv_s->next = top_s->next;
                }
                FREE(top_s->cur);
                FREE(top_s);
                break;
            }
        }

        if (volume_ptr->label_count[value] == 0)
        {
            FREE( label_stack[value] );
            label_stack[value] = NULL;

            update_current_marker(three_d_window, volume_index, voxel_real);
            get_current_object( three_d_window, &object );
            remove_current_object_from_hierarchy(three_d_window, &object);
            delete_object(object);
            rebuild_selected_list(three_d_window, marker_window);
        }
    }
}

/**
 * Set the label associated with the given voxel coordinates and the
 * specified volume.
 *
 * \param display A pointer to the display_struct for the slice view window.
 * \param volume_index The zero-based index of the volume.
 * \param x The voxel x coordinate
 * \param y The voxel y coordinate
 * \param z The voxel z coordinate
 * \param label The desired label value.
 */
void  set_voxel_label(
    display_struct   *display,
    int              volume_index,
    int              x,
    int              y,
    int              z,
    int              label )
{
    assert( display->window_type == SLICE_WINDOW );

    tell_surface_extraction_label_changed( display, volume_index, x, y, z );
    if( Tags_from_label )
        update_label_tag(display, volume_index, x, y, z, label);

    set_volume_label_data_5d( get_nth_label_volume(display, volume_index),
            x, y, z, 0, 0, label );

}

  VIO_Status  load_user_defined_colour_coding(
    display_struct   *slice_window,
    VIO_STR           filename )
{
    VIO_Status   status;

    status = input_user_defined_colour_coding( &slice_window->slice.volumes[
                         get_current_volume_index(slice_window)].colour_coding,
                         filename );

    return( status );
}
