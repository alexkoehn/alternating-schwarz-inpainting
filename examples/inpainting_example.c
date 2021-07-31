#include "../src/asi_io.h"
#include "../src/asi_mask.h"
#include "../src/asi_convolution.h"
#include <stdio.h>
#include <math.h>

int main()
{
    image_type image;
    int return_code;

    // Load image
    return_code = image_read_pnm(&image, "examples/pinguin_256.pgm");

    if (return_code != ASI_EXIT_SUCCESS)
    {
        printf("Error loading image: Error code %d\n", return_code);
        return return_code;
    }

    printf("Width: %d, Height: %d, Data type: %d\n", image.width, image.height, image.dtype);

    //Floyd Steinberg
    image_type image_dithering;
    return_code = floyd_steinberg_dithering(image, &image_dithering);
    
    if (return_code != ASI_EXIT_SUCCESS)
    {
        printf("Error during image dithering: Error code %d\n", return_code);
        return return_code;
    }

    return_code = image_write_pnm(image_dithering, "examples/test.pgm", 0);
    printf("Return code: %d\n", return_code);

    // Convolve image
    kernel_type kernel;
    return_code = kernel_init(&kernel, ASI_GAUSSIAN, 2, 5.0, 2.0);
    printf("Return code: %d\n", return_code);

    return_code = image_convolve(image, kernel);
    printf("Return code: %d\n", return_code);

    // Linearly scale result to range 0 to 255 to export
    /*int min, max;
    image_max(image, &max);
    image_min(image, &min);

    double fmax, fmin;
    fmin = (double) min;
    fmax = (double) max;
    double range = fmax - fmin;

    for (int i = 0; i < image.height; i++)
    {
        for (int j = 0; j < image.width; j++)
        {
            int val = (int)floor(((double)image_get(image, i, j) - fmin) / range * 255.0);
            image_put(image, val, i, j);
        }
    }*/

    return_code = image_write_pnm(image, "examples/sobel.pgm", 0);

    return 0;
}
