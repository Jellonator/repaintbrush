#include "core/util.h"
#include "cli/base.h"
#include <iostream>

int main (int argc, char *argv[])
{
    std::locale("");
    boost::filesystem::path::codecvt();
    // Glib::init();
    // Gio::init();
    std::vector<std::string> args;
    // Skip first argument (executable path)
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    cli::init(args);
    std::cout << std::endl;
}
