#include "asi_image.h"
#include <stdlib.h>
#include <limits.h>

int image_init (image_type *image, int width, int height, dtype_enum dtype)
{
    image->width = width;
    image->height = height;
    image->dtype = dtype;

    /* Allocate memory for integer image */
    if (dtype == ASI_DTYPE_INT)
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

    return ASI_EXIT_INVALID_DTYPE;
}

int image_iget(image_type image, int i, int j)
{
    /* 'Quick 'n dirty' accessing, no sanity checks */
    return *((int*) image.data + i * image.width + j);  
}

int image_imax(image_type image, int *max)
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
            value = image_iget(image, i, j);

            if (value > temp_max)
            {
                temp_max = value;
            }
        }
    }

    *max = temp_max;

    return ASI_EXIT_SUCCESS;
}
    
int image_imin(image_type image, int *min)
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
            value = image_iget(image, i, j);

            if (value < temp_min)
            {
                temp_min = value;
            }
        }
    }

    *min = temp_min;

    return ASI_EXIT_SUCCESS;
}
    