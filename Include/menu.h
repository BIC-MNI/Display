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
static char display_menu_rcsid[] = "$Header: /private-cvsroot/visualization/Display/Include/menu.h,v 1.20 1995-07-31 19:53:44 david Exp $";
#endif

#include  <volume_io.h>

typedef  Status   menu_function_type();

typedef  menu_function_type  (*menu_function_pointer);

#define  DEF_MENU_FUNCTION( m )  Status m( \
   display_struct    *display, \
   display_struct    *menu_window, \
   menu_entry_struct *menu_entry )


typedef  BOOLEAN   menu_update_type();

typedef  menu_update_type  (*menu_update_pointer);

#define  DEF_MENU_UPDATE(m)  BOOLEAN GLUE(menu_update_,m)( \
                                       display_struct    *display, \
                                       display_struct    *menu_window, \
                                       menu_entry_struct *menu_entry )

typedef  struct  menu_entry_struct
{
    BOOLEAN                     permanent_flag;
    int                         key;
    STRING                      label;
    BOOLEAN                     is_active;
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
    Real                 x_dx;
    Real                 x_dy;
    Real                 y_dx;
    Real                 y_dy;

    int                  n_chars_per_unit_across;
    int                  n_lines_in_entry;

    Real                 character_height;
    Real                 character_width;

    int                  n_entries;
    menu_entry_struct    *entries;

    int                  depth;
    menu_entry_struct    *stack[MAX_MENU_DEPTH];

    menu_entry_struct    *key_menus[N_CHARACTERS];

    BOOLEAN              shift_key_down;

    object_struct        *menu_name_text;
} menu_window_struct;

#endif
