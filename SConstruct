import os

env = Environment()
env.MergeFlags([
    '!pkg-config sqlite3 --cflags --libs',
    '!wx-config --cxxflags --libs',
    '-fPIC', '-Wall', '-Wextra', '-Wpedantic', '-lboost_system', '-lboost_filesystem'])
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
    "src/cli/cmd/filter.cpp",
    "src/cli/cmd/export.cpp",
    "src/gui/base.cpp",
    "src/gui/workspace.cpp",
]
program = env.Program(target='build/repaintbrush', source=program_source)
