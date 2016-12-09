/**
 * \file polygon_ops.c
 * \brief Menu commands to operate on polygons.
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

  VIO_BOOL  get_current_polygons(
    display_struct      *display,
    polygons_struct     **polygons )
{
    VIO_BOOL                 found;
    object_struct           *current_object, *object;
    object_traverse_struct  object_traverse;

    found = FALSE;

    if( get_current_object( display, &current_object ) )
    {
        initialize_object_traverse( &object_traverse, FALSE, 1,&current_object);

        while( get_next_object_traverse(&object_traverse,&object) )
        {
            if( !found && object->object_type == POLYGONS &&
                get_polygons_ptr(object)->n_items > 0 )
            {
                found = TRUE;
                *polygons = get_polygons_ptr(object);
            }
        }
    }

    return( found );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( input_polygons_bintree )
{
    VIO_Status            status;
    polygons_struct   *polygons;
    VIO_STR            filename;
    FILE              *file;

    status = VIO_OK;

    if( get_current_polygons(display,&polygons) &&
        polygons->bintree == (bintree_struct_ptr) 0 )
    {
        status = get_user_file( "Enter filename: " , FALSE, NULL, &filename);
        if (status == VIO_OK )
        {
            status = open_file_with_default_suffix( filename, "btr", READ_FILE,
                                                    BINARY_FORMAT, &file );

            if( status == VIO_OK )
            {
                polygons->bintree = allocate_bintree();

                status = io_bintree( file, READ_FILE, BINARY_FORMAT,
                                     polygons->bintree );

                /* If the operation fails, clean up the partially-loaded
                 * bintree.
                 */
                if (status != VIO_OK)
                {
                    delete_the_bintree( &polygons->bintree );
                }

                close_file( file );
            }

            delete_string( filename );

            print( "Done.\n" );
        }
    }

    return( status );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(input_polygons_bintree )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( save_polygons_bintree )
{
    VIO_Status            status;
    polygons_struct   *polygons;
    VIO_STR            filename;
    FILE              *file;

    status = VIO_OK;

    if( get_current_polygons(display,&polygons) &&
        polygons->bintree != (bintree_struct_ptr) 0 )
    {
        status = get_user_file( "Enter filename: " , TRUE, "btr", &filename);

        if( status == VIO_OK)
        { 
            status = open_file_with_default_suffix( filename, "btr",
                                                    WRITE_FILE, 
                                                    BINARY_FORMAT, &file );

            if( status == VIO_OK )
            {
                status = io_bintree( file, WRITE_FILE, BINARY_FORMAT,
                                     polygons->bintree );

                close_file( file );
            }

            delete_string( filename );

            print( "Done.\n" );
        }
    }

    return( status );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(save_polygons_bintree )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( create_bintree_for_polygons )
{
    polygons_struct   *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        if( polygons->bintree != (bintree_struct_ptr) 0 )
            delete_the_bintree( (bintree_struct **) (&polygons->bintree) );

        create_polygons_bintree( polygons,
                     VIO_ROUND( (VIO_Real) polygons->n_items * Bintree_size_factor ) );
        print( "Done.\n" );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(create_bintree_for_polygons )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( create_normals_for_polygon )
{
    polygons_struct   *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        compute_polygon_normals( polygons );
        graphics_models_have_changed( display );

        print( "Done computing polygon normals.\n" );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(create_normals_for_polygon )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( average_normals_for_polygon )
{
    int               n_iters;
    VIO_Real              neighbour_weight;
    polygons_struct   *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        if (get_user_input( "Enter #iterations  neighbour_weight: ", "dr",
                            &n_iters, &neighbour_weight) == VIO_OK)

        {
            average_polygon_normals( polygons, n_iters, neighbour_weight );

            graphics_models_have_changed( display );
        }

        print( "Done averaging polygon normals.\n" );
    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE(average_normals_for_polygon )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( smooth_current_polygon )
{
    polygons_struct   *polygons;

    if( get_current_polygons(display,&polygons) )
    {
        smooth_polygon( polygons, Max_smoothing_distance,
                        Smoothing_ratio, Smoothing_threshold,
                        Smoothing_normal_ratio,
                        FALSE, (VIO_Volume) NULL, 0, 0 );

        compute_polygon_normals( polygons );

        delete_the_bintree( (bintree_struct **) (&polygons->bintree) );

        set_update_required( display, NORMAL_PLANES );

        print( "Done smoothing polygon.\n" );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(smooth_current_polygon )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( reverse_polygons_order )
{
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        reverse_polygons_vertices( polygons );

        set_update_required( display, NORMAL_PLANES );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(reverse_polygons_order )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( make_polygon_sphere )
{
    VIO_Point         centre;
    VIO_Real          x_size, y_size, z_size;
    int               n_up, n_around;
    object_struct     *object;

    if (get_user_input( "Enter x_centre, y_centre, z_centre, x_size, y_size, z_size,\n      n_up, n_around: ",  "fffrrrdd", 
                        &Point_x(centre), &Point_y(centre), &Point_z(centre),
                        &x_size, &y_size, &z_size, &n_up, &n_around) == VIO_OK)
    {
        object = create_object( POLYGONS );

        create_polygons_sphere( &centre, x_size, y_size, z_size,
                                n_up, n_around, FALSE,
                                get_polygons_ptr(object) );

        get_polygons_ptr(object)->colours[0] = WHITE;
        compute_polygon_normals( get_polygons_ptr(object) );

        add_object_to_current_model( display, object, Update_3d_view_on_new );
    }
    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(make_polygon_sphere )
{
    return( TRUE );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( make_tetrahedral_sphere )
{
    VIO_Point             centre;
    VIO_Real              x_size, y_size, z_size;
    int               n_triangles;
    object_struct     *object;

    if (get_user_input( "Enter x_centre, y_centre, z_centre, x_size, y_size, z_size,\n      n_triangles: ", "fffrrrd", 
                        &Point_x(centre), &Point_y(centre), &Point_z(centre),
                        &x_size, &y_size, &z_size, &n_triangles ) == VIO_OK)
    {
        object = create_object( POLYGONS );

        create_tetrahedral_sphere( &centre, x_size, y_size, z_size,
                                   n_triangles, get_polygons_ptr(object) );

        get_polygons_ptr(object)->colours[0] = WHITE;
        compute_polygon_normals( get_polygons_ptr(object) );

        add_object_to_current_model( display, object, Update_3d_view_on_new );
    }
    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(make_tetrahedral_sphere )
{
    return( TRUE );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( subdivide_current_polygon )
{
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        subdivide_polygons( polygons );

        compute_polygon_normals( polygons );

        graphics_models_have_changed( display );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(subdivide_current_polygon )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( reset_polygon_neighbours )
{
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        if( polygons->neighbours != (int *) NULL )
            FREE( polygons->neighbours );
        polygons->neighbours = (int *) NULL;

        check_polygons_neighbours_computed( polygons );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(reset_polygon_neighbours )
{
    polygons_struct   *polygons;

    return( get_current_polygons(display,&polygons) &&
            polygons->neighbours != (int *) NULL );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( cut_polygon_neighbours )
{
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        print( "Cutting polygon neighbours:\n" );
        cut_polygon_neighbours_from_lines( display, polygons );
        print( "...done\n" );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(cut_polygon_neighbours )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( set_polygon_line_thickness )
{
    VIO_Real              line_thickness;
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        if (get_user_input( "Enter line thickness: " , "r", 
                            &line_thickness ) == VIO_OK)
        {
            polygons->line_thickness = (float) line_thickness;
            graphics_models_have_changed( display );
        }
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(set_polygon_line_thickness )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( print_polygons_surface_area )
{
    VIO_Real              surface_area;
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        surface_area = get_polygons_surface_area( polygons );
        print( "Surface area of polygons: %g\n", surface_area );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(print_polygons_surface_area )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( coalesce_current_polygons )
{
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        if( polygons == display->three_d.surface_extraction.polygons )
        {
            print( "Cannot coalesce in progress surface extraction.\n" );
            return( VIO_OK );
        }

        coalesce_object_points( &polygons->n_points, &polygons->points,
                                polygons->end_indices[polygons->n_items-1],
                                polygons->indices );

        REALLOC( polygons->normals, polygons->n_points );

        if( polygons->colour_flag == PER_VERTEX_COLOURS )
            REALLOC( polygons->colours, polygons->n_points );

        compute_polygon_normals( polygons );

        graphics_models_have_changed( display );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(coalesce_current_polygons )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

  DEF_MENU_FUNCTION( separate_current_polygons )
{
    polygons_struct   *polygons;

    if( get_current_polygons( display, &polygons ) )
    {
        if( polygons == display->three_d.surface_extraction.polygons )
        {
            print( "Cannot separate in progress surface extraction.\n" );
            return( VIO_OK );
        }

        separate_object_points( &polygons->n_points, &polygons->points,
                                polygons->end_indices[polygons->n_items-1],
                                polygons->indices,
                                polygons->colour_flag, &polygons->colours );

        REALLOC( polygons->normals, polygons->n_points );

        compute_polygon_normals( polygons );

        graphics_models_have_changed( display );
    }

    return( VIO_OK );
}

/* ARGSUSED */

  DEF_MENU_UPDATE(separate_current_polygons )
{
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( find_nearest_vertex ) {

    int                i;
    polygons_struct   *polygons;

    if( get_current_polygons(display,&polygons) ) {

      double x = Point_x(display->three_d.cursor.origin );
      double y = Point_y(display->three_d.cursor.origin );
      double z = Point_z(display->three_d.cursor.origin );

      double min_dist = 1.0e10;
      Nearest_vertex_to_move = -1;
      for( i = 0; i < polygons->n_points; i++ ) {
        double dist = ( x - polygons->points[i].coords[0] ) *
                      ( x - polygons->points[i].coords[0] );
        if( dist < min_dist ) {
          dist += ( y - polygons->points[i].coords[1] ) *
                  ( y - polygons->points[i].coords[1] );
          if( dist < min_dist ) {
            dist += ( z - polygons->points[i].coords[2] ) *
                    ( z - polygons->points[i].coords[2] );
            if( dist < min_dist ) {
              min_dist = dist;
              Nearest_vertex_to_move = i;
            }
          }
        }
      }

    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE( find_nearest_vertex ) {
    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}

/**
 * Construct the edges around each node. The edges are sorted to
 * make an ordered closed loop.
 */
VIO_Status get_surface_neighbours( polygons_struct * surface,
                                   int * n_neighbours_return[],
                                   int ** neighbours_return[] ) {

  int    i, j, k, jj;
  int  * tri;
  int  * n_ngh;
  int ** ngh;
  int  * ngh_array;

  // Check if all polygons are triangles.
  if( 3 * surface->n_items != surface->end_indices[surface->n_items-1] ) {
    printf( "Surface must contain only triangular polygons.\n" );
    return VIO_ERROR;
  }

  // Check if the node numbering starts at 0 or 1.

  int min_idx, max_idx;

  min_idx = 100*surface->n_points;  // anything big
  max_idx = 0;                      // anything small

  for( i = 0; i < 3*surface->n_items; i++ ) {
    if( surface->indices[i] < min_idx ) min_idx = surface->indices[i];
    if( surface->indices[i] > max_idx ) max_idx = surface->indices[i];
  }

  // Shift numbering to start at zero, for array indexing. Note
  // that we don't care if surface->indices array is modified.

  if( min_idx != 0 ) {
    for( i = 0; i < 3*surface->n_items; i++ ) {
      surface->indices[i] -= min_idx;
    }
  }

  // Count number of triangles attached to each node.

  ALLOC( n_ngh, surface->n_points );
  ALLOC( ngh, surface->n_points );
  ALLOC( ngh_array, 3*surface->n_items );

  for( i = 0; i < surface->n_points; i++ ) {
    n_ngh[i] = 0;
  }

  for( i = 0; i < 3*surface->n_items; i++ ) {
    n_ngh[surface->indices[i]]++;
    ngh_array[i] = -1;
  }

  int max_ngh = 0;
  int sum_ngh = 0;
  for( i = 0; i < surface->n_points; i++ ) {
    ngh[i] = &(ngh_array[sum_ngh]);
    sum_ngh += n_ngh[i];
    max_ngh = MAX( max_ngh, n_ngh[i] );
  }

  // At first, store the indices of the triangles in the neighbours.
  for( i = 0; i < surface->n_items; i++ ) {
    for( j = 0; j < 3; j++ ) {
      jj = surface->indices[3*i+j];
      for( k = 0; k < n_ngh[jj]; k++ ) {
        if( ngh[jj][k] == -1 ) {
          ngh[jj][k] = i;
          break;
        }
      }
    }
  }

  /* Now create a sort closed loop of the node neighbours.
   *
   *         1 ----- 2
   *          /\   / \
   *         /  \ /   \
   *       0 ----P---- 3
   *         \  / \  /
   *          \/   \/
   *         5 ----- 4
   */

  int * tmp;
  ALLOC( tmp, 2*max_ngh );

  for( i = 0; i < surface->n_points; i++ ) {
    for( k = 0; k < n_ngh[i]; k++ ) {
      tri = &(surface->indices[3*ngh[i][k]]);
      for( j = 0; j < 3; j++ ) {
        if( tri[j] == i ) break;
      }
      tmp[2*k+0] = tri[(j+1)%3];
      tmp[2*k+1] = tri[(j+2)%3];
    }

    ngh[i][0] = tmp[0];
    ngh[i][1] = tmp[1];
    for( k = 2; k < n_ngh[i]; k++ ) {
      for( j = 1; j < n_ngh[i]; j++ ) {
        if( tmp[2*j] == ngh[i][k-1] || tmp[2*j+1] == ngh[i][k-1] ) {
          if( tmp[2*j] == ngh[i][k-1] ) {
            ngh[i][k] = tmp[2*j+1];
          } else {
            ngh[i][k] = tmp[2*j];
          }
          tmp[2*j] = -1;
          tmp[2*j+1] = -1;
          break;
        }
      }
    }
  }

  *n_neighbours_return = n_ngh;
  *neighbours_return = ngh;

  FREE( tmp );

  return VIO_OK;

}


/* ARGSUSED */

DEF_MENU_FUNCTION( move_vertex_to_cursor ) {
    polygons_struct   *polygons;

    if( get_current_polygons(display,&polygons) ) {

      if( Nearest_vertex_to_move >= 0 ) {

        double x = Point_x(display->three_d.cursor.origin );
        double y = Point_y(display->three_d.cursor.origin );
        double z = Point_z(display->three_d.cursor.origin );

        int * n_ngh = NULL;
        int ** ngh = NULL;

        if( get_surface_neighbours( polygons, &n_ngh, &ngh ) == VIO_OK ) {

          /* Define a neighbourhood around vertex to move. */

          int * mask = (int *)malloc( polygons->n_points * sizeof( int ) );
          int i, j, k, dim;

          for( i = 0; i < polygons->n_points; i++ ) {
            mask[i] = 0;
          }
          mask[Nearest_vertex_to_move] = 1;
          for( j = 0; j < n_ngh[Nearest_vertex_to_move]; j++ ) {
            k = ngh[Nearest_vertex_to_move][j];
            mask[k] = 2;
          }

          int level;
          for( level = 2; level <= Levels_around_vertex_to_move; level++ ) {
            for( i = 0; i < polygons->n_points; i++ ) {
              if( mask[i] == level ) {
                for( j = 0; j < n_ngh[i]; j++ ) {
                  k = ngh[i][j];
                  if( mask[k] == 0 ) mask[k] = level+1;
                }
              }
            }
          }

          /* Do some basic geometric smoothing. */

          VIO_Real * mag = (VIO_Real *)malloc( polygons->n_points *
                                               sizeof( VIO_Real ) );
          VIO_Real * mag_tmp = (VIO_Real *)malloc( polygons->n_points *
                                                   sizeof( VIO_Real ) );
          for( i = 0; i < polygons->n_points; i++ ) {
            mag[i] = 0.0;
            mag_tmp[i] = 0.0;
          }

          i = Nearest_vertex_to_move;
          mag[i] = 1.0;  /* unitary displacement at selected vertex */
          VIO_Real mag0 = ( x - polygons->points[i].coords[0] ) *
                          polygons->normals[i].coords[0] +
                          ( y - polygons->points[i].coords[1] ) *
                          polygons->normals[i].coords[1] +
                          ( z - polygons->points[i].coords[2] ) *
                          polygons->normals[i].coords[2];

          VIO_Real avglen = 0.0;
          for( j = 0; j < n_ngh[i]; j++ ) {
            avglen += sqrt( ( polygons->points[ngh[i][j]].coords[0] -
                              polygons->points[i].coords[0] ) * 
                            ( polygons->points[ngh[i][j]].coords[0] -
                              polygons->points[i].coords[0] ) + 
                            ( polygons->points[ngh[i][j]].coords[1] -
                              polygons->points[i].coords[1] ) * 
                            ( polygons->points[ngh[i][j]].coords[1] -
                              polygons->points[i].coords[1] ) + 
                            ( polygons->points[ngh[i][j]].coords[2] -
                              polygons->points[i].coords[2] ) * 
                            ( polygons->points[ngh[i][j]].coords[2] -
                              polygons->points[i].coords[2] ) );
          }
          avglen /= (VIO_Real)n_ngh[i];

          /* Tfinal = fwhm*fwhm/(16*log(2))
             dt = Tfinal/niter
             Must have dt small enough such that dt/(h*h) < 0.5 where
             h is some minimum mesh spacing. */

          int IterMax = 100.0;

          VIO_Real peak = 2.0 * sqrt( log( 2.0 ) ) /
                          ( ( FWHM_around_vertex_to_move / avglen ) * 
                            sqrt( 2.0 * 3.1415926 ) );
          VIO_Real relax = 0.50;

          int iter = 0;
          do {
            iter++;
            for( i = 0; i < polygons->n_points; i++ ) {
              if( mask[i] > 0 ) {
                mag_tmp[i] = 0.0;
                for( j = 0; j < n_ngh[i]; j++ ) {
                  mag_tmp[i] += mag[ngh[i][j]];
                }
                mag_tmp[i] = relax * mag[i] + ( 1.0 - relax ) * mag_tmp[i] / n_ngh[i];
              }
            }
            for( i = 0; i < polygons->n_points; i++ ) {
              if( mask[i] > 0 ) {
                mag[i] = mag_tmp[i];
              }
            }
            if( mag[Nearest_vertex_to_move] < peak ) break;
          } while( iter <= IterMax );
          if( mag_tmp ) free( mag_tmp );
// printf( "peak = %g max = %g iter = %d\n", peak, mag[Nearest_vertex_to_move], iter );

          /* scale to have unit amplification factor at picked location. */
          VIO_Real scale = mag[Nearest_vertex_to_move];
          for( i = 0; i < polygons->n_points; i++ ) {
            if( mask[i] > 0 ) {
              mag[i] /= scale;
            }
          }

          VIO_Real dx = polygons->normals[Nearest_vertex_to_move].coords[0] * mag0;
          VIO_Real dy = polygons->normals[Nearest_vertex_to_move].coords[1] * mag0;
          VIO_Real dz = polygons->normals[Nearest_vertex_to_move].coords[2] * mag0;

          for( i = 0; i < polygons->n_points; i++ ) {
            if( mask[i] > 0 ) {
              polygons->points[i].coords[0] += dx * mag[i];
              polygons->points[i].coords[1] += dy * mag[i];
              polygons->points[i].coords[2] += dz * mag[i];
            }
          }

          /* do a bit of Taubin smoothing on the final mesh
             around the neighbourhood of the moved vertex. */

          VIO_Real * new_coords = (VIO_Real *)malloc( 3 * polygons->n_points *
                                                      sizeof( VIO_Real ) );

          /* Taubin parameters for shrinkage free smoothing */

          VIO_Real kPB = 0.075;
          VIO_Real mu = ( -kPB - sqrt( kPB * kPB + 20.0 - 12.0 * kPB ) ) /
                        ( 2.0 * ( 5.0 - 3.0 * kPB ) );
          VIO_Real lambda = mu / ( kPB * mu - 1.0 );

          for( iter = 0; iter < 4; iter++ ) {

            for( i = 0; i < polygons->n_points; i++ ) {
              if( mask[i] > 0 ) {
                for( dim = 0; dim < 3; dim++ ) {
                  new_coords[3*i+dim] = 0.0;
                  for( j = 0; j < n_ngh[i]; j++ ) {
                    new_coords[3*i+dim] += polygons->points[ngh[i][j]].coords[dim];
                  }
                  new_coords[3*i+dim] /= (VIO_Real)n_ngh[i];
                }
              }
            }

            for( i = 0; i < polygons->n_points; i++ ) {
              if( mask[i] > 0 ) {
                if( iter % 2 == 0 ) {
                  relax = 1.0 - lambda * sqrt(mag[i]);
                } else {
                  relax = 1.0 - mu * sqrt(mag[i]);
                }
                for( dim = 0; dim < 3; dim++ ) {
                  polygons->points[i].coords[dim] = relax * polygons->points[i].coords[dim] + 
                                                    ( 1.0 - relax ) * new_coords[3*i+dim];
                }
              }
            }
          }

          if( mag ) free( mag );
          if( new_coords ) free( new_coords );
          if( mask ) free( mask );
          if( n_ngh ) FREE( n_ngh );
          if( ngh ) {
            FREE( ngh[0] );
            FREE( ngh );
          }
        }

        compute_polygon_normals( polygons );
        delete_the_bintree( (bintree_struct **) (&polygons->bintree) );

        graphics_models_have_changed( display );
        set_update_required( display, NORMAL_PLANES );

      }

    }

    return( VIO_OK );
}


/* ARGSUSED */

DEF_MENU_UPDATE( move_vertex_to_cursor ) {

    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}


/* ARGSUSED */

DEF_MENU_FUNCTION( smooth_around_vertex ) {

    polygons_struct   *polygons;

    if( get_current_polygons(display,&polygons) ) {

      if( Nearest_vertex_to_move >= 0 ) {

        int * n_ngh = NULL;
        int ** ngh = NULL;

        if( get_surface_neighbours( polygons, &n_ngh, &ngh ) == VIO_OK ) {

          /* Define a neighbourhood around vertex to move. */

          int * mask = (int *)malloc( polygons->n_points * sizeof( int ) );
          int i, j, k, dim;

          for( i = 0; i < polygons->n_points; i++ ) {
            mask[i] = 0;
          }
          mask[Nearest_vertex_to_move] = 1;
          for( j = 0; j < n_ngh[Nearest_vertex_to_move]; j++ ) {
            k = ngh[Nearest_vertex_to_move][j];
            mask[k] = 2;
          }

          int level;
          for( level = 2; level <= Levels_around_vertex_to_move; level++ ) {
            for( i = 0; i < polygons->n_points; i++ ) {
              if( mask[i] == level ) {
                for( j = 0; j < n_ngh[i]; j++ ) {
                  k = ngh[i][j];
                  if( mask[k] == 0 ) mask[k] = level+1;
                }
              }
            }
          }

          /* Do some basic geometric smoothing. */

          VIO_Real * mag = (VIO_Real *)malloc( polygons->n_points *
                                               sizeof( VIO_Real ) );
          VIO_Real * mag_tmp = (VIO_Real *)malloc( polygons->n_points *
                                                   sizeof( VIO_Real ) );
          for( i = 0; i < polygons->n_points; i++ ) {
            mag[i] = 0.0;
            mag_tmp[i] = 0.0;
          }

          i = Nearest_vertex_to_move;
          mag[i] = 1.0;  /* unitary displacement at selected vertex */

          VIO_Real avglen = 0.0;
          for( j = 0; j < n_ngh[i]; j++ ) {
            avglen += sqrt( ( polygons->points[ngh[i][j]].coords[0] -
                              polygons->points[i].coords[0] ) * 
                            ( polygons->points[ngh[i][j]].coords[0] -
                              polygons->points[i].coords[0] ) + 
                            ( polygons->points[ngh[i][j]].coords[1] -
                              polygons->points[i].coords[1] ) * 
                            ( polygons->points[ngh[i][j]].coords[1] -
                              polygons->points[i].coords[1] ) + 
                            ( polygons->points[ngh[i][j]].coords[2] -
                              polygons->points[i].coords[2] ) * 
                            ( polygons->points[ngh[i][j]].coords[2] -
                              polygons->points[i].coords[2] ) );
          }
          avglen /= (VIO_Real)n_ngh[i];

          /* Tfinal = fwhm*fwhm/(16*log(2))
             dt = Tfinal/niter
             Must have dt small enough such that dt/(h*h) < 0.5 where
             h is some minimum mesh spacing. */

          int IterMax = 100.0;

          VIO_Real peak = 2.0 * sqrt( log( 2.0 ) ) /
                          ( ( FWHM_around_vertex_to_move / avglen ) * 
                            sqrt( 2.0 * 3.1415926 ) );
          VIO_Real relax = 0.50;

          int iter = 0;
          do {
            iter++;
            for( i = 0; i < polygons->n_points; i++ ) {
              if( mask[i] > 0 ) {
                mag_tmp[i] = 0.0;
                for( j = 0; j < n_ngh[i]; j++ ) {
                  mag_tmp[i] += mag[ngh[i][j]];
                }
                mag_tmp[i] = relax * mag[i] + ( 1.0 - relax ) * mag_tmp[i] / n_ngh[i];
              }
            }
            for( i = 0; i < polygons->n_points; i++ ) {
              if( mask[i] > 0 ) {
                mag[i] = mag_tmp[i];
              }
            }
            if( mag[Nearest_vertex_to_move] < peak ) break;
          } while( iter <= IterMax );
          if( mag_tmp ) free( mag_tmp );

          /* scale to have unit amplification factor at picked location. */
          VIO_Real scale = mag[Nearest_vertex_to_move];
          for( i = 0; i < polygons->n_points; i++ ) {
            if( mask[i] > 0 ) {
              mag[i] /= scale;
            }
          }

          /* do a bit of geometric smoothing on the final mesh. */

          VIO_Real * new_coords = (VIO_Real *)malloc( 3 * polygons->n_points *
                                                      sizeof( VIO_Real ) );

          iter = 5;
          do {
            iter--;
            for( i = 0; i < polygons->n_points; i++ ) {
              if( mask[i] > 0 ) {
                for( dim = 0; dim < 3; dim++ ) {
                  new_coords[3*i+dim] = 0.0;
                  for( j = 0; j < n_ngh[i]; j++ ) {
                    new_coords[3*i+dim] += polygons->points[ngh[i][j]].coords[dim];
                  }
                  new_coords[3*i+dim] /= (VIO_Real)n_ngh[i];
                }
              }
            }

            /* mag[i] is almost zero on the periphery of the masked region, so
               smoothing with relax = 1-mag[i] will blend in smoothly with mesh
               outside masked region. Relax=1-sqrt(mag) is even better! */
            for( i = 0; i < polygons->n_points; i++ ) {
              if( mask[i] > 0 ) {
                VIO_Real relax = 1.0 - sqrt(mag[i]); 
                for( dim = 0; dim < 3; dim++ ) {
                  polygons->points[i].coords[dim] = relax * polygons->points[i].coords[dim] + 
                                                    ( 1.0 - relax ) * new_coords[3*i+dim];
                }
              }
            }
          } while( iter >= 0 );

          if( mag ) free( mag );
          if( new_coords ) free( new_coords );
          if( mask ) free( mask );
          if( n_ngh ) FREE( n_ngh );
          if( ngh ) {
            FREE( ngh[0] );
            FREE( ngh );
          }
        }

        compute_polygon_normals( polygons );
        delete_the_bintree( (bintree_struct **) (&polygons->bintree) );

        graphics_models_have_changed( display );
        set_update_required( display, NORMAL_PLANES );

      }

    }

    return( VIO_OK );
}

/* ARGSUSED */

DEF_MENU_UPDATE( smooth_around_vertex ) {

    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}


/* ARGSUSED */

DEF_MENU_FUNCTION( set_vector_based_params ) {

    polygons_struct   *polygons;

    if( get_current_polygons(display,&polygons) ) {

      int               n_levels;
      VIO_Real          fwhm;

      printf( "Current values are levels = %d and fwhm = %g\n", 
              Levels_around_vertex_to_move, FWHM_around_vertex_to_move );
      if (get_user_input( "Enter number of mesh connectivity levels and fwhm: ", "dr", 
                          &n_levels, &fwhm ) == VIO_OK) {
        if( n_levels > 2 ) {
          Levels_around_vertex_to_move = n_levels;
        }
        if( fwhm > 0.0 ) {
          FWHM_around_vertex_to_move = fwhm;
        }
      }

    }

    return( VIO_OK );
}


/* ARGSUSED */

DEF_MENU_UPDATE( set_vector_based_params ) {

    return( current_object_is_this_type(display,POLYGONS) ||
            current_object_is_this_type(display,MODEL) );
}


