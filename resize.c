// Program that resizes a BMP file
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

#include "funct.h"

int main(int argc, char *argv[])
{
    // Store resize factor through string
    double factor;
    sscanf(argv[1], "%lf", &factor);

    // Ensures the corerct number of inputs are introduced
    if (argc != 4 || !isNumeric(argv[1]) || factor <= 0 || factor > 100)
    {
        fprintf(stderr, "Usage: factor infile outfile\n");
        return EXIT_ERROR_COM;
    }

    int match = resize(argv[2], argv[3], atof(argv[1]));

    switch(match)
    {
        case EXIT_ERROR_IN:
            fprintf(stderr, "Input file was not found\n");
            return EXIT_ERROR_IN;
        case EXIT_ERROR_OUT:
            fprintf(stderr, "Couldn't create file\n");
            return EXIT_ERROR_OUT;
        case EXIT_ERROR_BMP:
            fprintf(stderr, "Input file is not a 24-bit BMP\n");
            return EXIT_ERROR_BMP;
        default:
            return EXIT_SUCCESS;
    }
}

int resize(char* iName, char* oName, double factor)
{
    FILE* input = fopen(iName, "rb");

    // Check if there was any problem
    if (input == NULL)
        return EXIT_ERROR_IN;

    FILE* output = fopen(oName, "wb");

    // Checks if there was any problem with the creation of the file
    if (output == NULL)
    {
        fclose(input);
        return EXIT_ERROR_OUT;
    }

    // Store the data of the input BitMap in the respective headers structs
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    int match = storeHeader(input, &bf, &bi);

    if (match)
        return EXIT_ERROR_BMP;

    // Calculates padding of input BitMap
    int padding = calcPadd(bi.biWidth);

    // Allocates memory for an array of RGBTRIPLEs of picture width and height
    int height = abs(bi.biHeight);
    RGBTRIPLE** pixels = malloc(height * sizeof(RGBTRIPLE*));
    for (int i = 0; i < height; i++)
    {
        pixels[i] = malloc(bi.biWidth * sizeof(RGBTRIPLE));
    }
    storePixels(pixels, input, height, bi.biWidth, padding);

    // Now we create the structs and copy them with the new values
    int paddingOut = 0;
    int multInt = 0;
    BITMAPINFOHEADER bi2;
    BITMAPFILEHEADER bf2;
    copyHeaders(&bi, &bi2, &bf, &bf2, factor, &paddingOut, &multInt);

    // Create the new array for pixels of output BitMap
    int heightOut = abs(bi2.biHeight);
    RGBTRIPLE** pixelsOut = malloc(heightOut * sizeof(RGBTRIPLE*));
    for (int i = 0; i < heightOut; i++)
    {
        pixelsOut[i] = malloc(bi2.biWidth * sizeof(RGBTRIPLE));
    }

    // Allocate pixels in output array and write them to output file
    // Different algorithms if shrinking or enlarging
    if (factor >= 1)
        enlarge(pixelsOut, pixels, &bi, multInt);
    else
        shrink(pixelsOut, pixels, &bi2, multInt);

    writeOutput(pixelsOut, output, &bf2, &bi2, paddingOut);

    // Frees memory used by malloc in pixels input array
    freeMem(pixels, height);

    // Frees memory used by malloc in pixels output array
    freeMem(pixelsOut, heightOut);

    // Close both input and output FILEs
    fclose(input);
    fclose(output);

    return EXIT_SUCCESS;
}

int storeHeader(FILE* input, BITMAPFILEHEADER* bf, BITMAPINFOHEADER* bi)
{
    fread(bf, sizeof(BITMAPFILEHEADER), 1, input);
    fread(bi, sizeof(BITMAPINFOHEADER), 1, input);

    // We check if the input BitMap is a 24-bit BMP, if not return 0
    if (bi->biBitCount != 24 || bi->biCompression != 0 ||
        bi->biSize != 40     || bf->bfType != 0x4D42 ||
        bf->bfOffBits != 54)
    {
        return EXIT_ERROR_BMP;
    }

    return EXIT_SUCCESS;
}

void storePixels(RGBTRIPLE** array, FILE* input, int height,
                int width, int padding)
{
    // Store the pixels of the input file in an array of
    // RGBTRIPLEs (pointer to pointer to RGBTRIPLEs)
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fread(&array[i][j], sizeof(RGBTRIPLE), 1, input);
        }

        // Here we skip the padding
        if (padding)
            fseek(input, padding, SEEK_CUR);
    }
}

void writeOutput(RGBTRIPLE** array, FILE* output,
                BITMAPFILEHEADER* bf, BITMAPINFOHEADER* bi,
                int padding)
{
    // Write the headers to the output file
    fwrite(bf, sizeof(BITMAPFILEHEADER), 1, output);
    fwrite(bi, sizeof(BITMAPINFOHEADER), 1, output);

    // Write the pixels to the output file
    for (int i = 0, height = abs(bi->biHeight); i < height; i++)
    {
        for (int j = 0; j < bi->biWidth; j++)
        {
            fwrite(&array[i][j], sizeof(RGBTRIPLE), 1, output);
        }

        // Here we write the padding, if any
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, output);
        }
    }
}

void copyHeaders(BITMAPINFOHEADER* bi, BITMAPINFOHEADER* bi2,
                BITMAPFILEHEADER* bf, BITMAPFILEHEADER* bf2,
                double factor, int* padding, int* multInt)
{
    *bi2 = *bi;
    *bf2 = *bf;

    // Calculates new width and height guranteed that they are an integer
    // number of pixels
    wholePixels(bi2, bi, factor, multInt);

    // Calculates padding of Output BitMap
    *padding = calcPadd(bi2->biWidth);

    bi2->biSizeImage = (sizeof(RGBTRIPLE) * bi2->biWidth + *padding) *
                        abs(bi2->biHeight);

    bf2->bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                  bi2->biSizeImage;
}

void freeMem(RGBTRIPLE** array, int column)
{
    for (int i = 0; i < column; i++)
    {
        free(array[i]);
    }
    free(array);
}

int calcPadd(LONG width)
{
    int padding = 0;

    if ((width * sizeof(RGBTRIPLE)) % 4 != 0)
        padding = 4 - (width * sizeof(RGBTRIPLE)) % 4;

    return padding;
}

void enlarge(RGBTRIPLE** pixelsOut, RGBTRIPLE** pixelsIn,
                BITMAPINFOHEADER* bi, int mult)
{
    for (int i = 0, height = abs(bi->biHeight); i < height; i++)
    {
        for (int j = 0; j < bi->biWidth; j++)
        {
            // Copy the input pixel mult times, horizontally and vertically
            for (int k = 0; k < mult; k++)
            {
                for (int w = 0; w < mult; w++)
                {
                    pixelsOut[mult * i + w][mult * j + k] = pixelsIn[i][j];
                }
            }
        }
    }
}

void shrink(RGBTRIPLE** pixelsOut, RGBTRIPLE** pixelsIn,
                BITMAPINFOHEADER* bi, int mult)
{
    for (int i = 0, height = abs(bi->biHeight); i < height; i++)
    {
        for (int j = 0; j < bi->biWidth; j++)
        {
            // Selects the corresponding input pixel and puts it
            // in the out array
            pixelsOut[i][j] = pixelsIn[mult* i][mult * j];
        }
    }
}

int isNumeric(char* string)
{
    int i = 0;
    int points = 0;
    while (string[i] != '\0')
    {
        if (string[i] == '.')
            points++;
            
        if (!isdigit(string[i]) && string[i] != '.' || points > 1)
            return 0;
        
        i++;
    }

    return 1;
}

void wholePixels(BITMAPINFOHEADER* bi2, BITMAPINFOHEADER* bi, double factor,
                int* multInt)
{
    // Type conversions trunkates doubles to ints. To be more "realistic"
    // needs the addition of 0.5. The division of two integers a / b will
    // yield an integer, that is the reason for the casting to dobules.
    if (factor >= 1)
    {
        *multInt = (int)(factor + 0.5);
        bi2->biWidth = bi->biWidth * *multInt;
        bi2->biHeight = bi->biHeight * *multInt;
    }
    else
    {
        *multInt = (int)(1.0 / factor + 0.5);
        bi2->biWidth = (int)((double)bi->biWidth / (double)*multInt);
        bi2->biHeight = (int)((double)bi->biHeight / (double)*multInt);
    }
}