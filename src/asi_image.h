#ifndef _ASI_IMAGE_H_
#define _ASI_IMAGE_H_

#define ASI_EXIT_SUCCESS 1
#define ASI_EXIT_FAILURE 0
#define ASI_EXIT_FAILED_ALLOC 100
#define ASI_EXIT_INVALID_DTYPE 101
#define ASI_EXIT_INVALID_FTYPE 102
#define ASI_EXIT_INVALID_DDEPTH 103
#define ASI_EXIT_INVALID_IMG_DIM 104
#define ASI_EXIT_FILE_NOT_FOUND 105
#define ASI_EXIT_FILE_OPEN_FAILED 106
#define ASI_NOT_IMPLEMENTED_YET 999

/* Supported datatypes for image structs */
typedef enum dtype
{
    ASI_DTYPE_BOOLEAN,
    ASI_DTYPE_INT,
    ASI_DTYPE_DOUBLE,
    ASI_DTYPE_INT_RGB,
    ASI_DTYPE_DOUBLE_RGB
} dtype_enum;

/* Image data structure */
typedef struct image
{
    void *data; /* Void pointer to image data */
    int width;  /* Image width (Number of columns) */
    int height; /* Image height (Number of rows) */
    dtype_enum dtype; /* Image data type */
} image_type;

/* Allocate memory for image struct */
int image_init (image_type *image, int width, int height, 
        dtype_enum dtype);

/* Free memory */
int image_delete (image_type *image);

/* Accessing image pixels */
int image_iget(image_type image, int i, int j);
double image_fget(image_type image, int i, int j);

/* Writing image pixels */
int image_iput(image_type image, int value, int i, int j);
int image_fput(image_type image, double value, int i, int j);

/* Image statistics */
int image_imax(image_type image);
int image_imin(image_type image);

#endif
