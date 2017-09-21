#pragma once
#include "../base.h"
#include "../arg.h"

namespace cli {
    extern const char* command_help_string;
    void command_help_func(ArgChain& args);
}
