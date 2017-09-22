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
        Project(Glib::RefPtr<Gio::File>& path,
            bool force, int flags);
    public:
        // May move a project
        Project(Project&& other) = default;
        Project& operator=(Project&& other) = default;
        // May NOT copy a project
        Project(const Project& other) = delete;
        Project& operator=(const Project& other) = delete;

        /// Connect to an existing database.
        /// The database must exist and will throw an error if it does not.
        static Project connect(Glib::RefPtr<Gio::File>& path, bool force);

        /// Connect to a database, creating it if necessary.
        /// Throws an exception if a database could not be created.
        static Project create(Glib::RefPtr<Gio::File>& path, bool force);

        /// Get a reference to this project's underlying database

        database::Database& get_database();

        /// Add a folder to input folders
        /// Returns true if the folder already exists.
        bool add_inputfolder(Glib::RefPtr<Gio::File>& path);

        /// Remove a folder from input folders
        /// Returns true if the folder does not exist.
        bool remove_inputfolder(Glib::RefPtr<Gio::File>& path);

        /// Get a list of input folders.
        std::vector<std::string> list_input_folders();
    };

    boost::optional<Project> get_project(bool force);
}
