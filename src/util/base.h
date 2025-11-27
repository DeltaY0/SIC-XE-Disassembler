#pragma once

#include "../core/defines.h"
#include <algorithm>

// for handling base-n operations
namespace base {

    u64 hextobin(string hexstr) {
        u64 val = std::stoi(hexstr, nullptr, 16);
        return val;
    }

    string bintohex(i32 val, i32 width) {
        stringstream ss;

        ss << std::uppercase << std::hex;
        ss << std::setfill('0') << std::setw(width);
        ss << val;

        return ss.str();
    }

    bool checkbit(i32 val, i32 pos) {
        return (val && (1 << pos)) != 0;
    }
};