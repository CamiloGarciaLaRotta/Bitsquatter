#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "regexer.h"

// fill an array with the binary representation of a character
void get_binary(int, int, char[]);

// fill an array with the ascii representation of a binary string
void to_character(const char*, int, char[]);

// return the bitstring representation of a string
void get_binary_string(const int, const char*, char[]);

// return the string representation of a binary string
void get_string(const int, const char*, char[]);

// split url into domain name and domain extension
void split_url(const char*, char*, char*);
