#ifndef BASE_H
#define BASE_H

#include "../core/defines.h"
#include <algorithm>

// for handling base-n operations
namespace base {
    template<typename T>
    inline T hextobin(string hexstr) {
        T val = (T) std::stoi(hexstr, nullptr, 16);
        return val;
    }

    inline string bintohex(i32 val, i32 width) {
        stringstream ss;

        ss << std::uppercase << std::hex;
        ss << std::setfill('0') << std::setw(width);
        ss << val;

        return ss.str();
    }

    inline bool checkbit(i32 val, i32 pos) {
        return (val && (1 << pos)) != 0;
    }
};

#endif // BASE_H