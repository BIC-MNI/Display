/* ----------------------------------------------------------------------------
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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/input_files/input_files.c,v 1.24 1995-07-31 19:54:07 david Exp $";
#endif


#include  <display.h>

public  Status  load_graphics_file( 
    display_struct   *display,
    char             filename[],
    BOOLEAN          is_label_file )
{
    Status                   status;
    object_struct            *object;
    model_struct             *model;
    int                      n_items, sizes[N_DIMENSIONS];
    Volume                   volume_read_in;
    object_struct            *current_object;
    object_traverse_struct   object_traverse;
    STRING                   volume_description;
    BOOLEAN                  volume_present;
    display_struct           *slice_window;

    object = create_object( MODEL );

    print( "Inputting %s.\n", filename );

    model = get_model_ptr( object );
    initialize_display_model( model );
    initialize_3D_model_info( model );

    (void) strcpy( model->filename, filename );

    volume_present = FALSE;

    status = OK;

    if( filename_extension_matches(filename,"mnc") ||
        filename_extension_matches(filename,"mni") ||
        filename_extension_matches(filename,"nil") ||
        filename_extension_matches(filename,"iff") ||
        filename_extension_matches(filename,"fre") )
    {
        if( !is_label_file )
        {
            status = input_volume_file( filename, &volume_read_in );

            if( status == OK )
                volume_present = TRUE;
        }
        else
        {
            if( get_n_volumes(display) == 0 )
            {
                print( "No volume to load labels for.\n" );
                status = ERROR;
            }
            else
            {
                status = input_label_volume_file( display, filename );
            }
        }
    }
    else if( filename_extension_matches(filename,"cnt") )
    {
        print( "Cannot read .cnt files.\n" );
        status = ERROR;
    }
    else if( filename_extension_matches(filename,
                                        get_default_colour_map_suffix()) &&
             get_n_volumes(display) > 0 &&
             get_slice_window( display, &slice_window ) )
    {
        status = load_label_colour_map( slice_window, filename );
    }
    else if( filename_extension_matches(filename,"roi") )
    {
        print( "Cannot read .roi files.\n" );
        status = ERROR;
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
                                     &model->objects );
        }
    }

    if( status == OK )
    {
        print( "Objects input.\n" );

        initialize_object_traverse( &object_traverse, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( !Visibility_on_input )
                current_object->visibility = OFF;
        }
    }

    if( status == OK )
    {
        initialize_object_traverse( &object_traverse, 1, &object );

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

                if( n_items > Polygon_bintree_threshold )
                {
                    create_polygons_bintree( polygons,
                              ROUND( (Real) n_items * Bintree_size_factor ) );
                }

                if( Compute_neighbours_on_input )
                    check_polygons_neighbours_computed( polygons );
            }
        }
    }

    if( status == OK && model->n_objects > 0 )
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

        rebuild_selected_list( display, display->associated[MENU_WINDOW] );
    }
    else
        delete_object( object );

    if( status == OK && volume_present )
    {
        get_volume_sizes( volume_read_in, sizes );

        (void) sprintf( volume_description, "%s : %d %d %d",
                        filename, sizes[X], sizes[Y], sizes[Z] );

        add_slice_window_volume( display, volume_description, volume_read_in );
    }

    return( status );
}
