#pragma once

#include "../core/defines.h"
#include <algorithm>

// file for handling base-n operations

namespace base {

    u64 hextobin(string hexstr) {
        u64 val = std::stoi(hexstr, nullptr, 16);
        return val;
    }
};