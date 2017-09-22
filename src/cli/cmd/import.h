#pragma once
#include "../base.h"
#include "../arg.h"

namespace cli {
    extern const char* command_import_string;
    void command_import_func(ArgChain& args);
}
