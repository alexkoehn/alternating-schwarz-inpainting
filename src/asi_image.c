#include "asi_image.h"
#include <stdlib.h>

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

