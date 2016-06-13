#ifndef  DEF_MAIN
#define  DEF_MAIN

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

#include  <display_types.h>
#include  <menu.h>
#include  <slice.h>
#include  <surface_extract.h>
#include  <stack.h>

/* for graphics windows */
enum three_d_models {
  CURSOR_MODEL,
  OVERLAY_MODEL,
  THREED_MODEL,
  MISCELLANEOUS_MODEL,
  CUT_BUFFER_MODEL,
  STATUS_MODEL,
  VTX_CODING_MODEL
};

/* for menu windows */
enum menu_models {
  MENU_BUTTONS_MODEL,
  UTILITY_MODEL,
  CURSOR_POS_MODEL
};

/* for slice windows */

enum slice_models {
  FULL_WINDOW_MODEL,
  SLICE_MODEL1,
  SLICE_MODEL2,
  SLICE_MODEL3,
  SLICE_MODEL4,
  COLOUR_BAR_MODEL,
  SLICE_READOUT_MODEL,
  INTENSITY_PLOT_MODEL
};

/* for marker windows */
enum marker_models {
  SELECTED_MODEL
};

typedef  enum  { THREE_D_WINDOW,
                 MENU_WINDOW,
                 SLICE_WINDOW,
                 MARKER_WINDOW,
                 N_WINDOW_TYPES }
               window_types;

#ifndef VIO_T
#define VIO_T 3                 /* Time axis. */
#endif
#ifndef VIO_V
#define VIO_V 4                 /* Vector axis. */
#endif

typedef  struct
{
  object_struct *owner;
  int           column_index;
  colour_coding_struct colour_coding;
  int           ndims;
  int           *dims;
  VIO_Real      *data;
  VIO_Real      *min_v;
  VIO_Real      *max_v;
  VIO_STR       *column_names;
} vertex_data_struct;

typedef  struct
{
    cursor_struct              cursor;
    view_struct                view;
    light_struct               lights[N_LIGHTS];
    int                        current_object;
    VIO_Point                  min_limit;
    VIO_Point                  max_limit;
    VIO_Point                  centre_of_objects;
    surface_extraction_struct  surface_extraction;
    film_loop_struct           film_loop;
    surface_edit_struct        surface_edit;
    cursor_contours_struct     cursor_contours;
    surface_curve_struct       surface_curve;
    int                        default_marker_structure_id;
    int                        default_marker_patient_id;
    Marker_types               default_marker_type;
    VIO_Real                   default_marker_size;
    VIO_Colour                 default_marker_colour;
    VIO_STR                    default_marker_label;
    object_struct              *volume_outline;
    object_struct              *cross_section;
    int                        vertex_data_count;
    vertex_data_struct         **vertex_data_array;
    object_struct              *mouse_obj;
    int                        mouse_point;
    colour_bar_struct          colour_bar;
    int                        animation_axis;
    VIO_Real                   animation_rpm;
} three_d_window_struct;


typedef  struct  display_struct
{
    window_types               window_type;

    three_d_window_struct      three_d;    /* should be a union of four */
    menu_window_struct         menu;
    slice_window_struct        slice;
    marker_window_struct       marker;

    Gwindow                    window;
    VIO_Point                  prev_mouse_position;
    VIO_Point                  prev_mouse_pixel_position;
    action_table_struct        action_table;
    int                        models_changed_id;
    object_struct              *models[N_MODELS];

    int                        frame_number;
    VIO_BOOL                   update_required[N_BITPLANE_TYPES];

    viewport_picking_struct    viewport_picking;

} display_struct;

#endif
