#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <display.h>

int ppm_to_pixels(char *infile, char *outfile)
{
    pixels_struct file_pixels;
    int status = input_rgb_file( infile, &file_pixels );
    FILE *fp = fopen( outfile, "w" );
    int x, y;

    if (status != VIO_OK)
      return -1;

    if (fp == NULL)
      return -1;

    fprintf(fp, "static VIO_Colour pixels[] = {\n");
    for (y = 0; y < 16; y++)
    {
      for (x = 16; x < 48; x++)
      {
        VIO_Colour c = PIXEL_RGB_COLOUR( file_pixels, x, y );
        fprintf(fp, "  (VIO_Colour) %#lx,\n", (unsigned long) c );
      }
    }
    fprintf(fp, "};\n");
    fprintf(fp, "static pixels_struct checkbox = {\n");
    fprintf(fp, "    0,   0,   // position\n");
    fprintf(fp, "  %3d, %3d,   // size\n", 32, 16);
    fprintf(fp, "  1.0, 1.0,   // zoom\n");
    fprintf(fp, "  RGB_PIXEL,  // pixel type\n");
    fprintf(fp, "  { pixels }\n");
    fprintf(fp, "};\n");
    fclose(fp);
    return 0;
}

int main(int argc, char **argv)
{
  // Force us to link to the right version...
  (void) make_rgba_Colour(0, 0, 0, 0);
  return ppm_to_pixels(argv[1], argv[2]);
}

