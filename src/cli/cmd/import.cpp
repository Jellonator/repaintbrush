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
        // TODO: Only copy files that have not already been imported
        bool force = block.has_option("force");
        auto project = core::get_project(force);
        if (!project) return;
        // make sure export folder exists
        export_folder = fs::current_path() / export_folder;
        fs::create_directories(export_folder);
        // copy files
        int folder_count = 0;
        int file_count = 0;
        for (const fs::path& folder : project->list_input_folders()) {
            if (!import_folder || fs::equivalent(*import_folder, folder)) {
                ++folder_count;
                auto fileiter = fs::recursive_directory_iterator(folder);
                for (const fs::path& file : fileiter) {
                    fs::path outfile = export_folder/file.filename();
                    if (fs::is_regular_file(file) && !fs::exists(outfile)
                    && !project->has_file(outfile)) {
                        fs::copy(file, outfile);
                        project->register_file(outfile);
                        ++file_count;
                    }
                }
            }
        }
        // Report information back to user.
        if (folder_count == 0) {
            std::cout << "No such import folder " << *import_folder
                      << std::endl;
        } else if (file_count == 0) {
            std::cout << "No new files to import." << std::endl;
        } else {
            std::cout << "Successfully imported " << file_count << " files"
                      << std::endl;
        }
    }
}
