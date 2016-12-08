/**
 * \file scalebars.c
 * \brief Drawing the scalebars that can be superimposed on the slice view.
 *
 * \copyright
              Copyright 1993-2016 David MacDonald and Robert D. Vincent,
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

/**
 * \brief Initialize the objects used by the slice ruler.
 *
 * Adds a new model at the \c RULERS relative offset.
 *
 * \param model_ptr A pointer to the model_struct for the slice view.
 */
void
initialize_slice_scalebar(model_struct *model_ptr)
{
  object_struct     *object_ptr;
  model_struct      *sub_model_ptr;
  model_info_struct *model_info_ptr;
  pixels_struct     *pixels_ptr;
  text_struct       *text_ptr;

  object_ptr = create_object( MODEL );
  set_object_visibility( object_ptr, TRUE );
  add_object_to_model( model_ptr, object_ptr );
  sub_model_ptr = get_model_ptr( object_ptr );
  initialize_display_model( sub_model_ptr );
  model_info_ptr = get_model_info( sub_model_ptr );
  sub_model_ptr->n_objects = 0;
  model_info_ptr->view_type = PIXEL_VIEW;
  model_info_ptr->bitplanes = NORMAL_PLANES;

  object_ptr = create_object( PIXELS );
  pixels_ptr = get_pixels_ptr( object_ptr );
  initialize_pixels( pixels_ptr, 0, 0, 0, 0, 1.0, 1.0, RGB_PIXEL );
  set_object_visibility( object_ptr, TRUE );
  add_object_to_model( sub_model_ptr, object_ptr );

  object_ptr = create_object( TEXT );
  text_ptr = get_text_ptr( object_ptr );
  initialize_text( text_ptr, NULL, TRANSPARENT,
                   Slice_readout_text_font,
                   Slice_readout_text_font_size );
  set_object_visibility( object_ptr, TRUE );
  add_object_to_model( sub_model_ptr, object_ptr );
}

/**
 * For any 'x', return the 'significand' part for exponential
 * representation.
 * For example:
 *    x of 0.15 will return 1.5
 *    x of 939.2 will return 9.392
 * \param x The input value.
 * \returns The significand of 'x'
 */
static VIO_Real
get_significand(VIO_Real x)
{
  return x * pow(10, ceil(-log10(x)));
}

/**
 * For any positive value, returns a result between 1 and 2. This
 * represents the multiple of the minimum size used to set the scale
 * bar length in pixels.
 * \param x The input value.
 * \param min_size The minimum size in pixels.
 */
static VIO_Real
normalize(VIO_Real x, int min_size)
{
  VIO_Real significand = get_significand( x );
  VIO_Real result = get_significand(significand / get_significand( min_size ) );
  if ( result >= 5 )
  {
    result /= 5;
  }
  if ( result >= 4 )
  {
    result /= 4;
  }
  if ( result >= 2 )
  {
    result /= 2;
  }
  return result;
}

/**
 * Returns the original number with the significand rounded to
 * a specific number of decimal places.
 * \param x The number to round.
 * \param decimal_places The number of decimal places.
 * \returns The rounded value.
 */
static VIO_Real
round_significand(VIO_Real x, int decimal_places)
{
  VIO_Real exponent = -ceil(-log10(x));
  VIO_Real power = decimal_places - exponent;
  VIO_Real significand = x * pow(10, power);
  VIO_Real result;
  if (power < 0)
  {
    result = round(significand) * pow(10, -power);
  }
  else
  {
    result = round(significand) / pow(10, power);
  }
  return result;
}

/**
 * Calculates the length in pixels and appropriate units for the
 * scale bar.
 */
static VIO_Real
get_scalebar_size(VIO_Real ppm, int min_size, char unit_prefix[],
                  VIO_Real *n_pixels_ptr)
{
  VIO_Real multiplier = normalize( ppm, min_size );
  VIO_Real length = round_significand( multiplier / ppm * min_size, 3 );

  unit_prefix[0] = unit_prefix[1] = 0;
  *n_pixels_ptr = multiplier * min_size;

  if (length < 0.000001)
  {
    unit_prefix[0] = 'n';
    return length * 1.0e9;
  }
  else if (length < 0.001)
  {
    unit_prefix[0] = 'u';       /* closest ascii to "mu" */
    return length * 1.0e6;
  }
  else if (length < 1)
  {
    unit_prefix[0] = 'm';
    return length * 1000;
  }
  else if (length >= 1000)
  {
    unit_prefix[0] = 'k';
    return length / 1000;
  }
  return length;
}

/**
 * Update the position, size, and text of a scalebar model.
 */
void
rebuild_one_scalebar( model_struct *model_ptr, VIO_Real ppm, int quadrant,
                      int cy_bar, int x_size, int y_size )
{
  int               x_base, y_base;
  text_struct       *text_ptr;
  pixels_struct     *pixels_ptr;
  char              label[100];
  char              unit_prefix[2];
  VIO_Real          pixel_width;
  VIO_Real          units_width;
  int               i;
  int               n_pixels;

  units_width = get_scalebar_size( ppm, 50, unit_prefix, &pixel_width );

  snprintf( label, sizeof(label), "%g %sm", units_width, unit_prefix);
  int cx_bar = (int)round( pixel_width );
  int cx_txt = G_get_text_length( label, Slice_readout_text_font,
                                  Slice_readout_text_font_size);
  int cy_txt = G_get_text_height( Slice_readout_text_font,
                                  Slice_readout_text_font_size );

  text_ptr = get_text_ptr( model_ptr->objects[1] );
  text_ptr->colour = Scalebar_colour;
  replace_string( &text_ptr->string, create_string( label ) );

  switch ( quadrant )
  {
  default:
  case 1:                       /* upper right */
    x_base = x_size - MAX( cx_bar, cx_txt );
    y_base = y_size - ( cy_bar + cy_txt + 2 ) - 10;
    break;
  case 2:                       /* upper left */
    x_base = 0;
    y_base = y_size - ( cy_bar + cy_txt + 2 ) - 10;
    break;
  case 3:                       /* lower left */
    x_base = 0;
    y_base = 0;
    break;
  case 4:                       /* lower right */
    x_base = x_size - MAX( cx_bar, cx_txt );
    y_base = 0;
    break;
  }

  if ( cx_bar > cx_txt )
  {
    fill_Point( text_ptr->origin, x_base + ( cx_bar - cx_txt ) / 2,
                y_base + cy_txt, 0.0 );
  }
  else
  {
    fill_Point( text_ptr->origin, x_base, y_base + cy_txt, 0.0 );
  }

  pixels_ptr = get_pixels_ptr( model_ptr->objects[0] );

  pixels_ptr->x_position = x_base;
  pixels_ptr->y_position = y_base + ( cy_txt + cy_bar + 2 );

  n_pixels = pixels_ptr->x_size * pixels_ptr->y_size;

  modify_pixels_size( &n_pixels, pixels_ptr, cx_bar, cy_bar, RGB_PIXEL );

  for ( i = 0; i < n_pixels; i++ )
  {
    pixels_ptr->data.pixels_rgb[i] = Scalebar_colour;
  }
}

/**
 * \brief Recreate the object used to display the scalebars on the slice view.
 *
 * \todo Assumes dimensions in millimetres! This should be generalized.
 *
 * \param slice_window A pointer to the display_struct of the slice window.
 * \param view_index The zero-based index of the view.
 */
void
rebuild_slice_scalebar( display_struct *slice_window, int view_index )
{
  model_struct      *model_ptr;
  object_struct     *object_ptr;
  int               x_min, x_max, y_min, y_max;
  int               volume_index;
  VIO_Volume        volume;
  int               axis_index, x_index, y_index;
  VIO_Real          voxel[VIO_MAX_DIMENSIONS];
  VIO_Real          w_ll[VIO_N_DIMENSIONS];
  VIO_Real          w_lr[VIO_N_DIMENSIONS];
  int               dummy_index;
  VIO_Real          x_diff;
  VIO_Real          ppm;
  
  if (!Scalebar_enabled)
    return;

  /* Don't even bother to display this for the oblique plane yet.
   */
  if (view_index == get_arbitrary_view_index( slice_window ) )
    return;

  volume_index = get_current_volume_index( slice_window );
  if (volume_index < 0)
    return;

  if ( !get_slice_visibility( slice_window, volume_index, view_index ) )
    return;

  if (!slice_has_ortho_axes( slice_window, volume_index, view_index,
                             &x_index, &y_index, &axis_index ))
  {
    return;
  }

  model_ptr = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
  object_ptr = model_ptr->objects[2 * get_n_volumes( slice_window ) + SCALEBAR];
  if (!get_object_visibility( object_ptr ))
  {
    return;
  }

  model_ptr = get_model_ptr( object_ptr );

  /* Calculate the bounding box of the view in world coordinates.
   */
  get_slice_viewport( slice_window, view_index,
                      &x_min, &x_max, &y_min, &y_max );

  volume = get_nth_volume( slice_window, volume_index );

  convert_pixel_to_voxel( slice_window, volume_index, x_min, y_min,
                          voxel, &dummy_index);
  convert_voxel_to_world( volume, voxel, &w_ll[0], &w_ll[1], &w_ll[2]);

  convert_pixel_to_voxel( slice_window, volume_index, x_max, y_min,
                          voxel, &dummy_index);
  convert_voxel_to_world( volume, voxel, &w_lr[0], &w_lr[1], &w_lr[2]);

  x_diff = ( w_lr[x_index] - w_ll[x_index] );

  /* Compute pixels per metre. */
  ppm = ( x_max - x_min ) * 1000.0 / x_diff;

  rebuild_one_scalebar( model_ptr, ppm, Scalebar_quadrant, Scalebar_height,
                        x_max - x_min, y_max - y_min );
}

