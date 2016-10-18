/**
 \file slice_window/outline.c
 \brief Show outline of current object superimposed on slice view.

 This file contains functions used to superimpose the outline of any
 currently-loaded 3D "objects" onto the currently loaded volume view.

 \author Robert D. Vincent

\copyright
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

/**
 * Initialize the object outline display on the slice.
 *
 * This function should always called, even if the actual outline
 * drawing is not enabled (e.g. if Object_outline_enabled is FALSE).
 * Otherwise necessary data structures might not be properly set up
 * when the outline is enabled.
 */
void
initialize_slice_object_outline(display_struct *display)
{
  int            view_index;
  object_struct  *object;
  lines_struct   *lines;
  model_struct   *model;
  display_struct *slice_window;

  if ( !get_slice_window( display, &slice_window ) )
    return;

  for_less( view_index, 0, N_SLICE_VIEWS )
  {
    model = get_graphics_model( slice_window, SLICE_MODEL1 + view_index );
    object = create_object( LINES );
    lines = get_lines_ptr( object );
    initialize_lines( lines, WHITE );
    lines->line_thickness = Object_outline_width;

    add_object_to_model( model, object );

    slice_window->slice.outlines[view_index].lines = object;
    slice_window->slice.outlines[view_index].n_points_alloced = 0;
    slice_window->slice.outlines[view_index].n_indices_alloced = 0;
    slice_window->slice.outlines[view_index].n_end_indices_alloced = 0;
  }
}

/**
 * Converts each point on a lines_struct from 3D world coordinates to
 * 2D pixel coordinates.
 *
 * \param lines_object The object_struct that corresponds to the lines
 * we will draw in the appropriate view.
 * \param slice_window A pointer to the slice window's display_struct.
 * \param volume_index The index of the current volume.
 * \param view_index The index of the current view.
 */
static void
convert_world_lines_to_pixel(object_struct *lines_object,
                             display_struct *slice_window,
                             int volume_index,
                             int view_index)
{
  VIO_Volume volume = get_volume(slice_window);
  lines_struct *lines = get_lines_ptr(lines_object);
  int i;

  for (i = 0; i < lines->n_points; i++)
  {
    VIO_Real x;
    VIO_Real y;
    VIO_Real voxel[VIO_MAX_DIMENSIONS];

    convert_world_to_voxel(volume,
                           Point_x(lines->points[i]),
                           Point_y(lines->points[i]),
                           Point_z(lines->points[i]),
                           voxel);
    convert_voxel_to_pixel(slice_window, volume_index, view_index,
                           voxel, &x, &y);
    fill_Point(lines->points[i], x, y, 0.0);
  }
}

/**
 * Given an an arbitrary object "index", generate a colour
 * which should be different from the previous colour in a significant
 * way, and probably unlike other previous colours.
 * \param index An arbitrary index we use to differentiate the contexts
 * in which the colours are used.
 */
static VIO_Colour
get_automatic_colour(int index)
{
  switch (index)
  {
  case 0:
    return GREEN;
  case 1:
    return BLUE;
  case 2:
    return RED;
  case 3:
    return YELLOW;
  case 4:
    return MAGENTA;
  case 5:
    return CYAN;
  case 6:
    return ORANGE;
  default:
    return WHITE;
  }
}

/**
 * Function to return points for the four possible corners of
 * a square.
 */
static void
square_points(int i, int n, VIO_Real *p_cx, VIO_Real *p_cy)
{
  switch ( i ) {
  case 0:
    *p_cx = 1.0;
    *p_cy = 1.0;
    break;
  case 1:
    *p_cx = 1.0;
    *p_cy = -1.0;
    break;
  case 2:
    *p_cx = -1.0;
    *p_cy = -1.0;
    break;
  case 3:
    *p_cx = -1.0;
    *p_cy = 1.0;
    break;
  default:
    handle_internal_error("Illegal point for square.");
    break;
  }
}

/**
 * Function to return one of n pairs of points for a unit circle.
 */
static void
circle_points(int i, int n, VIO_Real *p_cx, VIO_Real *p_cy)
{
  VIO_Real theta = 2.0 * M_PI * (VIO_Real) i / (VIO_Real) n;
  *p_cx = cos( theta );
  *p_cy = sin( theta );
}

/**
 * Calculates the intersection between a plane and a set of markers.
 *
 * \param display The display_struct for the three-D window.
 * \param plane_normal A vector normal to the current view's plane.
 * \param plane_constant A scalar giving the position in the current view.
 * \param x_axis Direction of x axis.
 * \param y_axis Direction of y axis.
 * \param lines A structure representing the lines to display.
 * \param n_points_alloced Number of points in lines_struct.
 * \param n_indices_alloced Number of indices in lines_struct.
 * \param n_end_indices_alloced Number of end_indices in lines_struct.
 */
static void
intersect_plane_with_markers(display_struct *display,
                             VIO_Vector *plane_normal,
                             VIO_Real plane_constant,
                             VIO_Real x_axis[],
                             VIO_Real y_axis[],
                             lines_struct *lines,
                             int *n_points_alloced,
                             int *n_indices_alloced,
                             int *n_end_indices_alloced)
{
  object_traverse_struct object_traverse;
  object_struct          *current_object;

  initialize_object_traverse( &object_traverse, TRUE, 1,
                              &display->models[THREED_MODEL] );

  while ( get_next_object_traverse(&object_traverse, &current_object) )
  {
    if ( current_object->object_type == MARKER &&
         get_object_visibility( current_object ))
    {
      marker_struct *marker = get_marker_ptr( current_object );
      VIO_Real dist = ( DOT_POINT_VECTOR( marker->position, *plane_normal ) -
                        plane_constant );

      /* Marker intersects plane if and only if the distance is less than
       * the marker size.
       */
      if ( fabs(dist) < marker->size )
      {
        VIO_Point centre;
        VIO_Point pt;
        VIO_Real  radius;
        int       point_index = lines->n_points;
        int       n_indices = NUMBER_INDICES( *lines );
        int       i;
        int       n_segments;
        void      (*calc_points)(int, int, VIO_Real *, VIO_Real *);

        fill_Point(centre,
                   Point_x(marker->position) + dist * Vector_x(*plane_normal),
                   Point_y(marker->position) + dist * Vector_y(*plane_normal),
                   Point_z(marker->position) + dist * Vector_z(*plane_normal));

        if (marker->type == BOX_MARKER)
        {
          n_segments = 4;
          radius = marker->size;
          calc_points = square_points;
        }
        else
        {
          n_segments = 16;
          radius = sqrt(marker->size * marker->size - dist * dist);
          calc_points = circle_points;
        }

        for_less( i, 0, n_segments )
        {
          VIO_Real cx;
          VIO_Real cy;

          (*calc_points)(i, n_segments, &cx, &cy);

          fill_Point(pt,
                     Point_x(centre) + radius*(cx*x_axis[0] + cy*y_axis[0]),
                     Point_y(centre) + radius*(cx*x_axis[1] + cy*y_axis[1]),
                     Point_z(centre) + radius*(cx*x_axis[2] + cy*y_axis[2]));
          ADD_ELEMENT_TO_ARRAY_WITH_SIZE( lines->points,
                                          *n_points_alloced,
                                          lines->n_points,
                                          pt,
                                          DEFAULT_CHUNK_SIZE );
          ADD_ELEMENT_TO_ARRAY_WITH_SIZE( lines->indices,
                                          *n_indices_alloced,
                                          n_indices,
                                          point_index + i,
                                          DEFAULT_CHUNK_SIZE );
        }
        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( lines->indices,
                                        *n_indices_alloced,
                                        n_indices,
                                        point_index,
                                        DEFAULT_CHUNK_SIZE );

        int n_items = lines->n_items;
        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( lines->end_indices,
                                        *n_end_indices_alloced,
                                        lines->n_items,
                                        n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY(lines->colours,
                             n_items,
                             marker->colour,
                             DEFAULT_CHUNK_SIZE);
      }
    }
  }
}

/**
 * Get an appropriate colour for a line segment.
 */
static VIO_Colour
get_line_segment_colour(lines_struct *lines, int iindex, int pindex)
{
  switch ( lines->colour_flag )
  {
  case PER_ITEM_COLOURS:
    return lines->colours[iindex];
  case PER_VERTEX_COLOURS:
    return lines->colours[pindex];
  default:
    return lines->colours[0];
  }
}

/**
 * This function calculates the intersection between a plane and a set
 * of lines. Line segments are displayed on the plane if they are
 * within a specified distance of the plane. It assigns distinct
 * colours to the segments of outline it creates.
 *
 * \param display The display_struct for the three-D window.
 * \param plane_normal A vector normal to the current view's plane.
 * \param plane_constant A scalar giving the position in the current view.
 * \param dst_lines A structure representing the outline to display.
 * \param n_points_alloced Number of points in lines_struct.
 * \param n_indices_alloced Number of indices in lines_struct.
 * \param n_end_indices_alloced Number of end_indices in lines_struct.
 */
static void
intersect_plane_with_lines_coloured(display_struct *display,
                                    VIO_Vector *plane_normal,
                                    VIO_Real plane_constant,
                                    lines_struct *dst_lines,
                                    int *n_points_alloced,
                                    int *n_indices_alloced,
                                    int *n_end_indices_alloced)
{
  object_traverse_struct object_traverse;
  object_struct          *current_object;
  int                    item;

  initialize_object_traverse(&object_traverse, TRUE, 1,
                             &display->models[THREED_MODEL]);

  while (get_next_object_traverse(&object_traverse, &current_object))
  {
    if (current_object->object_type == LINES &&
        get_object_visibility( current_object ))
    {
      lines_struct *src_lines = get_lines_ptr(current_object);
      int start_index = 0;

      if (src_lines->n_items == 0)
      {
        continue;
      }

      for (item = 0; item < src_lines->n_items;
           start_index = src_lines->end_indices[item++])
      {
        int size = GET_OBJECT_SIZE(*src_lines, item);
        int n_indices = NUMBER_INDICES(*dst_lines);
        int j;

        for_less( j, 0, size - 1 )
        {
          int i1 = src_lines->indices[start_index + j];
          int i2 = src_lines->indices[start_index + j + 1];

          VIO_Point p1 = src_lines->points[i1];
          VIO_Point p2 = src_lines->points[i2];

          VIO_Real d1 = DIST_FROM_PLANE(*plane_normal, plane_constant, p1);
          VIO_Real d2 = DIST_FROM_PLANE(*plane_normal, plane_constant, p2);

          if (fabs(d1) < Object_outline_distance &&
              fabs(d2) < Object_outline_distance)
          {
            int point_index = dst_lines->n_points;

            ADD_ELEMENT_TO_ARRAY_WITH_SIZE(dst_lines->points,
                                           *n_points_alloced,
                                           dst_lines->n_points,
                                           p1, DEFAULT_CHUNK_SIZE);

            ADD_ELEMENT_TO_ARRAY_WITH_SIZE(dst_lines->points,
                                           *n_points_alloced,
                                           dst_lines->n_points,
                                           p2, DEFAULT_CHUNK_SIZE);

            ADD_ELEMENT_TO_ARRAY_WITH_SIZE(dst_lines->indices,
                                           *n_indices_alloced,
                                           n_indices,
                                           point_index,
                                           DEFAULT_CHUNK_SIZE);

            ADD_ELEMENT_TO_ARRAY_WITH_SIZE(dst_lines->indices,
                                           *n_indices_alloced,
                                           n_indices,
                                           point_index+1,
                                           DEFAULT_CHUNK_SIZE);

            ADD_ELEMENT_TO_ARRAY_WITH_SIZE(dst_lines->end_indices,
                                           *n_end_indices_alloced,
                                           dst_lines->n_items,
                                           n_indices,
                                           DEFAULT_CHUNK_SIZE);
            int n_colours = dst_lines->n_items - 1;
            ADD_ELEMENT_TO_ARRAY(dst_lines->colours,
                                 n_colours,
                                 get_line_segment_colour(src_lines, item, i1),
                                 DEFAULT_CHUNK_SIZE);
          }
        }
      }
    }
  }
}

/**
 * Like the functions in intersect/plane_polygons.c, this function
 * calculates the intersection between a plane and a set of polygons.
 * Unlike those older functions, it assigns distinct colours to the
 * outline it creates.
 *
 * We generate a list of colours for the lines, so that each surface
 * can have a different colour.  If possible, we use the colours of
 * the polygons themselves. However, most polygons are going to be
 * plain white. Rather than just adopt white as our color, we
 * artificially "colourize" the surfaces so that duplicate colours
 * aren't easily possible.
 *
 * The value poly_no just counts up the polygons as we see them, it is
 * used to try to guarantee that we assign a consistent colour for
 * each polygonal object.
 *
 * \param plane_normal A vector normal to the current view's plane.
 * \param plane_constant A scalar giving the position in the current view.
 * \param lines A structure representing the lines to display.
 * \param n_points_alloced Number of points in lines_struct.
 * \param n_indices_alloced Number of indices in lines_struct.
 * \param n_end_indices_alloced Number of end_indices in lines_struct.
 * \param poly_no Pointer to the polygon index.

 * \param model_ptr The model that may contain polygons.
 * \param is_visible TRUE if this model (and its parent) is visible.
 */
static void
intersect_plane_with_polygons_coloured(VIO_Vector *plane_normal,
                                       VIO_Real plane_constant,
                                       lines_struct *lines,
                                       int *n_points_alloced,
                                       int *n_indices_alloced,
                                       int *n_end_indices_alloced,
                                       int *poly_no,
                                       model_struct *model_ptr,
                                       VIO_BOOL is_visible)
{
  int i_obj;

  for (i_obj = 0; i_obj < model_ptr->n_objects; i_obj++)
  {
    object_struct *current_object = model_ptr->objects[i_obj];
    VIO_BOOL current_visible = get_object_visibility(current_object);

    if (current_object->object_type == MODEL)
    {
      intersect_plane_with_polygons_coloured(plane_normal,
                                             plane_constant,
                                             lines,
                                             n_points_alloced,
                                             n_indices_alloced,
                                             n_end_indices_alloced,
                                             poly_no,
                                             get_model_ptr(current_object),
                                             is_visible && current_visible);
    }
    else if (current_object->object_type == POLYGONS)
    {
      polygons_struct *current_polygons = get_polygons_ptr(current_object);

      /* Ignore empty polygons.
       */
      if (current_polygons->n_items == 0)
      {
        continue;
      }

      if (is_visible && current_visible)
      {
        VIO_Colour             current_colour;
        int                    item;

        /* This is where we colourize the polygons. Most polygons
         * will just specify a white colour, so we make sure we
         * don't repeat colours if we can avoid it.
         */
        if (!get_object_colour(current_object, &current_colour) ||
            current_colour == WHITE)
        {
          current_colour = get_automatic_colour(*poly_no);
        }

        for (item = 0; item < current_polygons->n_items; item++)
        {
          if (intersect_plane_one_polygon(plane_normal,
                                          plane_constant,
                                          current_polygons,
                                          item,
                                          lines,
                                          n_points_alloced,
                                          n_indices_alloced,
                                          n_end_indices_alloced ))
          {
            int n_items = lines->n_items - 1;
            ADD_ELEMENT_TO_ARRAY(lines->colours,
                                 n_items,
                                 current_colour,
                                 DEFAULT_CHUNK_SIZE);
          }
        }
      }
      (*poly_no)++;             /* Increment even if invisible. */
    }
  }
}

/**
 * Recalculates the "object outline" for a slice view.
 *
 * When superimposing a 3D object onto a volume, the slice view includes a
 * trace of the 3D object on the currently viewed plane. The code looks through
 * all of the currently loaded 3D polygon objects, and calculates the line
 * segments that comprise the intersection between the polygons and the view
 * plane. This works for the oblique slice as well as the orthogonal slices.
 *
 * \param slice_window The slice window structure.
 * \param view_index The index of the view to update (0..N_SLICE_VIEWS-1)
 */
void
rebuild_slice_object_outline(display_struct *slice_window, int view_index)
{
  VIO_Vector     plane_normal;
  VIO_Real       plane_constant;
  int            volume_index;
  outline_struct *outline_ptr;
  display_struct *display;
  VIO_Real       perp_axis[VIO_N_DIMENSIONS];
  VIO_Point      world_origin;
  VIO_Real       xw, yw, zw;
  VIO_Real       origin[VIO_MAX_DIMENSIONS];
  VIO_Real       x_axis[VIO_MAX_DIMENSIONS];
  VIO_Real       y_axis[VIO_MAX_DIMENSIONS];
  lines_struct   *lines;
  int            poly_no = 0;

  /* If the display of the outline isn't enabled, just bail out
   * immediately.
   */
  if (!Object_outline_enabled)
    return;

  display = get_three_d_window(slice_window);

  /* See if we should display anything.
   */
  volume_index = get_current_volume_index(slice_window);
  if (volume_index < 0)
    return;

  outline_ptr = &slice_window->slice.outlines[view_index];

  if (!get_slice_visibility(slice_window, volume_index, view_index))
  {
    if (get_object_visibility(outline_ptr->lines))
    {
      set_object_visibility(outline_ptr->lines, FALSE);
      set_update_required(slice_window, NORMAL_PLANES);
    }
    return;
  }

  get_cursor_origin(slice_window, &world_origin);

  /* Get the axis perpendicular to this view. This is in voxel coordinates.
   */
  get_slice_perp_axis(slice_window, volume_index, view_index, perp_axis);

  get_slice_plane(slice_window, volume_index, view_index, origin,
                  x_axis, y_axis);

  /* Convert the perpendicular axis from voxel to world coordinates.
   */
  convert_voxel_vector_to_world(get_nth_volume(slice_window, volume_index),
                                perp_axis, &xw, &yw, &zw);

  /* Convert the perpendicular axis to the actual VIO_Vector form that
   * we'll need later.
   */
  fill_Vector(plane_normal, xw, yw, zw);

  /* Get the constant along the normal axis by taking the dot product
   * of the origin with the normal.
   */
  plane_constant = (Point_x(world_origin) * Vector_x(plane_normal) +
                    Point_y(world_origin) * Vector_y(plane_normal) +
                    Point_z(world_origin) * Vector_z(plane_normal));

  /* Get the pointer to our current lines object, and reset its
   * effective size. Note that we separately account for the ALLOCATED
   * size of these arrays in the outline_struct, so these arrays will
   * get allocated early on, and gradually reach some maximum size
   * that corresponds to the longest set of lines needed to represent
   * the object intersection.
   */
  lines = get_lines_ptr( outline_ptr->lines );
  lines->n_points = 0;
  lines->n_items = 0;
  if (lines->colours != NULL)
  {
    FREE(lines->colours);
    lines->colours = NULL;
  }
  lines->colour_flag = PER_ITEM_COLOURS;

  /* Calculate the intersection between the polygons and the plane.
   */
  intersect_plane_with_polygons_coloured(&plane_normal,
                                         plane_constant,
                                         lines,
                                         &outline_ptr->n_points_alloced,
                                         &outline_ptr->n_indices_alloced,
                                         &outline_ptr->n_end_indices_alloced,
                                         &poly_no,
                                         get_model_ptr(display->models[THREED_MODEL]),
                                         TRUE);

  /* Calculate the intersection between any lines and the plane.
   */
  intersect_plane_with_lines_coloured(display,
                                      &plane_normal,
                                      plane_constant,
                                      lines,
                                      &outline_ptr->n_points_alloced,
                                      &outline_ptr->n_indices_alloced,
                                      &outline_ptr->n_end_indices_alloced);

  if (Show_markers_on_slice)
    intersect_plane_with_markers( display,
                                  &plane_normal,
                                  plane_constant,
                                  x_axis, y_axis,
                                  lines,
                                  &outline_ptr->n_points_alloced,
                                  &outline_ptr->n_indices_alloced,
                                  &outline_ptr->n_end_indices_alloced);


  /* Convert the lines to pixel coordinates.
   */
  convert_world_lines_to_pixel(outline_ptr->lines, slice_window, volume_index,
                               view_index);

  set_object_visibility(outline_ptr->lines, TRUE);

  set_update_required( slice_window, NORMAL_PLANES);
}
