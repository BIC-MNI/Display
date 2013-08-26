#ifndef  DEF_MENU
#define  DEF_MENU

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
static char display_menu_rcsid[] = "$Header: /private-cvsroot/visualization/Display/Include/menu.h,v 1.26 2001/05/27 00:19:36 stever Exp $";
#endif

#include  <volume_io.h>

struct display_struct;
struct menu_entry_struct;

typedef  VIO_Status   menu_function_type( struct display_struct *,
                                      struct display_struct *,
                                      struct menu_entry_struct * );

typedef  menu_function_type  (*menu_function_pointer);

#define  DEF_MENU_FUNCTION( m )  VIO_Status m( \
   display_struct    *display, \
   display_struct    *menu_window, \
   menu_entry_struct *menu_entry )

typedef  VIO_BOOL   menu_update_type( struct display_struct    *,
                                      struct display_struct    *,
                                      struct menu_entry_struct * );

typedef  menu_update_type  (*menu_update_pointer);

#define  DEF_MENU_UPDATE(m)  VIO_BOOL menu_update_##m( \
                                       display_struct    *display, \
                                       display_struct    *menu_window, \
                                       menu_entry_struct *menu_entry )

typedef  struct  menu_entry_struct
{
    VIO_BOOL                    permanent_flag;
    int                         key;
    VIO_STR                     label;
    VIO_BOOL                    is_active;
    int                         current_depth;
    int                         n_children;
    int                         n_chars_across;
    struct  menu_entry_struct   **children;
    menu_function_pointer       action;
    menu_update_pointer         update_action;
    object_struct               **text_list;
} menu_entry_struct;

#define  MAX_MENU_DEPTH     10
#define  N_CHARACTERS       256

typedef  struct
{
    int                  default_x_size;
    int                  default_y_size;

    VIO_Real                 x_dx;
    VIO_Real                 x_dy;
    VIO_Real                 y_dx;
    VIO_Real                 y_dy;

    int                  n_chars_per_unit_across;
    int                  n_lines_in_entry;

    VIO_Real                 character_height;
    VIO_Real                 character_width;
    VIO_Real                 character_offset;
    VIO_Real                 selected_box_height;

    VIO_Real                 x_menu_name;
    VIO_Real                 y_menu_name;

    VIO_Real                 x_menu_origin;
    VIO_Real                 y_menu_origin;

    VIO_Real                 selected_x_origin;
    VIO_Real                 selected_y_origin;
    VIO_Real                 selected_x_offset;
    VIO_Real                 selected_y_offset;

    VIO_Real                 cursor_pos_x_origin;
    VIO_Real                 cursor_pos_y_origin;

    VIO_Real                 x_menu_text_offset;
    VIO_Real                 y_menu_text_offset;
    VIO_Real                 font_size;

    int                  n_entries;
    menu_entry_struct    *entries;

    int                  depth;
    menu_entry_struct    *stack[MAX_MENU_DEPTH];

    menu_entry_struct    *key_menus[N_CHARACTERS];
    object_struct        *box_objects[N_CHARACTERS];
    object_struct        *text_objects[N_CHARACTERS];

    object_struct        *menu_name_text;

} menu_window_struct;

#endif
