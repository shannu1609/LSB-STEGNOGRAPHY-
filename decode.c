#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //default.bmp(source file)
    if(argv[2][0] != '.')//check whether the file name is there or not before .
    {
        if(strstr(argv[2],".bmp") != NULL)//check the source file extension is matched or not(.bmp)
        {
            decInfo->output_image_fname = argv[2];
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

    //secret file
    if(argv[3] != NULL)
    {
        int len = strlen(argv[3]);
        if(argv[3][len-1] != '.')
        {
            char *ptr = strstr(argv[3],".");
            *(ptr+1) = '\0';
        }
        decInfo->secret_fname = argv[3];
    }
    else
    {
        decInfo->secret_fname = malloc(50);
        if(decInfo->secret_fname == NULL)
        {
            perror("Memory not allocated\n");
            return e_failure;
        }
        strcpy(decInfo->secret_fname,"output.");
    }
    return e_success;          
}

Status open_files_decode(DecodeInfo *decInfo)
{
    //Src Image file to decode
    decInfo->fptr_output_image = fopen(decInfo->output_image_fname,"r");
    //Do error handling
    if (decInfo->fptr_output_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_image_fname);

        return e_failure;
    }

    //Move 54 bytes to decode original data
    fseek(decInfo->fptr_output_image,54,SEEK_SET);

    //No failure return e_success
    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char buffer[8];
    char ch ;
    char str[5];
    size_t i;
    for(i = 0;i<strlen(magic_string);i++)//Loop to decode magic string from encoded image
    {
        ch = 0;
        fread(buffer,8,1,decInfo->fptr_output_image);
        decode_byte_to_lsb(buffer,&ch);
        str[i] = ch;
    }
    str[i] = '\0';
   // printf("Decoded magic string is %s\n",str);
    if(strcmp(magic_string,str) == 0)//Condition to compare decoded magic string with original magic string
    {
        printf("Magic string is decoded succesfully\n");
    }
    else
    {
        printf("Magic string is not decoded successfully\n");
        return e_failure;
    }
    return e_success;

}
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    int size = 0;
    fread(buffer,32,1,decInfo->fptr_output_image);
    decode_size_to_lsb(buffer,&size);//To decode size of secret file extn from encoded image
    decInfo->size_secret_file_extn = size;
    //printf("In struct %ld\n",decInfo->size_secret_file_extn);
    //printf("Length of secret file extension is : %d\n",size);
    printf("Length of secret  file extension is decoded successfully\n");
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch = 0;
    char str[5];
    size_t i;
    for(i=0;i<decInfo->size_secret_file_extn;i++)//Loop to decdode extension from encoded image
    {
        ch = 0;
        fread(buffer,8,1,decInfo->fptr_output_image);
        decode_byte_to_lsb(buffer,&ch);
        str[i] = ch;
    }
    str[i] = '\0';
   // printf("%s\n",decInfo->secret_fname);
    strcat(decInfo->secret_fname,str);//Concatinate extension with output file
    //printf("Secret file name is %s\n",decInfo->secret_fname);
    printf("Extension of secret file decoded successfully\n");

    return e_success;

}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    int secret_file_size = 0;
    fread(buffer,32,1,decInfo->fptr_output_image);
    decode_size_to_lsb(buffer,&secret_file_size);//To decode size of secret data
    decInfo-> size_secret_data = secret_file_size;
   // printf("secret file size is %ld\n",decInfo-> size_secret_data);
   printf("Size of secret file is decoded successfully\n");
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    decInfo->fptr_secret = fopen(decInfo->secret_fname,"w");//Now open the output file with extension in write mode
    //Do error handling
    if (decInfo->fptr_secret == NULL)
    {
        printf("Error in opening the file");

        return e_failure;
    }
    char buffer[8];
    char ch;
    size_t i;
    for(i=0;i<decInfo->size_secret_data;i++)//Loop to Decode secret data from encoded image
    {
        ch = 0;
        fread(buffer,8,1,decInfo->fptr_output_image);
        decode_byte_to_lsb(buffer,&ch);
        fwrite(&ch,1,1,decInfo->fptr_secret);
    }
    printf("Secret message is decoded successfully\n");
    return e_success;
}

Status decode_byte_to_lsb(char *image_buffer,char *data)
{
    for(size_t i = 0; i < 8; i++)//Loop to decode lsb of each byte from encoded data of each character by character
    {
        *data = (image_buffer[i] & 1) << i | *data; 
    }
    return e_success;

}
Status decode_size_to_lsb(char *imageBuffer,int *data)
{
    for(size_t i = 0; i < 32; i++)//Loop to decode lsb of each byte to decode integer data
    {
        *data = (imageBuffer[i] & 1) << i | *data; 
    }
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("Decoding Mode\n");
    //Conditions to validate all functions to perform decoding.
    if(open_files_decode(decInfo) != e_success) return e_failure;
    if(decode_magic_string(MAGIC_STRING,decInfo) != e_success) return e_failure;
    if(decode_secret_file_extn_size(decInfo) != e_success) return e_failure;
    if(decode_secret_file_extn(decInfo) != e_success) return e_failure;
    if(decode_secret_file_size(decInfo) != e_success) return e_failure;
    if(decode_secret_file_data(decInfo) != e_success) return e_failure;
    return e_success;
    
}
