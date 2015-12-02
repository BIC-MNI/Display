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

    status = get_user_file("Enter path to vertex data: ", FALSE, NULL,
                           &filename);
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

    status = get_user_file("Enter path of file to load: " , FALSE, NULL,
                           &filename);
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
        status = get_user_file("Enter path of file to save: ", TRUE, "obj",
                               &filename);
        if (status != VIO_OK)
        {
            return VIO_ERROR;
        }

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

DEF_MENU_FUNCTION(load_oblique_plane)
{
    display_struct *slice_window;
    float          wx, wy, wz;
    VIO_Real       perp_axis[VIO_MAX_DIMENSIONS];
    VIO_Real       voxel[VIO_MAX_DIMENSIONS];
    VIO_STR        filename;
    VIO_Status     status;
    int            volume_index;
    FILE           *fp;
    int            version;
    VIO_Volume     volume;
    int            view_index = get_arbitrary_view_index( display );

    if( !get_slice_window( display, &slice_window ))
    {
        return VIO_ERROR;
    }
    status = get_user_file("Enter path to saved oblique plane: ", FALSE, NULL,
                           &filename);
    if (status != VIO_OK)
    {
        return VIO_ERROR;
    }

    volume = get_volume( slice_window );
    volume_index = get_current_volume_index( slice_window );

    if ((fp = fopen(filename, "r")) == NULL)
    {
        print_error("Can't open file '%s'.\n", filename);
        return VIO_ERROR;
    }

    if (fscanf(fp, "MNI-Oblique-Plane V%d\n", &version) != 1 || version != 1)
    {
        print_error("Missing or incorrect version (%d).\n", version);
        return VIO_ERROR;
    }

    if (fscanf(fp, "%g %g %g\n", &wx, &wy, &wz) != 3)
    {
        print_error("Can't read oblique plane vector.\n");
        return VIO_ERROR;
    }
    convert_world_vector_to_voxel(volume, wx, wy, wz, perp_axis);

    if (fscanf(fp, "%g %g %g\n", &wx, &wy, &wz) != 3)
    {
        print_error("Can't read oblique plane origin.\n");
        return VIO_ERROR;
    }
    fclose(fp);

    convert_world_to_voxel(volume, wx, wy, wz, voxel);

    set_slice_plane_perp_axis( slice_window, volume_index, view_index,
                               perp_axis);

    set_current_voxel( slice_window, volume_index, voxel );
    update_cursor_from_voxel( slice_window );
    reset_slice_view( slice_window, view_index );

    for (volume_index = 0; volume_index < get_n_volumes(slice_window);
         volume_index++)
    {
        set_slice_visibility( slice_window, volume_index, view_index, TRUE );
    }
    return( VIO_OK );
}

DEF_MENU_UPDATE(load_oblique_plane)
{
    display_struct *slice_window;

    return (get_slice_window( display, &slice_window ) &&
            get_n_volumes( slice_window ) > 0);
}

DEF_MENU_FUNCTION(save_oblique_plane)
{
    display_struct *slice_window;
    VIO_STR        filename;
    VIO_Status     status;
    int            view_index = get_arbitrary_view_index( display );
    int            volume_index;
    VIO_Real       voxel[VIO_MAX_DIMENSIONS];
    VIO_Real       xw, yw, zw;
    VIO_Vector     normal;
    FILE           *fp;
    VIO_Volume     volume;

    if( !get_slice_window( display, &slice_window ) ||
        get_n_volumes( slice_window ) == 0)
    {
        return VIO_ERROR;
    }

    status = get_user_file("Enter path to save oblique plane: ", TRUE, NULL,
                           &filename);
    if (status != VIO_OK)
    {
        return VIO_ERROR;
    }

    if ((fp = fopen(filename, "w")) == NULL)
    {
        print_error("Unable to open file '%s'.\n", filename);
        return VIO_ERROR;
    }

    volume = get_volume( slice_window );
    volume_index = get_current_volume_index( slice_window );
    get_slice_perp_axis( slice_window, volume_index, view_index, voxel );

    convert_voxel_vector_to_world( volume, voxel, &xw, &yw, &zw );

    fill_Vector( normal, xw, yw, zw );
    NORMALIZE_VECTOR( normal, normal );

    fprintf(fp, "MNI-Oblique-Plane V1\n");
    fprintf(fp, "%g %g %g\n",
            Vector_x(normal), Vector_y(normal), Vector_z(normal) );

    get_current_voxel( slice_window, volume_index, voxel );

    convert_voxel_to_world( volume, voxel, &xw, &yw, &zw );

    fprintf(fp, "%g %g %g\n", xw, yw, zw );
    fclose(fp);
    return( VIO_OK );
}

DEF_MENU_UPDATE(save_oblique_plane)
{
    display_struct *slice_window;

    return (get_slice_window( display, &slice_window ) &&
            get_n_volumes( slice_window ) > 0);
}


