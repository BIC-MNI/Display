/**
 * \file line_ops.c
 * \brief Menu commands for creating 3D lines objects.
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
#include  <bicpl/splines.h>

static  VIO_BOOL  get_current_lines(
    display_struct    *display,
    lines_struct      **lines )
{
    VIO_BOOL          found;
    object_struct    *current_object;

    if( get_current_object( display, &current_object ) &&
        get_object_type( current_object ) == LINES )
    {
        *lines = get_lines_ptr( current_object );
        found = TRUE;
    }
    else
        found = FALSE;

    return( found );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( smooth_current_lines )
{
    VIO_Status      status;
    lines_struct    *lines;
    VIO_Real        smooth_distance;

    status = VIO_OK;

    if( get_current_lines( display, &lines ) )
    {
        status = get_user_input( "Enter smoothing distance: ", "r",
                                 &smooth_distance );

        if( status == VIO_OK )
        {
            smooth_lines( lines, smooth_distance );
            set_update_required( display, NORMAL_PLANES );
        }
    }

    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(smooth_current_lines )
{
    return( current_object_is_this_type( display, LINES ) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( make_current_line_tube )
{
    VIO_Status      status;
    lines_struct    *lines;
    VIO_Real        radius;
    int             n_around;

    status = VIO_OK;

    if( get_current_lines( display, &lines ) )
    {
        status = get_user_input( "Enter n_around radius: ", "dr",
                                 &n_around,
                                 &radius );

        if( status == VIO_OK )
            convert_lines_to_tubes_objects( display, lines, n_around, radius );

        if( status == VIO_OK )
            graphics_models_have_changed( display );
    }

    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(make_current_line_tube )
{
    return( current_object_is_this_type( display, LINES ) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( convert_line_to_spline_points )
{
    object_struct   *object;
    lines_struct    *lines;
    lines_struct    new_lines;
    render_struct   *render;

    if( get_current_lines( display, &lines ) )
    {
        render = get_main_render( display );

        create_line_spline( lines, render->n_curve_segments, &new_lines );

        object = create_object( LINES );

        *(get_lines_ptr(object)) = new_lines;
        add_object_to_current_model( display, object );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(convert_line_to_spline_points )
{
    return( current_object_is_this_type( display, LINES ) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( make_line_circle )
{
    VIO_Point         centre;
    VIO_Real          x_size, y_size;
    int               plane_axis, n_around;
    object_struct     *object;


    if (get_user_input(
           "Enter x_centre, y_centre, z_centre, plane_axis, x_size, y_size\n"
           "      n_around: ", "fffdrrd",
           &Point_x(centre),
           &Point_y(centre),
           &Point_z(centre),
           &plane_axis,
           &x_size,
           &y_size,
           &n_around ) == VIO_OK)
    {
        if (plane_axis < 0 || plane_axis > 2)
        {
            print("Plane must be 0, 1, or 2.\n");
        }
        else
        {
            object = create_object( LINES );

            create_line_circle( &centre, plane_axis, x_size, y_size,
                                n_around, get_lines_ptr(object) );

            get_lines_ptr(object)->colours[0] = WHITE;
            add_object_to_current_model( display, object );
        }
    }
    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(make_line_circle )
{
    return( TRUE );
}


/* ARGSUSED */

DEF_MENU_FUNCTION( subdivide_current_lines )
{
    lines_struct      *lines;

    if( get_current_lines( display, &lines ) )
    {
        subdivide_lines( lines );

        graphics_models_have_changed( display );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(subdivide_current_lines )
{
    return( current_object_is_this_type( display, LINES ) );
}

/**
 * Convert any markers contained within the current object to lines.
 */
static  void  convert_to_lines(
    display_struct   *display,
    VIO_BOOL          closed )
{
    lines_struct            *lines;
    int                     i, c, n_markers, curr_index, max_index;
    VIO_Real                    dist, curr_dist, ratio, next_dist;
    VIO_Real                    desired_dist;
    VIO_Point                   *markers, point;
    VIO_Point                   p1, p2, p3, p4;
    int                     n_points;
    object_struct           *object, *current_object;
    object_traverse_struct  object_traverse;
    VIO_BOOL                 interpolate;

    if( !get_current_object( display, &current_object ) )
    {
        print( "Current object is not a marker or model containing markers.\n");
        return;
    }

    n_markers = 0;
    markers = NULL;

    initialize_object_traverse( &object_traverse, FALSE, 1, &current_object );

    while( get_next_object_traverse(&object_traverse,&object) )
    {
        if( object->object_type == MARKER )
        {
            ADD_ELEMENT_TO_ARRAY( markers, n_markers,
                                  get_marker_ptr(object)->position,
                                  DEFAULT_CHUNK_SIZE );
        }
    }

    if( n_markers >= 4 )
    {
        dist = 0.0;
        for_less( i, 0, n_markers-1 )
            dist += distance_between_points( &markers[i], &markers[i+1] );

        if( closed )
            dist += distance_between_points( &markers[n_markers-1],
                                             &markers[0] );

        if (get_user_input( "Enter number of points desired: ", "d",
                            &n_points ) == VIO_OK )
        {
            interpolate = (n_points >= 2);
            if( !interpolate )
                n_points = n_markers;

            object = create_object( LINES );
            lines = get_lines_ptr( object );
            initialize_lines( lines, WHITE );
            lines->n_points = n_points;
            lines->n_items = 1;

            ALLOC( lines->points, lines->n_points );
            ALLOC( lines->end_indices, 1 );
            lines->end_indices[0] = n_points;
            if( closed )
                ++lines->end_indices[0];
            ALLOC( lines->indices, lines->end_indices[0] );

            for_less( i, 0, lines->end_indices[0] )
                lines->indices[i] = i % n_points;

            if( interpolate )
            {
                curr_index = 0;
                curr_dist = 0.0;
                next_dist = distance_between_points( &markers[0], &markers[1] );

                if( closed )
                    max_index = n_markers-1;
                else
                    max_index = n_markers-2;

                for_less( i, 0, n_points )
                {
                    if( closed )
                        desired_dist = (VIO_Real) i / (VIO_Real) n_points * dist;
                    else
                        desired_dist = (VIO_Real) i / (VIO_Real) (n_points-1) * dist;

                    while( curr_index < max_index && desired_dist >= next_dist )
                    {
                        ++curr_index;
                        curr_dist = next_dist;
                        next_dist += distance_between_points(
                                       &markers[curr_index],
                                       &markers[(curr_index+1)%n_markers] );
                    }

                    if( curr_index == 0 && closed )
                        p1 = markers[n_markers-1];
                    else if( curr_index == 0 && !closed )
                        p1 = markers[0];
                    else
                        p1 = markers[curr_index-1];

                    p2 = markers[curr_index];

                    if( curr_index + 1 > n_markers - 1 && closed )
                        p3 = markers[(curr_index+1) % n_markers];
                    else if( curr_index + 1 > n_markers - 1 && !closed )
                        p3 = markers[n_markers-1];
                    else
                        p3 = markers[curr_index+1];

                    if( curr_index + 2 > n_markers - 1 && closed )
                        p4 = markers[(curr_index+2) % n_markers];
                    else if( curr_index + 2 > n_markers - 1 && !closed )
                        p4 = markers[n_markers-1];
                    else
                        p4 = markers[curr_index+2];

                    ratio = (desired_dist - curr_dist) /(next_dist - curr_dist);
                    if( ratio < 0.0 || ratio > 1.0 )
                        handle_internal_error( "Dang.\n" );

                    for_less( c, 0, VIO_N_DIMENSIONS )
                    {
                        Point_coord(point,c) = (float) cubic_interpolate( ratio,
                          (VIO_Real) Point_coord(p1,c), (VIO_Real) Point_coord(p2,c),
                          (VIO_Real) Point_coord(p3,c), (VIO_Real) Point_coord(p4,c) );
                    }
                    lines->points[i] = point;
                }
            }
            else
            {
                for_less( i, 0, n_points )
                    lines->points[i] = markers[i];
            }

            add_object_to_current_model( display, object );
        }
    }

    if( n_markers > 0 )
        FREE( markers );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( convert_markers_to_lines )
{
    convert_to_lines( display, FALSE );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(convert_markers_to_lines )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( convert_markers_to_closed_lines )
{
    convert_to_lines( display, TRUE );

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(convert_markers_to_closed_lines )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( set_line_widths )
{
    VIO_Status          status;
    lines_struct    *lines;
    VIO_Real            width;

    status = VIO_OK;

    if( get_current_lines( display, &lines ) )
    {
        if (get_user_input( "Enter line width: ", "r", &width) == VIO_OK)
        {
            lines->line_thickness = (float) width;
            set_update_required( display, NORMAL_PLANES );
        }
    }

    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(set_line_widths )
{
    return( current_object_is_this_type( display, LINES ) );
}

VIO_Colour
points_to_colour( VIO_Point *pt0, VIO_Point *pt1 )
{
  VIO_Real dx = fabs( Point_x(*pt0) - Point_x(*pt1) );
  VIO_Real dy = fabs( Point_y(*pt0) - Point_y(*pt1) );
  VIO_Real dz = fabs( Point_z(*pt0) - Point_z(*pt1) );
  VIO_Real scale = dx + dy + dz;
  return ( scale == 0 ) ? WHITE : make_Colour_0_1( dx / scale,
                                                   dy / scale,
                                                   dz / scale );
}

void
colourize_lines_by_xyz( lines_struct *lines_ptr, VIO_BOOL per_vertex )
{
  int        n_colours = 0;
  int        item;
  int        n_vertices;
  VIO_Colour col;
  int        a, b;

  if ( per_vertex )
  {
    lines_ptr->colour_flag = PER_VERTEX_COLOURS;
  }
  else
  {
    lines_ptr->colour_flag = PER_ITEM_COLOURS;
  }
  FREE( lines_ptr->colours );

  if ( per_vertex )
  {
    int vtx;

    for ( item = 0; item < lines_ptr->n_items; item++ )
    {
      n_vertices = GET_OBJECT_SIZE( *lines_ptr, item );

      for ( vtx = 0; vtx < n_vertices - 1; vtx++ )
      {
        a = lines_ptr->indices[POINT_INDEX( lines_ptr->end_indices, item, vtx + 0 )];
        b = lines_ptr->indices[POINT_INDEX( lines_ptr->end_indices, item, vtx + 1 )];
        col = points_to_colour( &lines_ptr->points[a], &lines_ptr->points[b] );

        ADD_ELEMENT_TO_ARRAY( lines_ptr->colours, n_colours, col,
                              DEFAULT_CHUNK_SIZE );
      }
      ADD_ELEMENT_TO_ARRAY( lines_ptr->colours, n_colours, col,
                            DEFAULT_CHUNK_SIZE );
    }
  }
  else
  {
    for ( item = 0; item < lines_ptr->n_items; item++ )
    {
      n_vertices = GET_OBJECT_SIZE( *lines_ptr, item );
      a = lines_ptr->indices[POINT_INDEX( lines_ptr->end_indices, item, 0 )];
      b = lines_ptr->indices[POINT_INDEX( lines_ptr->end_indices, item, n_vertices - 1 )];
      col = points_to_colour( &lines_ptr->points[a], &lines_ptr->points[b] );
      ADD_ELEMENT_TO_ARRAY( lines_ptr->colours, n_colours, col,
                            DEFAULT_CHUNK_SIZE );
    }
  }
}

DEF_MENU_FUNCTION( menu_colourize_lines )
{
  lines_struct *lines_ptr;

  if (get_current_lines( display, &lines_ptr ))
  {
    colourize_lines_by_xyz( lines_ptr, TRUE );
    graphics_models_have_changed( display );
  }
  return VIO_OK;
}

DEF_MENU_UPDATE( menu_colourize_lines )
{
  return( current_object_is_this_type( display, LINES ) );
}

/**
 * Get the length of a particular line segment.
 * \param lines_ptr The lines object.
 * \param item The index of the segment.
 * \returns The total length of the segment.
 */
VIO_Real
get_segment_length( lines_struct *lines_ptr, int item )
{
  VIO_Real len = 0.0;

  if ( item >= 0 && item < lines_ptr->n_items )
  {
    int n_vertices = GET_OBJECT_SIZE( *lines_ptr, item );
    int i, a, b;

    for ( i = 0; i < n_vertices - 1; i++ )
    {
      a = lines_ptr->indices[POINT_INDEX(lines_ptr->end_indices, item, i + 0)];
      b = lines_ptr->indices[POINT_INDEX(lines_ptr->end_indices, item, i + 1)];
      len += distance_between_points( &lines_ptr->points[a],
                                      &lines_ptr->points[b] );
    }
  }
  return( len );
}

/**
 * Create a new lines object, deleting lines below some fixed length
 * threshold.
 *
 * \param lines_ptr The lines object to copy.
 * \param min_length The minimum line length to copy.
 * \returns A new lines object consisting of only the lines over the 
 * desired threshold.
 */
object_struct *
filter_lines_by_length( lines_struct *lines_ptr, VIO_Real min_length )
{
  int           item;
  object_struct *object_ptr;
  lines_struct  *new_lines_ptr;

  object_ptr = create_object( LINES );
  new_lines_ptr = get_lines_ptr( object_ptr );
  initialize_lines( new_lines_ptr, WHITE );

  for ( item = 0; item < lines_ptr->n_items; item++ )
  {
    VIO_Real length = get_segment_length( lines_ptr, item );
    if ( length >= min_length )
    {
      int n = GET_OBJECT_SIZE( *lines_ptr, item );
      int i, j;

      start_new_line( new_lines_ptr );
      for ( i = 0; i < n; i++ )
      {
        j = lines_ptr->indices[POINT_INDEX(lines_ptr->end_indices, item, i)];
        add_point_to_line( new_lines_ptr, &lines_ptr->points[j] );
      }
    }
  }
  return object_ptr;
}

DEF_MENU_FUNCTION( menu_threshold_lines )
{
  lines_struct *lines_ptr;
  object_struct *object_ptr;

  if (get_current_lines( display, &lines_ptr ))
  {
    VIO_Real thresh;
    if ( get_user_input( "Enter length threshold: ", "r", &thresh ) == VIO_OK )
    {
      object_ptr = filter_lines_by_length( lines_ptr,  thresh );
      add_object_to_current_model( display, object_ptr );
      graphics_models_have_changed( display );
    }
  }
  return VIO_OK;
}

DEF_MENU_UPDATE( menu_threshold_lines )
{
  return( current_object_is_this_type( display, LINES ) );
}
