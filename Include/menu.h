#ifndef  DEF_MENU
#define  DEF_MENU

/**
 * \file menu.h
 * \brief Macros and structures related to menu commands.
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

#include  <volume_io.h>

struct display_struct;
struct menu_entry;

/** Defines the type signature of a menu command function.
 *
 * These are the functions that actually implement the command actions.
 */
typedef  VIO_Status   menu_function_type( struct display_struct *,
                                      struct display_struct *,
                                      struct menu_entry * );

/** Shorthand for a pointer to a menu command function. */
typedef  menu_function_type  (*menu_function_pointer);

/** Macro used to define or declare a menu command function. */
#define  DEF_MENU_FUNCTION( m )  VIO_Status m( \
   display_struct    *display, \
   display_struct    *menu_window, \
   menu_entry_struct *menu_entry )

/** Defines the type signature of a menu update function.
 *
 * These are functions that are used to check whether a command should
 * be enabled or not, and to update the text of commands that offer
 * feedback about the state of the program.
 */
typedef  VIO_BOOL   menu_update_type( struct display_struct    *,
                                      struct display_struct    *,
                                      struct menu_entry * );

/** Shorthand for a pointer to a menu update function. */
typedef  menu_update_type  (*menu_update_pointer);

/** Macro used to define or declare a menu update function. */
#define  DEF_MENU_UPDATE(m)  VIO_BOOL menu_update_##m( \
                                       display_struct    *display, \
                                       display_struct    *menu_window, \
                                       menu_entry_struct *menu_entry )

/**
 * This is the structure that represents a menu command in the table.
 */
typedef struct menu_entry
{
    /** If TRUE, this command is permanent - it is left in place when the
     * rest of the menu is pushed or popped.
     */
    VIO_BOOL              permanent_flag;
    /** Key associated with this command. This corresponds to the
     * key code returned from the graphics library.
     */
    int                   key;
    /** Raw label text for the keyboard display.
     */
    VIO_STR               label;
    /** True if command is enabled or active. */
    VIO_BOOL              is_active;
    /** Current nesting depth of this command. */
    int                   current_depth;
    /** Number of submenu items, if this is a menu. */
    int                   n_children;
    /** Width of key text in pixels. */
    int                   key_text_width;
    /** Array of pointers to submenu items. */
    struct  menu_entry    **children;
    /** Pointer to the command function. */
    menu_function_pointer action;
    /** Pointer to the status or text update function. */
    menu_update_pointer   update_action;
    /** Command help text that is to appear when the mouse pointer is over
     * the command key display.
     */
    VIO_STR               help_text;
    /** Array of pointers to text objects for the key label.  */
    object_struct         **text_list;
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
typedef struct menu_window
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

    VIO_Real          font_size; /**< Desired font height. */

    int               n_entries; /**< Total number of menu entries.  */
    menu_entry_struct *entries;  /**< List of all menu entries.  */

    int               depth;    /**< Current menu stack depth.  */

    menu_entry_struct *stack[MAX_MENU_DEPTH]; /**< Stack of previous entries. */

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
typedef struct marker_window
{
    int      default_x_size;    /**< Original window width.  */
    int      default_y_size;    /**< Original window height.  */

    /** Defines the position in pixesl of the left side of the selection box.
     */
    VIO_Real selected_x_origin;

    /** Defines the position in pixels of the bottom of the selection box.
     */
    VIO_Real selected_y_origin;

    /** Defines the x padding of text within the selection box.
     */
    VIO_Real selected_x_offset;

    /** Defines the y padding of text within the selection box.
     */
    VIO_Real selected_y_offset;

    VIO_Real font_size;         /**< Desired size of marker window font.  */

} marker_window_struct;

#endif
