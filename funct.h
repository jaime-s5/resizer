#include "bmp.h"

#define EXIT_SUCCESS    0
#define EXIT_ERROR_COM  1
#define EXIT_ERROR_IN   2
#define EXIT_ERROR_OUT  3
#define EXIT_ERROR_BMP  4

// Function that enlarges the input BitMap by an integer factor n
int resize(char* in, char* out, double factor);

// Stores header info in structs
int storeHeader(FILE* input, BITMAPFILEHEADER* bf, BITMAPINFOHEADER* bi);

// Stores pixels of input BitMap in array of RGBTRIPLEs
void storePixels(RGBTRIPLE** array, FILE* input, int height,
                int width, int padding);

// Store the pixels and headers to the output file
void writeOutput(RGBTRIPLE** array, FILE* output,
                BITMAPFILEHEADER* bf, BITMAPINFOHEADER* bi,
                int padding);

// Copy contents of input headers and creates new ones with
// caracteristics of output file
void copyHeaders(BITMAPINFOHEADER* bi, BITMAPINFOHEADER* bi2,
                BITMAPFILEHEADER* bf, BITMAPFILEHEADER* bf2,
                double factor, int* padding, int* multInt);

// Frees array memory
void freeMem(RGBTRIPLE** array, int column);

// The scanline function, the function that goes over the "rows" in the
// bitmap "array" of the file, reads and writes in multiples of 4 bytes.
// Because of this, if we have a BMP of size 3x3, there is going to
// be some padding at the end of each "row" to make it a multiple of 4.
int calcPadd(LONG width);

// This functions enlarges or shrinks the input Array of pixels, storing the
// new values in the output Array
void enlarge(RGBTRIPLE** pixelsOut, RGBTRIPLE** pixelsIn,
            BITMAPINFOHEADER* bi, int mult);
void shrink(RGBTRIPLE** pixelsOut, RGBTRIPLE** pixelsIn,
            BITMAPINFOHEADER* bi, int mult);

// Checks if string factor is a number
int isNumeric(char* string);

// Guarantees that the size of the output bitmap is an integer multiple
// of the input bitmap, since we can't have half pixels
void wholePixels(BITMAPINFOHEADER* bi2, BITMAPINFOHEADER* bi, double factor,
                int* multInt);