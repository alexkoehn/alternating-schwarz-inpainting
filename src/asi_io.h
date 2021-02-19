#ifndef _ASI_IO_H_
#define _ASI_IO_H_

#define ASI_EXIT_SUCCESS 1
#define ASI_EXIT_FAILURE 0
#define ASI_EXIT_FAILED_ALLOC 100
#define ASI_EXIT_INVALID_DTYPE 101
#define ASI_EXIT_INVALID_FTYPE 102
#define ASI_EXIT_INVALID_DDEPTH 103
#define ASI_EXIT_INVALID_IMG_DIM 104
#define ASI_EXIT_FILE_NOT_FOUND 105

/* Supported datatypes for image structs */
typedef enum dtype
{
    ASI_DTYPE_INT,
    ASI_DTYPE_DOUBLE,
    ASI_DTYPE_INT_RGB,
    ASI_DTYPE_DOUBLE_RGB
} dtype_enum;

typedef enum pnm_ftype
{
    PNM_P1,
    PNM_P2,
    PNM_P3,
    PNM_P4,
    PNM_P5,
    PNM_P6
} pnm_ftype_enum;

/* Image data structure */
typedef struct image
{
    void *data; /* Void pointer to image data */
    int width;  /* Image width (Number of columns) */
    int height; /* Image height (Number of rows) */
    dtype_enum dtype; /* Image data type */
} image_type;

typedef struct pnm_header
{
    pnm_ftype_enum ftype;
    int width;
    int height;
    int data_depth;
    int header_length;
} pnm_header_type;

/* Allocate memory for image struct */
int image_init (image_type *image, int width, int height, 
        dtype_enum dtype);

/* Access image data using x and y coordinates */
//void * image_at (image_type image, int i, int j);

/* Import */
int image_read_pnm_header(pnm_header_type *header, const char* filename);
int image_read_pnm_body(image_type *image, const char* filename,
        pnm_header_type header);
int image_read_pgm (image_type *image, const char *filename);

#endif
