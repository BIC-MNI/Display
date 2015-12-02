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

        add_object_to_current_model( display, object );
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

        add_object_to_current_model( display, object );
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
