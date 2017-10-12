#include "import.h"
#include <iostream>
#include "../../core/project.h"

namespace cli {
    const char* command_import_string =
R"(Usage: repaintbrush import [-i input] [-f] <target>

Import input images into the target folder.

Options:
    -f, --force            Force opening of the project
    -i, --input <input>    Only import from the given input folder)";

    void command_import_func(ArgChain& args)
    {
        ArgBlock block = args.parse(1, false, {
            {"force", false, 'f'},
            {"input", true, 'i'}
        });
        args.assert_finished();
        // get import folder
        boost::optional<fs::path> import_folder;
        if (block.has_option("input")) {
            import_folder = block.get_option("input");
        }
        // get export folder
        fs::path export_folder = "";
        if (block.size() > 0) {
            export_folder = block[0];
        }
        // get project.
        bool force = block.has_option("force");
        auto project = core::get_project(force);
        if (!project) return;

        // copy files
        auto result = project->import(export_folder, import_folder);

        // Report information back to user.
        if (result.folders == 0) {
            std::cout << "No such import folder " << *import_folder
                      << std::endl;
        } else if (result.files == 0) {
            std::cout << "No new files to import." << std::endl;
            std::cout << "Filtered out " << result.filtered
                      << " results." << std::endl;
        } else {
            std::cout << "Successfully imported " << result.files << " files"
                      << std::endl;
            std::cout << "Filtered out " << result.filtered
                      << " results." << std::endl;
        }
    }
}
