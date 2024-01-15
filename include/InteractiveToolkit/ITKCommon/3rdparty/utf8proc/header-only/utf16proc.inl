#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>

//
// CUSTOM UTF32 processing (for windows Path Conversions)
// https://en.wikipedia.org/wiki/UTF-16
//

// can read maximum of char16_t[2]
// return = 0, means you need to iterate one character ahead on input...
static size_t utf16proc_iterate(const char16_t *str, size_t strlen, char32_t *codepoint_ref)
{
    if (strlen == 0)
        return 0;

    char16_t _utf16_code_a = str[0];
    if (_utf16_code_a >= 0xD800 && _utf16_code_a <= 0xDBFF)
    {
        // need two chars to read the complete codepoint.
        if (strlen >= 2)
        {
            char16_t _utf16_code_b = str[1];
            if (_utf16_code_b >= 0xDC00 && _utf16_code_b <= 0xDFFF)
            {
                // normal reading, skipping 2 bytes
                *codepoint_ref = (((char32_t)_utf16_code_a - 0xD800) << 10) + ((char32_t)_utf16_code_b - 0xDC00) + 0x10000;
                return 2;
            }
        }
    }
    else
    {
        // normal case, the code is the same UTF16 and UTF32
        *codepoint_ref = (char32_t)_utf16_code_a;
        return 1;
    }

    // unknown character
    *codepoint_ref = 0xFFFD;
    return 1;
}

// can generate maximum of char16_t[2]
static size_t utf16proc_encode_char(char32_t codepoint, char16_t *dst)
{
    if (codepoint <= 0xFFFF)
    {
        // single character
        if (codepoint < 0xD800 || codepoint > 0xDFFF)
        {
            // the character is the same on both sides
            dst[0] = (char16_t)codepoint;
            return 1;
        }
    }
    else if (codepoint <= 0x10FFFF)
    { // codepoint > 0xFFFF &&
        // range 0xFFFF - 0x10FFFF
        codepoint -= 0x10000;
        dst[0] = (char16_t)((codepoint >> 10) + 0xD800);
        dst[1] = (char16_t)((codepoint & 0x3FF) + 0xDC00);
        return 2;
    }

    // unknown character
    dst[0] = 0xFFFD;
    return 1;
}
