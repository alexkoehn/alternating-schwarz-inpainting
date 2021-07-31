#include "asi_image.h"
#include <stdlib.h>
#include <limits.h>

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

/*
 * Frees memory of an image
 * @image   [ I ] Image to be deleted
 */
void image_delete (image_type *image)
{
    free(image->data);

    return;
}

int image_copy (const image_type image_master, image_type *image_copy)
{
    int i, j; /* Iteration variables */
    int ret; /* Return value */

    // TODO right now only for int valued greyscale images 
    if (image_master.dtype != ASI_DTYPE_INT 
            && image_master.dtype != ASI_DTYPE_BOOLEAN) 
    {
        return ASI_NOT_IMPLEMENTED_YET;
    }

    /* Initialise copy */
    ret = image_init(image_copy, image_master.width, image_master.height, 
            image_master.dtype); 

    if (ret != ASI_EXIT_SUCCESS)
    {
        return ret;
    }

    //TODO accomodate other dtypes
    /* Copy image pixel by pixel */
    for (i = 0; i < image_master.height; i++)
    {
        for (j = 0; j < image_master.width; j++)
        {
            int value = image_get(image_master, i, j);
            image_put(*image_copy, value, i, j);
        }
    }

    return ASI_EXIT_SUCCESS;
}

int image_get(image_type image, int i, int j)
{
    /* 'Quick 'n dirty' accessing, no sanity checks */
    return *((int*) image.data + i * image.width + j);  
}

void image_put(image_type image, int value, int i, int j)
{
    *((int*) image.data + i * image.width + j) = value;

    return;
}

double image_fget(image_type image, int i, int j)
{
    /* 'Quick 'n dirty' accessing, no sanity checks */
    return *((double*) image.data + i * image.width + j);  
}

void image_fput(image_type image, double value, int i, int j)
{
    *((double*) image.data + i * image.width + j) = value;

    return;
}


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
    
