
#include  <def_files.h>
#include  <def_string.h>
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_arrays.h>

public  Status  load_graphics_file( graphics, filename )
    graphics_struct  *graphics;
    char             filename[];
{
    Status                   status;
    Status                   input_graphics_file();
    Status                   input_landmark_file();
    Status                   input_volume_file();
    File_formats             format;
    Status                   create_object();
    Status                   push_current_object();
    Status                   add_object_to_model();
    object_struct            *object;
    model_struct             *model;
    model_struct             *get_current_model();
    Boolean                  get_range_of_object();
    void                     rebuild_selected_list();
    void                     set_current_object_index();
    int                      n_items;
    Status                   create_polygons_bintree();
    Status                   check_polygons_neighbours_computed();
    Status                   initialize_cursor();
    volume_struct            *volume, *volume_read_in;
    void                     set_update_required();
    Status                   initialize_object_traverse();
    object_struct            *current_object;
    object_traverse_struct   object_traverse;
    void                     markers_have_changed();
    Boolean                  volume_present;
    Boolean                  markers_present;
    Status                   create_slice_window();

    status = create_object( &object, MODEL );

    if( status == OK )
    {
        PRINT( "Inputting %s.\n", filename );

        model = object->ptr.model;

        (void) strcpy( model->filename, filename );

        if( string_ends_in(filename,".mni") ||
            string_ends_in(filename,".nil") ||
            string_ends_in(filename,".iff") ||
            string_ends_in(filename,".fre") )
        {
            status = input_volume_file( filename,
                                        &model->n_objects,
                                        &model->object_list );
        }
        else if( string_ends_in(filename,".lmk") )
        {
            (void) get_slice_window_volume( graphics, &volume );
            status = input_landmark_file( volume, filename,
                                          &model->n_objects,
                                          &model->object_list );
        }
        else if( string_ends_in(filename,".cnt") )
        {
            PRINT( "Cannot read .cnt files.\n" );
        }
        else if( string_ends_in(filename,".roi") )
        {
            PRINT( "Cannot read .roi files.\n" );
        }
        else
        {
            status = input_graphics_file( filename, &format,
                                          &model->n_objects,
                                          &model->object_list );
        }

        PRINT( "Objects input.\n" );
    }

    if( status == OK )
    {
        status = initialize_object_traverse( &object_traverse, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( !Visibility_on_input || current_object->object_type == VOLUME )
                current_object->visibility = OFF;
        }
    }

    if( status == OK )
    {
        markers_present = FALSE;
        volume_present = FALSE;

        status = initialize_object_traverse( &object_traverse, 1, &object );

        while( get_next_object_traverse(&object_traverse,&current_object) )
        {
            if( status == OK && current_object->object_type == VOLUME &&
                !volume_present )
            {
                volume_read_in = current_object->ptr.volume;
                volume_present = TRUE;
            }
            else if( status == OK && current_object->object_type == MARKER )
            {
                markers_present = TRUE;
            }
            else if( status == OK && current_object->object_type == POLYGONS )
            {
                polygons_struct   *polygons;

                polygons = current_object->ptr.polygons;

                n_items = polygons->n_items;

                if( n_items > Polygon_bintree_threshold )
                {
                    status = create_polygons_bintree( polygons,
                              ROUND( (Real) n_items * Bintree_size_factor ) );
                }

                if( Compute_neighbours_on_input )
                    status = check_polygons_neighbours_computed( polygons );
            }
        }
    }

    if( status == OK )
    {
        model = get_current_model( graphics );

        status = add_object_to_model( model, object );

        if( current_object_is_top_level(graphics) )
        {
            if( model->n_objects == 1 )               /* first object */
            {
                status = push_current_object( graphics );
            }
        }
        else
        {
            set_current_object_index( graphics, model->n_objects-1 );
        }
    }

    if( status == OK )
    {
        if( !get_range_of_object( graphics->models[THREED_MODEL], FALSE,
                                  &graphics->three_d.min_limit,
                                  &graphics->three_d.max_limit ) )
        {
            fill_Point( graphics->three_d.min_limit, 0.0, 0.0, 0.0 );
            fill_Point( graphics->three_d.max_limit, 1.0, 1.0, 1.0 );
            PRINT( "No objects range.\n" );
        }

        ADD_POINTS( graphics->three_d.centre_of_objects,
                    graphics->three_d.min_limit,
                    graphics->three_d.max_limit );
        SCALE_POINT( graphics->three_d.centre_of_objects,
                     graphics->three_d.centre_of_objects,
                     0.5 );

        status = initialize_cursor( graphics );
    }

    if( status == OK && markers_present )
        markers_have_changed( graphics );

    if( status == OK )
        rebuild_selected_list( graphics, graphics->associated[MENU_WINDOW] );

    if( status == OK && volume_present &&
        !get_slice_window_volume( graphics, &volume ) )
    {
        status = create_slice_window( graphics, volume_read_in );
    }

    return( status );
}
