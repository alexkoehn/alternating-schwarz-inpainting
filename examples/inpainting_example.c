#include "../src/asi_io.h"
#include "../src/asi_mask.h"
#include "../src/asi_convolution.h"
#include <stdio.h>
#include <math.h>

int main()
{
    image_type image, image_export;
    image_type image_f;
    int return_code;

    // Load image
    return_code = image_read_pnm(&image, "examples/pinguin_256.pgm");

    if (return_code != ASI_EXIT_SUCCESS)
    {
        printf("Error loading image: Error code %d\n", return_code);
        return return_code;
    }

    printf("Width: %d, Height: %d, Data type: %d\n", image.width, image.height, image.dtype);

    // Convert image to double
    image_init(&image_f, image.width, image.height, ASI_DTYPE_DOUBLE);
    image_copy(image, image_f);
    image_delete(&image);

    //Floyd Steinberg
    image_type image_dithering;
    image_type image_dithering_export;
    return_code = floyd_steinberg_dithering(image_f, &image_dithering);

    if (return_code != ASI_EXIT_SUCCESS)
    {
        printf("Error during image dithering: Error code %d\n", return_code);
        return return_code;
    }

    image_init(&image_dithering_export, image_dithering.width, image_dithering.height, ASI_DTYPE_INT);
    image_copy(image_dithering, image_dithering_export);
    image_delete(&image_dithering);

    return_code = image_write_pnm(image_dithering_export, "dithering.pgm", 0);
    printf("Return code: %d\n", return_code);

    image_type mask;
    return_code = mask_belhachmi_init(image_f, &mask, 0.1);

    if (return_code != ASI_EXIT_SUCCESS)
    {
        printf("Error during mask creation: Error code %d\n", return_code);
        return return_code;
    }
    
    image_init(&image_export, image_f.width, image_f.height, ASI_DTYPE_INT);
    image_copy(mask, image_export);

    return_code = image_write_pnm(image_export, "belhachmi_mask.pgm", 0);
    printf("Return code: %d\n", return_code);


    return 0;
}
