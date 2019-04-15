/**
 * MIT License
 * 
 * Copyright (c) 2019 Ilwoong Jeong, https://github.com/ilwoong
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "hex.h"

#include <stdio.h>
#include <string.h>
#include <regex.h>

static uint8_t ctoh(char in)
{
    if (in >= '0' && in <= '9') {
        return in - '0';
    }

    if (in >= 'a' && in <= 'f') {
        return in - 'a' + 10;
    }

    if (in >= 'A' && in <= 'F') {
        return in - 'A' + 10;
    }

    return 0;
}

static int isValidHexString(const char* str)
{
    if (str == NULL) {
        return 0;
    }

    regex_t regex;
    regcomp(&regex, "^[A-Fa-f0-9 \t]*$", 0);
    return regexec(&regex, str, 0, NULL, 0) == 0;
}

//! @remark hex string should have length of even number
//! @param [out] out output array
//! @param [in] str input string
//! @param [in] strlength length of str
//! @return 0 if str is not valid hex string
//! @return length of the converted hex array
size_t strToHex(uint8_t* hex, const char* str, size_t strlength)
{
    if (hex == NULL || isValidHexString(str) == 0) {
        return 0;
    }    

    size_t idx = 0;
    for (size_t i = 0; i < strlength; idx += 1) {
        while (str[i] == ' ' || str[i] == '\t') {
            i += 1;
        }
        hex[idx] = ctoh(str[i]) * 16;
        i += 1;

        while (str[i] == ' ' || str[i] == '\t') {
            i += 1;
        }
        hex[idx] += ctoh(str[i]);
        i += 1;
    }

    return idx;
}

//! @param [out] out output array
//! @param [in] str input hex array
//! @param [in] hexlength length of hex
//! @return 0 if str is not valid hex string
//! @return length of the converted string array
size_t hexToStr(char* str, const uint8_t* hex, size_t hexlength)
{
    if (str == NULL || hex == NULL) {
        return 0;
    }

    for (size_t i = 0; i < hexlength; ++i) {
        sprintf(str, "%02x", hex[i]);
        str += 2;
    }
    str[2 * hexlength] = '\0';

    return 2 * hexlength;
}

void printHex(const uint8_t* data, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        printf("%02x", data[i]);

        if ( ((i+1) & 0xf) == 0) {
            printf("\n");
        }
    }
    printf("\n");
}