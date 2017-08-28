/*------------------------------------------------------
 Student's Name: Khai Quoc Le
 Student's number: 5096182
 Student's email address: kql832@uowmail.edu.au
 -------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#pragma pack(push, 1)
/**** BMP file header structure ****/
typedef struct
{
    unsigned short bfType;           /* BM */
    unsigned int   bfSize;           /* Size of file */
    unsigned short bfReserved1;      /* Reserved */
    unsigned short bfReserved2;      /* ... */
    unsigned int   bfOffBits;        /* Offset to bitmap data */
}FILEHEADER;

/**** BMP file info structure ****/
typedef struct
{
    unsigned int   biSize;           /* Size of info header */
    int            width;          /* Width of image */
    int            height;         /* Height of image */
    unsigned short planes;         /* Number of color planes */
    unsigned short bitPix;       	/* Number of bits per pixel */
    unsigned int   biCompression;    /* Type of compression to use */
    unsigned int   biSizeImage;      /* Size of image data */
    int            biXPelsPerMeter;  /* X pixels per meter */
    int            biYPelsPerMeter;  /* Y pixels per meter */
    unsigned int   biClrUsed;        /* Number of colors used */
    unsigned int   biClrImportant;   /* Number of important colors */
}INFOHEADER;

/**** Colormap pixel structure ****/
typedef struct
{
    unsigned char  b;         /* Blue value */
    unsigned char  g;         /* Green value */
    unsigned char  r;         /* Red value */
}PIX;

#pragma pop

/*-- Function prototypes --*/
unsigned char **alloc2D( int height, int width );
PIX **alloc2Drgb( int height, int width );
void free2D( unsigned char **mar );
void free2Drgb( PIX **mar );
bool processCFAImage( unsigned char **cfaImage, PIX **rgbImage, int width, int height );
bool cfaInterpolation( unsigned char **cfaImage, PIX **rgbImage, int width, int height );
bool colorCorrection( PIX **rgbImage, int width, int height );
bool gammaCorrection( PIX **rgbImage, int width, int height );

int main( void )
{
    unsigned char **cfaImage; /* 2D array of CFA image pixels */
    PIX **rgbImage;           /* 2D array of RGB image pixels */
    
    char fileName[20];
    FILEHEADER header;
    INFOHEADER info;
    FILE *inFile, *outFile;
    bool errCode;
    int i;
    
    printf("Input CFA file name:");
    scanf("%s", fileName);
    
    /*-- Open the BMP file with CFA samples --*/
    if((inFile = fopen(fileName, "rb")) == NULL)
    {
        fprintf(stderr, "Error Opening BMP File");
        return -1;
        
    }
    /*-- read the file header --*/
    if( fread(&header, sizeof(FILEHEADER), 1, inFile) != 1)
    {
        fprintf(stderr, "Error reading File Header");
        fclose( inFile );
        return -1;
    }
    
    /*-- Check BM marker --*/
    if (header.bfType != 0x4D42)
    {
        fprintf(stderr, "Unsupported file format");
        fclose(inFile);
        return -1;
    }
    /*-- read info --*/
    if( fread(&info, sizeof(INFOHEADER), 1, inFile) != 1)
    {
        fprintf(stderr, "Error reading Info Header");
        fclose( inFile );
        return -1;
    }
    
    
    /* allocate memory for cfaImage */
    cfaImage = alloc2D( info.height, info.width );
    if( cfaImage == NULL )
    {
        fprintf( stderr, " Memory allocation error" );
        return -1;
    }
    
    fseek(inFile, header.bfOffBits, SEEK_SET);
    for(i=info.height-1; i>=0; i--)  /* from upside-down to raster */
        if( fread( cfaImage[i], sizeof(unsigned char), info.width, inFile) != info.width)
        {
            fprintf(stderr, "Error reading Image data");
            fclose( inFile );
            return -1;
        }
    
    /* allocate memory for an RGB Image */
    rgbImage = alloc2Drgb( info.height, info.width );
    if( rgbImage == NULL )
    {
        fprintf( stderr, " Memory allocation error" );
        return -1;
    }
    
    /*----- produce an RGB image from a CFA image ----*/
    errCode = processCFAImage( cfaImage, rgbImage, info.width, info.height );
    if( errCode == false )
    {
        printf("Data processing error \n");
        return -1;
    }
    
    
    /*-------- Save RGB file --------*/
    printf("RGB file name:");
    scanf("%s", fileName);
    /*-- Open the BMP file --*/
    if((outFile = fopen(fileName, "wb")) == NULL)
    {
        fprintf(stderr, "Error Opening output BMP File");
        return -1;
        
    }
    /*-- write the file header --*/
    if( fwrite(&header, sizeof(FILEHEADER), 1, outFile) != 1)
    {
        fprintf(stderr, "Error writing File Header");
        fclose( outFile );
        return -1;
    }
    
    info.bitPix = 24;  /* change to 24 bit/pix */
    if( fwrite(&info, sizeof(INFOHEADER), 1, outFile) != 1)
    {
        fprintf(stderr, "Error writing Info Header");
        fclose( outFile );
        return -1;
    }
    
    fseek(outFile, header.bfOffBits, SEEK_SET);
    for(i=info.height-1; i>=0; i--) /* from raster to upside-down */
        if( fwrite( rgbImage[i], sizeof(PIX), info.width, outFile) != info.width)
        {
            fprintf(stderr, "Error writing RGB Image data");
            fclose( outFile );
            return -1;
        }
    
    
    free2D( cfaImage );
    free2Drgb( rgbImage );
    fclose(inFile);
    fclose(outFile);
    
    return 0;
}


unsigned char **alloc2D( int height, int width )
{
    int i;
    unsigned int nelem;
    unsigned char *p, **pp;
    
    nelem = height * width;
    p = (unsigned char*)calloc( nelem, 1 );
    if( p == NULL ) return NULL;
    pp = (unsigned char**)calloc( height, sizeof(unsigned char*) );
    if( pp == NULL )  { free(p); return NULL; }
    for ( i=0; i<height; i++ )
        pp[i] = p + i*width;
    
    return pp;
}

void free2D( unsigned char **mar )
{
    if( mar != NULL ) free( (unsigned char*) *mar );
    free((unsigned char**) mar);
    
    return;
}

PIX **alloc2Drgb( int height, int width )
{
    int i;
    unsigned int nelem;
    PIX *p, **pp;
    
    nelem = height * width;
    p = (PIX*)calloc( nelem, 3 );
    if( p == NULL ) return NULL;
    pp = (PIX**)calloc( height, sizeof(PIX*) );
    if( pp == NULL )  { free(p); return NULL; }
    for ( i=0; i<height; i++ )
        pp[i] = p + i*width;
    
    return pp;
}

void free2Drgb( PIX **mar )
{
    if( mar != NULL ) free( (PIX*) *mar );
    free((PIX**) mar);
    
    return;
}


/*============ Your code starts here ============*/
bool processCFAImage( unsigned char **cfaImage, PIX **rgbImage, int width, int height )
{
    
    cfaInterpolation(cfaImage, rgbImage, width, height);
    colorCorrection(rgbImage, width, height);
    gammaCorrection(rgbImage, width, height);
    
    return true;
}

/*--- Bilinear CFA Interpolation ---*/
bool cfaInterpolation ( unsigned char **cfaImage, PIX **rgbImage, int width, int height )
{
    int row, col;
    
    for(row=0; row < height; row++)
        for( col=0; col < width; col++)
        {
        //the row with green and red positions
            if (row%2 == 0)
            {
                if (col%2 == 0) // green positions
                {
                    if (col == 0) // green at column 0
                    {
                        if (row == 0) // green at position (0, 0)
                            rgbImage[row][col].b = cfaImage[row+1][col];
                        else
                            rgbImage[row][col].b = (cfaImage[row-1][col]+cfaImage[row+1][col])/2;
                        rgbImage[row][col].r = cfaImage[row][col+1];
                    }
                    else // green at other columns not 0
                    {
                        if (row == 0)
                            rgbImage[row][col].b = cfaImage[row+1][col];
                        else
                            rgbImage[row][col].b = (cfaImage[row-1][col]+cfaImage[row+1][col])/2;
                        rgbImage[row][col].r = (cfaImage[row][col-1]+cfaImage[row][col+1])/2;
                    }
                    rgbImage[row][col].g = cfaImage[row][col];
                }
                else // red positions
                {
                    if (col == width-1) // red at column width-1
                    {
                        if(row == 0) // red at position (0, width-1)
                        {
                            rgbImage[row][col].g = (cfaImage[row][col-1] + cfaImage[row+1][col])/2;
                            rgbImage[row][col].b = cfaImage[row+1][col-1];
                        }
                        else
                        {
                            rgbImage[row][col].g = (cfaImage[row-1][col] + cfaImage[row][col-1] + cfaImage[row+1][col])/3;
                            rgbImage[row][col].b = (cfaImage[row-1][col-1]+cfaImage[row+1][col-1])/2;
                        }
                    }
                    else // red at other columns not width-1
                    {
                        if(row == 0)
                        {
                            rgbImage[row][col].g = (cfaImage[row][col-1] + cfaImage[row][col+1] + cfaImage[row+1][col])/3;
                            rgbImage[row][col].b = (cfaImage[row+1][col-1]+cfaImage[row+1][col+1])/2;
                        }
                        else
                        {
                            rgbImage[row][col].g = (cfaImage[row-1][col]+cfaImage[row+1][col]+cfaImage[row][col-1] + cfaImage[row][col+1])/4;
                            rgbImage[row][col].b = (cfaImage[row-1][col-1]+cfaImage[row-1][col+1]+cfaImage[row+1][col+1] + cfaImage[row+1][col-1])/4;
                        }
                    }
                    rgbImage[row][col].r = cfaImage[row][col];
                }
            }
        //the row with blue and green positions
            else
            {
                if (col%2 == 0) // blue positions
                {
                    if(col == 0) // blue at column 0
                    {
                        if(row == height-1) // blue at position (height-1, 0)
                        {
                            rgbImage[row][col].r = cfaImage[row-1][col+1];
                            rgbImage[row][col].g = (cfaImage[row-1][col] + cfaImage[row][col+1])/2;
                        }
                        else
                        {
                            rgbImage[row][col].r = (cfaImage[row-1][col+1]+cfaImage[row+1][col+1])/2;
                            rgbImage[row][col].g = (cfaImage[row-1][col] + cfaImage[row+1][col] + cfaImage[row][col+1])/3;
                        }
                    }
                    else // blue at other columns not 0
                    {
                        if(row == height-1)
                        {
                            rgbImage[row][col].r = (cfaImage[row-1][col-1]+cfaImage[row-1][col+1])/2;
                            rgbImage[row][col].g = (cfaImage[row][col-1] + cfaImage[row][col+1] + cfaImage[row-1][col])/3;
                        }
                        else
                        {
                            rgbImage[row][col].r = (cfaImage[row-1][col-1]+cfaImage[row-1][col+1]+cfaImage[row+1][col+1] + cfaImage[row+1][col-1])/4;
                            rgbImage[row][col].g = (cfaImage[row-1][col]+cfaImage[row+1][col]+cfaImage[row][col-1] + cfaImage[row][col+1])/4;
                        }
                    }
                    rgbImage[row][col].b = cfaImage[row][col];
                }
                else // green positions
                {
                    if(col == width-1)  // green at column width-1
                    {
                        if(row == height-1) // green at position (height-1, width-1)
                            rgbImage[row][col].r = cfaImage[row-1][col];
                        else
                            rgbImage[row][col].r = (cfaImage[row-1][col]+cfaImage[row+1][col])/2;
                        rgbImage[row][col].b = cfaImage[row][col-1];
                    }
                    else // green at other columns not width-1
                    {
                        if(row == height-1)
                            rgbImage[row][col].r = cfaImage[row-1][col];
                        else
                            rgbImage[row][col].r = (cfaImage[row-1][col]+cfaImage[row+1][col])/2;
                        rgbImage[row][col].b = (cfaImage[row][col-1]+cfaImage[row][col+1])/2;
                    }
                    rgbImage[row][col].g = cfaImage[row][col];
                }
            }
        }
    
    return true;
}

/*--- Colour Correction ---*/
bool colorCorrection (PIX **rgbImage, int width, int height )
{
    int row, col;
    unsigned char red, green, blue;
    float Tc[3][3] = {{1.18, -0.09, -0.13},
                      {-0.24, 1.19, -0.05},
                      {-0.18, -0.44, 1.71}};
    float tmp;
    
    for(row=0; row < height; row++)
        for( col=0; col < width; col++)
        {
            red = rgbImage[row][col].r;
            green = rgbImage[row][col].g;
            blue = rgbImage[row][col].b;
            
            // Red Correction
            tmp = Tc[0][0]*red + Tc[0][1]*green + Tc[0][2]*blue;
            if (tmp > 255)
                rgbImage[row][col].r = 255;
            else if (tmp < 0)
                rgbImage[row][col].r = 0;
            else
                rgbImage[row][col].r = tmp;
            
            // Green Correction
            tmp = Tc[1][0]*red + Tc[1][1]*green + Tc[1][2]*blue;
            if (tmp > 255)
                rgbImage[row][col].g = 255;
            else if (tmp < 0)
                rgbImage[row][col].g = 0;
            else
                rgbImage[row][col].g = tmp;
            
            // Blue Correction
            tmp = Tc[2][0]*red + Tc[2][1]*green + Tc[2][2]*blue;
            if (tmp > 255)
                rgbImage[row][col].b = 255;
            else if (tmp < 0)
                rgbImage[row][col].b = 0;
            else
                rgbImage[row][col].b = tmp;
        }
    
    return true;
}

/*--- Gramma Correction ---*/
bool gammaCorrection (PIX **rgbImage, int width, int height )
{
    int lutTable[256];
    int row, col;
    float gamma = 0.5;
    
    // Fill the LUT table
    for (int i = 0; i < 256; i++)
        lutTable[i] = pow((i / 255.0), gamma)*255.0;
    
    for (row = 0; row < height; row++)
        for ( col = 0; col < width; col++)
        {
            rgbImage[row][col].r = lutTable[rgbImage[row][col].r];
            rgbImage[row][col].g = lutTable[rgbImage[row][col].g];
            rgbImage[row][col].b = lutTable[rgbImage[row][col].b];
        }
    
    return true;
}
