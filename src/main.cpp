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
        args.push_back(argv[i]);
    }
    cli::init(args);
    std::cout << std::endl;
}
