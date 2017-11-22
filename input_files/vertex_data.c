/**
 * \file vertex_data.c
 * \brief Input functions for surface data.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <display.h>
#include <float.h>
#include <ctype.h>

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
      if (text_ptr - save_ptr > 0)
      {
          ADD_ELEMENT_TO_ARRAY( (*argv), len, save_ptr, 1 );
      }
      *text_ptr = 0;
      save_ptr = text_ptr + 1;
    }
  }
  return len;
}

/**
 * \brief Create and initialize a vertex data object.
 *
 * Allocates the memory and initializes the fields for a
 * vertex data object.
 *
 * \param n_dimensions The initial number of dimensions.
 * \returns An empty vertex data object.
 */
static vertex_data_struct *
create_vertex_data( int n_dimensions )
{
  vertex_data_struct *vtxd_ptr;
  int i;

  ALLOC( vtxd_ptr, 1 );
  ALLOC( vtxd_ptr->dims, n_dimensions );
  vtxd_ptr->ndims = n_dimensions;
  for (i = 0; i < n_dimensions; i++) {
    vtxd_ptr->dims[i] = 0;
  }
  vtxd_ptr->max_v = vtxd_ptr->min_v = NULL;
  vtxd_ptr->data = NULL;
  vtxd_ptr->column_names = NULL;
  /* We don't want to fully initialize the colour coding, because
     we need to read in the data before we know exactly which colour
     coding is appropriate.
  */
  vtxd_ptr->colour_coding.user_defined_n_colour_points = 0;
  return vtxd_ptr;
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
 * Helper function for sorting vertex values.
 */
static int realcmp(const void *a, const void *b)
{
  VIO_Real x = *(const VIO_Real *)a;
  VIO_Real y = *(const VIO_Real *)b;
  if (x > y) return 1;
  else if (x < y) return -1;
  return 0;
}

/**
 * Use a discrete "label" colour map for this vertex data set, rather
 * rather than the default continuous map. We model the discrete map
 * by creating a custom continuous colour map with distinct colour
 * points for each integral value.
 */
void use_label_colour_map( vertex_data_struct *vtxd_ptr, int len )
{
  VIO_Real  *indices;
  int        n_labels;
  int        i;
  VIO_Real   min_label;
  VIO_Real   max_label;
  VIO_Colour *colours;
  VIO_Real   *positions;

  if ( len <= 0 )
    return;

  /* Create a private copy of the vertex data, rounded to integer.
   */
  ALLOC(indices, len);

  for (i = 0; i < len; i++)
  {
    indices[i] = vtxd_ptr->data[i];
  }

  /* Sort the private copy- we do this so that the colour coding
   * function will see monotonically increasing position values, as
   * required.
   */
  qsort(indices, len, sizeof(VIO_Real), realcmp);
  n_labels = 1;
  for ( i = 1; i < len; i++ )
  {
    if ( indices[n_labels - 1] != indices[i] )
    {
      indices[n_labels++] = indices[i];
    }
  }
  print("Identified %d/%d distinct values.\n", n_labels, len);
  if (n_labels < len / 4)
  {
    min_label = indices[0];
    max_label = indices[n_labels - 1];
    ALLOC( colours, n_labels );
    ALLOC( positions, n_labels );

    /* Create the colour and position arrays needed to set up
     * the colour coding.
     */
    distinct_colours( n_labels, Initial_background_colour, colours );
    for ( i = 0; i < n_labels; i++ )
    {
      positions[i] = ( indices[i] - min_label ) / ( max_label - min_label);
    }

    /* For experimental purposes, report what we are doing. */

    print(" %4s  %8s %4s %3s %3s %3s\n", "#", "Label", "Fr.", "R", "G", "B");
    for (i = 0; i < n_labels; i++)
    {
      print(" %4d) %8.2f %4.2f %3d %3d %3d\n",
            i, indices[i], positions[i],
            get_Colour_r(colours[i]),
            get_Colour_g(colours[i]),
            get_Colour_b(colours[i]));
    }

    /* Configure the colour coding.
     */
    initialize_colour_coding( &vtxd_ptr->colour_coding,
                              USER_DEFINED_COLOUR_MAP,
                              colours[0],
                              colours[n_labels-1],
                              0.0, 1.0 );
    define_colour_coding_user_defined(&vtxd_ptr->colour_coding, n_labels,
                                      colours, positions, RGB_SPACE);
    set_colour_coding_min_max(&vtxd_ptr->colour_coding, min_label,
                              max_label);
    FREE(positions);
    FREE(colours);
  }
  FREE(indices);
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
    int n_integer = 0;

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

    vtxd_ptr = create_vertex_data( 2 );

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
            fprintf(stderr, "Failed to read '%s', '%s'\n", filename, items[i]);
            FREE(vtxd_ptr->data);
            FREE(vtxd_ptr->dims);
            FREE(vtxd_ptr);
            return NULL;
          }
          if (v > vtxd_ptr->max_v[i])
            vtxd_ptr->max_v[i] = v;
          if (v < vtxd_ptr->min_v[i])
            vtxd_ptr->min_v[i] = v;
          if (fabs(v - round(v)) < Vertex_label_tolerance)
          {
            ++n_integer;
          }
          ADD_ELEMENT_TO_ARRAY(vtxd_ptr->data, len, v, VERTEX_DATA_INCREMENT);
        }
        FREE(items);
    }
    vtxd_ptr->dims[0] = len / vtxd_ptr->dims[1];

    if ( n_integer == len )
    {
      use_label_colour_map( vtxd_ptr, len );
    }
    print("Read %d lines of vertex data.\n", vtxd_ptr->dims[0]);
    if (vtxd_ptr->dims[1] == 1)
      print("There is 1 item per line.\n");
    else
      print("There are %d items per line.\n", vtxd_ptr->dims[1]);
    fclose(fp);
    return vtxd_ptr;
}

/**
 * \brief Write per-vertex data to a text file.
 *
 * Data is assumed to be a series of lines with a consistent number of
 * fields per line. Conceptually, each line corresponds to a vertex,
 * and each column corresponds to a separate per-vertex statistic or
 * measurement.  The number of columns is arbitray but this code
 * probably will fail if there are more then one hundred. If the
 * initial lines don't contain numeric data, they will be
 * ignored.
 *
 * \param display The top-level display structure.
 * \param object The object whose vertex data should be saved.
 * \param filename The name of the file to write.
 */
VIO_Status
save_vertex_data_file( display_struct *display,
                       object_struct *object,
                       const VIO_STR filename )
{
    FILE *fp;
    int i, j, k;
    vertex_data_struct *vtxd_ptr;

    vtxd_ptr = find_vertex_data( display, object );
    if ( vtxd_ptr == NULL )
        return VIO_ERROR;

    fp = fopen( filename, "w" );
    if (fp == NULL)
        return VIO_ERROR;

    switch ( vtxd_ptr->ndims )
    {
    case 1:
      for_less (i, 0, vtxd_ptr->dims[0] )
      {
        fprintf( fp, "%f\n", vtxd_ptr->data[i] );
      }
      break;

    case 2:
      k = 0;
      for_less (i, 0, vtxd_ptr->dims[0] )
      {
        for_less (j, 0, vtxd_ptr->dims[1] )
        {
          fprintf( fp, "%f ", vtxd_ptr->data[k++] );
        }
        fprintf( fp, "\n");
      }
      break;

    }
    fclose(fp);
    return VIO_OK;
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
      print("Inconsistent GIFTI intent.\n");
      gifti_free_image( gii_ptr );
      return NULL;
    }
    if ( num_dim != gii_ptr->darray[i]->num_dim )
    {
      print("Inconsistent GIFTI dimension count.\n");
      gifti_free_image( gii_ptr );
      return NULL;
    }
    if ( datatype != gii_ptr->darray[i]->datatype )
    {
      print("Inconsistent GIFTI data type.\n");
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
    print("Incorrect dimension count for shape or label data.\n");
    gifti_free_image( gii_ptr );
    return NULL;
  }

  if ( intent == NIFTI_INTENT_LABEL || intent == NIFTI_INTENT_NONE )
  {
    if ( datatype != NIFTI_TYPE_INT32 && datatype != NIFTI_TYPE_UINT32 )
    {
      print("Incorrect data type for GIFTI label data.\n");
      gifti_free_image( gii_ptr );
      return NULL;
    }
  }
  else if ( intent == NIFTI_INTENT_SHAPE )
  {
    if ( datatype != NIFTI_TYPE_FLOAT32 )
    {
      print("Incorrect data type for GIFTI shape data.\n");
      gifti_free_image( gii_ptr );
      return NULL;
    }
  }
  else
  {
    print("Unrecognized or unsupported GIFTI intent.\n");
    gifti_free_image( gii_ptr );
    return NULL;
  }

  vtxd_ptr = create_vertex_data( 2 );

  vtxd_ptr->dims[0] = length;
  vtxd_ptr->dims[1] = gii_ptr->numDA;

  ALLOC( vtxd_ptr->data, length * gii_ptr->numDA );
  ALLOC( vtxd_ptr->max_v, gii_ptr->numDA );
  ALLOC( vtxd_ptr->min_v, gii_ptr->numDA );

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
    else if ( datatype == NIFTI_TYPE_UINT32 )
    {
      uint32_t *data_ptr = (uint32_t *)gii_ptr->darray[i]->data;
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
