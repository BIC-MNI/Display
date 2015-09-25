/**
 * \file file.c
 * \brief Menu commands for loading and saving various types of files.
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

/* ARGSUSED */

DEF_MENU_FUNCTION( load_vertex_data )
{
    object_struct      *object;
    VIO_STR            filename;
    VIO_Status         status = VIO_OK;
    vertex_data_struct *vtxd_ptr;
    polygons_struct    *polygons;

    if( !get_current_object( display, &object ) || 
        object->object_type != POLYGONS)
    {
        return VIO_ERROR;
    }

    status = get_user_file("Enter path to vertex data: ", FALSE, &filename);
    if (status != VIO_OK)
    {
        return VIO_ERROR;
    }

    if ((vtxd_ptr = input_vertex_data(filename)) == NULL)
    {
        print_error("Failed to read vertex data from '%s'.\n", filename);
        return VIO_ERROR;
    }

    print("Loaded %d vertex data items, in range [%f ... %f]\n",
          vtxd_ptr->dims[0],
          vtxd_ptr->min_v, vtxd_ptr->max_v);

    polygons = get_polygons_ptr(object);

    if (polygons->n_points != vtxd_ptr->dims[0])
    {
        print("Vertex data requires a polygon object with the same length.\n");
        return VIO_ERROR;
    }

    attach_vertex_data(display, object, vtxd_ptr);

    return VIO_OK;
}

DEF_MENU_UPDATE( load_vertex_data )
{
    object_struct *object;

    return (get_current_object(display, &object) &&
            object->object_type == POLYGONS);
}

DEF_MENU_FUNCTION( load_file )
{
    VIO_Status   status;
    VIO_STR   filename;

    status = get_user_file("Enter path of file to load: " , FALSE, &filename);
    if (status != VIO_OK)
    {
        return VIO_ERROR;
    }

    status = load_graphics_file( display, filename, FALSE );

    if( status == VIO_OK )
        graphics_models_have_changed( display );

    delete_string( filename );

    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(load_file )
{
    return( TRUE );
}

/* ARGSUSED */

DEF_MENU_FUNCTION( save_file )
{
    int            n_objects;
    object_struct  **object_list;
    object_struct  *current_object;
    VIO_Status         status;
    VIO_STR         filename;

    status = VIO_OK;

    if( get_current_object( display, &current_object ) )
    {
        status = get_user_file("Enter path of file to save: ", TRUE, &filename);
        if (status != VIO_OK)
        {
            return VIO_ERROR;
        }

        if( status == VIO_OK && !check_clobber_file_default_suffix( filename,
                                                                "obj" ) )
            status = VIO_ERROR;

        if( current_object->object_type == MODEL )
        {
            n_objects = get_model_ptr(current_object)->n_objects;
            object_list = get_model_ptr(current_object)->objects;
        }
        else
        {
            n_objects = 1;
            object_list = &current_object;
        }

        if( status == VIO_OK )
        {
            status = output_graphics_file( filename, (VIO_File_formats) Save_format,
                                           n_objects, object_list );
            print( "Done saving.\n" );
        }

        delete_string( filename );
    }

    return( status );
}

/* ARGSUSED */

DEF_MENU_UPDATE(save_file )
{
    return( TRUE );
}
