
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_math.h>
#include  <def_files.h>

static    void     get_min_max();
static    Boolean  min_max_present();

public  DEF_MENU_FUNCTION( label_point )   /* ARGSUSED */
{
    Status   status;
    int      id, x, y, z, axis_index;
    Status   add_point_label();

    status = OK;

    if( get_voxel_under_mouse( graphics, &x, &y, &z, &axis_index ) )
    {
        PRINT( "Enter id: " );
        (void) scanf( "%d", &id );
        status = add_point_label( graphics->associated[SLICE_WINDOW],
                                  x, y, z, id);
    }

    return( status );
}

public  DEF_MENU_UPDATE(label_point )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( generate_regions )   /* ARGSUSED */
{
    Status   status;
    int      voxel_index[3], voxel_axes[3];
    Status   generate_segmentation();
    void     set_slice_window_update();

    status = OK;

    if( !min_max_present(graphics) )
        get_min_max( graphics );

    if( min_max_present(graphics) &&
        get_voxel_under_mouse( graphics, &voxel_index[X_AXIS],
                               &voxel_index[Y_AXIS],
                               &voxel_index[Z_AXIS], &voxel_axes[2] ) )
    {
        voxel_axes[0] = (voxel_axes[2] + 1) % N_DIMENSIONS;
        voxel_axes[1] = (voxel_axes[2] + 2) % N_DIMENSIONS;

        status = generate_segmentation( graphics->associated[SLICE_WINDOW],
                                        voxel_index, voxel_axes );

        set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
        set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );
    }

    return( status );
}

public  DEF_MENU_UPDATE(generate_regions )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( reset_segmenting )   /* ARGSUSED */
{
    Status   status;
    void     set_slice_window_update();
    Status   reset_segmentation();

    status = reset_segmentation( graphics->associated[SLICE_WINDOW] );

    set_slice_window_update( graphics->associated[SLICE_WINDOW], 0 );
    set_slice_window_update( graphics->associated[SLICE_WINDOW], 1 );
    set_slice_window_update( graphics->associated[SLICE_WINDOW], 2 );

    return( status );
}

public  DEF_MENU_UPDATE(reset_segmenting )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_segmenting_threshold )   /* ARGSUSED */
{
    get_min_max( graphics );

    return( OK );
}

public  DEF_MENU_UPDATE(set_segmenting_threshold )   /* ARGSUSED */
{
    return( OK );
}

private  void  get_min_max( graphics )
    graphics_struct  *graphics;
{
    int      min, max;

    PRINT( "Enter min and max threshold: " );

    if( scanf( "%d %d", &min, &max ) != 2 )
    {
        min = -1;
        max = -1;
    }

    graphics->associated[SLICE_WINDOW]->slice.segmenting.min_threshold = min;
    graphics->associated[SLICE_WINDOW]->slice.segmenting.max_threshold = max;
}

private  Boolean  min_max_present( graphics )
    graphics_struct  *graphics;
{
    return( 
     graphics->associated[SLICE_WINDOW]->slice.segmenting.min_threshold >= 0 &&
     graphics->associated[SLICE_WINDOW]->slice.segmenting.max_threshold >= 0 &&
     graphics->associated[SLICE_WINDOW]->slice.segmenting.min_threshold <=
      graphics->associated[SLICE_WINDOW]->slice.segmenting.max_threshold );
}
