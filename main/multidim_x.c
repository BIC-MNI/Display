/**
 * \file multidim_x.c
 * \brief Extended functionality for multi-dimensional arrays.
 *
 * Functions for operations on multidimensional arrays that seemed
 * fairly generic. These probably belong in volume IO.
 */

#include <volume_io.h>
#include <float.h>
#include <multidim_x.h>

/**
 * Actually scan through a multidimensional array and determine the
 * true minimum and maximum values.
 * \param array The array to scan.
 * \param min_value_p Where to store the minimum value.
 * \param max_value_p Where to store the maximum value.
 */
void multidim_scan_range( const VIO_multidim_array *array,
                          VIO_Real *min_value_p,
                          VIO_Real *max_value_p )
{
  int x, y, z, t, v;
  VIO_Real value;
  VIO_Real min_value = DBL_MAX;
  VIO_Real max_value = -DBL_MAX;

  /* Scan for the true minimum and maximum.
   */
  switch ( array->n_dimensions )
  {
  case 5:
    for_less ( x, 0, array->sizes[VIO_X] )
    {
      for_less ( y, 0, array->sizes[VIO_Y] )
      {
        for_less ( z, 0, array->sizes[VIO_Z] )
        {
          for_less ( t, 0, array->sizes[3] )
          {
            for_less (v, 0, array->sizes[4] )
            {
              GET_MULTIDIM_5D( value, (VIO_Real), *array, x, y, z, t, v );
              if ( value < min_value )
                min_value = value;
              if ( value > max_value )
                max_value = value;
            }
          }
        }
      }
    }
    break;

  case 4:
    for_less ( x, 0, array->sizes[VIO_X] )
    {
      for_less ( y, 0, array->sizes[VIO_Y] )
      {
        for_less ( z, 0, array->sizes[VIO_Z] )
        {
          for_less ( t, 0, array->sizes[3] )
          {
            GET_MULTIDIM_4D( value, (VIO_Real), *array, x, y, z, t );
            if ( value < min_value )
              min_value = value;
            if ( value > max_value )
              max_value = value;
          }
        }
      }
    }
    break;

  case 3:
    for_less ( x, 0, array->sizes[VIO_X] )
    {
      for_less ( y, 0, array->sizes[VIO_Y] )
      {
        for_less ( z, 0, array->sizes[VIO_Z] )
        {
          GET_MULTIDIM_3D( value, (VIO_Real), *array, x, y, z );
          if ( value < min_value )
            min_value = value;
          if ( value > max_value )
            max_value = value;
        }
      }
    }
    break;

  case 2:
    for_less ( x, 0, array->sizes[VIO_X] )
    {
      for_less ( y, 0, array->sizes[VIO_Y] )
      {
        GET_MULTIDIM_2D( value, (VIO_Real), *array, x, y );
        if ( value < min_value )
          min_value = value;
        if ( value > max_value )
          max_value = value;
      }
    }
    break;

  case 1:
    for_less (x, 0, array->sizes[VIO_X] )
    {
      GET_MULTIDIM_1D( value, (VIO_Real), *array, x );
      if ( value < min_value )
        min_value = value;
      if ( value > max_value )
        max_value = value;
    }
    break;

  default:
    break;
  }
  *min_value_p = min_value;
  *max_value_p = max_value;
}
