#include "../src/asi_io.h"
#include "../src/asi_mask.h"
#include "../src/asi_convolution.h"
#include "stdio.h"

int main()
{
    printf("Hello World\n");
    image_type image;
    int return_code;

    return_code = image_read_pnm(&image, "examples/pinguin_256.pgm");

    printf("%d %d %d\n", image.width, image.height, image.dtype);
    printf("%d\n", return_code);

    //Floyd Steinberg
    image_type image_dithering;
    return_code = floyd_steinberg_dithering(image, &image_dithering);
    printf("%d\n", return_code);

    for (int i = 0; i < image_dithering.height; i++)
    {
        for (int j = 0; j < image_dithering.width; j++)
        {
            printf("%d ", image_get(image_dithering, i, j));
        }
        printf("\n");
    }

    int min, max;
    image_max(image_dithering, &max);
    image_min(image_dithering, &min);

    return_code = image_write_pnm(image_dithering, "examples/test.pgm", 0);
    printf("Return code: %d\n", return_code);

    printf("Min %d, max %d\n", min, max);

    kernel_type kernel;
    return_code = kernel_init(&kernel, ASI_SOBEL_X, 0);
    printf("Return code: %d\n", return_code);

    return_code = image_convolve(&image, kernel);
    printf("Return code: %d\n", return_code);
    for (int i = 0; i < image_dithering.height; i++)
    {
        for (int j = 0; j < image_dithering.width; j++)
        {
            printf("%d ", image_get(image, i, j));
        }
        printf("\n");
    }


    return_code = image_write_pnm(image, "examples/sobel.pgm", 0);

    return 0;
}
