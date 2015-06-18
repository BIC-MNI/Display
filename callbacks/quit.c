/**
 * \file quit.c
 * \brief Handle the quit command and a few other odds and ends.
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include  <display.h>

/* ARGSUSED */

DEF_MENU_FUNCTION( exit_program )
{
    return( VIO_QUIT );
}

/* ARGSUSED */

DEF_MENU_UPDATE(exit_program )
{
    return( TRUE );
}

DEF_MENU_FUNCTION(save_window_state)
{
  char filename[1024];
  char *direct = getenv("HOME");
  FILE *fp;
  if (direct == NULL)
  {
    direct = ".";
  }
  snprintf(filename, sizeof(filename), "%s/.mni-displayrc", direct);
  if ((fp = fopen(filename, "w")) != NULL)
  {
    print_graphics_state(fp);
    printf("Saved window state to %s\n", filename);
    fclose(fp);
  }
  return VIO_OK;
}

DEF_MENU_UPDATE(save_window_state)
{
  return TRUE;
}
