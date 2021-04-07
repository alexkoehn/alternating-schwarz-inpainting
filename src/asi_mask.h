#ifndef _ASI_MASK_H_
#define _ASI_MASK_H_

#include "asi_image.h"

/* Dithering with the Floyd-Steinberg algorithm */
int floyd_steinberg_dithering(const image_type image, image_type 
        *image_dithered);

int floyd_steinberg_mask(const image_type image, image_type
        *mask, double compression_ratio);

/* Randomly selected mask */
int random_mask(const image_type image, image_type
        *mask, double compression_ratio);

#endif
