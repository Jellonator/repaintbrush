#include <iostream>
#include <functional>
#include "base.h"
#include "arg.h"
#include "../core/project.h"

namespace cli {
    struct Command {
        std::function<void(ArgChain&)> m_func;
        std::string m_help;
    };
    extern const std::map<std::string, Command> command_definitions;

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

Use `repaintbrush help <command> to get further information about a command.`)";
    void base_help()
    {
        std::cout << base_help_string << '\n' << std::endl;
    }

    const std::string command_help_string =
R"(Usage: repaintbrush help <command>

Print help text about the given command.)";
    void command_help(ArgChain& args)
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

    const std::string command_init_string =
R"(Usage: repaintbrush init [-f] <directory>

Create a new Repaintbrush project.

Options:
    -f, --force        Force the creation of a project)";
    void command_init(ArgChain& args)
    {
        ArgBlock block = args.parse(1, {
            {"force", false, 'f'}
        });
        block.assert_all_args();
        auto path = Gio::File::create_for_commandline_arg(block[0]);
        auto path_project = path->get_child(core::rbrush_folder_name);
        try {
            if (!path_project->make_directory_with_parents()) {
                std::cout << "Could not create project." << std::endl;
                return;
            }
        } catch (Gio::Error& e) {
            if (e.code() != Gio::Error::Code::EXISTS) {
                throw e;
            } else {
                std::cout << "Project already exists." << std::endl;
                return;
            }
        }
        bool do_force = block.get_option("force");
        {
            auto project = core::Project::create(path, do_force);
        }
        std::cout << "Successfully created project." << std::endl;
    }

    const std::map<std::string, Command> command_definitions = {
        {"help", {command_help, command_help_string}},
        {"init", {command_init, command_init_string}}
    };

    void base(const std::vector<std::string>& args)
    {
        ArgChain argchain(args);
        ArgBlock argblock = argchain.parse(1, {
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
