#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //beatiful.bmp(source file)
    if(argv[2][0] != '.')//check whether the file name is there or not before .
    {
        if(strstr(argv[2],".bmp") != NULL)//check the source file extension is matched or not(.bmp)
        {
            encInfo->src_image_fname = argv[2];
        }
        else
        {
            printf("Error : source file should be .bmp file\n");
            return e_failure;
        }
    }
    else
    {
        printf("Error : source file name should not start with '.' \n");
        return e_failure;
    }
                          
    //secret.txt
    if(argv[3][0] != '.')//check whether the file name is there or not before .
    {
        if((strstr(argv[3],".txt") != NULL))//check the secret file extension is matched or not(.bmp or .txt or .sh or .h) or not
        {
            encInfo->secret_fname = argv[3];
            strcpy(encInfo->extn_secret_file,"txt");
        }
        else if(strstr(argv[3],".bmp") != NULL)
        {
            encInfo->secret_fname = argv[3];
            strcpy(encInfo->extn_secret_file,"bmp");
        }
        else if(strstr(argv[3],".h") != NULL)
        {
            encInfo->secret_fname = argv[3];
            strcpy(encInfo->extn_secret_file,"h");
        }
        else if(strstr(argv[3],".sh") != NULL)
        {
            encInfo->secret_fname = argv[3];
            strcpy(encInfo->extn_secret_file,"sh");
        }
        else
        {
            printf("Error : secret file should be .txt,.bmp,.h or .sh\n");
            return e_failure;
        }
    }
    else
    {
        printf("Error : secret file name should not start with '.'\n");
        return e_failure;
    }

    //dest file
    if(argv[4] != NULL)//check wherther the argv[4] is having NULL or not
    {
        if(argv[4][0] != '.')//check whether the file name is there or not before .
        {
            if(strstr(argv[4],".bmp") != NULL)//check the source file extension is matched or not(.bmp)
            {
                encInfo -> stego_image_fname = argv[4];
            }
            else
            {
                printf("Error : output file should be .bmp file\n");
                return e_failure;
            }

        }
        else
        {
            printf("Error : output file should not start with '.'\n");
            return e_failure;
        }
        
    }
    else
    {
        encInfo -> stego_image_fname = "default.bmp";
    }

    return e_success;

}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);//To get image capacity from get_image_size_for_bmp func
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);//To fget size of the secret size
    char *extn = strstr(encInfo->secret_fname,".txt");
    int extn_size = strlen(extn);
    int total_bytes = 54 + (strlen(MAGIC_STRING) * 8) + 32 + (extn_size*8) + 32 + (encInfo->size_secret_file * 8);

    if(encInfo->image_capacity > total_bytes)//Condition to check image capacity is greater than the no of bytes to insert
      return e_success;
    else
    {
        return e_failure;
        printf("Error : image capacity is less than total bytes\n");
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];
    rewind(fptr_src_image);//To get file pointer to 0th position
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);
    if(ftell(fptr_src_image) == ftell(fptr_dest_image))
       return e_success;
    else
    {
        return e_failure;
        printf("Error : at copy_bmp_to_header\n");
    }
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buffer[8];
    for(size_t i = 0;i<strlen(magic_string);i++)//Loop to encode magic string
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i],buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
       return e_success;
    else
    {
        return e_failure;
        printf("Error at encode_magic string\n");
    }

}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(size,buffer);// To encode size of the extension of file
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
       return e_success;
    else
    {
        return e_failure;
        printf("Error at encode sf extn size\n");
    }
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char buffer[8];
    for(size_t i = 0;i<strlen(file_extn);i++)//Loop  to encode extension of secret file
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i],buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
       return e_success;
    else
    {
        return e_failure;
        printf("Error at sf file extn\n");
    }
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,buffer);//To encode size of the secret data
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
       return e_success;
    else
    {
        return e_failure;
        printf("Erroor at encode sf size\n");
    }
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);//rewind the secret file to starting
    fread(encInfo->secret_data,encInfo->size_secret_file,1,encInfo->fptr_secret);
    char buffer[8];
    for(size_t i = 0 ;i < encInfo->size_secret_file;i++)//Loop to encode secret data using size of the secret data size
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(encInfo->secret_data[i],buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
       return e_success;
    else
    {
        return e_failure;
        printf("Error at encode secret file data");
    }
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,1,1,fptr_src))//Loop to encode the remaining data in the .bmp file
    {
        fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;   
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(size_t i = 0;i < 8;i++)//Loop to encode character data into lsb of the each byte
    {
        image_buffer[i] = image_buffer[i] & (~1) | (data >> i) & 1 ; 
    }
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for(size_t i = 0;i < 32;i++)//Loop to encode integer data into lsb of the each byte
    {
        imageBuffer[i] = imageBuffer[i] & (~1) | (size >> i) & 1 ; 
    }
}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("Encoding Mode\n");
    //Conditions in order to encode data into destination image.
    if(open_files(encInfo) == e_success)
    {
        if(check_capacity(encInfo) == e_success)
        {
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
            {
                if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
                {
                    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)
                    {
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)
                        {
                            if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
                            {
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
                                    {
                                        return e_success;
                                    }
                                    else
                                    {
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    return e_failure;
                                }
                            }
                            else
                            {
                                return e_failure;
                            }
                        }
                        else
                        {
                            return e_failure;
                        }

                    }
                    else
                    {
                        return e_failure;
                    }

                }
                else
                {
                    return e_failure;
                }

            }
            else
            {
                return e_failure;
            }

        }
        else
        {
            return e_failure;
        }

    }
    else
    {
        return e_failure;
    }
    
}
