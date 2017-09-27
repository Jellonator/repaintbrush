#pragma once
#include "../base.h"
#include "../arg.h"

namespace cli {
    extern const char* command_filter_string;
    void command_filter_func(ArgChain& args);
}
