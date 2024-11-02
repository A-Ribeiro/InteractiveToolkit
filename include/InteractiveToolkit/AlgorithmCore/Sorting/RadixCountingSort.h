#pragma once

#include "../../common.h"
#include "RadixCountingSort32.h"
#include "RadixCountingSort64.h"

namespace AlgorithmCore
{

    namespace Sorting
    {
        using RadixCountingSortu32 = RadixCountingSort<uint32_t>;
        using RadixCountingSorti32 = RadixCountingSort<int32_t>;

        using RadixCountingSortu64 = RadixCountingSort<uint64_t>;
        using RadixCountingSorti64 = RadixCountingSort<int64_t>;

    }
}
