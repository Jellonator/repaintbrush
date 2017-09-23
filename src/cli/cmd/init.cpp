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
        ArgBlock block = args.parse(1, false, {
            {"force", false, 'f'}
        });
        args.assert_finished();
        // block.assert_all_args();
        fs::path path_base = ".";
        if (block.size() >= 1) {
            path_base = block[0];
        }
        auto path_project = path_base / core::rbrush_folder_name;
        // Create project folder
        if (fs::exists(path_project)) {
            std::cout << "Could not create project; project already exists."
                      << std::endl;
            return;
        }
        fs::create_directories(path_project);
        // Create version file
        fs::ofstream file_version(path_project / core::rbrush_version_name);
        file_version << core::version;
        file_version.close();
        // Create project
        bool do_force = block.has_option("force");
        auto project = core::Project::create(path_base, do_force);
        std::cout << "Successfully created project." << std::endl;
    }
}
