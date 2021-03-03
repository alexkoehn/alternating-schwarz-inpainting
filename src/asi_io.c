#include "asi_io.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    FILE *file;
    int i;

    /* Load ASCII-coded pgm file */
    if (header.ftype == PNM_P2)
    {
        char buffer[256];
        int px_count;

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
    /* Load binary pgm file */
    else if (header.ftype == PNM_P5)
    {
        char *buffer;
        size_t buffer_size;
        char header_char;

        /* Open file in binary mode */
        file = fopen(filename, "rb");

        if (!file)
        {
            return ASI_EXIT_FILE_NOT_FOUND;
        }
        
        /* Skip header */
        i = 0;
        while (i < header.header_length)
        {
            header_char = fgetc(file);

            /* Increment i if parsed chars correspond to line break */
            if (header_char == '\n')
            {
                i++;
            }
            else if (header_char == EOF)
            {
                return ASI_EXIT_FAILURE;
            }
        }

        /* Allocate memory for file body */
        buffer_size = header.width * header.height * sizeof(char);
        buffer = (char *) malloc(buffer_size);

        /* Parse file body */
        //TODO seems to be faulty, some values are false
        fread(buffer, buffer_size, 1, file);

        /* Convert chars to intensity values */
        for (i = 0; i < header.width * header.height; i++)
        {
            *((int*)image->data + i) = ((int) buffer[i]) + 256; //TODO weirdly we have to add 256 to get correct number
        }

        /* Free buffer memory */
        free(buffer);

        fclose(file);
        return ASI_EXIT_SUCCESS;
    }
   
    return ASI_NOT_IMPLEMENTED_YET;
}

int image_read_pnm (image_type *image, const char *filename)
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

int image_init_pnm_header(image_type image, pnm_header_type *header, 
        int binary_mode)
{
    pnm_ftype_enum ftype; 
    int min, max; /* Intensity minimum / maximum */
    
    /* Determine file type */
    if (image.dtype == ASI_DTYPE_BOOLEAN && binary_mode == 0)
    {
        ftype = PNM_P1; 
    }
    else if (image.dtype == ASI_DTYPE_BOOLEAN && binary_mode == 1)
    {
        ftype = PNM_P2;
    }
    else if (image.dtype == ASI_DTYPE_INT && binary_mode == 0)
    {
        ftype = PNM_P3;
    }
    else if (image.dtype == ASI_DTYPE_INT && binary_mode == 1)
    {
        ftype = PNM_P4;
    }
    else
    {
        return ASI_EXIT_INVALID_DTYPE;
    }

    /* Determine minimum and maximum */
    image_imin(image, &min); 
    image_imax(image, &max);

    /* Check that values are positive */
    if (min < 0 || max < 0)
    {
        return ASI_EXIT_INVALID_VALUE;
    }
   
    /* Write to header struct */
    header->ftype = ftype;
    header->width = image.width;
    header->height = image.height;
    header->data_depth = max;

    return ASI_EXIT_SUCCESS;
}

int image_write_pnm(image_type image, char* filename, int binary_mode)
{
    int ret;
    FILE *file;
    pnm_header_type header;

    /* Prepare header */
    ret = image_init_pnm_header(image, &header, binary_mode); 

    if (ret != ASI_EXIT_SUCCESS)
    {
        return ret;
    }

    /* Write file */
    if (binary_mode == 1)
    {
        //file = fopen(filename, "wb");
        return ASI_NOT_IMPLEMENTED_YET;
    } 
    else
    {
        file = fopen(filename, "w");

        if (!file)
        {
            return ASI_EXIT_FILE_OPEN_FAILED;
        }

        /* Write header */
        if (header.ftype == PNM_P1)
        {
            fprintf(file, "P1\n");
            fprintf(file, "%d %d\n", image.width, image.height);
        }
        else if (header.ftype == PNM_P3)
        {
            fprintf(file, "P3\n");
            fprintf(file, "%d %d\n", image.width, image.height);
            fprintf(file, "%d\n", header.data_depth);
        }
        else
        {
            //TODO should not be reached
            return ASI_NOT_IMPLEMENTED_YET;
        }
        //TODO

        /* Write image */
        //TODO
    }
   
    fclose(file); 
    return ASI_EXIT_SUCCESS;
}

