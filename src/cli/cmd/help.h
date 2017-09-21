#pragma once
#include "../base.h"
#include "../arg.h"

namespace cli {
    extern const std::string command_help_string;
    void command_help_func(ArgChain& args);
}
