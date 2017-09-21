#include "init.h"
#include <iostream>
#include "../../core/project.h"

namespace cli {
    const char* command_init_string =
R"(Usage: repaintbrush init [-f] <directory>

Create a new Repaintbrush project.

Options:
    -f, --force        Force the creation of a project)";

    void command_init_func(ArgChain& args)
    {
        ArgBlock block = args.parse(1, {
            {"force", false, 'f'}
        });
        // block.assert_all_args();
        std::string targetpath = ".";
        if (block.size() >= 1) {
            targetpath = block[0];
        }
        auto path = Gio::File::create_for_commandline_arg(targetpath);
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
                // If folder already exists, it's totally fine
                std::cout << "Project already exists." << std::endl;
                return;
            }
        }
        bool do_force = block.get_option("force");
        {
            auto versionpath = path_project->get_child(core::rbrush_version_name);
            auto versionwrite = versionpath->replace();
            versionwrite->write(core::version);
            auto project = core::Project::create(path, do_force);
        }
        std::cout << "Successfully created project." << std::endl;
    }
}
