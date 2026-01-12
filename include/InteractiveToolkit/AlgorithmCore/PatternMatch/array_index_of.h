#pragma once

#include "../../common.h"

namespace AlgorithmCore
{

    namespace PatternMatch
    {

        static ITK_INLINE int array_index_of(const uint8_t *input, int start_input, int input_size, const uint8_t *pattern, int pattern_size)
        {
            int test_limit = input_size - pattern_size;
            for (int i = start_input; i <= test_limit; i++)
            {
                if (input[i] == pattern[0] &&
                    input[i + pattern_size - 1] == pattern[pattern_size - 1] &&
                    memcmp(&input[i], pattern, pattern_size) == 0)
                    return i;
            }
            return input_size;
        }

    }
}
