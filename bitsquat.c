#include "bitsquat.h"

// fill an array with the binary representation of a character
void to_binary(int ch, int position, char arr[])
{
        int remainder;
        while(ch > 0)
        {
                remainder = ch % 2;
                ch /= 2;
                arr[position--] = remainder + '0'; // int -> char
        }
}

// fill an array with the ascii representation of a binary string
void to_character(const char* binary_str, int position, char arr[])
{
        // printf("to_character():\t%s\n", binary_str);
        arr[position] = strtol(binary_str, 0, 2);
        // printf("%s\n", arr[position]);
}

// return the binary representation of a string
void get_binary_string(const int str_length, const char* str, char binary_str[])
{
        int i;    // string index
        int byte; // character index in binary string
        for(i=0, byte=7; i<str_length; i++, byte+=8)
        {
                to_binary(str[i], byte, binary_str);
        }
}

// return the string representation of a binary string
void get_string(const int binary_str_length, const char* binary_str, char str[])
{
        // printf("to_string():\t%s\n\n",binary_str);

        int byte,i;
        char byte_buffer[8+1];
        for (i=0, byte=0; byte<binary_str_length; i++, byte+=8)
        {
                memcpy(byte_buffer, &binary_str[byte], 8);
                byte_buffer[8] = '\0';
                // printf("Buffer:\t%s\n", byte_buffer);
                to_character(byte_buffer,i, str);
        }

        // printf("Bitsquatted Domain Name:\t%s\n\n", str);
}

// split url into domain name and domain extension
void split_url(const char* url, char* dom, char* ext)
{
        int start, end;

        match_regex("^[[:alnum:]]*(.)[[:alnum:].]*", url, &start, &end);

        sprintf(dom, "%.*s", (start), url);
        sprintf(ext, "%.*s", (int)strlen(url)-end, url+start+1);
}
