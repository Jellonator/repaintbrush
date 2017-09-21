#include "core/util.h"
#include "core/project.h"
#include "cli/base.h"
#include <gtkmm/application.h>
#include <glibmm.h>
#include <gtkmm.h>
#include <iostream>

int main (int argc, char *argv[])
{
    Glib::init();
    Gio::init();

    std::vector<std::string> args;
    // Skip first argument (executable path)
    for (int i = 1; i < argc; ++i) {
        // std::cout << argv[i] << std::endl;
        args.push_back(argv[i]);
    }

    cli::init(args);

    // auto path = core::get_project_directory(Glib::get_current_dir());
    // if (path) {
    //     try {
    //         core::Project project = core::Project::create(*path);
    //         std::cout << "Success" << std::endl;
    //     } catch (std::exception& e) {
    //         std::cout << "ERROR: " << e.what() << std::endl;
    //     }
    // } else {
    //     std::cout << "Could not find repaintbrush project folder." << std::endl;
    // }
}
