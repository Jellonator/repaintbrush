#pragma once
#include "../base.h"
#include "../arg.h"

namespace cli {
    extern const std::string command_init_string;
    void command_init_func(ArgChain& args);
}
