/**
 * \file slice_window/distinct_colours.c
 * \brief Generate a set of visually distinct colours. 
 *
 * Details cribbed from various public sources, especially the following:
 *
 * https://www.mathworks.com/matlabcentral/fileexchange/29702-generate-maximally-perceptually-distinct-colors
 *
 * \copyright
              Copyright 1993-2016 David MacDonald, Robert D. Vincent
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
#include <display.h>
#include <float.h>

/** The number of components in a colour (3) */
#define N_COMPONENTS 3

/** Determines how fine-grained our search grid is. */
#define N_STEPS 30

/** Total size of the search grid. */
#define N_TOTAL (N_STEPS*N_STEPS*N_STEPS)

/** White point for D65 */
#define X_N 95.047
#define Y_N 100.0
#define Z_N 108.883

static VIO_Real
hi(VIO_Real x)
{
  return (x > 0.04045 ? pow((x + 0.055) / 1.055, 2.4) : x / 12.92) * 100.0;
}

/** Convert (s)RGB to XYZ assuming D65 illuminant. */
static void
rgb_to_xyz(const VIO_Real rgb[], VIO_Real xyz[])
{
  VIO_Real r = hi(rgb[0] / 255.0);
  VIO_Real g = hi(rgb[1] / 255.0);
  VIO_Real b = hi(rgb[2] / 255.0);
  /* correct for D65, not for ICC */
  xyz[0] = r * 0.4124564 + g * 0.3575761 + b * 0.1804375;
  xyz[1] = r * 0.2126729 + g * 0.7151522 + b * 0.0721750;
  xyz[2] = r * 0.0193339 + g * 0.1191920 + b * 0.9503041;
}

#if NOT_USED                    /* for information only */
static VIO_Real
fi(VIO_Real x)
{
  return (x > 6.0/29.0) ? pow(x, 3.0) : 3.0 * 6.0/29.0 * 6.0/29.0 * (x - 4.0 / 29.0);
}

/** Convert Lab to XYZ */
static void
lab_to_xyz(const VIO_Real lab[], VIO_Real xyz[])
{
  xyz[0] = X_N * fi(1.0/116.0 * (lab[0] + 16.0) + 1.0/500.0 * lab[1]);
  xyz[1] = Y_N * fi(1.0/116.0 * (lab[0] + 16.0));
  xyz[2] = Z_N * fi(1.0/116.0 * (lab[0] + 16.0) - 1.0/200.0 * lab[2]);
}

#endif /* NOT_USED */

static VIO_Real
f(VIO_Real x)
{
  return (x > pow(6.0/29.0, 3.0)) ? pow(x, 1.0 / 3.0) : (1.0/3.0 * 29.0/6.0 * 29.0/6.0 * x + 4.0 / 29.0);
}

static void
xyz_to_lab(const VIO_Real xyz[], VIO_Real lab[])
{
  lab[0] = 116.0 * f(xyz[1] / Y_N) - 16;
  lab[1] = 500.0 * (f(xyz[0] / X_N) - f(xyz[1] / Y_N));
  lab[2] = 200.0 * (f(xyz[1] / Y_N) - f(xyz[2] / Z_N));
}

#if NOT_USED                    /* for information only */

static VIO_Real
h(VIO_Real x)
{
  VIO_Real y = x > 0.0031308 ? 1.055 * pow(x, 1.0 / 2.4) - 0.055 : 12.92 * x;
  y *= 255.0;
  if (y > 255.0)
  {
    y = 255.0;
  }
  if (y < 0.0)
  {
    y = 0.0;
  }
  return y;
}

static void
xyz_to_rgb(const VIO_Real xyz[], VIO_Real rgb[])
{
  VIO_Real x = xyz[0] / 100.0;
  VIO_Real y = xyz[1] / 100.0;
  VIO_Real z = xyz[2] / 100.0;
  VIO_Real r = x *  3.2404542 + y * -1.5371385 + z * -0.4985314;
  VIO_Real g = x * -0.9692660 + y *  1.8760108 + z *  0.0415560;
  VIO_Real b = x *  0.0556434 + y * -0.2040259 + z *  1.0572252;
  rgb[0] = h(r);
  rgb[1] = h(g);
  rgb[2] = h(b);
}

#endif /* NOT_USED */

static VIO_Real
cdist(const VIO_Real x[N_COMPONENTS], const VIO_Real y[N_COMPONENTS])
{
  VIO_Real a = x[0] - y[0];
  VIO_Real b = x[1] - y[1];
  VIO_Real c = x[2] - y[2];

  return a*a + b*b + c*c;
}

/**
 * Generate a list of visually distinct colours, given a number of
 * colours and a desired background.
 *
 * \param n_colours The number of colours to generate.
 * \param bkgd The current background colour.
 * \param colours An existing array of at least n_colours items, which 
 * will be the output.
 */
void
distinct_colours( int n_colours, VIO_Colour bkgd, VIO_Colour colours[] )
{
  VIO_Real r, g, b;
  VIO_Real dc = 1.0 / (N_STEPS - 1);
  VIO_Real rgb[N_TOTAL][N_COMPONENTS];
  VIO_Real lab[N_TOTAL][N_COMPONENTS];
  VIO_Real last[N_COMPONENTS];
  VIO_Real dist[N_TOTAL];
  int i, j;
  VIO_Real xyz[N_COMPONENTS];
  VIO_Real bg[N_COMPONENTS];

  bg[0] = get_Colour_r(bkgd);
  bg[1] = get_Colour_g(bkgd);
  bg[2] = get_Colour_b(bkgd);

  i = 0;
  for (b = 0; b <= 1.0; b += dc)
  {
    for (g = 0; g <= 1.0; g += dc)
    {
      for (r = 0; r <= 1.0; r += dc)
      {
        rgb[i][0] = r * 255.0;
        rgb[i][1] = g * 255.0;
        rgb[i][2] = b * 255.0;
        i++;
      }
    } 
  }

  /* Convert all of the colours to Lab colourspace */
  for (i = 0; i < N_TOTAL; i++)
  {
    rgb_to_xyz( rgb[i], xyz );
    xyz_to_lab( xyz, lab[i] );
  }

  /* Start with the background as the first colour.
   */
  rgb_to_xyz( bg, xyz );
  xyz_to_lab( xyz, last );      /* last is now bg in Lab colourspace. */

  /* Initialize the distance from all colours to the selected colours.
   * 
   * This array will be updated to represent the minimum distance
   * from this colour to any of the colours selected so far.
   */
  for (i = 0; i < N_TOTAL; i++)
  {
    dist[i] = DBL_MAX;
  }

  /* Start generating our list of colours.
   */
  for (j = 0; j < n_colours; j++) 
  {
    VIO_Real max_dist = -1;
    int      max_ix = 0;

    /* Find the shortest distance between the last colour
     * selected and the entire grid of colours.
     */
    for (i = 0; i < N_TOTAL; i++)
    {
      VIO_Real l2 = cdist(last, lab[i]);
      if (l2 < dist[i])
      {
        dist[i] = l2;
      }
      if (dist[i] > max_dist)
      {
        max_dist = dist[i];
        max_ix = i;
      }
    }

    /* Update the last colour selected in Lab colourspace. */
    last[0] = lab[max_ix][0];
    last[1] = lab[max_ix][1];
    last[2] = lab[max_ix][2];

    /* Return the colour in RGB colourspace. */
    colours[j] = make_Colour(rgb[max_ix][0], rgb[max_ix][1], rgb[max_ix][2]);
  }
}
