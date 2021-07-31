#include "asi_image.h"
#include <stdlib.h>
#include <limits.h>

/*----------------------------------------------------------------------------*/

/*
 * Initialisation of an image, sets all entries to zero.
 * @image   [ O ] Image
 * @width   [ I ] Image width (number of pixel columns)
 * @height  [ I ] Image height (number of pixel rows)
 * @dtype   [ I ] Image data type
 */
int image_init (image_type *image, int width, int height, dtype_enum dtype)
{
    image->width = width;
    image->height = height;
    image->dtype = dtype;

    /* Allocate memory for integer image */
    if (dtype == ASI_DTYPE_INT || dtype == ASI_DTYPE_BOOLEAN)
    {
        image->data = (int *) calloc(width * height, sizeof(int));

        if (image->data == NULL)
        {
            return ASI_EXIT_FAILED_ALLOC;
        }

        return ASI_EXIT_SUCCESS; 
    }

    /* Allocate memory for double image */
    if (dtype == ASI_DTYPE_DOUBLE)
    {
        image->data = (double *) calloc(width * height, sizeof(double));

        if (image->data == NULL)
        {
            return ASI_EXIT_FAILED_ALLOC;
        }

        return ASI_EXIT_SUCCESS; 
    }

    /* Allocate memory for integer-valued RGB image */
    if (dtype == ASI_DTYPE_INT_RGB) 
    {
        image->data = (int *) calloc(width * height * 3, sizeof(int));

        if (image->data == NULL)
        {
            return ASI_EXIT_FAILED_ALLOC;
        }

        return ASI_EXIT_SUCCESS;
    }

    /* Allocate memory for double-valued RGB image */
    if (dtype == ASI_DTYPE_DOUBLE_RGB) 
    {
        image->data = (double *) calloc(width * height * 3, sizeof(double));

        if (image->data == NULL)
        {
            return ASI_EXIT_FAILED_ALLOC;
        }

        return ASI_EXIT_SUCCESS;
    }

    return ASI_EXIT_INVALID_DTYPE;
}

/*----------------------------------------------------------------------------*/

/*
 * Frees memory of an image
 * @image   [ I ] Image to be deleted
 */
void image_delete (image_type *image)
{
    free(image->data);

    return;
}

/*----------------------------------------------------------------------------*/

/*
 * Creates a copy of an image.
 * @src  [ I ] Source image to be copied
 * @target  [ O ] Target image, needs to be initialised beforehand
 */
int image_copy (const image_type src, image_type target)
{
    int i, j; /* Iteration variables */
    int dval; /* Integer value */
    double fval; /* Double value */

    // TODO right now only for greyscale images 
    if (src.dtype == ASI_DTYPE_INT_RGB 
            || src.dtype == ASI_DTYPE_DOUBLE_RGB) 
    {
        return ASI_NOT_IMPLEMENTED_YET;
    }

    /* Check if image dimensions and datatype match of src and target */
    if (src.width != target.width || src.height != target.height)
    {
        return ASI_EXIT_IMG_DIM_MISMATCH;
    }

    if (src.dtype != target.dtype)
    {
        return ASI_EXIT_IMG_DTYPE_MISMATCH;
    }

    //TODO accomodate other dtypes
    /* Copy image pixel by pixel */
    if (src.dtype == ASI_DTYPE_INT || src.dtype == ASI_DTYPE_BOOLEAN)
    {
        for (i = 0; i < src.height; i++)
        {
            for (j = 0; j < src.width; j++)
            {
                dval = image_get(src, i, j);
                image_put(target, dval, i, j);
            }
        }
    }
    else if (src.dtype == ASI_DTYPE_DOUBLE)
    {
        for (i = 0; i < src.height; i++)
        {
            for (j = 0; j < src.width; j++)
            {
                fval = image_fget(src, i, j);
                image_fput(target, fval, i, j);
            }
        }
    }
    else
    {
        return ASI_NOT_IMPLEMENTED_YET; // Note: should never be reached
    }

    return ASI_EXIT_SUCCESS;
}

/*----------------------------------------------------------------------------*/

/*
 * Returns integer-valued pixel value from given location. 'Quick 'n dirty' 
 * accessing, no sanity checks
 * @image   [ I ] Image
 * @i       [ I ] y coordinate (row number)
 * @j       [ I ] x coordinate (column number)
 */
int image_get(image_type image, int i, int j)
{
    return *((int*) image.data + i * image.width + j);  
}

/*----------------------------------------------------------------------------*/

/*
 * Puts an integer at a given location in an image. 'Quick 'n dirty' 
 * accessing, no sanity checks
 * @image   [ I ] Image
 * @value   [ I ] Integer pixel value
 * @i       [ I ] y coordinate (row number)
 * @j       [ I ] x coordinate (column number)
 */
void image_put(image_type image, int value, int i, int j)
{
    *((int*) image.data + i * image.width + j) = value;

    return;
}

/*----------------------------------------------------------------------------*/
/*
 * Returns double-valued pixel value from given location. 'Quick 'n dirty' 
 * accessing, no sanity checks
 * @image   [ I ] Image
 * @i       [ I ] y coordinate (row number)
 * @j       [ I ] x coordinate (column number)
 */
double image_fget(image_type image, int i, int j)
{
    /* 'Quick 'n dirty' accessing, no sanity checks */
    return *((double*) image.data + i * image.width + j);  
}

/*----------------------------------------------------------------------------*/

/*
 * Puts a double at a given location in an image. 'Quick 'n dirty' 
 * accessing, no sanity checks
 * @image   [ I ] Image
 * @value   [ I ] Double pixel value
 * @i       [ I ] y coordinate (row number)
 * @j       [ I ] x coordinate (column number)
 */
void image_fput(image_type image, double value, int i, int j)
{
    *((double*) image.data + i * image.width + j) = value;

    return;
}

/*----------------------------------------------------------------------------*/

/*
 * Mirrors a column index along the image boundaries
 * @image   [ I ] Image
 * @j       [ I ] Column index
 */
int image_mirror_boundary_x(image_type image, int j)
{
    // Only supports mirroring for indices within +- width of image bounds
    if (j >= 0 && j < image.width)
    {
        return j;
    }
    else if (j < 0 && j >= -image.width)
    {
        return -j - 1;
    }
    else if (j > image.width-1 && j <= 2 * image.width - 1)
    {
        return  2 * image.width - j - 1;
    }
    else
    {
        // Case: index cannot be mirrored because it is too far away
        return ASI_EXIT_OUT_OF_BOUNDS;
    }
}

/*----------------------------------------------------------------------------*/

/*
 * Mirrors a row index along the image boundaries
 * @image   [ I ] Image
 * @i       [ I ] Row index
 */
int image_mirror_boundary_y(image_type image, int i)
{
    // Only supports mirroring for indices within +- height of image bounds
    if (i >= 0 && i < image.height)
    {
        return i;
    }
    else if (i < 0 && i >= -image.height)
    {
        return -i - 1;
    }
    else if (i > image.height-1 && i <= 2 * image.height - 1)
    {
        return  2 * image.height - i - 1;
    }
    else
    {
        // Case: index cannot be mirrored because it is too far away
        return ASI_EXIT_OUT_OF_BOUNDS;
    }
}

/*----------------------------------------------------------------------------*/

/*
 * Finds maximum pixel value of an integer-valued image.
 * @image   [ I ] Image
 * @max     [ O ] Maximum pixel value
 */
int image_max(image_type image, int *max)
{
    int i, j; /* Iteration variables */
    int temp_max; /* Temporary maximum */
    int value; /* Current pixel value */

    /* Check that data type is correct */
    if (image.dtype != ASI_DTYPE_INT)
    {
        return ASI_EXIT_INVALID_DTYPE; 
    }

    /* Iterate over image and find maximum */
    temp_max = INT_MIN;

    for (i = 0; i < image.height; i++)
    {
        for (j = 0; j < image.width; j++)
        {
            value = image_get(image, i, j);

            if (value > temp_max)
            {
                temp_max = value;
            }
        }
    }

    *max = temp_max;

    return ASI_EXIT_SUCCESS;
}
    
/*----------------------------------------------------------------------------*/

/*
 * Finds minimum pixel value of an integer-valued image.
 * @image   [ I ] Image
 * @min     [ O ] Minimum pixel value
 */
int image_min(image_type image, int *min)
{
    int i, j; /* Iteration variables */
    int temp_min; /* Temporary minimum */
    int value; /* Current pixel value */

    /* Check that data type is correct */
    if (image.dtype != ASI_DTYPE_INT)
    {
        return ASI_EXIT_INVALID_DTYPE; 
    }

    /* Iterate over image and find minimum */
    temp_min = INT_MAX;

    for (i = 0; i < image.height; i++)
    {
        for (j = 0; j < image.width; j++)
        {
            value = image_get(image, i, j);

            if (value < temp_min)
            {
                temp_min = value;
            }
        }
    }

    *min = temp_min;

    return ASI_EXIT_SUCCESS;
}
    
