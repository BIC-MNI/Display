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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/events/utilities.c,v 1.7 1995-10-19 15:51:24 david Exp $";
#endif


#include  <display.h>

public  void   terminate_any_interactions(
    display_struct   *display )
{
    Status               status;
    event_function_type  *actions;
    int                  i, n_actions;

    n_actions = get_event_actions( &display->action_table,
                                   TERMINATE_INTERACTION_EVENT,
                                   &actions );

    status = OK;

    for_less( i, 0, n_actions )
    {
        status = (*actions[i]) ( display, TERMINATE_INTERACTION_EVENT, 0 );
        if( status != OK )
        {
            break;
        }
    }
}
