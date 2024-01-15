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
static size_t utf16proc_iterate(const uint16_t *utf16_str, size_t strlen, uint32_t *codepoint_ref)
{
    if (strlen == 0)
        return 0;

    uint32_t _utf16_code_a = (uint32_t)utf16_str[0];
    // high surrogates (0xD800–0xDBFF)
    if (_utf16_code_a >= 0xD800 && _utf16_code_a <= 0xDBFF)
    {
        // need two chars to read the complete codepoint.
        if (strlen >= 2)
        {
            uint32_t _utf16_code_b = (uint32_t)utf16_str[1];
            // low surrogates (0xDC00–0xDFFF)
            if (_utf16_code_b >= 0xDC00 && _utf16_code_b <= 0xDFFF)
            {
                // normal reading, skipping 2 elements
                *codepoint_ref = ((_utf16_code_a - 0xD800) << 10) + (_utf16_code_b - 0xDC00) + 0x10000;
                return 2;
            }
        }
    }
    else
    {
        // normal case, the code is the same UTF16 and UTF32
        *codepoint_ref = _utf16_code_a;
        return 1;
    }

    // unknown character
    *codepoint_ref = 0xFFFD;
    return 1;
}

// can generate maximum of uint16_t[2]
static size_t utf16proc_encode_char(uint32_t codepoint, uint16_t *dst)
{
    if (codepoint <= 0xFFFF)
    {
        // single character
        if (codepoint <= 0xD7FF || codepoint >= 0xE000)
        {
            // the character is the same on both sides
            dst[0] = (uint16_t)codepoint;
            return 1;
        }
    }
    else if (codepoint <= 0x10FFFF) // codepoint > 0xFFFF &&
    {
        // range 0xFFFF - 0x10FFFF

        // Subtract 0x10000 from the code point
        codepoint -= 0x10000;

        // For the high surrogate, shift right by 10 (divide by 0x400), then add 0xD800
        dst[0] = (uint16_t)((codepoint >> 10) + 0xD800);

        // For the low surrogate, take the low 10 bits (remainder of dividing by 0x400 or bitwise-and 0x3FF), then add 0xDC00
        dst[1] = (uint16_t)((codepoint & 0x3FF) + 0xDC00);
        return 2;
    }

    // unknown character
    dst[0] = (uint16_t)0xFFFD;
    return 1;
}
