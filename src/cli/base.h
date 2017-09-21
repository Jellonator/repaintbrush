#pragma once
#include <vector>
#include <string>
#include <functional>
#include "arg.h"

namespace cli {
    struct Command {
        std::function<void(ArgChain&)> m_func;
        std::string m_help;
    };

    extern const std::map<std::string, Command> command_definitions;

    void base_help();
    void init(const std::vector<std::string>& args);
}
