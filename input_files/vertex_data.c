/** 
 * \file vertex_data.c
 * \brief Input functions for surface data.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <display.h>
#include <float.h>

/**
 * Allocate this many array entries at a time.
 */
#define VERTEX_DATA_INCREMENT 500

/**
 * Destructively split a line into fields based on a separator character.
 *
 * \param text_ptr The input text. This will be modified!
 * \param sep The separator character.
 * \param argv A pointer to the array of strings that will be created.
 * \returns The number of fields.
 */
int split_line(char *text_ptr, int sep, char ***argv)
{
  int len = 0;
  char *save_ptr;

  /* Ignore leading spaces if sep is space */
  if (sep == ' ')
    while (*text_ptr == ' ')
      text_ptr++;

  for (save_ptr = text_ptr; *text_ptr != '\0'; text_ptr++)
  {
    /* Handle quotes if present.
     */
    if (*text_ptr == '"' || *text_ptr == '\'')
    {
      int qch = *text_ptr++;
      save_ptr = text_ptr;
      while (*text_ptr != qch)
      {
        if (*text_ptr == '\0')
          return -1;

        text_ptr++;
      }
      if (*text_ptr == qch)
        *text_ptr++ = '\0';
    }
    if (*text_ptr == sep || *text_ptr == '\n')
    {
      ADD_ELEMENT_TO_ARRAY( (*argv), len, save_ptr, 1 );
      *text_ptr = 0;
      save_ptr = text_ptr + 1;
    }
  }
  return len;
}

/**
 * \brief Read per-vertex data from a text file.
 * 
 * Data is assumed to be a series of lines with a consistent number of
 * fields per line. Conceptually, each line corresponds to a vertex,
 * and each column corresponds to a separate per-vertex statistic or
 * measurement.  The number of columns is arbitray but this code
 * probably will fail if there are more then one hundred. If the
 * initial lines don't contain numeric data, they will be
 * ignored. Tries to be somewhat intelligent by adapting the separator
 * character according to the file name extension.  
 *
 * \param filename The name of the file to read.
 * \returns A pointer to the vertex_data_struct, or NULL if failure.
 */
vertex_data_struct *
input_vertex_data( VIO_STR filename )
{
    FILE *fp;
    int  len = 0;
    int  n_line = 0;
    char buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    vertex_data_struct *vtxd_ptr;
    int sep;
    int is_vertstats = 0;

    if (string_ends_in( filename, ".csv" ))
    {
        sep = ',';
    }
    else if (string_ends_in( filename, ".tsv" ))
    {
        sep = '\t';
    }
    else
    {
        sep = ' ';
    }

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return NULL;
    }
    ALLOC(vtxd_ptr, 1);

    vtxd_ptr->ndims = 2;

    ADD_ELEMENT_TO_ARRAY(vtxd_ptr->dims, len, 0, 1 );
    ADD_ELEMENT_TO_ARRAY(vtxd_ptr->dims, len, 0, 1 );

    vtxd_ptr->column_names = NULL;

    len = 0;

    while (fgets(buffer, sizeof(buffer) - 1, fp))
    {
        float v;
        char **items = NULL;
        int n = split_line(buffer, sep, &items);
        int i;

        n_line++;

        if (n == 1 && !strcmp(buffer, "<header>"))
        {
          is_vertstats = 1;
        }

        if (is_vertstats)
        {
          if (!strcmp(buffer, "</header>"))
          {
            n_line = 0;
            is_vertstats = 0;
          }
          continue;
        }

        if (vtxd_ptr->dims[1] == 0)
        {
          if (sscanf(items[0], "%f", &v) == 1)
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
            if ( n_line == 1 )
            {
              /* Assume this line is a header line. */
              ALLOC(vtxd_ptr->column_names, n);
              for (i = 0; i < n; i++)
              {
                vtxd_ptr->column_names[i] = create_string( items[i] );
              }
            }
            n = 0;
          }
        }
        else if (n != vtxd_ptr->dims[1])
        {
          fprintf(stderr, "Warning: inconsistent number of items per line.\n");
        }

        for (i = 0; i < n; i++)
        {
          if (sscanf(items[i], "%f", &v) != 1)
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
        FREE(items);
    }
    vtxd_ptr->dims[0] = len / vtxd_ptr->dims[1];
    print("Read %d lines of vertex data.\n", vtxd_ptr->dims[0]);
    print("There are %d items per line.\n", vtxd_ptr->dims[1]);
    fclose(fp);
    return vtxd_ptr;
}
