
#ifndef  DEF_MENU
#define  DEF_MENU

#include  <def_mni.h>

typedef  Status   menu_function_type();

typedef  menu_function_type  (*menu_function_pointer);

#define  DEF_MENU_FUNCTION( m )  Status m( graphics, menu_window, menu_entry ) \
                                               graphics_struct   *graphics; \
                                               graphics_struct   *menu_window; \
                                               menu_entry_struct *menu_entry;


typedef  Status   menu_update_type();

typedef  menu_update_type  (*menu_update_pointer);

#define  DEF_MENU_UPDATE(m)  Status GLUE(menu_update_,m)( graphics, menu_window, menu_entry, label ) \
                                       graphics_struct   *graphics; \
                                       graphics_struct   *menu_window; \
                                       menu_entry_struct *menu_entry; \
                                       char              label[]; \

typedef  struct  menu_entry_struct
{
    Boolean                     active;
    Boolean                     permanent_flag;
    int                         key;
    String                      label;
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
} menu_window_struct;

#endif
