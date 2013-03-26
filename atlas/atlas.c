
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint

#endif

#include  <display.h>

private  VIO_Status  input_pixel_map(
    VIO_STR          default_directory,
    VIO_STR          image_filename,
    pixels_struct   *pixels );

private  const  int   ATLAS_SIZE[VIO_N_DIMENSIONS] = { 256, 256, 80 };
private  const  VIO_Real  ATLAS_STEPS[VIO_N_DIMENSIONS] = { 0.67, 0.86, 1.5 };
private  const  VIO_Real  ATLAS_STARTS[VIO_N_DIMENSIONS] = {  -86.095, -126.51, -37.5 };

public  void  initialize_atlas(
    atlas_struct   *atlas )
{
    atlas->input = FALSE;
    atlas->enabled = FALSE;
    atlas->opacity = Initial_atlas_opacity;
    atlas->transparent_threshold = Initial_atlas_transparent_threshold;
    atlas->slice_tolerance[X] = Initial_atlas_tolerance_x;
    atlas->slice_tolerance[Y] = Initial_atlas_tolerance_y;
    atlas->slice_tolerance[Z] = Initial_atlas_tolerance_z;
    atlas->flipped[X] = FALSE;
    atlas->flipped[Y] = FALSE;
    atlas->flipped[Z] = FALSE;
    atlas->n_images = 0;
}

public  void  delete_atlas(
    atlas_struct   *atlas )
{
    int    im;

    for_less( im, 0, atlas->n_images )
    {
        delete_volume( atlas->images[im].image );
    }

    if( atlas->n_images > 0 )
    {
        FREE( atlas->images );
    }
}

private  VIO_Volume  convert_pixels_to_volume(
    int            axis_index,
    VIO_Real           slice_position,
    pixels_struct  *pixels )
{
    int      x, y, sizes[2];
    int      ind, dim, x_index, y_index;
    VIO_STR   dim_names[2];
    int      dim_orders[VIO_N_DIMENSIONS][2] = {
                                                         { Y, Z },
                                                         { X, Z },
                                                         { X, Y }
                                                      };
    VIO_Volume   volume;
    VIO_Real     separations[2];
    VIO_Real     bottom_left[2];
    VIO_Real     world_corner[VIO_N_DIMENSIONS];

    for_less( dim, 0, 2 )
        dim_names[dim] = XYZ_dimension_names[dim_orders[axis_index][dim]];

    volume = create_volume( 2, dim_names, NC_BYTE, FALSE, 0.0, 0.0 );

    set_volume_real_range( volume, 0.0, 255.0 );

    sizes[0] = pixels->x_size;
    sizes[1] = pixels->y_size;

    set_volume_sizes( volume, sizes );

    alloc_volume_data( volume );

    for_less( x, 0, pixels->x_size )
    for_less( y, 0, pixels->y_size )
    {
        ind = (int) PIXEL_COLOUR_INDEX_8( *pixels, x, y );

        set_volume_voxel_value( volume, x, y, 0, 0, 0, (VIO_Real) ind );
    }

    x_index = dim_orders[axis_index][0];
    y_index = dim_orders[axis_index][1];

    separations[0] = ATLAS_STEPS[x_index] * (VIO_Real) ATLAS_SIZE[x_index] /
                     (VIO_Real) pixels->x_size;
    separations[1] = ATLAS_STEPS[y_index] * (VIO_Real) ATLAS_SIZE[y_index] /
                     (VIO_Real) pixels->y_size;

    bottom_left[0] = -0.5;
    bottom_left[1] = -0.5;

    world_corner[x_index] = ATLAS_STARTS[x_index] - ATLAS_STEPS[x_index] / 2.0;
    world_corner[y_index] = ATLAS_STARTS[y_index] - ATLAS_STEPS[y_index] / 2.0;
    world_corner[axis_index] = slice_position;

    set_volume_separations( volume, separations );
    set_volume_translation( volume, bottom_left, world_corner );

    return( volume );
}

private  VIO_Status  input_atlas(
    atlas_struct   *atlas,
    VIO_STR         filename )
{
    VIO_Status           status;
    FILE             *file;
    VIO_STR           *image_filenames, image_filename;
    char             axis_letter;
    VIO_Real             talairach_position;
    int              axis_index, image;
    VIO_STR           atlas_directory;
    pixels_struct    pixels;
    progress_struct  progress;

    atlas_directory = extract_directory( filename );

    atlas->n_images = 0;

    status = open_file( filename, READ_FILE, ASCII_FORMAT, &file );

    if( status != OK )
        return( status );

    image_filenames = (VIO_STR *) NULL;

    while( input_string( file, &image_filename, ' ' ) == OK )
    {
        status = ERROR;

        if( input_nonwhite_character( file, &axis_letter ) != OK )
            break;

        if( axis_letter >= 'x' && axis_letter <= 'z' )
            axis_index = axis_letter - 'x';
        else if( axis_letter >= 'X' && axis_letter <= 'Z' )
            axis_index = axis_letter - 'X';
        else
            break;

        if( input_real( file, &talairach_position ) != OK )
            break;

        SET_ARRAY_SIZE( image_filenames,
                        atlas->n_images, atlas->n_images+1,
                        DEFAULT_CHUNK_SIZE );
        image_filenames[atlas->n_images] = image_filename;

        SET_ARRAY_SIZE( atlas->images, atlas->n_images, atlas->n_images+1,
                        DEFAULT_CHUNK_SIZE );
        atlas->images[atlas->n_images].axis = axis_index;
        atlas->images[atlas->n_images].axis_position = talairach_position;

        ++atlas->n_images;

        status = OK;
    }

    if( status == OK )
    {
        status = close_file( file );

        initialize_progress_report( &progress, FALSE, atlas->n_images,
                                    "Reading Atlas" );

        for_less( image, 0, atlas->n_images )
        {
            status = input_pixel_map( atlas_directory,
                                      image_filenames[image],
                                      &pixels );
            if( status != OK )
                break;

            atlas->images[image].image = convert_pixels_to_volume(
                                          atlas->images[image].axis,
                                          atlas->images[image].axis_position,
                                          &pixels );

            delete_pixels( &pixels );

            update_progress_report( &progress, image+1 );
        }

        terminate_progress_report( &progress );
    }

    if( status == OK && atlas->n_images > 0 )
    {
        for_less( image, 0, atlas->n_images )
            delete_string( image_filenames[image] );

        FREE( image_filenames );
    }

    if( status == OK )
        atlas->input = TRUE;
    else
        print( "Error inputting atlas.\n" );

    delete_string( atlas_directory );

    return( status );
}

private  VIO_Status  input_pixel_map(
    VIO_STR         default_directory,
    VIO_STR         image_filename,
    pixels_struct  *pixels )
{
    VIO_Status         status;
    VIO_STR         absolute_filename;
    VIO_File_formats   format;
    Object_types   object_type;
    VIO_BOOL        eof;
    FILE           *file;

    absolute_filename = get_absolute_filename( image_filename,
                                               default_directory );

    status = open_file( absolute_filename, READ_FILE, BINARY_FORMAT, &file );

    if( status == OK )
        status = input_object_type( file, &object_type, &format, &eof );

    if( status == OK && !eof && object_type == PIXELS )
        status = io_pixels( file, READ_FILE, format, pixels );
    else
        status = ERROR;

    if( status == OK && pixels->pixel_type != COLOUR_INDEX_8BIT_PIXEL )
        status = ERROR;

    if( status == OK )
        status = close_file( file );

    delete_string( absolute_filename );

    return( status );
}

public  void  regenerate_atlas_lookup(
    display_struct    *slice_window )
{
}

public  void  set_atlas_state(
    display_struct    *slice_window,
    VIO_BOOL           state )
{
    VIO_Status   status;

    status = OK;

    if( state && !slice_window->slice.atlas.input )
    {
        status = input_atlas( &slice_window->slice.atlas, Atlas_filename );

        if( status == OK )
            regenerate_atlas_lookup( slice_window );
    }

    slice_window->slice.atlas.enabled = state;
}

public  VIO_BOOL  is_atlas_loaded(
    display_struct  *display )
{
    display_struct  *slice_window;

    return( get_slice_window( display, &slice_window ) &&
            slice_window->slice.atlas.n_images > 0 );
}

private  VIO_BOOL  find_appropriate_atlas_image(
    atlas_struct      *atlas,
    int               x_n_pixels,
    int               y_n_pixels,
    VIO_Real              world_start[],
    VIO_Real              world_x_axis[],
    VIO_Real              world_y_axis[],
    VIO_Volume            *image,
    VIO_Real              origin[],
    VIO_Real              x_axis[],
    VIO_Real              y_axis[],
    VIO_Real              *x_scale,
    VIO_Real              *y_scale )
{
    VIO_Real            min_dist, dist, slice_position, best_scale, scale_dist;
    VIO_Real            separations[2];
    VIO_Real            tmp_x_scale, tmp_y_scale, tmp_origin[2];
    VIO_Real            tmp_x_axis[2];
    VIO_Real            tmp_y_axis[2];
    int             im, axis, dim, a1, a2;

    *image = NULL;
    min_dist = 0.0;
    best_scale = 0.0;

    a1 = -1;
    a2 = -1;
    for_less( dim, 0, VIO_N_DIMENSIONS )
    {
        if( world_x_axis[dim] != 0.0 )
        {
            if( a1 != -1 )
                return( FALSE );
            a1 = dim;
        }
        if( world_y_axis[dim] != 0.0 )
        {
            if( a2 != -1 )
                return( FALSE );
            a2 = dim;
        }
    }

    axis = VIO_N_DIMENSIONS - a1 - a2;

    slice_position = world_start[axis];

    for_less( im, 0, atlas->n_images )
    {
        if( atlas->images[im].axis == axis )
        {
            dist = FABS( slice_position - atlas->images[im].axis_position );

            if( dist <= atlas->slice_tolerance[axis] &&
                (*image == NULL || dist <= min_dist) )
            {
                convert_world_to_voxel( atlas->images[im].image,
                                world_start[X], world_start[Y], world_start[Z],
                                tmp_origin );

                convert_world_vector_to_voxel( atlas->images[im].image,
                                       world_x_axis[X], world_x_axis[Y],
                                       world_x_axis[Z], tmp_x_axis );

                convert_world_vector_to_voxel( atlas->images[im].image,
                                       world_y_axis[X], world_y_axis[Y],
                                       world_y_axis[Z], tmp_y_axis );

                get_volume_separations( atlas->images[im].image, separations );

                for_less( dim, 0, 2 )
                {
                    if( tmp_x_axis[dim] != 0.0 )
                    {
                        tmp_x_scale = 1.0 / FABS( separations[dim] *
                                                  tmp_x_axis[dim] );
                        scale_dist = FABS( 1.0 / FABS(tmp_x_axis[dim]) - 1.0 );
                    }

                    if( tmp_y_axis[dim] != 0.0 )
                        tmp_y_scale = 1.0 / FABS( separations[dim] *
                                                  tmp_y_axis[dim] );

                    if( tmp_x_axis[dim] == 0.0 && tmp_y_axis[dim] == 0.0 )
                        tmp_origin[dim] = 0.0;
                }

                if( *image == NULL || dist < min_dist ||
                    scale_dist < best_scale )
                {
                    *image = atlas->images[im].image;
                    min_dist = dist;
                    best_scale = scale_dist;
                    *x_scale = tmp_x_scale;
                    *y_scale = tmp_y_scale;
                    for_less( dim, 0, 2 )
                    {
                        origin[dim] = tmp_origin[dim];
                        x_axis[dim] = tmp_x_axis[dim];
                        y_axis[dim] = tmp_y_axis[dim];
                    }
                }
            }
        }
    }

    return( *image != NULL );
}

public  VIO_BOOL  render_atlas_slice_to_pixels(
    atlas_struct  *atlas,
    VIO_Colour        image[],
    int           image_x_size,
    int           image_y_size,
    VIO_Real          world_start[],
    VIO_Real          world_x_axis[],
    VIO_Real          world_y_axis[] )
{
    int            x, y;
    int            r_atlas, g_atlas, b_atlas, a_atlas;
    VIO_Real           x_scale, y_scale;
    VIO_Real           origin[2];
    VIO_Real           x_axis[2];
    VIO_Real           y_axis[2];
    int            transparent_threshold;
    int            n_alloced;
    VIO_Colour         atlas_pixel, *lookup;
    pixels_struct  pixels;
    VIO_Volume         atlas_image;
    VIO_Real           opacity;

    if( !atlas->enabled || atlas->opacity <= 0.0 ||
        !find_appropriate_atlas_image( atlas, image_x_size, image_y_size,
                                       world_start, world_x_axis, world_y_axis,
                                       &atlas_image, origin, x_axis, y_axis,
                                       &x_scale, &y_scale ) )
    {
        return( FALSE );
    }

    lookup = get_8bit_rgb_pixel_lookup();

    n_alloced = image_x_size * image_y_size;
    initialize_pixels( &pixels, 0, 0, 0, 0, 1.0, 1.0, RGB_PIXEL );
    pixels.x_size = image_x_size;
    pixels.y_size = image_y_size;
    pixels.data.pixels_rgb = image;

    create_volume_slice( atlas_image, NEAREST_NEIGHBOUR, 0.0,
                         origin, x_axis, y_axis, 0.0, 0.0, x_scale, y_scale,
                         NULL, NEAREST_NEIGHBOUR, 0.0, NULL, NULL, NULL,
                         0.0, 0.0, 0.0, 0.0,
                         image_x_size, image_y_size,
                         0, image_x_size - 1, 0, image_y_size - 1,
                         RGB_PIXEL, -1,
                         NULL, &lookup, make_rgba_Colour( 0, 0, 0, 0 ),
                         NULL, FALSE, &n_alloced, &pixels );

    opacity = atlas->opacity;
    transparent_threshold = atlas->transparent_threshold;

    for_less( x, 0, image_x_size )
    {
        for_less( y, 0, image_y_size )
        {
            atlas_pixel = PIXEL_RGB_COLOUR( pixels, x, y ); 

            r_atlas = get_Colour_r(atlas_pixel);
            g_atlas = get_Colour_g(atlas_pixel);
            b_atlas = get_Colour_b(atlas_pixel);
            a_atlas = get_Colour_a(atlas_pixel);

            if( r_atlas > transparent_threshold &&
                g_atlas > transparent_threshold &&
                b_atlas > transparent_threshold )
            {
                PIXEL_RGB_COLOUR( pixels, x, y ) = make_rgba_Colour(0,0,0,0);
            }
            else
            {
                PIXEL_RGB_COLOUR( pixels, x, y ) = make_rgba_Colour(
                                               r_atlas,g_atlas,b_atlas,
                                               ROUND((VIO_Real) a_atlas*opacity));
            }
        }
    }

    return( TRUE );
}
