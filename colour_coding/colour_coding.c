
#include  <def_graphics.h>
#include  <def_globals.h>

typedef struct {
    Real           position;
    Colour         colour;
    Colour_spaces  interpolation_space;
} colour_point;

public  void  rebuild_colour_coding( colour_coding )
    colour_coding_struct   *colour_coding;
{
    void   build_gray_scale_coding();
    void   build_hot_metal_coding();
    void   build_contour_coding();
    void   build_spectral_coding();
    void   build_user_defined_coding();

    switch( colour_coding->type )
    {
    case  GRAY_SCALE:
        build_gray_scale_coding( colour_coding );
        break;

    case  HOT_METAL:
        build_hot_metal_coding( colour_coding );
        break;

    case  SPECTRAL:
        build_spectral_coding( colour_coding );
        break;

    case  USER_DEFINED:
        build_user_defined_coding( colour_coding );
        break;

    case  CONTOUR_COLOUR_MAP:
        build_contour_coding( colour_coding );
        break;
    }
}

private  void  build_user_defined_coding( colour_coding )
    colour_coding_struct  *colour_coding;
{
    Status           status;
    void             rebuild_colour_map();
    colour_point     *desc;
    Colour           min_colour, max_colour;
    int              i, n_intervals;

    n_intervals = colour_coding->user_defined_n_intervals;

    ALLOC( status, desc, n_intervals * 2 );

    if( status == OK )
    {
        for_less( i, 0, n_intervals )
        {
            if( User_defined_colour_coding_flip && i % 2 == 1 )
            {
                min_colour = colour_coding->user_defined_max_colour;
                max_colour = colour_coding->user_defined_min_colour;
            }
            else
            {
                min_colour = colour_coding->user_defined_min_colour;
                max_colour = colour_coding->user_defined_max_colour;
            }

            desc[2*i+0].position = (Real) i / (Real) n_intervals;
            desc[2*i+0].colour = min_colour;
            desc[2*i+0].interpolation_space =
                         colour_coding->user_defined_interpolation_space;

            desc[2*i+1].position = (Real) (i+1) / (Real) n_intervals;
            desc[2*i+1].colour = max_colour;
            desc[2*i+1].interpolation_space =
                         colour_coding->user_defined_interpolation_space;
        }

        rebuild_colour_map( colour_coding, 2 * n_intervals, desc );
    }
}

public  void  build_gray_scale_coding( colour_coding )
    colour_coding_struct  *colour_coding;
{
    void    rebuild_colour_map();
    static  colour_point  desc[] = { {0.0, { BLACK_COL }, RGB_SPACE },
                                     {1.0, { WHITE_COL }, RGB_SPACE } };

    rebuild_colour_map( colour_coding, SIZEOF_STATIC_ARRAY(desc), desc );
}

public  void  build_spectral_coding( colour_coding )
    colour_coding_struct  *colour_coding;
{
    void    rebuild_colour_map();
    static  colour_point  desc[] = { {0.0,     { 0.0, 0.0, 1.0 }, HSL_SPACE },
                                     {0.333,   { 0.0, 1.0, 0.0 }, HSL_SPACE },
                                     {0.74,    { 1.0, 0.0, 0.0 }, HSL_SPACE },
                                     {0.86,    { 1.0, 0.0, 1.0 }, HSL_SPACE },
                                     {1.0,     { 1.0, 1.0, 1.0 }, HSL_SPACE } };

    rebuild_colour_map( colour_coding, SIZEOF_STATIC_ARRAY(desc), desc );
}

public  void  build_hot_metal_coding( colour_coding )
    colour_coding_struct  *colour_coding;
{
    void    rebuild_colour_map();
    static  colour_point  desc[] = { {0.0,  { 0.0, 0.0, 0.0 }, RGB_SPACE },
                                     {0.25, { 0.5, 0.0, 0.0 }, RGB_SPACE },
                                     {0.5,  { 1.0, 0.5, 0.0 }, RGB_SPACE },
                                     {0.75, { 1.0, 1.0, 0.5 }, RGB_SPACE },
                                     {1.0,  { 1.0, 1.0, 1.0 }, RGB_SPACE } };

    rebuild_colour_map( colour_coding, SIZEOF_STATIC_ARRAY(desc), desc );
}

public  void  build_contour_coding( colour_coding )
    colour_coding_struct  *colour_coding;
{
    void    rebuild_colour_map();
    static  colour_point  desc[] = { {0.0,   { 0.0, 0.0, 0.3 }, RGB_SPACE },
                                     {0.166, { 0.0, 0.0, 1.0 }, RGB_SPACE },
                                     {0.166, { 0.0, 0.3, 0.3 }, RGB_SPACE },
                                     {0.333, { 0.0, 1.0, 1.0 }, RGB_SPACE },
                                     {0.333, { 0.0, 0.3, 0.0 }, RGB_SPACE },
                                     {0.5,   { 0.0, 1.0, 0.0 }, RGB_SPACE },
                                     {0.5,   { 0.3, 0.3, 0.0 }, RGB_SPACE },
                                     {0.666, { 1.0, 1.0, 0.0 }, RGB_SPACE },
                                     {0.666, { 0.3, 0.0, 0.0 }, RGB_SPACE },
                                     {0.833, { 1.0, 0.0, 0.0 }, RGB_SPACE },
                                     {0.833, { 0.3, 0.3, 0.3 }, RGB_SPACE },
                                     {1.0,   { 1.0, 1.0, 1.0 }, RGB_SPACE } };

    rebuild_colour_map( colour_coding, SIZEOF_STATIC_ARRAY(desc), desc );
}

private  void  rebuild_colour_map( colour_coding, n_points, points ) 
    colour_coding_struct  *colour_coding;
    int                   n_points;
    colour_point          points[];
{
    int    i, n_entries, first_entry, last_entry;
    void   interpolate_colours();

    n_entries = colour_coding->colour_table_size;

    for_less( i, 0, n_points-1 )
    {
        if( points[i].position != points[i+1].position )
        {
            if( i == 0 )
                first_entry = 0;
            else
                first_entry = ROUND( points[i].position * (Real) (n_entries-1));

            if( i < n_points-1 )
                last_entry = ROUND( points[i+1].position * (Real)(n_entries-1));
            else
                last_entry = n_entries - 1;

            interpolate_colours( first_entry, last_entry,
                                 colour_coding->colour_table,
                                 &points[i].colour, &points[i+1].colour,
                                 points[i].interpolation_space );
        }
    }
}

private  void  interpolate_colours( first_entry, last_entry, table,
                                    first_colour, last_colour,
                                    interpolation_space )
    int            first_entry;
    int            last_entry;
    Colour         table[];
    Colour         *first_colour;
    Colour         *last_colour;
    Colour_spaces  interpolation_space;
{
    int   i;
    Real  ratio, r,g, b, r0, g0, b0, r1, g1, b1;
    void  hsl_to_rgb();
    void  rgb_to_hsl();

    r0 = Colour_r( *first_colour );
    g0 = Colour_g( *first_colour );
    b0 = Colour_b( *first_colour );

    r1 = Colour_r( *last_colour );
    g1 = Colour_g( *last_colour );
    b1 = Colour_b( *last_colour );

    if( interpolation_space == HSL_SPACE )
    {
        rgb_to_hsl( r0, g0, b0, &r0, &g0, &b0 );
        rgb_to_hsl( r1, g1, b1, &r1, &g1, &b1 );

        if( r0 == 1.0 && r1 < 0.5 )
            r0 = 0.0;
        if( r1 == 1.0 && r0 < 0.5 )
            r1 = 0.0;
    }

    for_inclusive( i, first_entry, last_entry )
    {
        ratio = (Real) (i - first_entry) /
                (Real) (last_entry - first_entry);

        r = r0 + (r1 - r0) * ratio;
        g = g0 + (g1 - g0) * ratio;
        b = b0 + (b1 - b0) * ratio;

        if( interpolation_space == HSL_SPACE )
            hsl_to_rgb( r, g, b, &r, &g, &b );

        fill_Colour( table[i], r, g, b );
    }
}

