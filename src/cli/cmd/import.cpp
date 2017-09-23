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
        bool force = block.has_option("force");
        fs::path import_folder = ".";
        if (block.has_option("input")) {
            import_folder = block.get_option("input");
        }
        auto project = core::get_project(force);
        if (!project) return;
    }
}
