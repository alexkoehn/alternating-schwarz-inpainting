#include "asi_convolution.h"
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>

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
        //TODO
        return ASI_NOT_IMPLEMENTED_YET;
    }
    else
    {
        return ASI_NOT_IMPLEMENTED_YET;
    }

    return ASI_EXIT_SUCCESS;
}

/* Free memory of kernel */
void kernel_delete(kernel_type *kernel)
{
    free(kernel->weights);

    return;
}

/* Convolution of an image with a kernel */
int image_convolve(image_type *image, kernel_type kernel)
{
    image_type image_temp; /* Temporary image for holding convolution results */
    int i, j, k, l; /* Loop variables */
    int i_shifted, j_shifted; /* Loop variables shifted by convolution */
    int half_w, half_h; /* Half width and height of kernel */
    
    /* Initialise temporary image by zeros */
    image_init(&image_temp, image->width, image->height, image->dtype);

    //TODO implement separated 2d convolution
    if (kernel.name == ASI_GAUSSIAN)
    {
        return ASI_NOT_IMPLEMENTED_YET;
    }

    half_w = (int) floor(kernel.width / 2.0);
    half_h = (int) floor(kernel.height / 2.0);

    /* Loop over image */
    for (i = 0; i < image->height; i++)
    {
        for (j = 0; j < image->width; j++)
        {
            for (k = -half_h; k <= half_h; k++)
            {
                for (l = -half_w; l <= half_w; l++)
                {
                    /* Evaluate convolution at position (i,j) */
                    i_shifted = image_mirror_boundary_y(*image, i+k);
                    j_shifted = image_mirror_boundary_x(*image, j+l);
                    double img_value 
                        = (double) image_get(*image, i_shifted, j_shifted);
                    double k_value = (double) kernel.weights[(k+half_h) 
                        * kernel.height + l + half_w];
                    
                    image_put(image_temp, img_value * k_value, i, j);
                }
            }
        }
    }

    //TODO create function for image_copy (without allocation)
    for (i = 0; i < image->height; i++)
    {
        for (j = 0; j < image->width; j++)
        {
            double value = image_get(image_temp, i, j);
            image_put(*image, value, i, j);
        }
    }

    return ASI_EXIT_SUCCESS;
}
