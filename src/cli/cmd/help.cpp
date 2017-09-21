#include "help.h"
#include <iostream>

namespace cli {
    const char* command_help_string =
R"(Usage: repaintbrush help <command>

Print help text about the given command.)";
    void command_help_func(ArgChain& args)
    {
        ArgBlock block = args.parse(1, {});
        if (block.size() == 0) {
            base_help();
        } else {
            auto& cmdname = block[0];
            if (command_definitions.count(cmdname) != 0) {
                auto& def = command_definitions.at(cmdname);
                std::cout << def.m_help << '\n' << std::endl;
            } else {
                std::cout << "No such command '" << cmdname << "'" << std::endl;
            }
        }
    }
}
