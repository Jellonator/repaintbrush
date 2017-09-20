#pragma once
#include <memory>
#include <sqlite3.h>
#include <giomm/file.h>
#include "util.h"
#include "db/database.h"

namespace core {
    class Project {
        std::shared_ptr<ProjectFolderLock> m_lock;
        Glib::RefPtr<Gio::File> m_path;
        database::Database m_database;
        // Project(Glib::RefPtr<Gio::File>& path, bool force);
        // Project(Glib::RefPtr<Gio::File>& path, sqlite3* database, bool force);
        Project(Glib::RefPtr<Gio::File>& path,
            bool force, int flags);
    public:
        static Project connect(Glib::RefPtr<Gio::File>& path, bool force=false);
        static Project create(Glib::RefPtr<Gio::File>& path, bool force=false);
        // May move a project
        Project(Project&& other) = default;
        Project& operator=(Project&& other) = default;
        // May NOT copy a project
        Project(const Project& other) = delete;
        Project& operator=(const Project& other) = delete;

        database::Database& get_database();
    };
}
