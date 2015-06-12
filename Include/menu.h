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
    VIO_BOOL                   permanent_flag; /**< TRUE if permanent.  */
    int                        key;   /**< Key for this command.  */
    VIO_STR                    label; /**< "Raw" command label text. */
    VIO_BOOL                   is_active; /**< True if enabled/active.  */
    int                        current_depth; /**< Nesting depth. */
    int                        n_children; /**< Number of submenu items. */
    int                        key_text_width; /**< Text width in pixels. */
    struct  menu_entry_struct  **children; /**< Submenu items. */
    menu_function_pointer      action; /**< Menu action function   */
    menu_update_pointer        update_action; /**< Menu text update function. */
    VIO_STR                    help_text;     /**< Command help text. */
    object_struct              **text_list;   /**< Text objects for label.  */
} menu_entry_struct;

#define  MAX_MENU_DEPTH     10  /**< Maximum menu hierarchy depth. */
#define  N_CHARACTERS       256 /**< Number of "keys" that can be displayed. */

#define N_HELP_LINES 3          /**< Number of help text lines. */

/** Structure that defines the menu window. 
 *
 * The menu displays a number of boxes that are intended to correspond
 * the buttons on a keyboard. Each box or button has both a fixed key label
 * and a varying command label.
 *
 * The menu window also displays: 
 *  - A menu title for the currently active menu.
 *  - The current cursor position in world coordinates.
 *  - Dynamic help text describing the function of the button under the 
 *    mouse.
 */
typedef  struct
{
    int               default_x_size; /**< Original window width.  */
    int               default_y_size; /**< Original window height.  */

    VIO_Real          x_dx;     /**< X spacing between key boxes. */
    VIO_Real          y_dy;     /**< Y spacing between key boxes. */
    VIO_Real          x_dy;     /**< Normally zero. */
    VIO_Real          y_dx;     /**< Accounts for slanted key column offsets. */

    int               n_lines_in_entry; /**< Text lines per key entry.  */

    VIO_Real          basic_key_width; /**< Max. width of text in box.  */
    VIO_Real          character_height; /**< Height of current font. */
    VIO_Real          character_offset; /**< Offset for key labels.  */

    VIO_Real          x_menu_name; /**< X coordinate for menu name text. */
    VIO_Real          y_menu_name; /**< Y coordinate for menu name text. */

    VIO_Real          x_menu_origin; /**< X coordinate for first box.  */
    VIO_Real          y_menu_origin; /**< Y coordinate for first box.  */

    VIO_Real          help_x_origin; /**< X coordinate for help text. */
    VIO_Real          help_y_origin; /**< Y coordinate for help text. */

    VIO_Real          cursor_pos_x_origin; /**< X coordinate for cursor text. */
    VIO_Real          cursor_pos_y_origin; /**< Y coordinate for cursor text. */

    VIO_Real          x_menu_text_offset; /**< X offset for key text.  */
    VIO_Real          y_menu_text_offset; /**< Y offset for key text. */

    VIO_Real          font_size; /**< Desired font height in pixels. */

    int               n_entries; /**< Total number of menu entries.  */
    menu_entry_struct *entries;  /**< List of all menu entries.  */

    int               depth;    /**< Current menu stack depth.  */
    menu_entry_struct *stack[MAX_MENU_DEPTH]; /**<   */

    menu_entry_struct *key_menus[N_CHARACTERS]; /**< Currently displayed entries.  */
    object_struct     *box_objects[N_CHARACTERS]; /**< Key box objects. */
    object_struct     *text_objects[N_CHARACTERS]; /**< Key text labels. */

    object_struct     *menu_name_text; /**< Menu name text object. */
    object_struct     *menu_help_text[N_HELP_LINES]; /**< Help text objects. */

} menu_window_struct;

/** Structure that defines the marker window. 
 *
 * The marker, or object list window used to be part of the menu window,
 * but is now a separate top-level window.
 */
typedef  struct
{
    int      default_x_size;    /**< Original window width.  */
    int      default_y_size;    /**< Original window height.  */

    VIO_Real selected_x_origin;
    VIO_Real selected_y_origin;
    VIO_Real selected_x_offset;
    VIO_Real selected_y_offset;

    VIO_Real font_size;         /**< Desired size of marker window font.  */

} marker_window_struct;

#endif
