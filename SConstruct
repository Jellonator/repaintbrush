import os

env = Environment()
env.MergeFlags(['!pkg-config gtkmm-3.0 glibmm-2.4 giomm-2.4 sqlite3 --cflags --libs', '-Wall'])
env.Append(CXXFLAGS='-std=c++14')
env['ENV']['TERM'] = os.environ['TERM']

program_source = [
    "src/main.cpp",
    "src/core/project.cpp",
    "src/core/filter.cpp",
    "src/core/util.cpp",
    "src/core/db/database.cpp",
    "src/core/db/statement.cpp",
    "src/cli/arg.cpp",
    "src/cli/base.cpp",
    "src/cli/cmd/help.cpp",
    "src/cli/cmd/import.cpp",
    "src/cli/cmd/init.cpp",
    "src/cli/cmd/input.cpp",
    # "src/gui/base.cpp",
]
program = env.Program(target='build/repaintbrush', source=program_source)
