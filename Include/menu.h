
#ifndef  DEF_MENU
#define  DEF_MENU

#include  <def_standard.h>

typedef  Status   menu_function_type();

typedef  menu_function_type  (*menu_function_pointer);

#define  DEF_MENU_FUNCTION( m )  Status m( graphics, menu_window, menu_entry ) \
                                               graphics_struct   *graphics; \
                                               graphics_struct   *menu_window; \
                                               menu_entry_struct *menu_entry; \
                                                /* ARGSUSED */

#define  MAX_MENU_CHILDREN    15

typedef  struct  menu_entry_struct
{
    Boolean                     active;
    Boolean                     permanent_flag;
    char                        key;
    String                      label;
    int                         n_children;
    struct  menu_entry_struct   *children[MAX_MENU_CHILDREN];
    menu_function_pointer       action;
    object_struct               *text;
} menu_entry_struct;

#define  MAX_MENU_DEPTH     10
#define  N_CHARACTERS       256

typedef  struct
{
    int                  n_entries;
    menu_entry_struct    *entries;

    int                  depth;
    menu_entry_struct    *stack[MAX_MENU_DEPTH];

    menu_entry_struct    *key_menus[N_CHARACTERS];
} menu_window_struct;




#endif
