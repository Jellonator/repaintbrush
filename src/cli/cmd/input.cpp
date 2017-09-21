#include "input.h"
#include <iostream>
#include <glibmm.h>
#include "../../core/project.h"

namespace cli {
    const char* command_input_string =
R"(Usage: repaintbrush input add [-f] <directory>
   or: repaintbrush input remove [-f] <directory>
   or: repaintbrush input list [-f]

Manage the input folders of a RepaintBrush project.

Options:
    -f, --force        Force the creation of a project

Commands:
    add                Add a folder as an input source
    remove             Remove a folder from input sources
    list               List all input folders)";

    void command_input_add(ArgChain& args);
    void command_input_remove(ArgChain& args);
    void command_input_list(ArgChain& args);

    void command_input_func(ArgChain& args)
    {
        ArgBlock block = args.parse(1, {});
        block.assert_all_args();
        const std::string& cmd = block[0];
        if (cmd == "add") {
            command_input_add(args);
        } else if (cmd == "remove") {
            command_input_remove(args);
        } else if (cmd == "list") {
            command_input_list(args);
        } else {
            std::cout << "Unrecognized command 'input "
                << cmd << "'" << std::endl;
        }
    }

    void command_input_add(ArgChain& args)
    {
        ArgBlock block = args.parse(1, {
            {"force", false, 'f'}
        });
        block.assert_all_args();
        auto path = core::get_project_directory(Glib::get_current_dir());
        if (!path) {
            std::cout << "Could not find repaintbrush project folder." << std::endl;
        }
        auto force = block.has_option("force");
        core::Project project = core::Project::connect(*path, force);
        auto inputpath = Gio::File::create_for_commandline_arg(block[0]);
        if (project.add_inputfolder(inputpath)) {
            std::cout << "Input folder '" << inputpath->get_path() <<
                "' already exists." << std::endl;
        } else {
            std::cout << "Successfully added input folder." << std::endl;
        }
    }

    void command_input_remove(ArgChain& args)
    {
        ArgBlock block = args.parse(1, {
            {"force", false, 'f'}
        });
        block.assert_all_args();
        auto path = core::get_project_directory(Glib::get_current_dir());
        if (!path) {
            std::cout << "Could not find repaintbrush project folder." << std::endl;
        }
        auto force = block.has_option("force");
        core::Project project = core::Project::connect(*path, force);
        auto inputpath = Gio::File::create_for_commandline_arg(block[0]);
        if (project.remove_inputfolder(inputpath)) {
            std::cout << "Folder '" << inputpath->get_path() <<
                "' does not exist." << std::endl;
        } else {
            std::cout << "Successfully removed input folder." << std::endl;
        }
    }

    void command_input_list(ArgChain& args)
    {
        ArgBlock block = args.parse(0, {
            {"force", false, 'f'}
        });
        block.assert_all_args();
        auto path = core::get_project_directory(Glib::get_current_dir());
        if (!path) {
            std::cout << "Could not find repaintbrush project folder." << std::endl;
        }
        auto force = block.has_option("force");
        core::Project project = core::Project::connect(*path, force);
    }

}
