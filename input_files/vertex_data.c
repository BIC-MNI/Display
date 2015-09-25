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

vertex_data_struct *
input_vertex_data( const char *filename )
{
    FILE *fp;
    int  len = 0;
    char buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    vertex_data_struct *vtxd_ptr;

    printf("input_vertex_data(%s)\n", filename);

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return NULL;
    }
    ALLOC(vtxd_ptr, 1);

    vtxd_ptr->ndims = 1;
    vtxd_ptr->max_v = -FLT_MAX;
    vtxd_ptr->min_v = FLT_MAX;

    ADD_ELEMENT_TO_ARRAY(vtxd_ptr->dims, len, 0, 1 );

    while (fgets(buffer, sizeof(buffer) - 1, fp))
    {
        float v;

        if (sscanf(buffer, "%f", &v) != 1)
        {
            fprintf(stderr, "Failed to read '%s'\n", filename);
            FREE(vtxd_ptr->data);
            FREE(vtxd_ptr->dims);
            FREE(vtxd_ptr);
            return NULL;
        }
        if (v > vtxd_ptr->max_v)
            vtxd_ptr->max_v = v;
        if (v < vtxd_ptr->min_v)
            vtxd_ptr->min_v = v;

        ADD_ELEMENT_TO_ARRAY(vtxd_ptr->data, vtxd_ptr->dims[0], v,
                             VERTEX_DATA_INCREMENT);
    }
    fclose(fp);
    return vtxd_ptr;
}
