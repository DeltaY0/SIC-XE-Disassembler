#pragma once
#include "defines.h"

// error code macros
#define YLIB_ERR_OK                  0
#define YLIB_ERR_UNKNOWN             1
#define YLIB_ERR_INVALID_ARGUMENT    2
#define YLIB_ERR_OUT_OF_MEMORY       3
#define YLIB_ERR_INDEX_OUT_OF_BOUNDS 4

namespace ylib {

class Error
{
    private:
    const char *message;
    const u32 code;

    public:
    Error(const char *message)
        : message{message}, code{YLIB_ERR_UNKNOWN}
    {
    }

    Error(const u32 code, const char *message)
        : code{code}, message{message}
    {
    }

    const char *what() const { return message; }
    const u32 errcode() const { return code; }
};

} // namespace ylib