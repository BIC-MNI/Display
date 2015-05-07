/**
 \file slice_window/outline.c
 \brief Show outline of current object superimposed on slice view.

 This file contains functions used to superimpose the outline of any
 currently-loaded 3D "objects" onto the currently loaded volume view.

 \author Robert D. Vincent

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
    VIO_Real voxel[VIO_N_DIMENSIONS];

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
  switch (index) {
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
 * Like the functions in intersect/plane_polygons.c, this function
 * calculates the intersection between a plane and a set of polygons.
 * Unlike those older functions, it assigns distinct colours to the 
 * outline it creates.
 * \param display The display_struct for the three-D window.
 * \param plane_normal A vector normal to the current view's plane.
 * \param plane_constant A scalar giving the position in the current view.
 * \param lines_object An object corresponding to the lines we'll display.
 * \param n_points_alloced Number of points in lines_struct.
 * \param n_indices_alloced Number of indices in lines_struct.
 * \param n_end_indices_alloced Number of end_indices in lines_struct.
 */
static void
intersect_plane_with_polygons_coloured(display_struct *display,
                                       VIO_Vector *plane_normal,
                                       VIO_Real plane_constant,
                                       object_struct *lines_object,
                                       int *n_points_alloced,
                                       int *n_indices_alloced,
                                       int *n_end_indices_alloced)
{
  object_traverse_struct object_traverse; 
  lines_struct           *lines;
  VIO_Colour             current_colour;
  object_struct          *current_object;
  int                    colours_size;
  int                    colours_alloced;
  VIO_Colour             *colours_ptr;
  int                    poly_no;
  int                    i;

  /* Get the pointer to our current lines object, and reset its
   * effective size. Note that we separately account for the ALLOCATED
   * size of these arrays in the outline_struct, so these arrays will
   * get allocated early on, and gradually reach some maximum size
   * that corresponds to the longest set of lines needed to represent
   * the object intersection.
   */
  lines = get_lines_ptr(lines_object);

  lines->n_points = 0;
  lines->n_items = 0;

  /* We generate a list of colours for the lines, so that each surface
   * can have a different colour.  If possible, we use the colours of
   * the polygons themselves. However, most polygons are going to be
   * plain white. Rather than just adopt white as our color, we
   * artificially "colourize" the surfaces so that duplicate colours
   * aren't easily possible.
   */
  colours_size = 0;
  colours_alloced = 0;
  colours_ptr = NULL;

  /* This value just counts up the polygons as we see them, it is used
   * only to try to guarantee that we use a "unique" colour for each
   * polygonal object.
   */
  poly_no = 0;

  initialize_object_traverse(&object_traverse, TRUE, N_MODELS, display->models);

  while (get_next_object_traverse(&object_traverse, &current_object))
  {
    if (current_object->object_type == POLYGONS)
    {
      polygons_struct *current_polygons = get_polygons_ptr(current_object);

      /* Ignore empty polygons.
       */
      if (current_polygons->n_items == 0) {
        continue;
      }

      /* This is where we colourize the polygons. Most polygons
       * will just specify a white colour, so we make sure we
       * don't repeat colours if we can avoid it.
       */
      if (!get_object_colour(current_object, &current_colour) ||
          current_colour == WHITE)
      {
        current_colour = get_automatic_colour(poly_no);
      }
      poly_no++;

      for (i = 0; i < current_polygons->n_items; i++) {
        if (intersect_plane_one_polygon(plane_normal,
                                        plane_constant,
                                        current_polygons,
                                        i,
                                        lines,
                                        n_points_alloced,
                                        n_indices_alloced,
                                        n_end_indices_alloced ))
          {
            ADD_ELEMENT_TO_ARRAY_WITH_SIZE(colours_ptr,
                                           colours_alloced,
                                           colours_size,
                                           current_colour,
                                           DEFAULT_CHUNK_SIZE);
          }
      }
    }
  }

  if (lines->colours != NULL)
  {
    FREE(lines->colours);
  }
  set_object_colours(lines_object, colours_ptr);
  lines->colour_flag = PER_ITEM_COLOURS;
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

  /* If the display of the outline isn't enabled, just bail out
   * immediately.
   */
  if (!Object_outline_enabled)
    return;

  /* See if we should display anything.
   */
  volume_index = get_current_volume_index(slice_window);
  if (volume_index < 0)
    return;
  if (!get_slice_visibility(slice_window, volume_index, view_index))
    return;

  display = get_three_d_window(slice_window);
  world_origin = display->three_d.cursor.origin;

  /* Get the axis perpendicular to this view.
   */
  get_slice_perp_axis(slice_window, volume_index, view_index, perp_axis);

  /* Convert the perpendicular axis to the actual VIO_Vector form that 
   * we'll need later.
   */
  fill_Vector(plane_normal, perp_axis[0], perp_axis[1], perp_axis[2]);

  /* Get the constant along the normal axis by taking the dot product
   * of the origin with the normal.
   */
  plane_constant = (Point_x(world_origin) * Vector_x(plane_normal) +
                    Point_y(world_origin) * Vector_y(plane_normal) + 
                    Point_z(world_origin) * Vector_z(plane_normal));

  outline_ptr = &slice_window->slice.outlines[view_index];

  /* Calculate the intersection between the polygons and the plane.
   */
  intersect_plane_with_polygons_coloured(display,
                                         &plane_normal,
                                         plane_constant,
                                         outline_ptr->lines,
                                         &outline_ptr->n_points_alloced,
                                         &outline_ptr->n_indices_alloced,
                                         &outline_ptr->n_end_indices_alloced);

  /* Convert the lines to pixel coordinates.
   */
  convert_world_lines_to_pixel(outline_ptr->lines, slice_window, volume_index, 
                               view_index);

  set_update_required( slice_window,
                       get_model_bitplanes(get_graphics_model(slice_window, 
                                                              SLICE_MODEL1 + view_index)));
}
