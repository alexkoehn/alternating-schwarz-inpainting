#include "asi_io.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int image_init (image_type *image, int width, int height, dtype_enum dtype)
{
    image->width = width;
    image->height = height;
    image->dtype = dtype;

    /* Allocate memory for integer image */
    if (dtype == ASI_DTYPE_INT)
    {
        image->data = (int *) calloc(width * height, sizeof(int));

        if (image->data == NULL)
        {
            return ASI_EXIT_FAILED_ALLOC;
        }

        return ASI_EXIT_SUCCESS; 
    }

    /* Allocate memory for double image */
    if (dtype == ASI_DTYPE_DOUBLE)
    {
        image->data = (double *) calloc(width * height, sizeof(double));

        if (image->data == NULL)
        {
            return ASI_EXIT_FAILED_ALLOC;
        }

        return ASI_EXIT_SUCCESS; 
    }

    return ASI_EXIT_INVALID_DTYPE;
}

int image_read_pnm_header(pnm_header_type *header, const char* filename)
{
    FILE *file;
    char buffer[256];

    /* Attempt to open file */
    file = fopen(filename, "rb");

    if (!file)
    {
        return ASI_EXIT_FILE_NOT_FOUND;
    }

    /* Minimal header length (= PBM header without comments) */
    header->header_length = 2;

    /* Parse file type */
    if (fgets(buffer, 256, file) == NULL)
    {
        fclose(file);
        return ASI_EXIT_INVALID_FTYPE;
    }

    /* Skip comments and empty lines */
    while (buffer[0] == '#' || buffer[0] == ' ' || buffer[0] == '\n')
    {
        if (fgets(buffer, 256, file) == NULL)
        {
            fclose(file);
            return ASI_EXIT_INVALID_FTYPE;
        }

        header->header_length++;
    }

    if (buffer[0] == 'P' && buffer[1] != '\n')
    {
        switch (buffer[1])
        {
            case '1' : 
                header->ftype = PNM_P1;
                break;
            case '2' :
                header->ftype = PNM_P2;
                break;
            case '3' :
                header->ftype = PNM_P3;
                break;
            case '4' :
                header->ftype = PNM_P4;
                break;
            case '5' :
                header->ftype = PNM_P5;
                break;
            case '6' :
                header->ftype = PNM_P6;
                break;
            default :
                fclose(file);
                return ASI_EXIT_INVALID_FTYPE;
        }
    }
    else
    {
        fclose(file);
        return ASI_EXIT_INVALID_FTYPE;
    }

    /* Parse image dimensions */
    if (fgets(buffer, 256, file) == NULL)
    {
        fclose(file);
        return ASI_EXIT_INVALID_FTYPE;
    }

    /* Skip comments and empty lines */
    while (buffer[0] == '#' || buffer[0] == ' ' || buffer[0] == '\n')
    {
        if (fgets(buffer, 256, file) == NULL)
        {
            fclose(file);
            return ASI_EXIT_INVALID_FTYPE;
        }

        header->header_length++;
    }

    sscanf(buffer, "%d %d", &header->width, &header->height);

    if (header->width < 1 || header->height < 1)
    {
        fclose(file);
        return ASI_EXIT_INVALID_IMG_DIM;
    }

    /* Parse value depth (only defined for .pgm) */
    if (header->ftype == PNM_P1 || header->ftype == PNM_P4)
    {
        header->data_depth = 1;
    }
    else
    {
        if (fgets(buffer, 256, file) == NULL)
        {
            fclose(file);
            return ASI_EXIT_INVALID_FTYPE;
        }
        
        sscanf(buffer, "%d", &header->data_depth);

        if (header->data_depth <= 0)
        {
            fclose(file);
            return ASI_EXIT_INVALID_DDEPTH;
        }

        header->header_length++;
    }

    /* Skip empty lines after header */
    while (fgets(buffer, 256, file))
    {
        if (buffer[0] == '#' || buffer[0] == ' ' || buffer[0] == '\n')
        {
            header->header_length++;
        }
        else
        {
            break;
        }
    }
    
    fclose(file);

    return ASI_EXIT_SUCCESS;
}

int image_read_pnm_body (image_type *image, const char *filename,
        pnm_header_type header)
{
    int px_count;
    int i;
    FILE *file;
    char buffer[256];

    if (header.ftype == PNM_P2)
    {
        /* Open file */
        file = fopen(filename, "r");

        if (!file)
        {
            return ASI_EXIT_FILE_NOT_FOUND;
        }

        /* Skip header */
        for (i = 0; i < header.header_length; i++)\
        {
            if (fgets(buffer, 256, file) == NULL)
            {
                return ASI_EXIT_FAILURE;
            }
        }

        /* Iterate over file body */
        px_count = 0;

        while (fgets(buffer, 256, file))
        {
            /* Split string in case multiple pixel values per line exist */
            char *token;
            token = strtok(buffer, " ");

            /* Iterate over tokens and parse pixel values */
            while (token != NULL)
            {
                int value = (int) strtol(token, &token, 10);
                *((int*)image->data + px_count) = value;

                token = strtok(NULL, " ");
                px_count++;
            }
        }

        fclose(file);
        return ASI_EXIT_SUCCESS;
    }
   
    return ASI_EXIT_INVALID_FTYPE;
}

int image_read_pgm (image_type *image, const char *filename)
{
    int ret;
    pnm_header_type header;
    dtype_enum dtype;

    /* Parse PNM header */
    ret = image_read_pnm_header(&header, filename);

    if (ret != ASI_EXIT_SUCCESS)
    {
        return ret;
    }

    /* Determine data type from file type */
    if (header.ftype == PNM_P3 || header.ftype == PNM_P6)
    {
        dtype = ASI_DTYPE_INT_RGB;
    }
    else
    {
        dtype = ASI_DTYPE_INT;
    }

    /* Initialise image */
    ret = image_init(image, header.width, header.height, dtype);

    if (ret != ASI_EXIT_SUCCESS)
    {
        return ret;
    }

    /* Read PNM body */
    ret = image_read_pnm_body(image, filename, header);

    if (ret != ASI_EXIT_SUCCESS)
    {
        return ret;
    }

    return ASI_EXIT_SUCCESS;
}

