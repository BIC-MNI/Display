
#ifndef  DEF_MENU
#define  DEF_MENU

#include  <def_standard.h>

typedef  Status   menu_function_type();

typedef  menu_function_type  (*menu_function_pointer);

#define  DEF_MENU_FUNCTION( m )  Status m( graphics, menu_window, menu_entry ) \
                                               graphics_struct   *graphics; \
                                               graphics_struct   *menu_window; \
                                               menu_entry_struct *menu_entry;

#define  MAX_MENU_CHILDREN    15

typedef  struct  menu_entry_struct
{
    Boolean                     visible;
    Boolean                     active;
    char                        key;
    int                         x_pos, y_pos;
    String                      label;
    struct  menu_entry_struct   *parent;
    int                         n_children;
    struct  menu_entry_struct   *children[MAX_MENU_CHILDREN];
    menu_function_pointer       action;
} menu_entry_struct;

#define  MAX_MENU_ENTRIES   100

typedef  struct
{
    int                  n_entries;
    menu_entry_struct    entries[MAX_MENU_ENTRIES];
} menu_window_struct;




#endif
