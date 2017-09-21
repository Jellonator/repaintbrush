#pragma once
#include "../base.h"
#include "../arg.h"

namespace cli {
    extern const char* command_input_string;
    void command_input_func(ArgChain& args);
}
