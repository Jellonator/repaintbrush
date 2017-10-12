#include "export.h"
#include <iostream>
#include "../../core/project.h"

namespace cli {
    const char* command_export_string =
R"(Usage repaintbrush export [-f] folder

Export all images into a folder.

Options:
    -f, --force    Force opening of a project.)";

    void command_export_func(ArgChain& args)
    {
        ArgBlock block = args.parse(1, true, {
            {"force", false, 'f'}
        });
        block.assert_all_args();
        args.assert_finished();

        bool force = block.has_option("force");
        auto project = core::get_project(force);
        if (!project) return;

        // std::cout << fs::weakly_canonical(block[0]) << std::endl;
        // std::cout << fs::canonical(block[0]) << std::endl;
        fs::path exportpath = core::resolve_path(block[0]);
        std::cout << fs::path(block[0]).is_absolute() << ", " << exportpath << std::endl;
        std::cout << project->get_path() << std::endl;
        if (!force && core::is_path_within_path(exportpath, project->get_path())) {
            std::cout << "Error: export path " << exportpath
                      << " is inside project folder." << std::endl
                      << "Use --force to override this warning." << std::endl;
            std::cout << project->get_path() << std::endl;
            return;
        }
        fs::create_directories(exportpath);
        auto result = project->export_to_folder(exportpath);
        std::cout << "Successfully exported " << result.files
                  << " files." << std::endl;
        std::cout << "Filtered out " << result.filtered
                  << " results." << std::endl;
    }
}
