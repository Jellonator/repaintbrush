#include "filter.h"
#include <iostream>
#include "../../core/project.h"
#include "../../core/filter.h"

namespace cli {
    const char* command_filter_string =
R"(Usage: repaintbrush filter list [-f] [-n]
   or: repaintbrush filter add [-f] <input/output> <name> [arg]
   or: repaintbrush filter remove [-f] <id>

Manage filters in a repaintbrush project. A filter determines if a file should
or should not be copied. In the case of an input filter, a filter determines if
a given file should or should not be included. In the case of an output filter,
a filter determines if a given file should be exported.

Subcommands:
    list           List all filters used for this project.
    add            Add a filter to this project. A filter can either be an 
                   input filter or an output filter, which is determined by the
                   first argument.
    remove         Remove a filter by its ID.

Options:
    -f, --force    Force opening of a project.
    -t, --types    List filter types.)";
    
    void command_filter_add(ArgChain& args);
    void command_filter_list(ArgChain& args);
    void command_filter_remove(ArgChain& args);
    void command_filter_func(ArgChain& args)
    {
        ArgBlock block = args.parse(1, true, {});
        block.assert_all_args();
        const std::string& cmd = block[0];
        if (cmd == "add") {
            command_filter_add(args);
        } else if (cmd == "remove") {
            command_filter_remove(args);
        } else if (cmd == "list") {
            command_filter_list(args);
        } else {
            std::cout << "Unrecognized command 'filter "
                      << cmd << "'" << std::endl;
        }
    }

    void command_filter_add(ArgChain& args)
    {

    }

    void command_filter_list(ArgChain& args)
    {

    }

    void command_filter_remove(ArgChain& args)
    {

    }
}
