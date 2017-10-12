#include "filter.h"
#include <iostream>
#include <iomanip>
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
    -n, --names    List filter types.)";
    
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
        ArgBlock block = args.parse(3, false, {
            {"force", false, 'f'}
        });
        block.assert_least_num_args(2);
        args.assert_finished();
        bool force = block.has_option("force");
        auto project = core::get_project(force);
        if (!project) return;
        
        core::Project::filter_t filter_type;
        if (block.match_arg(0, "input")) {
            filter_type = core::Project::FILTER_INPUT;
        } else if (block.match_arg(0, "output")) {
            filter_type = core::Project::FILTER_OUTPUT;
        } else {
            std::cout << "Unknown filter type '" << block[0] 
                      << "'" << std::endl;
            return;
        }
        const std::string& filter_name = block[1];
        std::string filter_arg = "";
        if (block.size() >= 3) {
            filter_arg = block[2];
        }
        
        core::FilterFactory factory;
        core::Filter filter = factory.create(filter_name, filter_arg);
        if (!filter) {
            if (!filter.is_name_valid()) {
                std::cout << "Filter name '" << filter_name 
                          << "' is not valid." << std::endl;
            } else {
                std::cout << "Invalid argument '" << filter_arg 
                          << "' for filter type '" << filter_name << "'" 
                          << std::endl;
            }
        }
        project->add_filter(filter_type, filter);
        std::cout << "Successfully added filter" << std::endl;
    }

    int get_int_width(int i)
    {
        int ret = i > 0 ? 0 : 1;
        do {
            ret ++;
            i /= 10;
        } while (i > 0);
        return ret;
    }

    const std::string TYPE_INPUT = "input";
    const std::string TYPE_OUTPUT = "output";
    const std::string TYPE_NONE = "";
    const std::string& get_ftype_name(core::Project::filter_t type)
    {
        switch (type) {
        case core::Project::FILTER_INPUT:
            return TYPE_INPUT;
            break;
        case core::Project::FILTER_OUTPUT:
            return TYPE_OUTPUT;
            break;
        }
        return TYPE_NONE;
    }

    const std::string LIST_TYPES =
R"(Filter Types:
    mipmap         Filter all files that end with _mipN where N is a number.
    type <type>    Filer all files with the file extension <type>.
)";
    void command_filter_list(ArgChain& args)
    {
        ArgBlock block = args.parse(0, false, {
            {"force", false, 'f'},
            {"names", false, 'n'}
        });
        args.assert_finished();
        if (block.has_option("types")) {
            std::cout << LIST_TYPES << std::endl;
            return;
        }

        bool force = block.has_option("force");
        auto project = core::get_project(force);
        if (!project) return;

        auto filter_list = project->get_filters();
        if (filter_list.empty()) {
            std::cout << "No filters are defined." << std::endl;
            return;
        }
        int largest_id = filter_list.back().id;
        auto idw = get_int_width(largest_id);
        if (idw < 2) {
            idw = 2;
        }
        std::cout << std::setw(idw) << std::right << "ID" << " "
                  << std::setw(6) << std::right << "TYPE" << " "
                  << std::setw(8) << std::right << "NAME" << " "
                  << "ARGUMENT" << std::endl;
        for (auto& filter : filter_list) {
            const std::string& ftype = get_ftype_name(filter.type);
            const std::string& fname = filter.filter.get_name();
            std::string arg = filter.filter.serialize();
            std::cout << std::setw(idw) << std::right << filter.id << " "
                      << std::setw(6) << std::right << ftype << " "
                      << std::setw(8) << std::right << fname << " "
                      << arg << std::endl;
        }
    }

    void command_filter_remove(ArgChain& args)
    {
        ArgBlock block = args.parse(1, false, {
            {"force", false, 'f'}
        });
        block.assert_all_args();
        args.assert_finished();

        bool force = block.has_option("force");
        auto project = core::get_project(force);
        if (!project) return;

        int id;
        try {
            size_t pos;
            const std::string& id_str = block[0];
            id = std::stoi(id_str, &pos, 10);
            if (pos != id_str.size()) {
                // just need to throw something. This is fine.
                throw id;
            }
        } catch (...) {
            std::cout << "ID is not a valid integer." << std::endl;
            return;
        }

        if (project->remove_filter(id)) {
            std::cout << "No filter with ID " << id << std::endl;
            return;
        }
        std::cout << "Successfully removed filter." << std::endl;
    }
}
