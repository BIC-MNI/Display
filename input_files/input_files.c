/**
 * \file input_files.c
 * \brief Function to load graphics or volume files.
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

/**
 * Load either a graphical object (MNI .obj format, e.g.) or a volume file
 * (MINC voxel data, e.g.).
 * \param display A pointer to the display_struct of the 3D view window.
 * \param filename The name of the file to open.
 * \param is_label_file A boolean that indicates whether the volume file
 * should be treated as containing anatomical labels for an existing
 * volume.
 * \returns VIO_OK on success.
 */
  VIO_Status  load_graphics_file( 
    display_struct   *display,
    VIO_STR           filename,
    VIO_BOOL          is_label_file )
{
    VIO_Status               status;
    object_struct            *object;
    model_struct             *model;
    int                      n_items, sizes[VIO_N_DIMENSIONS];
    VIO_Volume               volume_read_in;
    object_struct            *current_object;
    object_traverse_struct   object_traverse;
    char                     volume_description[VIO_EXTREMELY_LARGE_STRING_SIZE];
    VIO_BOOL                 volume_present;
    display_struct           *slice_window;

    object = create_object( MODEL );

    print( "Input %s\n", filename );

    model = get_model_ptr( object );
    initialize_display_model( model );
    initialize_3D_model_info( model );

    replace_string( &model->filename, create_string(filename) );

    volume_present = FALSE;

    status = VIO_OK;

    if( filename_extension_matches(filename,"mnc") ||
        filename_extension_matches(filename,"mni") ||
        filename_extension_matches(filename,"nil") ||
        filename_extension_matches(filename,"iff") ||
        filename_extension_matches(filename,"mgh") ||
        filename_extension_matches(filename,"mgz") ||
        filename_extension_matches(filename,"nii") ||
        filename_extension_matches(filename,"hdr") ||
        filename_extension_matches(filename,"fre") )
    {
        if( !is_label_file )
        {
            status = input_volume_file( filename, &volume_read_in );

            if( status == VIO_OK )
                volume_present = TRUE;
        }
        else
        {
            if( get_n_volumes(display) == 0 )
            {
                print( "No volume to load labels for.\n" );
                status = VIO_ERROR;
            }
            else
            {
                status = input_label_volume_file( display, filename );
                if (Tags_from_label)
                {
                input_tag_objects_label(display,
                  display->three_d.default_marker_colour,
                  display->three_d.default_marker_size,
                  display->three_d.default_marker_type,
                  &model->n_objects, &model->objects);
                /* TODO: why was this here??
                   &display->label_stack); */
                }
            }
        }
    }
    else if( filename_extension_matches(filename,"cnt") )
    {
        print( "Cannot read .cnt files.\n" );
        status = VIO_ERROR;
    }
    else if( filename_extension_matches(filename,
                                        get_default_colour_map_suffix()) &&
             get_n_volumes(display) > 0 &&
             get_slice_window( display, &slice_window ) )
    {
        status = load_label_colour_map( slice_window, filename );
    }
    else if( filename_extension_matches(filename,
                    get_default_transform_file_suffix()) )
    {
        transform_current_volume_from_file( display, filename );
        status = VIO_OK;
    }
    else if( filename_extension_matches(filename,"roi") )
    {
        print( "Cannot read .roi files.\n" );
        status = VIO_ERROR;
    }
    else
    {
        if( is_label_file &&
            (filename_extension_matches( filename,
                                         get_default_tag_file_suffix() ) ||
             filename_extension_matches( filename,
                                         get_default_landmark_file_suffix()) ) )
        {
            status = input_tag_label_file( display, filename );
        }
        else
        {
            status = input_objects_any_format( get_volume(display), filename,
                                     display->three_d.default_marker_colour,
                                     display->three_d.default_marker_size,
                                     display->three_d.default_marker_type,
                                     &model->n_objects,
                                     &model->objects); //VF:
        }
    }

    if( status == VIO_OK )
    {
        print( "Objects input.\n" );

        initialize_object_traverse( &object_traverse, FALSE, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( !Visibility_on_input )
                set_object_visibility( current_object, FALSE );
        }
    }

    if( status == VIO_OK )
    {
        initialize_object_traverse( &object_traverse, FALSE, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( current_object != object &&
                current_object->object_type == MODEL )
            {
                initialize_3D_model_info( get_model_ptr(current_object) );
            }
            else if( current_object->object_type == POLYGONS )
            {
                polygons_struct   *polygons;

                polygons = get_polygons_ptr( current_object );

                n_items = polygons->n_items;

                if( Polygon_bintree_threshold >= 0 &&
                    n_items > Polygon_bintree_threshold )
                {
                    create_polygons_bintree( polygons,
                              VIO_ROUND( (VIO_Real) n_items * Bintree_size_factor ) );
                }

                if( Compute_neighbours_on_input )
                    check_polygons_neighbours_computed( polygons );
            }
        }
    }

    if( status == VIO_OK && model->n_objects > 0 )
    {
        model = get_current_model( display );

        add_object_to_model( model, object );

        if( current_object_is_top_level(display) )
        {
            if( model->n_objects == 1 )               /* first object */
            {
                push_current_object( display );
            }
        }
        else
        {
            set_current_object_index( display, model->n_objects-1 );
        }

        /*rebuild_selected_list( display, display->associated[MENU_WINDOW] );*/
        rebuild_selected_list( display, display->associated[MARKER_WINDOW] );
    }
    else
        delete_object( object );

    if( status == VIO_OK && volume_present )
    {
        get_volume_sizes( volume_read_in, sizes );

        (void) sprintf( volume_description, "%s : X:%d Y:%d Z:%d",
                        filename, sizes[VIO_X], sizes[VIO_Y], sizes[VIO_Z] );
        if (get_volume_n_dimensions(volume_read_in) > VIO_N_DIMENSIONS)
        {
          sprintf(volume_description + strlen(volume_description),
                  " T:%d", sizes[VIO_T]);
        }

        add_slice_window_volume( display, volume_description, volume_read_in );
    }

    return( status );
}
