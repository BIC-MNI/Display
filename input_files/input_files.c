
#include  <display.h>

public  Status  load_graphics_file( 
    display_struct   *display,
    char             filename[] )
{
    Status                   status;
    display_struct           *slice_window;
    object_struct            *object;
    model_struct             *model;
    int                      n_items;
    Volume                   volume, volume_read_in;
    object_struct            *current_object;
    object_traverse_struct   object_traverse;
    BOOLEAN                  volume_present;

    object = create_object( MODEL );

    print( "Inputting %s.\n", filename );

    model = get_model_ptr( object );
    initialize_display_model( model );

    (void) strcpy( model->filename, filename );

    volume_present = FALSE;

    status = OK;

    if( filename_extension_matches(filename,"mnc") ||
        filename_extension_matches(filename,"mni") ||
        filename_extension_matches(filename,"nil") ||
        filename_extension_matches(filename,"iff") ||
        filename_extension_matches(filename,"fre") )
    {
        status = input_volume_file( filename, &volume_read_in );

        if( status == OK )
            volume_present = TRUE;
    }
    else if( filename_extension_matches(filename,"cnt") )
    {
        print( "Cannot read .cnt files.\n" );
        status = ERROR;
    }
    else if( filename_extension_matches(filename,"roi") )
    {
        print( "Cannot read .roi files.\n" );
        status = ERROR;
    }
    else
    {
        (void) get_slice_window_volume( display, &volume );
        status = input_objects_any_format( volume, filename,
                                 display->three_d.default_marker_colour,
                                 display->three_d.default_marker_size,
                                 display->three_d.default_marker_type,
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
        initialize_object_traverse( &object_traverse, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( current_object != object &&
                current_object->object_type == MODEL )
            {
                initialize_model_info( get_model_ptr(current_object) );
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

        rebuild_selected_list( display, display->associated[MENU_WINDOW] );
    }
    else
        delete_object( object );

    if( status == OK && volume_present )
    {
        if( !get_slice_window_volume( display, &volume ) )
        {
            create_slice_window( display, filename, volume_read_in );
        }
        else if( get_slice_window( display, &slice_window ) )
        {
            set_slice_window_volume( slice_window, volume_read_in );
        }
    }

    return( status );
}
