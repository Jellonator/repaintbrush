#include <iostream>
#include "base.h"
#include "../core/util.h"
#include "cmd/help.h"
#include "cmd/import.h"
#include "cmd/init.h"
#include "cmd/input.h"

namespace cli {
    // Base help is here instead of cmd/help.cpp since it does not correspond
    // to the `repaintbrush help` command.
    const std::string base_help_string =
R"(Usage:  repaintbrush [--version] <command> ...

RepaintBrush is a tool for managing folders of images, where images are
continually inserted into said folder. Specifically, this tool is useful for
repainting games using emulators that can dump images, e.g. the Dolphin
emulator. Although this tool was made with this use case in mind, I'm sure that
this tool can be used in other situations too.

Options:
    -v, --version      Print the current version

Commands:
    init               Create a new rbrush project
    help               Give help for a command
    input              Add or remove input directories
    import             Import files from input directories

Use `repaintbrush help <command> to get further information about a command.`)";
    void base_help()
    {
        std::cout << base_help_string << std::endl;
    }

    const std::map<std::string, Command> command_definitions = {
        {  "help", {  command_help_func,   command_help_string}},
        {  "init", {  command_init_func,   command_init_string}},
        { "input", { command_input_func,  command_input_string}},
        {"import", {command_import_func, command_import_string}}
    };

    void base(const std::vector<std::string>& args)
    {
        ArgChain argchain(args);
        ArgBlock argblock = argchain.parse(1, true, {
            {"version", false, 'v'}
        });
        if (argblock.has_option("version")) {
            argblock.assert_options({"version"});
            argblock.assert_num_args(0);
            argchain.assert_finished();
            std::cout << "Repaintbrush version " << core::version << std::endl;
        } else {
            if (argblock.size() == 0) {
                argchain.assert_finished();
                base_help();
            } else {
                auto& cmdname = argblock[0];
                if (command_definitions.count(cmdname) != 0) {
                    auto& def = command_definitions.at(cmdname);
                    def.m_func(argchain);
                } else {
                    std::cout << "Unrecognized commmand '" << cmdname
                        << "'" << std::endl;
                }
            }
        }
    }

    void init(const std::vector<std::string>& args)
    {
        try {
            base(args);
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}
