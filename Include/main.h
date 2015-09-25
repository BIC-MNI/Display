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

#define  CURSOR_MODEL           0
#define  OVERLAY_MODEL          1
#define  THREED_MODEL           2
#define  MISCELLANEOUS_MODEL    3
#define  CUT_BUFFER_MODEL       4
#define  STATUS_MODEL           5

/* for menu windows */

#define  MENU_BUTTONS_MODEL     0
#define  SELECTED_MODEL         1
#define  UTILITY_MODEL          2
#define  CURSOR_POS_MODEL       3

/* for slice windows */

#define  FULL_WINDOW_MODEL      0
#define  SLICE_MODEL1           1
#define  SLICE_MODEL2           2
#define  SLICE_MODEL3           3
#define  SLICE_MODEL4           4
#define  COLOUR_BAR_MODEL       5
#define  SLICE_READOUT_MODEL    6

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
  int           ndims;
  int           *dims;
  VIO_Real      *data;
  VIO_Real      min_v;
  VIO_Real      max_v;
} vertex_data_struct;

typedef  struct
{
    cursor_struct              cursor;
    view_struct                view;
    light_struct               lights[N_LIGHTS];
    selection_struct           current_object;
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
} three_d_window_struct;


typedef  struct  display_struct
{
    window_types               window_type;

    struct  display_struct     *associated[N_WINDOW_TYPES];

    three_d_window_struct      three_d;    /* should be a union of three */
    menu_window_struct         menu;
    slice_window_struct        slice;
    marker_window_struct       marker;

    Gwindow                    window;
    VIO_Point                  prev_mouse_position;
    action_table_struct        action_table;
    int                        models_changed_id;
    object_struct              *models[N_MODELS];
    struct stack_list          **label_stack;


    int                        frame_number;
    VIO_BOOL                   update_required[N_BITPLANE_TYPES];
    update_interrupted_struct  update_interrupted;

    viewport_picking_struct    viewport_picking;

} display_struct;

#endif
