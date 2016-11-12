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

/**
 * Define the maximum colour table sizes we use. We don't
 * allow colour tables to be much greater than the 16M or so colours
 * possible with standard 24-bit colour depth.
 */
#define    MAX_COLOUR_TABLE_SIZE    16777216

/**
 * Define the default extension used by label colour map files.
 */
#define    DEFAULT_COLOUR_MAP_SUFFIX                    "map"

static  void  rebuild_colour_table(
    display_struct    *slice_window,
    int               volume_index );

/**
 * Returns TRUE if the current volume shares its label volume with another volume.
 * \param slice A pointer to the slice_window_struct from the slice window's
 * display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \returns TRUE if the labels of this volume are shared with another volume.
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
 * Delete the current volume's labels if they labels exist and are not
 * shared with any other volume.
 *
 * \param slice A pointer to the slice_window_struct from the slice window's
 * display_struct.
 * \param volume_index The zero-based index of the volume that is being
 * removed.
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

/**
 * Deletes all of the colour coding and labeling information associated
 * with a particular volume. Used when a volume is being deleted, or when
 * the program is exiting.
 *
 * \param slice A pointer to the slice_window_struct from the slice window's
 * display_struct.
 * \param volume_index The zero-based index of the volume that is being
 * removed.
 */
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
        ptr += slice->volumes[volume_index].colour_offset;
        FREE( ptr );
        slice->volumes[volume_index].colour_table = NULL;
    }
}

/**
 * Label colour table historically used with Display, which repeats itself
 * after the first 15 or so colours.
 */
static void
classic_colour_table( display_struct *slice_window, int volume_index,
                      int n_labels )
{
    int        n_around, n_up, u, a;
    VIO_Colour col;
    VIO_Real   r, g, b, hue, sat;
    int        n_colours = 0;

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
 * Label colour table using "standard" colours adapted from other tools
 * used at the BIC.
 */
static void
standard_colour_table( display_struct *slice_window, int volume_index,
                       int n_labels )
{
  /* Adapted from code provided by Claude Lepage, 2016-07-08.
   */
  static  float label_points[][3] = {
    {1,  1,  1 },
    {0,  1,  0 },
    {0,  0,  1 },
    {1,  0,  0 },
    {0,  0.96552,  1 },
    {1,  0.48276,  0.86207 },
    {1,  0.86207,  0.48276 },
    {0,  0.55172,  1 },
    {0,  0.41379,  0.034483 },
    {0.62069,  0.27586,  0.2069 },
    {0.24138,  0,  0.41379 },
    {1,  0.93103,  1 },
    {0.55172,  1,  0.62069 },
    {0.89655,  0,  1 },
    {0.31034,  0.48276,  0.55172 },
    {0.86207,  1,  0 },
    {1,  0,  0.41379 },
    {0.93103,  0.51724,  0 },
    {0.48276,  0.34483,  0.96552 },
    {0.55172,  0,  0.34483 },
    {0.48276,  0.41379,  0 },
    {0.62069,  0.72414,  0.55172 },
    {0.55172,  0.41379,  0.68966 },
    {0.37931,  0.72414,  0 },
    {1,  0.62069,  0.68966 },
    {0,  0.17241,  0.37931 },
    {0.24138,  0.75862,  1 },
    {0.24138,  0.31034,  0.17241 },
    {0.65517,  0.034483,  0.65517 },
    {0.37931,  0.24138,  0.31034 },
    {0,  0.72414,  0.55172 },
    {1,  0.82759,  0 },
    {1,  0.41379,  0.31034 },
    {0,  0,  0.62069 },
    {0.27586,  0,  0 },
    {0.93103,  0.72414,  1 },
    {0.7931,  0.58621,  0.41379 },
    {1,  0,  0.65517 },
    {0.86207,  1,  0.48276 },
    {0.17241,  0.27586,  0.68966 },
    {0.62069,  0.65517,  0 },
    {0.58621,  0.82759,  0.86207 },
    {0.68966,  0.58621,  0.65517 },
    {0.7931,  0.51724,  1 },
    {0,  1,  0.44828 },
    {0.65517,  0,  0 },
    {0.51724,  0,  0.89655 },
    {0.17241,  0,  0.17241 },
    {1,  1,  0.7931 },
    {0.89655,  0.31034,  0.55172 },
    {0.68966,  1,  0.82759 },
    {0,  0.68966,  0.31034 },
    {0.34483,  0.2069,  0 },
    {0,  0.41379,  0.65517 },
    {0.58621,  0.65517,  0.89655 },
    {0.55172,  0.75862,  0.37931 },
    {0.13793,  0.48276,  0.37931 },
    {0.68966,  0.37931,  0.51724 },
    {0.68966,  0,  0.24138 },
    {0.65517,  0.34483,  0 },
    {1,  0.24138,  0.89655 },
    {0,  0.24138,  0.27586 },
    {0.58621,  1,  0.24138 },
    {0.51724,  0.41379,  0.34483 },
    {0,  1,  0.7931 },
    {0.7931,  0.58621,  0.17241 },
    {1,  0,  0.24138 },
    {0.41379,  0.51724,  0.24138 },
    {0,  0.24138,  1 },
    {0,  0.65517,  0.68966 },
    {0.7931,  0.34483,  1 },
    {1,  0.7931,  0.72414 },
    {0.41379,  0.17241,  0.41379 },
    {1,  0.62069,  0.37931 },
    {0.51724,  0.27586,  0.72414 },
    {0.68966,  0.27586,  0.62069 },
    {0.58621,  0.58621,  1 },
    {1,  0.41379,  0.48276 },
    {0,  0.41379,  1 },
    {0.44828,  0.034483,  0.68966 },
    {0.68966,  0.65517,  0.31034 },
    {0.89655,  0.31034,  0 },
    {0,  0,  0.27586 },
    {0.2069,  0.13793,  0.068966 },
    {0.34483,  0.55172,  0 },
    {0,  0.13793,  0.034483 },
    {0.10345,  0.82759,  0.068966 },
    {0.48276,  0.17241,  0.24138 },
    {0.34483,  0.34483,  0.51724 },
    {0.93103,  0.93103,  0.27586 },
    {0,  0.24138,  0 },
    {0.82759,  0.89655,  0.82759 },
    {0.44828,  0,  0 },
    {0.7931,  0.96552,  0.65517 },
    {0.65517,  0.82759,  0.034483 },
    {0.96552,  0.55172,  0.48276 },
    {0.10345,  0.55172,  0.31034 },
    {0.44828,  0.55172,  0.48276 },
    {0.75862,  0,  0.51724 },
    {0,  0.068966,  0.17241 },
    {0.41379,  0.86207,  0.7931 },
    {0.27586,  0.31034,  0 },
    {0.58621,  0.37931,  0.17241 },
    {0.72414,  0.44828,  0.41379 },
    {0,  0.10345,  0.55172 },
    {0,  1,  0.62069 },
    {1,  0.68966,  0.034483 },
    {0.48276,  0.7931,  0.55172 },
    {0.48276,  0.93103,  0.37931 },
    {0.31034,  0.34483,  0.34483 },
    {0.86207,  0.51724,  0.75862 },
    {0.44828,  0,  0.44828 },
    {0.82759,  0.75862,  0.55172 },
    {0.72414,  0.75862,  0.86207 },
    {0.31034,  0.37931,  0.68966 },
    {0.34483,  0,  0.2069 },
    {0.75862,  0.31034,  0.37931 },
    {0.27586,  0.2069,  0.48276 },
    {0.86207,  0.75862,  0.2069 },
    {0.72414,  0.65517,  0.58621 },
    {0.86207,  0.37931,  0.89655 },
    {0.55172,  0.48276,  0.27586 },
    {0,  0.55172,  0.72414 },
    {0.034483,  0.24138,  0.82759 },
    {0.44828,  0.41379,  0.89655 },
    {1,  0.75862,  0.89655 },
    {0.27586,  0.58621,  0.89655 },
    {0,  0.58621,  0 },
    {0.17241,  0.034483,  0.068966 },
    {0.75862,  0.31034,  0.17241 },
    {1,  0.75862,  0.51724 },
    {0.17241,  0.17241,  0.2069 },
    {0.2069,  0.86207,  1 },
    {0.82759,  0.2069,  0.24138 },
    {0.55172,  0.41379,  0.55172 },
    {0.41379,  0.68966,  0.58621 },
    {0.13793,  0,  0.7931 },
    {0.82759,  0.86207,  0.48276 },
    {0.48276,  0.2069,  0 },
    {0.65517,  0,  0.82759 },
    {0.068966,  0.31034,  0.44828 },
    {0.034483,  0.31034,  0.24138 },
    {0.96552,  0.62069,  1 },
    {0.44828,  0.68966,  0.82759 },
    {0.41379,  0.24138,  0.2069 },
    {0.2069,  0.41379,  0.2069 },
    {0.86207,  0.24138,  0.72414 },
    {0,  0.7931,  0.27586 },
    {0.24138,  0.17241,  0.31034 },
    {0.96552,  0.034483,  0.51724 },
    {0.72414,  0.58621,  0.7931 },
    {0.86207,  0.068966,  0.068966 },
    {0.68966,  1,  0.96552 },
    {0.24138,  0.86207,  0.58621 },
    {0.65517,  0.7931,  0.27586 },
    {0.51724,  0.55172,  0.68966 },
    {0.34483,  0.2069,  0.72414 },
    {0.75862,  0.58621,  0.93103 },
    {0.37931,  0.44828,  0 },
    {1,  0.51724,  0.72414 },
    {0.72414,  0,  0.37931 },
    {0.65517,  0.93103,  0.48276 },
    {1,  0.96552,  0 },
    {1,  0.75862,  0.31034 },
    {0.86207,  0.41379,  0.13793 },
    {0.48276,  0,  0.13793 },
    {0.75862,  1,  0.34483 },
    {0.62069,  0.72414,  0.68966 },
    {0.72414,  0.27586,  0.51724 },
    {0.7931,  0.48276,  0.27586 },
    {0.68966,  0.41379,  0.7931 },
    {0.31034,  0.58621,  0.24138 },
    {0,  0.24138,  0.55172 },
    {0.34483,  0.17241,  0.82759 },
    {0.96552,  0.31034,  0.68966 },
    {0.62069,  0.55172,  0.034483 },
    {0.48276,  0.48276,  0.51724 },
    {0.51724,  0.2069,  0.37931 },
    {0.41379,  0.62069,  0.41379 },
    {0.58621,  0.37931,  0.41379 },
    {0.41379,  0.7931,  0.34483 },
    {0.82759,  0.62069,  0.62069 },
    {0.62069,  0.34483,  0.58621 },
    {0.10345,  0.13793,  0.48276 },
    {0.24138,  0.034483,  0.31034 },
    {0.86207,  0.75862,  0.7931 },
    {0.58621,  0.24138,  0.93103 },
    {0.65517,  0.37931,  0.93103 },
    {0.37931,  0.31034,  0.17241 },
    {0,  0.41379,  0.41379 },
    {1,  0.96552,  0.48276 },
    {0.34483,  1,  0.89655 },
    {0.82759,  0.93103,  1 },
    {0.37931,  0.13793,  1 },
    {0.41379,  0.034483,  0.55172 },
    {0.51724,  0.24138,  0.58621 },
    {0.41379,  0.44828,  0.31034 },
    {0.51724,  1,  0.75862 },
    {0.62069,  0,  0.51724 },
    {0.37931,  0.62069,  0.62069 },
    {0.7931,  0,  0.7931 },
    {0.13793,  0.41379,  0.86207 },
    {0.13793,  0.17241,  0 },
    {0.86207,  0.37931,  0.34483 },
    {0.44828,  0.48276,  0.72414 },
    {0.58621,  0.068966,  0.27586 },
    {0.82759,  0.7931,  1 },
    {0.72414,  0,  1 },
    {0,  0.62069,  0.55172 },
    {0.82759,  0.58621,  0.72414 },
    {0.17241,  0.34483,  0 },
    {0.37931,  0.13793,  0.034483 },
    {0.48276,  0.37931,  0.75862 },
    {1,  0.68966,  0.58621 },
    {0.51724,  0.62069,  0.24138 },
    {0.10345,  0.068966,  0.24138 },
    {0.65517,  0.82759,  1 },
    {0.37931,  1,  0.31034 },
    {0.96552,  0.62069,  0.24138 },
    {0,  0.68966,  0.82759 },
    {0.24138,  0,  0.55172 },
    {0.86207,  0.44828,  0.55172 },
    {0.34483,  0.2069,  0.58621 },
    {0,  0.44828,  0.7931 },
    {0.65517,  0.51724,  0.2069 },
    {0.27586,  0.10345,  0.2069 },
    {0.27586,  0.41379,  0.34483 },
    {0.89655,  0.2069,  0.41379 },
    {0.10345,  0,  0.10345 },
    {0.62069,  0,  0.10345 },
    {1,  0.27586,  0.37931 },
    {0.62069,  0.68966,  0.41379 },
    {0.62069,  0.86207,  0.75862 },
    {0.41379,  0.27586,  0.44828 },
    {0,  0.51724,  0.58621 },
    {0.82759,  0.96552,  0.7931 },
    {0.58621,  0.17241,  0 },
    {0.96552,  0.89655,  0.7931 },
    {0.58621,  0.86207,  0.27586 },
    {0.31034,  0.48276,  0.65517 },
    {0.68966,  0.96552,  0 },
    {0.65517,  0.62069,  0.44828 },
    {0.93103,  0.27586,  1 },
    {0.65517,  0.24138,  0.7931 },
    {0.96552,  0.89655,  0.62069 },
    {0.34483,  0.93103,  0.48276 },
    {0.44828,  0.89655,  0.93103 },
    {0,  0,  0.37931 },
    {0.7931,  0.7931,  0.31034 },
    {0.17241,  0.2069,  0.13793 },
    {0.82759,  0.24138,  0.13793 },
    {0.82759,  0.65517,  0.34483 },
    {0.37931,  0.86207,  0.68966 },
    {1,  0,  0.75862 },
    {0.7931,  0.86207,  0.62069 },
    {0.41379,  0.51724,  0.86207 }
  };
  int i;

#define N_COLOURS_STD (sizeof(label_points) / sizeof(label_points[0]))

  set_colour_of_label( slice_window, volume_index, 0,
                       make_rgba_Colour( 0, 0, 0, 0 ) );
  for (i = 1; i < n_labels; i++)
  {
    float *col = label_points[i % N_COLOURS_STD];
    set_colour_of_label( slice_window, volume_index, i,
                         make_Colour_0_1( col[0], col[1], col[2] ) );
  }
}

/**
 * Label colour table using up to several thousand visually-distinct
 * colours.
 */
static void
distinct_colour_table( display_struct *slice_window, int volume_index,
                       int n_labels )
{
  int i;
  VIO_Colour *colours;

  ALLOC( colours, n_labels - 1 );

  distinct_colours( n_labels - 1, GREY, colours );

  set_colour_of_label( slice_window, volume_index, 0,
                       make_rgba_Colour( 0, 0, 0, 0 ) );

  for (i = 1; i < n_labels; i++)
  {
    set_colour_of_label( slice_window, volume_index, i, colours[i-1]);
  }

  FREE( colours );
}

static  void  realloc_label_colour_table(
    display_struct    *slice_window,
    int               volume_index )
{

    int n_labels = get_num_labels( slice_window, volume_index );
    ALLOC( slice_window->slice.volumes[volume_index].label_colour_table,
           n_labels );

    switch ( Initial_label_colour_table )
    {
    case 1:
      standard_colour_table( slice_window, volume_index, n_labels );
      break;
    case 2:
      distinct_colour_table( slice_window, volume_index, n_labels );
      break;
    default:
      classic_colour_table( slice_window, volume_index, n_labels );
      break;
    }
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
        print("Volume %d is sharing labels with volume %d.\n",
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
    VIO_Data_types  data_type;

    if( is_an_rgb_volume(volume) )
    {
        slice_window->slice.volumes[volume_index].colour_table = NULL;
        return;
    }

    data_type = get_volume_data_type( volume );
    get_volume_voxel_range( volume,  &min_voxel, &max_voxel );

    colour_table_size = max_voxel - min_voxel + 1;
    if ( data_type == VIO_FLOAT || data_type == VIO_DOUBLE ||
         colour_table_size > MAX_COLOUR_TABLE_SIZE )
    {
        /** It is impractical or undesirable to optimize colour map
         * access with an integral colour table in these cases, so
         * don't bother.
         */
        slice_window->slice.volumes[volume_index].colour_table = NULL;
        return;
    }

    ALLOC( ptr, (int) max_voxel - (int) min_voxel + 1 );
    if ( ptr == NULL )
    {
        print_error("Failed to allocate colour table!\n");
        return;
    }

    slice_window->slice.volumes[volume_index].colour_offset = (int) min_voxel;
    slice_window->slice.volumes[volume_index].colour_table =
                                              ptr - (int) min_voxel;
}


/**
 * Sets the initial volume color coding range by calculating a
 * histogram over the whole volume. This takes a while for larger
 * volumes, so it should be automatically disabled if the volume
 * is larger than some threshold.
 *
 * \param volume The loaded volume.
 * \param low_limit Address of variable that will receive the lower limit.
 * \param high_limit Address of variable that will receive the upper limit.
 * \returns TRUE if both values were calculated.
 */
static VIO_BOOL
calculate_contrast_from_histogram(VIO_Volume volume,
                                  VIO_Real *low_limit,
                                  VIO_Real *high_limit)
{
  VIO_Real            min_value, max_value;
  VIO_BOOL            low_limit_done;
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
  *low_limit = histogram.min_index * histogram.delta + histogram.offset;
  *high_limit = (histogram.max_index + 1) * histogram.delta + histogram.offset;

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
      break;
    }
    count += histo_counts[idx];
  }

  FREE(histo_counts);

  /**
   * Correct lower and upper limits here in case of an extremely skewed
   * distribution. This helps do a better job of displaying images with
   * a white background, such as the BigBrain.
   */
  if (*low_limit > (max_value - min_value) / 2.0)
  {
    *low_limit = min_value + (max_value - min_value) * Initial_histogram_low;
  }

  /*
   * Check range of low and high limit - force them to make sense.
   */
  if (*low_limit < min_value)
    *low_limit = min_value;
  if (*high_limit > max_value)
    *high_limit = max_value;

  delete_histogram(&histogram);
  return TRUE;
}

/**
 * Simple way to compute the initial colour coding range, without
 * going to all the trouble to calculate a histogram.
 *
 * \param volume The loaded volume.
 * \param low_limit Address of variable that will receive the lower limit.
 * \param high_limit Address of variable that will receive the upper limit.
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
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The zero-based index of the selected volume.
 */
void  initialize_slice_colour_coding(
    display_struct    *slice_window,
    int               volume_index )
{
    VIO_Real           low_limit, high_limit;
    Colour_coding_types colour_coding_type = Current_colour_coding_type;
    loaded_volume_struct *loaded_volume_ptr;
    VIO_Volume         volume;
    VIO_Real           colour_below, colour_above;

    /* For volumes after the first, adopt a different color coding
     * scheme than the default.
     */
    if (volume_index > 0 &&
        colour_coding_type == (Colour_coding_types) Initial_colour_coding_type)
    {
        colour_coding_type = SPECTRAL;
        colour_below = make_rgba_Colour(0, 0, 0, 0);
    }
    else
    {
        colour_below = Colour_below;
    }
    colour_above = Colour_above;

    loaded_volume_ptr = &slice_window->slice.volumes[volume_index];

    initialize_colour_coding(&loaded_volume_ptr->colour_coding,
                             colour_coding_type,
                             colour_below, colour_above, 0.0, 1.0 );

    loaded_volume_ptr->label_colour_opacity = Label_colour_opacity;
    loaded_volume_ptr->n_labels = Initial_num_labels;
    loaded_volume_ptr->colour_offset = 0;
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

/**
 * Get a pointer to the label volume corresponding to a particular
 * volume index. If label sharing is enabled, this may not be unique
 * to this volume index.
 *
 * \param display A pointer to any of the top-level display_struct objects.
 * \param volume_index The zero-based index of the desired volume.
 * \returns A pointer to the appropriate label volume, or NULL if failure.
 */
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

/**
 * Get a pointer to the label volume corresponding to the current
 * volume index.
 *
 * \param display A pointer to any of the top-level display_struct objects.
 * \returns A pointer to the appropriate label volume, or NULL if failure.
 */
VIO_Volume  get_label_volume(
    display_struct   *display )
{
    return( get_nth_label_volume( display,
                                  get_current_volume_index(display)) );
}

/**
 * Test whether a the current label volume "exists".
 *
 * \param display A pointer to any top-level window's display_struct.
 * \returns TRUE if the selected current volume has a valid label volume
 * associated with it.
 */
VIO_BOOL  label_volume_exists(
    display_struct   *display )
{
    VIO_Volume   label;

    label = get_label_volume( display );

    return( is_label_volume_initialized( label ) );
}

/**
 * Test whether the labels associated with a particular volume and view
 * are actually visible. Tests the visiblity of the slice, the visibility
 * of labels in general, and the validity of the label volume.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param view_index The view index (0...N_SLICE_VIEWS - 1) of the
 * desired view.
 * \returns TRUE if the labels are visible in this volume and view.
 */
VIO_BOOL  get_labels_visibility(
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

/**
 * Get the number of labels currently associated with a volume. Label
 * values can range from zero (which usually means "erased" or empty)
 * up to one less than the number of labels.
 *
 * \param display A pointer to any top-level window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \returns The number of labels configured for this volume.
 */
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

/**
 * Manually composite the colour of the current label with another colour.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param col The colour we want to combine with the label's colour.
 * \param label The label value.
 * \returns The combined colour.
 */
static  VIO_Colour  apply_label_colour(
    display_struct    *slice_window,
    int               volume_index,
    VIO_Colour        col,
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

/**
 * Get the combined slice colour coding for the given value and label.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param value The value of the volume at this position.
 * \param label The label assigned at this position.
 * \returns The combined colour.
 */
static  VIO_Colour  get_slice_colour_coding(
    display_struct    *slice_window,
    int               volume_index,
    VIO_Real          value,
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
 * The resulting colour table is stored in the slice window's display_struct,
 * where it is used in render_slice_to_pixels() to
 * provide fast access to the colour map.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
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

/**
 * Get the opacity (alpha-value of all of a volume's labels.
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \returns The current opacity, a floating point number in the range
 * [0, 1].
 */
VIO_Real get_label_opacity( display_struct *slice_window,
                            int volume_index )
{
  return slice_window->slice.volumes[volume_index].label_colour_opacity;
}

/**
 * Sets the colour associated with a particular label value.
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param label The label whose colour we wish to set.
 * \param colour The RGBA colour to associate with this label.
 */
void   set_colour_of_label(
    display_struct    *slice_window,
    int               volume_index,
    int               label,
    VIO_Colour        colour )
{
    if( get_Colour_a(colour) == 255 )
    {
        VIO_Real  r, g, b, a;

        r = get_Colour_r_0_1( colour );
        g = get_Colour_g_0_1( colour );
        b = get_Colour_b_0_1( colour );
        a = get_label_opacity( slice_window, volume_index );

        colour = make_rgba_Colour_0_1( r, g, b, a );
    }

    slice_window->slice.volumes[volume_index].label_colour_table[label] =colour;
}

/**
 * Get the colour associated with a particular volume and label.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param label The label value whose color we want.
 * \returns The colour associated with the label and volume.
 */
VIO_Colour   get_colour_of_label(
    display_struct    *slice_window,
    int               volume_index,
    int               label )
{
    return( slice_window->slice.volumes[volume_index].
                                             label_colour_table[label] );
}

/**
 * Set the opacity (alpha-value) for a volume. This sets the opacity of
 * the actual volume slice.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param opacity The new opacity to set.
 */
void   set_volume_opacity(
    display_struct   *slice_window,
    int              volume_index,
    VIO_Real         opacity )
{
    slice_window->slice.volumes[volume_index].opacity = opacity;

    colour_coding_has_changed( slice_window, volume_index, UPDATE_BOTH );
}

/**
 * Set the opacity (alpha-value) for all of a volume's labels, as
 * overlaid on the volume slice. Sets the transparency of the label
 * display - an opacity of one implies opaque labels, while an opacity
 * of zero makes the labels invisible.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param opacity The new opacity to set, a floating point number in the
 * range [0,1].
 */
void   set_label_opacity(
    display_struct   *slice_window,
    int              volume_index,
    VIO_Real         opacity )
{
    int        i, n_labels;
    VIO_Real   r, g, b;
    VIO_Colour *table;

    slice_window->slice.volumes[volume_index].label_colour_opacity = opacity;

    n_labels = get_num_labels( slice_window, volume_index );
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

/**
 * Called to let the display system know that the colour coding of the
 * slice window has changed.
 * \param display A pointer to any top-level window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param type A hint about the type of update required: UPDATE_LABELS,
 * UPDATE_SLICE, or UPDATE_BOTH.
 */
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

/**
 * Set the minimum and maximum values of the slice colour coding. Values
 * between the minimum and maximum are smoothly interpolated within the
 * chosen colour map, values below the minimum are painted using the "under"
 * colour, those above the maximum are painted using the "over" colour.
 *
 * This is called, for example, when the "sliders" on the colour bar
 * widget are moved.
 *
 * \param slice_window A pointer to slice window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param min_value The new minimum value.
 * \param max_value The new maximum value.
 */
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
 * the encoded colours from an associated volume to the vertices of the
 * object. Actually does most of the work for colour_code_object_points().
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param continuity Type of continuity to use in interpolation.
 * \param colour_flag Pointer to the colour flag of the object.
 * \param colours Pointer to the colour array of the object.
 * \param n_points Number of points in the object.
 * \param points The array of points associated with this object.
 * \param labels_only Only copy label colours.
 */
static  void  colour_code_points(
    display_struct        *slice_window,
    int                   continuity,
    Colour_flags          *colour_flag,
    VIO_Colour            *colours[],
    int                   n_points,
    VIO_Point             points[],
    VIO_BOOL              labels_only )
{
    int        i, int_voxel[VIO_MAX_DIMENSIONS];
    int        label, volume_index;
    VIO_Real   val, voxel[VIO_MAX_DIMENSIONS];
    VIO_Volume volume, label_volume;
    VIO_Colour colour, volume_colour;

    if( *colour_flag != PER_VERTEX_COLOURS )
    {
        if( n_points > 0 )
        {
            REALLOC( *colours, n_points );

            if ( *colour_flag == ONE_COLOUR )
            {
                for_less( i, 1, n_points )
                {
                    (*colours)[i] = (*colours)[0];
                }
            }
        }
        else
        {
            FREE( *colours );
        }
        *colour_flag = PER_VERTEX_COLOURS;
    }

    for_less( i, 0, n_points )
    {
        colour = (*colours)[i]; /* start with original surface colour. */

        for_less( volume_index, 0, slice_window->slice.n_volumes )
        {
            if ( !get_volume_visibility( slice_window, volume_index ) )
            {
                continue;
            }

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
                {
                    volume_colour = get_colour_coding_under_colour(
                               &slice_window->slice.volumes[volume_index].
                                        colour_coding );
                }
            }
            else
            {
                (void) evaluate_volume( volume, voxel, NULL,
                                        continuity, FALSE,
                                        get_volume_real_min(volume),
                                        &val, NULL, NULL );

                volume_colour = get_colour_code( &slice_window->slice.volumes[volume_index].colour_coding, val );
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

            if (labels_only)
            {
                colour = get_colour_of_label( slice_window, volume_index,
                                              label );
            }
            else
            {
                if( is_an_rgb_volume( volume ) )
                {
                    volume_colour = apply_label_colour( slice_window,
                                                        volume_index,
                                                        volume_colour, label );
                }
                else
                {
                    volume_colour = get_slice_colour_coding( slice_window,
                                                             volume_index,
                                                             val, label );
                }

                COMPOSITE_COLOURS( colour, volume_colour, colour )
            }
        }

        (*colours)[i] = colour;
    }
}

/**
 * Colour code each of the points associated with an object, by copying
 * the encoded colours from an associated volume to the vertices of the
 * object.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param continuity Type of continuity to use in interpolation.
 * \param object The object to colour code.
 * \param labels_only Copy the colours from the label volume _only_.
 */
static  void  colour_code_object_points(
    display_struct         *slice_window,
    int                    continuity,
    object_struct          *object,
    VIO_BOOL               labels_only )
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
                            polygons->n_points, polygons->points, labels_only );
        break;

    case QUADMESH:
        quadmesh = get_quadmesh_ptr( object );
        colour_code_points( slice_window, continuity,
                            &quadmesh->colour_flag, &quadmesh->colours,
                            quadmesh->m * quadmesh->n,
                            quadmesh->points, labels_only );
        break;

    case LINES:
        lines = get_lines_ptr( object );
        colour_code_points( slice_window, continuity,
                            &lines->colour_flag, &lines->colours,
                            lines->n_points, lines->points, labels_only );
        break;

    case MARKER:
        marker = get_marker_ptr( object );
        colour_flag = PER_VERTEX_COLOURS;
        colours = &marker->colour;
        colour_code_points( slice_window, continuity,
                            &colour_flag, &colours, 1, &marker->position,
                            labels_only );
        break;

    default:
        /* Do not apply colour coding to a MODEL, PIXELS,
         * or TEXT object.
         */
        break;
    }
}

/**
 * Public interface for colour_code_object_points(). This function
 * will apply the colours of the currently selected volume to the vertices
 * of the given 3D graphical object.
 *
 * \param display A pointer to one of the windows' display_struct.
 * \param object The object to colour code. This can be any polygon,
 * quadmesh, marker, or lines object.
 * \param labels_only Copy the colours from the label volume _only_.
 */
void  colour_code_an_object(
    display_struct   *display,
    object_struct    *object,
    VIO_BOOL         labels_only )
{
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        colour_code_object_points( slice_window, Volume_continuity,
                                   object, labels_only );
}

/**
 * Get the default filename extension for label colour maps.
 * \returns A static pointer to the extension.
 */
VIO_STR    get_default_colour_map_suffix( void )
{
    return( DEFAULT_COLOUR_MAP_SUFFIX );
}

/**
 * Load a label colour map from a file (with the default extension .map).
 * Label colour map files are ASCII text where each line has
 * the format:
 *
 * > label red green blue
 * OR
 * > label colour-name
 *
 * The label values are integers, while the RGB colour values are floating
 * point numbers in the interval [0,1]. Colour names are symbolic names
 * such as "black", "white", "blue", "transparent", "violet", etc.
 *
 * For each line, if the label
 * is within the current range, the colour of this label is replaced in the
 * colour map. In other words, it is possible to have an incomplete colour
 * map file that specifies only a few labels of interest.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param filename The filename to load from. The extension ".map" will be
 * added if there is no extension specified.
 * \returns VIO_OK on success.
 */
VIO_Status  load_label_colour_map(
    display_struct   *slice_window,
    VIO_STR           filename )
{
    VIO_Status status;
    FILE       *file;
    VIO_Colour col;
    VIO_STR    line;
    int        n_labels, index;

    if( open_file_with_default_suffix( filename,
                                       get_default_colour_map_suffix(),
                                       READ_FILE,
                                       ASCII_FORMAT,
                                       &file ) != VIO_OK )
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

        status = string_to_colour( line, &col );
        if ( status != VIO_OK )
        {
            print_error( "Incorrect colour value.\n" );
            break;
        }

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

/**
 * Saves the current label colour map to a file (with the extension .map).
 * When written, colour map files are ASCII text where each line has
 * the format:
 *
 * > label red green blue
 *
 * The label values are integers, while the colour values are floating
 * point numbers in the interval [0,1].
 * \param slice_window A pointer to the slice window's display_struct.
 * \param filename The filename to save under. The extension ".map" will be
 * added if there is no extension specified.
 * \returns VIO_OK on success.
 */
VIO_Status  save_label_colour_map(
    display_struct   *slice_window,
    VIO_STR           filename )
{
    VIO_Status status;
    FILE       *file;
    VIO_Real   red, green, blue;
    VIO_Colour col;
    int        n_labels, index;

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

/**
 * Set all of the labels in the label volume to the erased state, clearing
 * all of the labels on the volume.
 *
 * \param display A pointer to any top-level window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 */
void  clear_labels(
    display_struct   *display,
    int              volume_index )
{
    int        sizes[VIO_MAX_DIMENSIONS];
    VIO_Volume label_volume;

    label_volume = get_nth_label_volume( display, volume_index );

    get_volume_sizes( label_volume, sizes );

    tell_surface_extraction_label_changed( display, volume_index, 0, 0, 0 );
    tell_surface_extraction_label_changed( display, volume_index,
                                           sizes[VIO_X] - 1,
                                           sizes[VIO_Y] - 1,
                                           sizes[VIO_Z] - 1 );
    set_all_volume_label_data( label_volume, 0 );
}

/**
 * Return the integer label value associated with a particular voxel of a
 * particular volume.
 * \param display A pointer to any top-level window's display_struct.
 * \param volume_index The zero-based index of the desired volume.
 * \param x The voxel x coordinate of the desired voxel.
 * \param y The voxel y coordinate of the desired voxel.
 * \param z The voxel z coordinate of the desired voxel.
 * \returns The label value associated with this voxel.
 */
int  get_voxel_label(
    display_struct   *display,
    int              volume_index,
    int              x,
    int              y,
    int              z )
{
    return( get_3D_volume_label_data(
                     get_nth_label_volume( display, volume_index ),
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
static void update_label_tag(
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

    marker_window = get_display_by_type( MARKER_WINDOW );
    three_d_window = get_display_by_type( THREE_D_WINDOW );
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

            update_current_marker( three_d_window, volume_index, voxel_real );
            get_current_object( three_d_window, &object );
            if (remove_current_object_from_hierarchy( three_d_window, &object ))
            {
              graphics_models_have_changed( three_d_window );
              delete_object( object );
              rebuild_selected_list( three_d_window, marker_window );
            }
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

/**
 * Loads a colour coding for the _actual volume data_, rather than the
 * labels. Colour coding files are different from the colour map
 * files used to assign colours to integer label. Instead, these files
 * are intended to express a smooth colour mapping over a continuous
 * value.
 *
 * Each line consists of a floating-point value followed by a colour,
 * usually expressed in RGB format:
 *
 * > fraction red blue green
 *
 * Each of the four values must be in the range [0,1] and the `fraction`
 * must increase monotonically, and the file should always include _at least_
 * a value for `fraction=0` and `fraction=1`.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param filename The path to the colour coding file to load.
 * \returns VIO_OK if all goes well.
 */
VIO_Status  load_user_defined_colour_coding(
    display_struct   *slice_window,
    VIO_STR           filename )
{
    VIO_Status status;
    int        volume_index;

    volume_index = get_current_volume_index( slice_window );

    status = input_user_defined_colour_coding( &slice_window->slice.volumes[
                         volume_index ].colour_coding, filename );

    return( status );
}

/**
 * Set the visibility of an individual label.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The zero-based index of the volume.
 * \param label The label whose visibility we want to change.
 * \param is_visible TRUE if the label should be visible, FALSE if not.
 */
void
set_label_visible( display_struct *slice_window, int volume_index,
                   int label, VIO_BOOL is_visible)
{
  VIO_Colour *table;
  VIO_Colour col;
  VIO_Real r, g, b, a;          /*  colour components */

  if ( label >= get_num_labels( slice_window, volume_index ) )
    return;

  table = slice_window->slice.volumes[volume_index].label_colour_table;
  col = table[label];
  r = get_Colour_r_0_1( col );
  g = get_Colour_g_0_1( col );
  b = get_Colour_b_0_1( col );
  a = ( is_visible ) ? get_label_opacity( slice_window, volume_index ) : 0;
  table[label] = make_rgba_Colour_0_1( r, g, b, a );
}

/**
 * Check the visibility of an individual label.
 *
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The zero-based index of the volume.
 * \param label The label whose visibility we want to change.
 * \returns TRUE if the label is currently visible.
 */
VIO_BOOL
is_label_visible( display_struct *slice_window, int volume_index, int label )
{
  VIO_Colour *table;

  if ( label >= get_num_labels( slice_window, volume_index ))
    return FALSE;

  table = slice_window->slice.volumes[volume_index].label_colour_table;
  return get_Colour_a( table[label] ) > 0;
}
