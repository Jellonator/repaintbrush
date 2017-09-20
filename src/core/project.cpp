#include "project.h"
#include <iostream>


namespace core {
    void do_simple_statement(sqlite3* db, const std::string& stmt_str)
    {
        sqlite3_stmt* statement;
        int ok = sqlite3_prepare_v2(db,
            stmt_str.c_str(), stmt_str.size(), &statement, nullptr);
        if (ok != SQLITE_OK) {
            auto err = std::runtime_error(sqlite3_errmsg(db));
            sqlite3_finalize(statement);
            throw err;
        }
        ok = sqlite3_step(statement);
        if (ok != SQLITE_DONE) {
            auto err = std::runtime_error(sqlite3_errmsg(db));
            sqlite3_finalize(statement);
            throw err;
        }
        ok = sqlite3_finalize(statement);
        if (ok != SQLITE_OK) {
            auto err = std::runtime_error(sqlite3_errmsg(db));
            throw err;
        }
    }

    Project::Project(Glib::RefPtr<Gio::File>& path, bool force, int flags)
    : m_lock(std::make_shared<ProjectFolderLock>(path, force))
    , m_path(path)
    , m_database(path->get_child(rbrush_folder_name)->get_child(rbrush_db_name)
        ->get_path(), flags) {}

    Project Project::connect(Glib::RefPtr<Gio::File>& path, bool force)
    {
        return Project(path, force, SQLITE_OPEN_READWRITE);
    }

    Project Project::create(Glib::RefPtr<Gio::File>& path, bool force)
    {
        Project project(path, force, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
        {
            auto stmt = project.get_database().prepare(
                "CREATE TABLE IF NOT EXISTS images("\
                    "id INT PRIMARY KEY NOT NULL,"\
                    "name NTEXT NOT NULL,"\
                    "alias NTEXT"\
                ")"
            );
            stmt.finish();
        }
        {
            auto stmt = project.get_database().prepare(
                "CREATE TABLE IF NOT EXISTS inputfolders("\
                    "name TEXT NOT NULL"\
                ")"
            );
            stmt.finish();
        }
        return project;
    }

    database::Database& Project::get_database()
    {
        return this->m_database;
    }
}
