/*
 * ppm_to_pixels - generate checkbox.h (a compiled-in pixels_struct) from a PPM.
 *
 * This is a build-time code generator run during the Display build. It is kept
 * deliberately self-contained: it depends on nothing but the C standard library
 * so that it can run at build time without resolving the shared libminc/HDF5
 * stack (linking those made the generator fail on macOS with
 *   dyld: Library not loaded: @rpath/libhdf5.*.dylib
 * because that dylib is not on the build-tree rpath).
 *
 * The output is byte-for-byte identical to the previous version, which used
 * bicpl's input_rgb_file() + volume_io's make_rgba_Colour(). The two pieces of
 * behaviour reproduced here are:
 *   - input_rgb_file() stores the image bottom-to-top (vertical flip): the
 *     stored pixel (x, sy) is the file pixel (x, height-1-sy). See
 *     bicpl/Images/rgb_io_none.c / rgb_io_ppm.c.
 *   - make_rgba_Colour(r,g,b,a) packs the four bytes as
 *     p[0]=a; p[1]=b; p[2]=g; p[3]=r into an unsigned int. See
 *     libminc/volume_io/Geometry/colour_def.c. This is byte-order dependent but
 *     the generator and Display run on the same machine, so it is self
 *     consistent.
 */

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int Colour;

/* Verbatim copy of volume_io make_rgba_Colour() packing (colour_def.c). */
static Colour make_rgba_Colour(int r, int g, int b, int a)
{
    Colour c = 0;
    unsigned char *p = (unsigned char *) &c;
    p[0] = (unsigned char) a;
    p[1] = (unsigned char) b;
    p[2] = (unsigned char) g;
    p[3] = (unsigned char) r;
    return c;
}

/* Read the next unsigned integer from a binary PPM header, skipping ASCII
   whitespace and '#' comment lines (per the PPM spec). */
static int read_ppm_uint(FILE *fp, int *value)
{
    int c;

    do {
        c = fgetc(fp);
        if (c == '#') {
            while (c != '\n' && c != EOF)
                c = fgetc(fp);
        }
    } while (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '#');

    if (c == EOF || c < '0' || c > '9')
        return -1;

    *value = 0;
    while (c >= '0' && c <= '9') {
        *value = *value * 10 + (c - '0');
        c = fgetc(fp);
    }
    return 0;
}

int ppm_to_pixels(char *infile, char *outfile)
{
    FILE *in, *out;
    int width, height, maxval;
    unsigned char *data;
    long npix, nbytes;
    int x, y;

    in = fopen(infile, "rb");
    if (in == NULL)
        return -1;

    /* Magic number: only binary P6 is supported (as before). */
    if (fgetc(in) != 'P' || fgetc(in) != '6') {
        fclose(in);
        return -1;
    }

    if (read_ppm_uint(in, &width) != 0 ||
        read_ppm_uint(in, &height) != 0 ||
        read_ppm_uint(in, &maxval) != 0 ||
        maxval != 255) {
        fclose(in);
        return -1;
    }
    /* One whitespace separator between the header and the raster is already
       consumed by read_ppm_uint() reading past the last maxval digit. */

    npix = (long) width * (long) height;
    nbytes = npix * 3;
    data = (unsigned char *) malloc((size_t) nbytes);
    if (data == NULL) {
        fclose(in);
        return -1;
    }
    if (fread(data, 1, (size_t) nbytes, in) != (size_t) nbytes) {
        free(data);
        fclose(in);
        return -1;
    }
    fclose(in);

    out = fopen(outfile, "w");
    if (out == NULL) {
        free(data);
        return -1;
    }

    fprintf(out, "static VIO_Colour pixels[] = {\n");
    for (y = 0; y < 16; y++) {
        for (x = 16; x < 48; x++) {
            /* input_rgb_file() stores bottom-to-top: stored (x,y) == file
               (x, height-1-y). Replicate exactly for a 96x16 checkbox. */
            int fy = height - 1 - y;
            const unsigned char *px = &data[((long) fy * width + x) * 3];
            Colour c = make_rgba_Colour(px[0], px[1], px[2], 255);
            fprintf(out, "  (VIO_Colour) %#lx,\n", (unsigned long) c);
        }
    }
    fprintf(out, "};\n");
    fprintf(out, "static pixels_struct checkbox = {\n");
    fprintf(out, "    0,   0,   // position\n");
    fprintf(out, "  %3d, %3d,   // size\n", 32, 16);
    fprintf(out, "  1.0, 1.0,   // zoom\n");
    fprintf(out, "  RGB_PIXEL,  // pixel type\n");
    fprintf(out, "  { pixels }\n");
    fprintf(out, "};\n");
    fclose(out);
    free(data);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s input.ppm output.h\n", argv[0]);
        return 1;
    }
    return ppm_to_pixels(argv[1], argv[2]) == 0 ? 0 : 1;
}
