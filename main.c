#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>
OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{

    // Step 1 : Check the argc >= 4 true - > step 2
    // Step 2 : Call the check_operation_type(argv[1]) == e_encode )) true - > step 3
    // Step 3 : Declare structure variable EncodeInfo enc_info
    // Step 4 : Call the read_and_validate_encode_args(argv,&enc_info)== e_success)
    //  true -> Step 5 , false - > terminate the program
    // Step 5 : Call the do_encoding (&encInfo);

    if(argc >= 4)//Argument count must be validated that is greater than 3 or 4.
    {
        if(check_operation_type(argv[1]) == e_encode)//Condition to check for encoding 
        {
            EncodeInfo enc_info;
            if(read_and_validate_encode_args(argv,&enc_info) == e_success)//To read and validate remaining arguments
            {
                if(do_encoding(&enc_info) == e_success)//Condition to start encoding
                {
                    printf("Encoding is successful\n");
                }
                else
                {
                    printf("Encoding is not successful\n");
                }
                
            }
            else
            {
                return 0;
            }

        }
        else if(check_operation_type(argv[1]) == e_unsupported)
        {
            printf("Error : Argument 1 must be -e or -d\n");
            return 0;
        }
    }
    if(argc == 3 && strcmp(argv[1],"-e") == 0)
    {
        printf("Error : Arguments must be minimum 4 for encode\n");
        return 0;
    }
    if(argc >= 3)
    {
        if(check_operation_type(argv[1]) == e_decode)//Condition to check for decoding
        {
            DecodeInfo dec_info;
            if(read_and_validate_decode_args(argv,&dec_info) == e_success)//To read and validate remaining arguments
            {
                if(do_decoding(&dec_info) == e_success)//Condition to start decoding
                {
                    printf("Decoding is successful\n");
                }
                else
                {
                    printf("Decoding is not successful provide a valid encoded file\n");
                }
                
            }
            else
            {
                return 0;
            }
        }
        else if(check_operation_type(argv[1]) == e_unsupported)
        {
            printf("Error : Argument 1 must be -e or -d\n");
            return 0;
        }
    }
    else
    {
        printf("Error : Argument count is not correct\n");
        return 0;
    }

}

OperationType check_operation_type(char *symbol)
{
    // Step 1 : Check whether the symbol is -e or not true - > return e_encode false -> Step 2
    // Step 2 : Check whether the symbol is -d or not true - > return e_decode false -> return e_unsupported

    if(strcmp(symbol,"-e") == 0)//Condition to check for encoding or not
    {
        return e_encode;
    }
    else if(strcmp(symbol,"-d") == 0)//Condition to check for decoding or not
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
