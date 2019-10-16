// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: copy infile outfile\n");
        return 1;
    }

    if (atoi(argv[1]) > 100)
    {
        fprintf(stderr, "error\n");
        return 2;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // original file padding
    int opadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    //save dimensions of original file
    int obW = bi.biWidth;
    int obH = abs(bi.biHeight);

    //adjust previous image properties
    bi.biWidth = (bi.biWidth * atoi(argv[1]));
    bi.biHeight = (bi.biHeight * atoi(argv[1]));

    // determine padding for scanlines
    int padding = (4 - ((atoi(argv[1]) * obW) * sizeof(RGBTRIPLE)) % 4) % 4;

    //determine new biSizeImage
    bi.biSizeImage = (((3 * (bi.biWidth * atoi(argv[1]))) + padding) * (abs(bi.biHeight) * atoi(argv[1])));


    // write outfile's BITMAPFILEHEADER
    bf.bfSize = (bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPFILEHEADER));
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    //read offset of original files pixels
    int isl = 54;

    // read original files scan line, i for each line that exist in original
    for (int i = 0; i < obH; i++)
    {
        //d is the number of time to write each original line, based on user input
        for (int d = 0; d < atoi(argv[1]); d++)
        {
            //j counts which pixel is being read from original file(keep this)
            for (int j = 0; j < obW; j++)
            {

            RGBTRIPLE triple;

            // read RGB triple from infile
            isl = 54 + ((i * obW * obH) + (opadding * i)) + (j * 3);
            fseek(inptr, isl, SEEK_SET);
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple to outfile n times
            for (int l = 0; l < atoi(argv[1]); l++)
            {
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }

            }
            // skip over padding, if any
            fseek(inptr, opadding, SEEK_CUR);


            // add padding based on new file image size
            for (int k = 0; k < padding; k++)
            {
                fputc(0x00, outptr);
            }


        }

    }
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
