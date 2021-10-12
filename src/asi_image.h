#ifndef _ASI_IMAGE_H_
#define _ASI_IMAGE_H_

#define ASI_EXIT_SUCCESS 1
#define ASI_EXIT_FAILURE 0
#define ASI_EXIT_FAILED_ALLOC 100
#define ASI_EXIT_FAILED_DEALLOC 101
#define ASI_EXIT_INVALID_DTYPE 102
#define ASI_EXIT_INVALID_FTYPE 103
#define ASI_EXIT_INVALID_DDEPTH 104
#define ASI_EXIT_INVALID_IMG_DIM 105
#define ASI_EXIT_INVALID_VALUE 106
#define ASI_EXIT_FILE_NOT_FOUND 107
#define ASI_EXIT_FILE_OPEN_FAILED 108
#define ASI_EXIT_INVALID_ARG_COUNT 109
#define ASI_EXIT_IMG_DIM_MISMATCH 110
#define ASI_EXIT_IMG_DTYPE_MISMATCH 111
#define ASI_EXIT_OUT_OF_BOUNDS -99
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
// TODO rename: image_init -> image_new
int image_init (image_type *image, int width, int height, 
        dtype_enum dtype);

/* Copy image */
int image_copy (const image_type src, image_type target);

/* Convert data type */
int image_convert_dtype(const image_type src, image_type target, 
        dtype_enum target_dtype);

/* Free memory */
void image_delete (image_type *image);

/* Accessing image pixels */
int image_get(image_type image, int i, int j);
int * image_get_rgb(image_type image, int i, int j);
double image_fget(image_type image, int i, int j);
double * image_fget_rgb(image_type image, int i, int j);

/* Writing image pixels */
void image_put(image_type image, int value, int i, int j);
void image_fput(image_type image, double value, int i, int j);

/* Boundary handling for indices */
int image_mirror_boundary_x(image_type image, int j);
int image_mirror_boundary_y(image_type image, int i);


/* Image statistics */
int image_max(image_type image, int *max);
int image_min(image_type image, int *min);

#endif
