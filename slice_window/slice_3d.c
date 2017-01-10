/**
 * \file slice_3d.c
 * \brief Functions for displaying the current oblique plane and volume
 * bounding box in the 3D view window.
 *
 * Generally bound to the key sequence 'S' (Slice view) / 'Q' (Toggle
 * plane visibility)
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
 * Initialize the objects used to display the volume bounding box and
 * oblique plane in the 3D view window.
 *
 * \param display The display_struct of the 3D view window.
 */
void
initialize_volume_cross_section( display_struct *display )
{
    model_struct   *model;

    assert(display->window_type == THREE_D_WINDOW);

    model = get_graphics_model( display, MISCELLANEOUS_MODEL );

    display->three_d.volume_outline = create_object( LINES );
    initialize_lines( get_lines_ptr( display->three_d.volume_outline ),
                      Volume_outline_colour );
    set_object_visibility( display->three_d.volume_outline, FALSE );
    add_object_to_model( model, display->three_d.volume_outline );

    display->three_d.cross_section = create_object( POLYGONS );
    initialize_polygons( get_polygons_ptr( display->three_d.cross_section ),
                         Cross_section_colour, &Cross_section_spr );

    set_object_visibility( display->three_d.cross_section, FALSE );
    add_object_to_model( model, display->three_d.cross_section );
}

/**
 * Create the bounding box for the volume.
 * \param volume The volume for which the box is created.
 * \param object The lines object which will be used to draw the box.
 */
static void
create_box( VIO_Volume volume, object_struct  *object )
{
    int            i, c, sizes[VIO_MAX_DIMENSIONS];
    VIO_Real       voxel[VIO_MAX_DIMENSIONS], x, y, z;
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
                voxel[c] = (VIO_Real) sizes[c] - 0.5;
        }

        convert_voxel_to_world( volume, voxel, &x, &y, &z );
        fill_Point( lines->points[i], x, y, z );
    }
}

/**
 * Recreate the bounding box of the volume for display in the 3D view
 * window.
 *
 * \param slice_window The display_struct of the slice window.
 */
void
rebuild_volume_outline( display_struct *slice_window )
{
    display_struct    *display;
    VIO_Volume            volume;

    display = get_three_d_window( slice_window );

    if( get_slice_window_volume( slice_window, &volume ) )
    {
        create_box( volume, display->three_d.volume_outline );
        set_update_required( display, NORMAL_PLANES );
    }
    else
        set_object_visibility( display->three_d.volume_outline, FALSE );
}

/**
 * structure used for our trival open-hashing scheme for keeping track of
 * grid points on the oblique plane.
 */
struct hash_entry {
  struct hash_entry *link;      /**< next in chain */
  int index;                    /**< index in points array */
  int cx;                       /**< multiplier for the x axis vector. */
  int cy;                       /**< multiplier for the y axis vector. */
};

#define N_HASH 100003

/** Very simple hash function intended to map our x and y axis multipliers
 */
int hash( int x, int y )
{
  int sign_x = (x < 0);
  int sign_y = (y < 0);
  if (sign_x)
    x = -x;
  if (sign_y)
    y = -y;
  return (((x << 1) | sign_x) * 211 + ((y << 1) | sign_y)) % N_HASH;
}

#define N_DIR 8                 /**< number of neighbouring grid points. */

/** increase the default chunk size for efficiency. */
#undef DEFAULT_CHUNK_SIZE
#define DEFAULT_CHUNK_SIZE 20000

/**
 * Given a point on a plane, the x and y axes of the plane (in voxel
 * coordinates), and the overall sizes of the axes in voxels, this
 * function will construct a triangular mesh that contains a set of
 * grid points that lie on the plane, lying within the volume.
 *
 * \param origin A point in voxel space that lies on the plane.
 * \param x_axis A vector pointing along the "column" direction for the plane.
 * \param y_axis A vector pointing along the "row" direction for the plane.
 * \param sizes The sizes of the volume in voxels.
 * \param polygons The structure which will hold the triangulated surface.
 */
static void
expand_surface(const VIO_Real origin[], const VIO_Real x_axis[],
               const VIO_Real y_axis[], const int sizes[],
               polygons_struct *polygons )
{
  static short dx[N_DIR] = {  0, -1, -1, -1,  0,  1,  1,  1 };
  static short dy[N_DIR] = { -1, -1,  0,  1,  1,  1,  0, -1 };
  int neighbours[N_DIR];
  QUEUE_STRUCT( int ) queue;
  VIO_Point pt;
  int i, j;
  int centre;
  struct hash_entry *table[N_HASH];
  struct hash_entry *elem;
  int n_points_alloced = polygons->n_points;
  int n_items_alloced = polygons->n_items;
  int n_indices_alloced = NUMBER_INDICES( *polygons );
  int n_points = 0;
  int n_items = 0;
  int n_indices = 0;

  INITIALIZE_QUEUE( queue );

  /* Initialize the hash table. */
  for_less( i, 0, N_HASH )
  {
    table[i] = NULL;
  }

  /* We always start at the origin. */
  fill_Point( pt, 0, 0, 0 );
  INSERT_IN_QUEUE( queue, 0 );

  /* Add to the list of visited points. */
  ADD_ELEMENT_TO_ARRAY_WITH_SIZE( polygons->points,
                                  n_points_alloced,
                                  n_points,
                                  pt,
                                  DEFAULT_CHUNK_SIZE );

  j = hash( 0, 0 );
  elem = malloc(sizeof(struct hash_entry));
  elem->index = 0;
  elem->cx = 0;
  elem->cy = 0;
  elem->link = NULL;
  table[j] = elem;

  while (!IS_QUEUE_EMPTY( queue ))
  {
    REMOVE_FROM_QUEUE( queue, centre );

    pt = polygons->points[centre];

    int nx = Point_x( pt );
    int ny = Point_y( pt );

    for_less( i, 0, N_DIR )
    {
      VIO_Real x, y, z;
      int cx = nx + dx[i];
      int cy = ny + dy[i];

      neighbours[i] = -1;

      /* Convert from the counts along x and y to the actual coordinates
       * in voxel space.
       */
      x = origin[VIO_X] + cx * x_axis[VIO_X] + cy * y_axis[VIO_X];
      y = origin[VIO_Y] + cx * x_axis[VIO_Y] + cy * y_axis[VIO_Y];
      z = origin[VIO_Z] + cx * x_axis[VIO_Z] + cy * y_axis[VIO_Z];

      if (x >= -0.5 && x < sizes[VIO_X] + 0.5 &&
          y >= -0.5 && y < sizes[VIO_Y] + 0.5 &&
          z >= -0.5 && z < sizes[VIO_Z] + 0.5 )
      {
        /* If it is inside volume, need to add it to the points array.
         */
        j = hash( cx, cy );
        elem = table[j];
        for ( elem = table[j]; elem != NULL; elem = elem->link )
        {
          if ( elem->cx == cx && elem->cy == cy )
          {
            neighbours[i] = elem->index;
            break;
          }
        }
        if ( elem == NULL )     /* never visited? */
        {
          INSERT_IN_QUEUE( queue, n_points );
          elem = malloc( sizeof(struct hash_entry) );
          elem->link = table[j];
          table[j] = elem;
          elem->index = n_points;
          elem->cx = cx;
          elem->cy = cy;

          /* Set the cx and cy values in the points to be the
           * _multipliers_ along the x and y axes, rather than setting
           * the absolute (true) voxel coordinates. This makes exact
           * comparisons easier. We change the points to be
           * world-coordinate points only in a later processing step.
           */
          fill_Point( pt, cx, cy, 0 );

          /* Save the index of this point in the neighbours array.
           */
          neighbours[i] = n_points;
          ADD_ELEMENT_TO_ARRAY_WITH_SIZE( polygons->points,
                                          n_points_alloced,
                                          n_points,
                                          pt,
                                          DEFAULT_CHUNK_SIZE );
        }
      }
    }
    /* If the neighbours on the right and upper left are
     * present, create a triangle connecting them.
     */
    if ( neighbours[0] >= 0 && neighbours[1] >= 0 )
    {
      ADD_ELEMENT_TO_ARRAY_WITH_SIZE( polygons->indices,
                                      n_indices_alloced,
                                      n_indices,
                                      centre,
                                      DEFAULT_CHUNK_SIZE );
      ADD_ELEMENT_TO_ARRAY_WITH_SIZE( polygons->indices,
                                      n_indices_alloced,
                                      n_indices,
                                      neighbours[0],
                                      DEFAULT_CHUNK_SIZE );
      ADD_ELEMENT_TO_ARRAY_WITH_SIZE( polygons->indices,
                                      n_indices_alloced,
                                      n_indices,
                                      neighbours[1],
                                      DEFAULT_CHUNK_SIZE );
      ADD_ELEMENT_TO_ARRAY_WITH_SIZE( polygons->end_indices,
                                      n_items_alloced,
                                      n_items,
                                      n_indices,
                                      DEFAULT_CHUNK_SIZE );
    }
    /* If the neighbours to the upper left and directly up
     * are present, create a triangle connecting them.
     */
    if ( neighbours[1] >= 0 && neighbours[2] >= 0 )
    {
      ADD_ELEMENT_TO_ARRAY_WITH_SIZE( polygons->indices,
                                      n_indices_alloced,
                                      n_indices,
                                      centre,
                                      DEFAULT_CHUNK_SIZE );
      ADD_ELEMENT_TO_ARRAY_WITH_SIZE( polygons->indices,
                                      n_indices_alloced,
                                      n_indices,
                                      neighbours[1],
                                      DEFAULT_CHUNK_SIZE );
      ADD_ELEMENT_TO_ARRAY_WITH_SIZE( polygons->indices,
                                      n_indices_alloced,
                                      n_indices,
                                      neighbours[2],
                                      DEFAULT_CHUNK_SIZE );
      ADD_ELEMENT_TO_ARRAY_WITH_SIZE( polygons->end_indices,
                                      n_items_alloced,
                                      n_items,
                                      n_indices,
                                      DEFAULT_CHUNK_SIZE );
    }
  }

  DELETE_QUEUE( queue );

  int n = 0;
  int m = 0;
  for_less( i, 0, N_HASH )
  {
    if (table[i] != NULL)
      n++;
    while (table[i] != NULL)
    {
      struct hash_entry *elem = table[i];
      table[i] = elem->link;
      free(elem);
      m++;
    }
  }

  polygons->n_points = n_points;
  polygons->n_items = n_items;

#ifndef NDEBUG
  printf("rate: %f, depth: %f\n", (double)n/N_HASH, (double)m/n);
  printf("%d/%d %d/%d %d/%d\n",
         polygons->n_points,
         n_points_alloced,
         NUMBER_INDICES( *polygons ),
         n_indices_alloced,
         polygons->n_items,
         n_items_alloced);
#endif
}

/**
 * Create the object that represents the cross section of the oblique
 * (arbitrary) view in the 3D view window.
 *
 * \param slice_window The slice window structure.
 * \param volume_index The current volume index.
 * \param object The polygons object which represents the plane.
 * \param origin The current location in voxel coordinates.
 * \param x_axis The current row direction.
 * \param y_axis The current column direction.
 * \param z_axis The current slice direction.
 */
static void
create_cross_section(
  display_struct *slice_window,
  int            volume_index,
  object_struct  *object,
  VIO_Real       origin[],
  VIO_Real       x_axis[],
  VIO_Real       y_axis[],
  VIO_Real       z_axis[] )
{
  int             i, j, k;
  VIO_Real        wx, wy, wz;
  VIO_Vector      normal;
  polygons_struct *polygons;
  VIO_Point       pt;
  VIO_Real        voxel[VIO_MAX_DIMENSIONS];
  int             sizes[VIO_MAX_DIMENSIONS];
  VIO_Volume      volume = get_nth_volume( slice_window, volume_index );
  VIO_Real        points[6][VIO_MAX_DIMENSIONS];
  colour_coding_struct *ccs_ptr;

  polygons = get_polygons_ptr( object );

  get_volume_sizes( volume, sizes );

  convert_voxel_vector_to_world( volume, z_axis, &wx, &wy, &wz );
  fill_Vector( normal, wx, wy, wz );
  NORMALIZE_VECTOR( normal, normal );

  /* We substitute an extreme point on the plane/bounding box
   * intersection for the origin. This guarantees that points on the
   * plane will overlay the sampling grid in a consistent way.
   */
  get_volume_cross_section( volume, origin, x_axis, y_axis, points );
  for_less( i, 0, VIO_N_DIMENSIONS )
  {
    origin[i] = points[0][i];
  }

  /* Create the triangulated surface. */
  expand_surface( origin, x_axis, y_axis, sizes, polygons );

  /* Set the normals */
  REALLOC( polygons->normals, polygons->n_points );
  for_less( i, 0, polygons->n_points )
  {
    polygons->normals[i] = normal;
  }

  /* Set up the colours */
  polygons->colour_flag = PER_ITEM_COLOURS;
  REALLOC( polygons->colours, polygons->n_items );
  ccs_ptr = &slice_window->slice.volumes[volume_index].colour_coding;
  for_less( i, 0, polygons->n_items )
  {
    int      cx, cy;
    VIO_Real val;

    k = (i % 2 == 0) ? (i * 3 + 2) : (i * 3 + 1);
    k = polygons->indices[k];
    pt = polygons->points[k];
    cx = Point_x( pt );
    cy = Point_y( pt );
    for_less(j, 0, VIO_N_DIMENSIONS)
    {
      voxel[j] = origin[j] + cx * x_axis[j] + cy * y_axis[j];
    }

    evaluate_volume( volume, voxel, NULL,
                     slice_window->slice.degrees_continuity,
                     FALSE, 0.0, &val, NULL, NULL );
    polygons->colours[i] = get_colour_code( ccs_ptr, val );
  }

  /* Fix the point coordinates. */
  for_less( i, 0, polygons->n_points )
  {
    int      cx, cy;

    pt = polygons->points[i];
    cx = Point_x( pt );
    cy = Point_y( pt );
    for_less(j, 0, VIO_N_DIMENSIONS)
    {
      voxel[j] = origin[j] + cx * x_axis[j] + cy * y_axis[j];
    }
    convert_voxel_to_world( volume, voxel, &wx, &wy, &wz);
    fill_Point( polygons->points[i], wx, wy, wz );
  }
}


/**
 * Recreate the cross section of the oblique plane for display
 * in the 3D view window.
 *
 * \param slice_window The display_struct of the slice window.
 */
void
rebuild_volume_cross_section( display_struct *slice_window )
{
  VIO_Real       origin[VIO_MAX_DIMENSIONS];
  VIO_Real       x_axis[VIO_MAX_DIMENSIONS];
  VIO_Real       y_axis[VIO_MAX_DIMENSIONS];
  VIO_Real       z_axis[VIO_MAX_DIMENSIONS];
  int            volume_index;
  display_struct *display;

  assert( slice_window->window_type == SLICE_WINDOW );

  display = get_three_d_window( slice_window );

  if( ( volume_index = get_current_volume_index( slice_window )) >= 0 &&
      get_object_visibility( display->three_d.cross_section ) )
  {
    int view_index = get_arbitrary_view_index( slice_window );

    get_slice_plane( slice_window, volume_index, view_index,
                     origin, x_axis, y_axis );

    get_slice_perp_axis( slice_window, volume_index, view_index, z_axis );

    /* Get the true origin. This is different from the one
     * get_slice_plane() fetches.
     */
    get_current_voxel( slice_window, volume_index, origin );

    create_cross_section( slice_window, volume_index,
                          display->three_d.cross_section,
                          origin, x_axis, y_axis, z_axis );

    set_update_required( display, NORMAL_PLANES );
  }
  else
  {
    set_object_visibility( display->three_d.cross_section, FALSE );
  }
}

/**
 * Set the current visibility of the volume "cross section" in the 3D view
 * window. This turns on or off the display of the plane of the current view
 * and the bounding box of the current volume in the 3D view window.
 * \param display The display_struct for any of the top-level windows.
 * \param state True if the plane and box should be displayed.
 */
void
set_volume_cross_section_visibility( display_struct *display, VIO_BOOL state )
{
    display_struct  *slice_window;

    display = get_three_d_window( display );
    state = state && get_n_volumes(display) > 0;

    if( get_slice_window( display, &slice_window ) )
    {
        set_object_visibility( display->three_d.volume_outline, state );
        set_object_visibility( display->three_d.cross_section, state );

        if( state )
        {
            rebuild_volume_cross_section( slice_window );

            G_set_visibility( display->window, TRUE );
        }

        set_update_required( display, NORMAL_PLANES );
    }
}

/**
 * Returns the flag controlling the display of the plane of the
 * current view and the bounding box of the current volume in the 3D
 * view window.
 * \param display The display_struct for any of the top-level windows.
 * \returns The current visibility state of the cross section.
 */

VIO_BOOL
get_volume_cross_section_visibility( display_struct    *display )
{
    VIO_BOOL         state;

    state = get_object_visibility(
                   get_three_d_window(display)->three_d.volume_outline );

    return( state );
}
