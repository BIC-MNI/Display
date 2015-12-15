/** 
 * \file vertex_data.c
 * \brief Input functions for surface data.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <display.h>
#include <float.h>

#define VERTEX_DATA_INCREMENT 500

int split_line(char *text_ptr, int sep, char ***argv)
{
  int len = 0;
  char *save_ptr;

  for (save_ptr = text_ptr; *text_ptr != '\0'; text_ptr++)
  {
    if (*text_ptr == sep || *text_ptr == '\n')
    {
      ADD_ELEMENT_TO_ARRAY( (*argv), len, save_ptr, 1 );
      *text_ptr = 0;
      save_ptr = text_ptr + 1;
    }
  }
  return len;
}

vertex_data_struct *
input_vertex_data( const char *filename )
{
    FILE *fp;
    int  len = 0;
    int  n_line = 0;
    char buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    vertex_data_struct *vtxd_ptr;

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return NULL;
    }
    ALLOC(vtxd_ptr, 1);

    vtxd_ptr->ndims = 2;

    ADD_ELEMENT_TO_ARRAY(vtxd_ptr->dims, len, 0, 1 );
    ADD_ELEMENT_TO_ARRAY(vtxd_ptr->dims, len, 0, 1 );

    len = 0;

    while (fgets(buffer, sizeof(buffer) - 1, fp))
    {
        float v;
        char **argv;
        int n = split_line(buffer, ' ', &argv);
        int i;

        n_line++;

        if (vtxd_ptr->dims[1] == 0)
        {
          if (sscanf(argv[0], "%f", &v) == 1)
          {
            vtxd_ptr->dims[1] = n;
            ALLOC(vtxd_ptr->max_v, n);
            for (i = 0; i < n; i++)
            {
                vtxd_ptr->max_v[i] = -FLT_MAX;
            }
            ALLOC(vtxd_ptr->min_v, n);
            for (i = 0; i < n; i++)
            {
                vtxd_ptr->min_v[i] = FLT_MAX;
            }
          }
          else
          {
            n = 0;
          }
        }
        else if (n != vtxd_ptr->dims[1])
        {
          fprintf(stderr, "Warning: inconsistent number of items per line.\n");
        }

        for (i = 0; i < n; i++)
        {
          if (sscanf(argv[i], "%f", &v) != 1)
          {
            fprintf(stderr, "Failed to read '%s'\n", filename);
            FREE(vtxd_ptr->data);
            FREE(vtxd_ptr->dims);
            FREE(vtxd_ptr);
            return NULL;
          }
          if (v > vtxd_ptr->max_v[i])
            vtxd_ptr->max_v[i] = v;
          if (v < vtxd_ptr->min_v[i])
            vtxd_ptr->min_v[i] = v;
          ADD_ELEMENT_TO_ARRAY(vtxd_ptr->data, len, v, VERTEX_DATA_INCREMENT);
        }
    }
    vtxd_ptr->dims[0] = len / vtxd_ptr->dims[1];
    print("Read %d lines of vertex data.\n", vtxd_ptr->dims[0]);
    print("There are %d items per line.\n", vtxd_ptr->dims[1]);
    fclose(fp);
    return vtxd_ptr;
}
