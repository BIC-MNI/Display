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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/structures/render.c,v 1.10 1995-10-19 15:52:31 david Exp $";
#endif


#include  <display.h>

public  void  initialize_render(
    render_struct  *render )
{
    render->shaded_mode = TRUE;
    render->shading_type = (Shading_types) GOURAUD_SHADING;
    render->master_light_switch = FALSE;
    render->backface_flag = FALSE;
    render->two_sided_surface_flag = FALSE;
    render->render_lines_as_curves = FALSE;
    render->show_marker_labels = FALSE;
    render->n_curve_segments = Initial_n_curve_segments;
}

public  void  initialize_render_3D(
    render_struct  *render )
{
    render->shaded_mode = Initial_render_mode;
    render->shading_type = (Shading_types) Initial_shading_type;
    render->master_light_switch = Initial_light_switch;
    render->backface_flag = Initial_backface_flag;
    render->two_sided_surface_flag = Initial_2_sided_flag;
    render->render_lines_as_curves = Initial_line_curves_flag;
    render->show_marker_labels = FALSE;
    render->n_curve_segments = Initial_n_curve_segments;
}

public  void  set_render_info(
    window_struct  *window,
    render_struct  *render )
{
    G_set_shaded_state( window, render->shaded_mode );
    G_set_shading_type( window, render->shading_type );
    G_set_lighting_state( window, render->master_light_switch );
    G_backface_culling_state( window, render->backface_flag );
    G_set_render_lines_as_curves_state( window, render->render_lines_as_curves);
    G_set_markers_labels_visibility( window, render->show_marker_labels );
    G_set_n_curve_segments( window, render->n_curve_segments );
}
