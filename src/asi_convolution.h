#ifndef _ASI_CONVOLUTION_H_
#define _ASI_CONVOLUTION_H_

#include "asi_image.h"

/* Supported kernel types */
typedef enum kernel_name
{
    ASI_GAUSSIAN,
    ASI_SOBEL_X,
    ASI_SOBEL_Y,
    ASI_LAPLACIAN
} kernel_name_enum;

/* Kernel struct */
typedef struct kernel
{
    double *weights;
    kernel_name_enum name;
    int width;
    int height;
} kernel_type;

/* Initialisations of a kernel */
int kernel_init(kernel_type *kernel, kernel_name_enum name, int argc, ...);

/* Memory deallocation of a kernel */
void kernel_delete(kernel_type *kernel);

/* Convolution of an image with a kernel */
int image_convolve(image_type *image, kernel_type kernel);

#endif
