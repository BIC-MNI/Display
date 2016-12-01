/**
 * \file vertex_data.c
 * \brief Input functions for surface data.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <display.h>
#include <float.h>

#if GIFTI_FOUND
#include <time.h>
#include "gifti_io.h"
#endif /* GIFTI_FOUND */

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
 * \brief Free memory associated with a vertex data object.
 *
 * Deletes the various allocated fields associated with a vertex data
 * object, as well as the structure itself.
 *
 * \param vtxd_ptr The vertex data to delete.
 */
void
delete_vertex_data( vertex_data_struct *vtxd_ptr )
{
  delete_colour_coding( &vtxd_ptr->colour_coding );
  FREE( vtxd_ptr->data );
  FREE( vtxd_ptr->min_v );
  FREE( vtxd_ptr->max_v );
  FREE( vtxd_ptr->dims );
  FREE( vtxd_ptr );
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
static vertex_data_struct *
input_vertstats_vertex_data( const VIO_STR filename )
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
    vtxd_ptr->colour_coding.user_defined_n_colour_points = 0;

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
    if (vtxd_ptr->dims[1] == 1)
      print("There is 1 item per line.\n");
    else
      print("There are %d items per line.\n", vtxd_ptr->dims[1]);
    fclose(fp);
    return vtxd_ptr;
}

#if GIFTI_FOUND

/**
 * trivial structure used only to allow proper sorting of indices
 * according to key value.
 */
typedef struct label_key {
  int index;
  int key;
} label_key_t;

/**
 * Function to perform comparisons for qsort().
 */
static int lblcmp(const void *a, const void *b)
{
  return ((const label_key_t *)a)->key - ((const label_key_t *)b)->key;
}

/**
 * \brief Read per-vertex data from a GIFTI shape or label file.
 *
 * \param filename The name of the file to read.
 * \returns A pointer to the vertex_data_struct, or NULL if failure.
 */
static vertex_data_struct *
input_gifti_vertex_data( const VIO_STR filename )
{
  vertex_data_struct *vtxd_ptr;
  gifti_image *gii_ptr;
  int i, j;
  int intent;
  int num_dim;
  int datatype;
  int length;

  gii_ptr = gifti_read_image( filename, 1 );
  if ( gii_ptr == NULL || gii_ptr->numDA == 0 )
  {
    return NULL;
  }

  /* Check the consistency of these fields. In practice, most GIFTI
   * shape and label files seem to contain a single dataarray. But
   * we definitely want to detect any inconsistencies.
   */
  intent = gii_ptr->darray[0]->intent;
  num_dim = gii_ptr->darray[0]->num_dim;
  datatype = gii_ptr->darray[0]->datatype;
  length = gii_ptr->darray[0]->dims[0];

  for ( i = 1; i < gii_ptr->numDA; i++ )
  {
    if ( intent != gii_ptr->darray[i]->intent )
    {
      print("Inconsistent GIFTI intent.");
      gifti_free_image( gii_ptr );
      return NULL;
    }
    if ( num_dim != gii_ptr->darray[i]->num_dim )
    {
      print("Inconsistent GIFTI dimension count.");
      gifti_free_image( gii_ptr );
      return NULL;
    }
    if ( datatype != gii_ptr->darray[i]->datatype )
    {
      print("Inconsistent GIFTI data type.");
      gifti_free_image( gii_ptr );
      return NULL;
    }
  }

  /* Require that datasets either have a single dimension, or two
   * dimensions with a singleton second dimension.
   */
  if ( (num_dim != 1 &&
        (num_dim != 2 || gii_ptr->darray[0]->dims[1] != 1)))
  {
    print("Incorrect dimension count for shape or label data.");
    gifti_free_image( gii_ptr );
    return NULL;
  }

  if ( intent == NIFTI_INTENT_LABEL || intent == NIFTI_INTENT_NONE )
  {
    if ( datatype != NIFTI_TYPE_INT32 )
    {
      print("Incorrect data type for GIFTI label data.");
      gifti_free_image( gii_ptr );
      return NULL;
    }
  }
  else if ( intent == NIFTI_INTENT_SHAPE )
  {
    if ( datatype != NIFTI_TYPE_FLOAT32 )
    {
      print("Incorrect data type for GIFTI shape data.");
      gifti_free_image( gii_ptr );
      return NULL;
    }
  }
  else
  {
    print("Unrecognized or unsupported GIFTI intent.");
    gifti_free_image( gii_ptr );
    return NULL;
  }

  ALLOC( vtxd_ptr, 1 );
  vtxd_ptr->ndims = 2;
  ALLOC( vtxd_ptr->dims, 2 );
  vtxd_ptr->dims[0] = length;
  vtxd_ptr->dims[1] = gii_ptr->numDA;
  vtxd_ptr->column_names = NULL;
  ALLOC( vtxd_ptr->data, length * gii_ptr->numDA );
  ALLOC( vtxd_ptr->max_v, gii_ptr->numDA );
  ALLOC( vtxd_ptr->min_v, gii_ptr->numDA );

  vtxd_ptr->colour_coding.user_defined_n_colour_points = 0;

  for ( i = 0; i < gii_ptr->numDA; i++ )
  {
    vtxd_ptr->max_v[i] = -DBL_MAX;
    vtxd_ptr->min_v[i] = DBL_MAX;
    if ( datatype == NIFTI_TYPE_INT32 )
    {
      int32_t *data_ptr = (int32_t *)gii_ptr->darray[i]->data;
      for ( j = 0; j < length; j++ )
      {
        float v = *data_ptr++;
        if ( v > vtxd_ptr->max_v[i] )
          vtxd_ptr->max_v[i] = v;
        if ( v < vtxd_ptr->min_v[i] )
          vtxd_ptr->min_v[i] = v;
        vtxd_ptr->data[i * length + j] = v;
      }
    }
    else
    {
      float *data_ptr = (float *)gii_ptr->darray[i]->data;
      for ( j = 0; j < length; j++ )
      {
        float v = *data_ptr++;
        if ( v > vtxd_ptr->max_v[i] )
          vtxd_ptr->max_v[i] = v;
        if ( v < vtxd_ptr->min_v[i] )
          vtxd_ptr->min_v[i] = v;
        vtxd_ptr->data[i * length + j] = v;
      }
    }
  }

  if (gii_ptr->labeltable.length != 0)
  {
    VIO_Colour *colours;
    VIO_Real *positions;
    label_key_t *indices;
    VIO_Real min_key, max_key;
    int n_labels = gii_ptr->labeltable.length;

    ALLOC(colours, n_labels);
    ALLOC(positions, n_labels);
    ALLOC(indices, n_labels);

    initialize_colour_coding( &vtxd_ptr->colour_coding,
                              USER_DEFINED_COLOUR_MAP,
                              Initial_vertex_under_colour,
                              Initial_vertex_over_colour,
                              0.0, 1.0 );

    /* Find minimum and maximum key values while copying the keys
     * and indices into our own array, which will will then sort.
     */
    min_key = max_key = gii_ptr->labeltable.key[0];
    for ( i = 0; i < n_labels; i++ )
    {
      indices[i].index = i;
      indices[i].key = gii_ptr->labeltable.key[i];

      if ( gii_ptr->labeltable.key[i] > max_key )
      {
        max_key = gii_ptr->labeltable.key[i];
      }
      if ( gii_ptr->labeltable.key[i] < min_key )
      {
        min_key = gii_ptr->labeltable.key[i];
      }
    }

    /* Sort the array- we do this so that the colour coding
     * function will see monotonically increasing position
     * values, as required.
     */
    qsort(indices, n_labels, sizeof(label_key_t), lblcmp);

    /* Create the colour and position arrays needed to set up
     * the colour coding.
     */
    for ( i = 0; i < n_labels; i++)
    {
      int j = indices[i].index * 4; /* index into RGBA quadruples */

      colours[i] = make_rgba_Colour_0_1(gii_ptr->labeltable.rgba[j + 0],
                                        gii_ptr->labeltable.rgba[j + 1],
                                        gii_ptr->labeltable.rgba[j + 2],
                                        gii_ptr->labeltable.rgba[j + 3]);

      positions[i] = (indices[i].key - min_key) / (max_key - min_key);
    }

    /* Configure the colour coding.
     */
    define_colour_coding_user_defined(&vtxd_ptr->colour_coding, n_labels,
                                      colours, positions, RGB_SPACE);
    set_colour_coding_min_max(&vtxd_ptr->colour_coding, min_key, max_key);

    FREE(colours);
    FREE(positions);
    FREE(indices);
  }
  gifti_free_image( gii_ptr );
  return vtxd_ptr;
}
#endif

/**
 * Load a vertex data file and associate it with the given object.
 * If the passed object pointer is NULL,
 * \param display A pointer to the 3D window's display information.
 * \param object The object to attach to, or NULL if we should use
 * the most recently loaded polygonal object.
 * \param filename The filename of the vertex data file.
 * \returns VIO_OK on successful reading and attaching of the vertex
 * data.
 */
VIO_Status
load_vertex_data_file( display_struct *display, object_struct *object_ptr,
                       VIO_STR filename )
{
    vertex_data_struct *vtxd_ptr = NULL;
    polygons_struct    *polygons_ptr;

#if GIFTI_FOUND
    if( filename_extension_matches(filename, "gii"))
      vtxd_ptr = input_gifti_vertex_data( filename );
#endif
    if (vtxd_ptr == NULL &&
        (vtxd_ptr = input_vertstats_vertex_data(filename)) == NULL)
    {
        print_error("Failed to read vertex data from '%s'.\n", filename);
        return VIO_ERROR;
    }

    print("Loaded %d vertex data items, in range [%f ... %f]\n",
          vtxd_ptr->dims[0],
          vtxd_ptr->min_v[0], vtxd_ptr->max_v[0]);

    polygons_ptr = get_polygons_ptr( object_ptr );
    if (polygons_ptr->n_points != vtxd_ptr->dims[0])
    {
        print("Vertex data requires a polygon object of the same length.\n");
        return VIO_ERROR;
    }
    attach_vertex_data(display, object_ptr, vtxd_ptr);
    return VIO_OK;
}
