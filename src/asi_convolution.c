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

        /* Compute size of kernel */
        int h_size = (int) round(sigma * 2.0);
        kernel->width = 2 * h_size + 1;
        kernel->height = 2 * h_size + 1;

        /* Initialise kernel */
        kernel->weights = (double *) calloc (kernel->width * kernel->height, 
                sizeof(double));

        //TODO fill weight matrix
    }

    return ASI_NOT_IMPLEMENTED_YET;
}
