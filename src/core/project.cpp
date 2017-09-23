#include "project.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace core {
    Project::Project(const fs::path& path, bool force, int flags)
    : m_lock(path / rbrush_folder_name, force)
    , m_path(path)
    , m_database(path / rbrush_folder_name / rbrush_db_name, flags) {}

    Project Project::connect(const fs::path& path, bool force)
    {
        return Project(path, force, SQLITE_OPEN_READWRITE);
    }

    Project Project::create(const fs::path& path, bool force)
    {
        Project project(path, force, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
        {
            auto stmt = project.get_database().prepare(
                R"(CREATE TABLE IF NOT EXISTS images(
                    id INT PRIMARY KEY NOT NULL,
                    name NTEXT NOT NULL,
                    alias NTEXT
                ))"
            );
            stmt.finish();
        }
        {
            auto stmt = project.get_database().prepare(
                R"(CREATE TABLE IF NOT EXISTS inputfolders(
                    name TEXT NOT NULL UNIQUE
                ))"
            );
            stmt.finish();
        }
        return project;
    }

    database::Database& Project::get_database()
    {
        return this->m_database;
    }

    bool Project::add_inputfolder(const fs::path& path)
    {
        if (!fs::is_directory(path)) {
            std::stringstream s;
            s << "Error: '" << path.string() << "' is not a valid directory!";
            throw std::runtime_error(s.str());
        }
        auto& db = this->get_database();
        auto stmt = db.prepare(
            R"(INSERT INTO inputfolders(name)
            VALUES (?))"
        );
        stmt.bind(1, fs::canonical(path).string());
        try {
            stmt.finish();
        } catch (std::exception& e) {
            if (std::string(e.what()) ==
                    "UNIQUE constraint failed: inputfolders.name") {
                return true;
            } else {
                throw e;
            }
        }
        return false;
    }

    bool Project::remove_inputfolder(const fs::path& path)
    {
        auto& db = this->get_database();
        auto stmt = db.prepare(
            R"(DELETE FROM inputfolders
            WHERE name = ?)"
        );
        stmt.bind(1, fs::canonical(path).string());
        stmt.finish();
        // std::cout << sqlite3_changes(db.get_ptr())
        // << " rows were deleted." << std::endl;
        return sqlite3_changes(db.get_ptr()) == 0;
    }

    std::vector<fs::path> Project::list_input_folders()
    {
        std::vector<fs::path> ret;
        auto& db = this->get_database();
        auto stmt = db.prepare(
            R"(SELECT name FROM inputfolders)"
        );
        while (!stmt.done()) {
            if (stmt.step() == SQLITE_ROW) {
                ret.push_back(stmt.column_value<std::string>(1));
            }
        }
        return ret;
    }

    boost::optional<Project> get_project(bool force)
    {
        auto path = core::get_project_directory(fs::current_path());
        if (!path) {
            std::cout << "Could not find repaintbrush project folder." << std::endl;
            return {};
        }
        core::Project project = core::Project::connect(*path, force);
        return project;
    }
}
