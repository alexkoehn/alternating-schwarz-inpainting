#ifndef _ASI_IO_H_
#define _ASI_IO_H_

#include "asi_image.h"

typedef enum pnm_ftype
{
    PNM_P1,
    PNM_P2,
    PNM_P3,
    PNM_P4,
    PNM_P5,
    PNM_P6
} pnm_ftype_enum;

typedef struct pnm_header
{
    pnm_ftype_enum ftype;
    int width;
    int height;
    int data_depth;
    int header_length;
} pnm_header_type;

/* Import */
int image_read_pnm_header(pnm_header_type *header, const char* filename);
int image_read_pnm_body(image_type *image, const char* filename,
        pnm_header_type header);
int image_read_pnm (image_type *image, const char *filename);

/* Export */
int image_write_pnm(image_type image, char* filename, int binary_mode);

#endif
