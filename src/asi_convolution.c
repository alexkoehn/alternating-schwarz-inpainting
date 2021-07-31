#include "asi_convolution.h"
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>

/*----------------------------------------------------------------------------*/

/*
 * Initialises a convolution kernel for a provided keyword and additional
 * arguments.
 * @kernel  [ O ] Convolution kernel
 * @name    [ I ] Convolution kernel name 
 * @argc    [ I ] Argument count (needs to be 0 if no arguments provided)
 * @...     [ I ] Optional additional arguments
 */
int kernel_init(kernel_type *kernel, kernel_name_enum name, int argc, ...)
{
    va_list args;
    va_start(args, argc);

    /* Case: Kernel is a Gaussian */
    if (name == ASI_GAUSSIAN)
    {
        kernel->name = ASI_GAUSSIAN;

        /* Fetch standard deviation and accuracy from function arguments */
        double sigma;
        double accuracy;

        /* If only one argument is given, assume standard deviation */
        if (argc == 1)
        {
            sigma = va_arg(args, double);
            accuracy = 2.0;
        }
        /* If there are two arguments, assume standard deviation and accuracy */
        else if (argc == 2)
        {
            sigma = va_arg(args, double);
            accuracy = va_arg(args, double);
        }
        /* Otherwise, return error code */
        else
        {
            return ASI_EXIT_INVALID_ARG_COUNT;
        }

        /* Size of kernel: Gaussian is separable so a 1D Gaussian suffices */
        int h_size = (int) round(sigma * 2.0);
        kernel->width = 2 * h_size + 1;
        kernel->height = 1;
        kernel->name = name;

        /* Initialise kernel */
        kernel->weights = (double *) calloc (kernel->width * kernel->height, 
                sizeof(double));

        //Fill weight matrix
        for (int i = -h_size; i <= h_size; i++)
        {
           kernel->weights[i] = 1.0 / (sigma * sqrt(2.0 * M_PI)) 
                   * exp (-0.5 * i * i / (sigma * sigma));
        }
    }
    /* Case: Kernel is a Sobel filter in x direction */
    else if (name == ASI_SOBEL_X)
    {
        kernel->width = 3;
        kernel->height = 3;
        kernel->name = name;

        /* Initialise kernel */
        kernel->weights = (double *) calloc (kernel->width * kernel->height, 
                sizeof(double));

        /* Fill weight matrix */
        kernel->weights[0] = -1.0;
        kernel->weights[2] = 1.0;
        kernel->weights[3] = -2.0;
        kernel->weights[5] = 2.0;
        kernel->weights[6] = -1.0;
        kernel->weights[8] = 1.0;
    }
    /* Case: Kernel is a Sobel filter in y direction */
    else if (name == ASI_SOBEL_Y) 
    {
        kernel->width = 3;
        kernel->height = 3;
        kernel->name = name;

        /* Initialise kernel */
        kernel->weights = (double *) calloc (kernel->width * kernel->height, 
                sizeof(double));

        /* Fill weight matrix */
        kernel->weights[0] = -1.0;
        kernel->weights[1] = -2.0;
        kernel->weights[2] = -1.0;
        kernel->weights[6] = 1.0;
        kernel->weights[7] = 2.0;
        kernel->weights[8] = 1.0;
    }
    /* Case: Kernel is a Laplacian */
    else if (name == ASI_LAPLACIAN)
    {
        kernel->width = 3;
        kernel->height = 3;
        kernel->name = name;

        /* Initialise kernel */
        kernel->weights = (double *) calloc (kernel->width * kernel->height,
                sizeof(double));

        /* Fill weight matrix */
        kernel->weights[1] = 1.0;
        kernel->weights[3] = 1.0;
        kernel->weights[4] = -4.0;
        kernel->weights[5] = 1.0;
        kernel->weights[7] = 1.0;
    }
    else
    {
        return ASI_NOT_IMPLEMENTED_YET;
    }

    return ASI_EXIT_SUCCESS;
}

/*----------------------------------------------------------------------------*/

/*
 * Free memory of kernel 
 * @kernel  [ I ] Kernel to be disposed of
 */
void kernel_delete(kernel_type *kernel)
{
    free(kernel->weights);

    return;
}

/*----------------------------------------------------------------------------*/

/*
 * Non-destructive 2D convolution of an image with a convolution kernel.
 * @src     [ I ] Source image
 * @target  [ O ] Target image, needs to be initialised beforehand
 * @kernel  [ I ] 2D convolution kernel
 */
// TODO incorporate different datatypes
void image_convolution_2d(const image_type src, image_type target, 
        const kernel_type kernel)
{
    int i, j, k, l; /* Loop variables */
    int i_shifted, j_shifted; /* Loop variables shifted by convolution */
    int half_w, half_h; /* Half width and height of kernel */
    double conv_sum; /* Convolution integrand */
    double img_val; /* Pixel value */
    double k_val; /* Kernel weight value */

    half_w = (int) floor(kernel.width / 2.0);
    half_h = (int) floor(kernel.height / 2.0);

    /* Loop over image */
    for (i = 0; i < src.height; i++)
    {
        for (j = 0; j < src.width; j++)
        {
            conv_sum = 0.0;

            for (k = -half_h; k <= half_h; k++)
            {
                /* Mirror shifted y coordinate if necessary */
                i_shifted = image_mirror_boundary_y(src, i+k);

                for (l = -half_w; l <= half_w; l++)
                {
                    /* Mirror shifted x coordinate if necessary */
                    j_shifted = image_mirror_boundary_x(src, j+l);

                    /* Evaluate convolution at position (i+k,j+l) */
                    img_val 
                        = (double) image_get(src, i_shifted, j_shifted);
                    k_val = (double) kernel.weights[(k+half_h) 
                        * kernel.height + l + half_w];

                    /* Add integrand to convolution sum */
                    conv_sum += img_val * k_val;
                }
            }

            /* Add convolution result to pixel at location (i,j) */
            image_put(target, conv_sum, i, j);
        }
    }

    return;
}

/*----------------------------------------------------------------------------*/

/*
 * Non-destructive 2D convolution leveraging the fact that some convolution
 * kernels can be seperated in two 1D convolutions (in x and y direction, 
 * respectively) which gives some speed-up over the standard 2D convolution.
 * @src     [ I ] Source image
 * @target  [ O ] Target image, needs to be initialised beforehand
 * @kernel  [ I ] 1D convolution kernel
 */
void image_convolution_2d_seperable(const image_type src, image_type target,
        const kernel_type kernel)
{
    int i, j, k; /* Loop variables */
    int i_shifted, j_shifted; /* Loop variables shifted by convolution */
    int half_w; /* Half width and height of kernel */
    double conv_sum; /* Convolution integrand */
    double img_val; /* Pixel value */
    double k_val; /* Kernel weight value */
    image_type tmp; /* Image to store intermediate convolution result */

    half_w = (int) floor(kernel.width / 2.0);

    /* Initialise temporary image */
    image_init(&tmp, src.width, src.height, src.dtype);

    /* Convolve in x direction first */
    for (i = 0; i < src.height; i++)
    {
        for (j = 0; j < src.width; j++)
        {
            conv_sum = 0.0;

            for (k = -half_w; k <= half_w; k++)
            {
                /* Mirror shifted x coordinate if necessary */
                j_shifted = image_mirror_boundary_x(src, j+k);

                /* Evaluate convolution at position (i, j+k) */
                img_val = (double) image_get(src, i, j_shifted);
                k_val = (double) kernel.weights[k + half_w];

                /* Add integrand to convolution sum */
                conv_sum += img_val * k_val;

            }
            
            /* Add convolution result to pixel at location (i,j) */
            image_put(tmp, conv_sum, i, j);
        }
    }

    /* Convolve in y direction */
    for (i = 0; i < src.height; i++)
    {
        for (j = 0; j < src.width; j++)
        {
            conv_sum = 0.0;

            for (k = -half_w; k <= half_w; k++)
            {
                /* Mirror shifted y coordinate if necessary */
                i_shifted = image_mirror_boundary_y(tmp, i+k);

                /* Evaluate convolution at position (i, j+k) */
                img_val = (double) image_get(tmp, i_shifted, j);
                k_val = (double) kernel.weights[k + half_w];

                /* Add integrand to convolution sum */
                conv_sum += img_val * k_val;
            }
            
            /* Add convolution result to pixel at location (i,j) */
            image_put(target, conv_sum, i, j);
        }
    }

    /* Remove temporary image */
    image_delete(&tmp);

    return;
}

/*----------------------------------------------------------------------------*/

/*
 *  Destructive convolution (original image does not get preserved) of an image 
 *  with a convolution kernel. Chooses the correct convolution procedure 
 *  depending on provided convolution kernel. 
 *  @image  [I/O] Image to be convolved 
 *  @kernel [ I ] Convolution kernel
 */
// TODO Pointer to image needed?
int image_convolve(image_type *image, kernel_type kernel)
{
    int i, j; /* Loop variables */
    image_type result; /* Temporary image for holding convolution results */
    
    /* Initialise temporary image by zeros */
    image_init(&result, image->width, image->height, image->dtype);

    /* Check if kernel is a Gaussian (seperable in two 1D convolutions) */
    if (kernel.name == ASI_GAUSSIAN)
    {
        return ASI_NOT_IMPLEMENTED_YET;
    }
    else
    {
        image_convolution_2d(*image, result, kernel);
    }

    //TODO create function for image_copy (without allocation)
    for (i = 0; i < image->height; i++)
    {
        for (j = 0; j < image->width; j++)
        {
            double value = image_get(result, i, j);
            image_put(*image, value, i, j);
        }
    }

    /* Remove result image */
    image_delete(&result);
    
    return ASI_EXIT_SUCCESS;
}
