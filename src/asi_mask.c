#include "asi_mask.h"
#include "asi_convolution.h"
#include <math.h>

/*----------------------------------------------------------------------------*/

/*
 * Floyd-Steinberg dithering algorithm
 * @image   [ I ] 8-bit integer valued input image
 * @result  [ O ] Dithered image
 */
int floyd_steinberg_dithering(const image_type image, image_type *result)
{
    int i, j; /* Iteration variables */
    int ret; /* Return value */
    double value_old, value_new; /* Temporary pixel values */
    double value; /* Pixel value of neighbouring pixels */
    double error; /* Quantisation error */

    //TODO implement Floyd-Steinberg for colour images
    /* Make sure image is of double type */
    if (image.dtype != ASI_DTYPE_DOUBLE)
    {
        return ASI_EXIT_INVALID_DTYPE;
    }

    /* First create a working copy of image */
    ret = image_init(result, image.width, image.height, ASI_DTYPE_DOUBLE);

    if (ret != ASI_EXIT_SUCCESS)
    {
        return ret;
    }

    ret = image_copy(image, *result);

    if (ret != ASI_EXIT_SUCCESS)
    {
        return ret;
    }

    /* Proceed through image starting from the top left */
    for (i = 0; i < image.height; i++)
    {
        for (j = 0; j < image.width; j++)
        {
            value_old = image_fget(*result, i, j);

            /* Put new value depending on if it is closer to 0 or 255 */
            if (value_old > 127.5)
            {
                value_new = 255.0; 
            }
            else
            {
                value_new = 0.0;
            }

            image_fput(*result, value_new, i, j);

            /* Compute error */
            error = value_old - value_new;

            /* Propagate error */
            // TODO Since images are int-valued, error will be slightly different
            // compared to real-valued images
            if (j < image.width-1)
            {
                value = image_fget(*result, i, j+1);
                image_fput(*result, 
                        value + error * 7.0 / 16.0, 
                        i, j+1);
            }
            if (i < image.height-1)
            {
                value = image_fget(*result, i+1, j);
                image_fput(*result, 
                        value + error * 5.0 / 16.0, 
                        i+1, j);
            }
            if (j > 0 && i < image.height-1)
            {
                
                value = image_fget(*result, i+1, j-1);
                image_fput(*result, 
                        value + error * 3.0 / 16.0, 
                        i+1, j-1);
            }
            if (j < image.width-1 && i < image.height-1)
            {
                value = image_fget(*result, i+1, j+1);
                image_fput(*result, value + error * 1.0 / 16.0, 
                        i+1, j+1);
            }
        }
    }

    return ASI_EXIT_SUCCESS;
}

/*----------------------------------------------------------------------------*/

/*
 * Prepares a mask used for inpainting based on the absolute value of the
 * Laplacian followed by Floyd-Steinberg dithering. Methology by Belhachmi et
 * al. (2009): How to choose interpolation data in images. SIAM Journal on
 * Applied Mathematics 70(1), pp. 333--352.
 * @image               [ I ] Input image
 * @mask                [ O ] Inpainting mask
 * @compression_ratio   [ I ] Compression ratio
 */
int mask_belhachmi_init(const image_type image, image_type
        *mask, double compression_ratio)
{
    double abs_mean; /* Average grey value */
    double lambda; /* Factor to enforce compression ratio */
    double fval; /* Pixel value */
    int i, j; /* Loop variables */
    int ret_val; /* Return value */
    kernel_type kernel_gauss, kernel_lapl; /* Convolution kernels */
    image_type image_f; /* Double valued copy of the input image */

    /* Initialise an double-valued copy of the input image */
    image_init(&image_f, image.width, image.height, ASI_DTYPE_DOUBLE);
    image_copy(image, image_f);

    /* Initialise Gaussian convolution kernel with sigma = 1.0 */
    ret_val = kernel_init(&kernel_gauss, ASI_GAUSSIAN, 2, 1.0, 3.0);

    if (ret_val != ASI_EXIT_SUCCESS)
    {
        return ret_val;
    }
    
    /* Do Gaussian smoothing */
    ret_val = image_convolve(image_f, kernel_gauss);

    if (ret_val != ASI_EXIT_SUCCESS)
    {
        return ret_val;
    }
    
    /* Initialise Laplacian convolution kernel */
    ret_val = kernel_init(&kernel_lapl, ASI_LAPLACIAN, 0);

    if (ret_val != ASI_EXIT_SUCCESS)
    {
        return ret_val;
    }

    /* Apply Laplace filter */
    ret_val = image_convolve(image_f, kernel_lapl);
    
    if (ret_val != ASI_EXIT_SUCCESS)
    {
        return ret_val;
    }

    /* Convert Laplace-filtered image to absolute values and compute its mean */
    abs_mean = 0.0;
    for (i = 0; i < image_f.height; i++)
    {
        for (j = 0; j < image_f.width; j++)
        {
            fval = fabs(image_fget(image_f, i, j));
            image_fput(image_f, fval, i, j); 
            abs_mean += fval;
        }
    }

    abs_mean /= (image_f.height * image_f.width);

    /* Compute Lambda and multiply it pointwise with the image */
    //TODO mask is zero if lambda is multiplied with fval 
    lambda = compression_ratio * 255.0 / abs_mean;

    for (i = 0; i < image_f.height; i++)
    {
        for (j = 0; j < image_f.width; j++)
        {
            fval = image_fget(image_f, i, j);
            image_fput(image_f, fval * lambda, i, j);
        }
    }
    
    /* Apply Floyd-Steinberg dithering to preprocessed image */
    ret_val = floyd_steinberg_dithering(image_f, mask);

    if (ret_val != ASI_EXIT_SUCCESS)
    {
        return ret_val;
    }

    return ASI_EXIT_SUCCESS;
}
