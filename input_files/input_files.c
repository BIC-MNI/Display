
#include  <def_display.h>

public  Status  load_graphics_file( 
    display_struct   *display,
    char             filename[] )
{
    Status                   status;
    File_formats             format;
    object_struct            *object;
    model_struct             *model;
    String                   filename_no_z;
    int                      n_items, len;
    Volume                   volume, volume_read_in;
    object_struct            *current_object;
    object_traverse_struct   object_traverse;
    Boolean                  volume_present;
    Boolean                  markers_present;

    (void) strcpy( filename_no_z, filename );
    len = strlen( filename );

    if( filename[len-2] == '.' && filename[len-1] == 'Z' )
        filename_no_z[len-2] = (char) 0;

    object = create_object( MODEL );

    print( "Inputting %s.\n", filename_no_z );

    model = get_model_ptr( object );
    initialize_display_model( model );

    (void) strcpy( model->filename, filename_no_z );

    volume_present = FALSE;

    status = OK;

    if( string_ends_in(filename_no_z,".mnc") ||
        string_ends_in(filename_no_z,".mni") ||
        string_ends_in(filename_no_z,".nil") ||
        string_ends_in(filename_no_z,".iff") ||
        string_ends_in(filename_no_z,".fre") )
    {
        status = input_volume_file( filename_no_z, &volume_read_in );

        volume_present = TRUE;
    }
    else if( string_ends_in(filename_no_z,".lmk") )
    {
        (void) get_slice_window_volume( display, &volume );
        status = input_landmark_file( volume, filename_no_z,
                                      display->three_d.default_marker_colour,
                                      display->three_d.default_marker_size,
                                      display->three_d.default_marker_type,
                                      &model->n_objects, &model->objects );
    }
    else if( string_ends_in(filename_no_z,".tag") )
    {
        status = input_tag_file( filename_no_z,
                                 display->three_d.default_marker_colour,
                                 display->three_d.default_marker_size,
                                 display->three_d.default_marker_type,
                                 &model->n_objects, &model->objects );
    }
    else if( string_ends_in(filename_no_z,".cnt") )
    {
        print( "Cannot read .cnt files.\n" );
        status = ERROR;
    }
    else if( string_ends_in(filename_no_z,".roi") )
    {
        print( "Cannot read .roi files.\n" );
        status = ERROR;
    }
    else
    {
        status = input_graphics_file( filename_no_z, &format,
                                      &model->n_objects,
                                      &model->objects );
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
        markers_present = FALSE;

        initialize_object_traverse( &object_traverse, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( current_object->object_type == MARKER )
            {
                markers_present = TRUE;
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

    if( model->n_objects > 0 )
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

        if( !get_range_of_object( display->models[THREED_MODEL], FALSE,
                                  &display->three_d.min_limit,
                                  &display->three_d.max_limit ) )
        {
            fill_Point( display->three_d.min_limit, 0.0, 0.0, 0.0 );
            fill_Point( display->three_d.max_limit, 1.0, 1.0, 1.0 );
            print( "No objects range.\n" );
        }

        ADD_POINTS( display->three_d.centre_of_objects,
                    display->three_d.min_limit,
                    display->three_d.max_limit );
        SCALE_POINT( display->three_d.centre_of_objects,
                     display->three_d.centre_of_objects,
                     0.5 );

        reset_cursor( display );

        if( markers_present )
            markers_have_changed( display );

        rebuild_selected_list( display, display->associated[MENU_WINDOW] );
    }
    else
        delete_model( model );

    if( volume_present )
    {
        if( !get_slice_window_volume( display, &volume ) )
        {
            create_slice_window( display, filename, volume_read_in );
        }
        else
        {
            set_slice_window_volume( display, volume_read_in );
        }
    }

    return( status );
}
