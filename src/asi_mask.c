#include "asi_mask.h"

int floyd_steinberg_dithering(const image_type image, image_type 
        *image_dithered)
{
    int i, j; /* Iteration variables */
    int ret; /* Return value */
    int value_old, value_new; /* Temporary pixel values */
    int error; /* Quantisation error */

    //TODO implement Floyd-Steinberg for colour images
    /* Make sure image is of integer type */
    if (image.dtype != ASI_DTYPE_INT)
    {
        return ASI_EXIT_INVALID_DTYPE;
    }

    /* First create a working copy of image */
    ret = image_copy(image, image_dithered);

    if (ret != ASI_EXIT_SUCCESS)
    {
        return ret;
    }

    /* Proceed through image starting from the top left */
    for (i = 0; i < image.height - 1; i++)
    {
        //TODO j = 1, probably need to mirror image at boundary 
        for (j = 1; j < image.width - 1; j++)
        {
            value_old = image_get(*image_dithered, i, j);
            value_new;
            
            if (value_old > 127)
            {
                value_new = 255; 
            }
            else
            {
                value_new = 255;
            }

            error = value_old - value_new;

            //TODO clean-up here
            /* Propagate error */
            image_put(*image_dithered, 
                    image_get(*image_dithered, i, j+1) + error * 7.0 / 16.0, 
                    i, j+1);
            image_put(*image_dithered, 
                    image_get(*image_dithered, i+1, j-1) + error * 3.0 / 16.0, 
                    i+1, j-1);
            image_put(*image_dithered, 
                    image_get(*image_dithered, i+1, j) + error * 5.0 / 16.0, 
                    i+1, j);
            image_put(*image_dithered, 
                    image_get(*image_dithered, i+1, j+1) * error * 1.0 / 16.0, 
                    i+1, j+1);
        }
    }

    return ASI_EXIT_SUCCESS;
}
