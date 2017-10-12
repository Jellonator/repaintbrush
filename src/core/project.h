#pragma once
#include <memory>
#include <sqlite3.h>
#include "util.h"
#include "db/database.h"
#include "filter.h"

namespace core {
    class Project {
        ProjectFolderLock m_lock;
        fs::path m_path;
        database::Database m_database;
        Project(const fs::path& path,
            bool force, int flags);
    public:
        /// Type defining a type of a filter
        enum filter_t {
            FILTER_INPUT = 0,
            FILTER_OUTPUT = 1
        };

        struct FilterData {
            Filter filter;
            filter_t type;
            int id;
        };

        struct Result {
            Result();
            int files;
            int folders;
            int filtered;
        };

        // May move a project
        Project(Project&& other) = default;
        Project& operator=(Project&& other) = default;
        // May NOT copy a project
        Project(const Project& other) = delete;
        Project& operator=(const Project& other) = delete;

        /// Connect to an existing database.
        /// The database must exist and will throw an error if it does not.
        static Project connect(const fs::path& path, bool force);

        /// Connect to a database, creating it if necessary.
        /// Throws an exception if a database could not be created.
        static Project create(const fs::path& path, bool force);

        /// Get a reference to this project's underlying database
        database::Database& get_database();

        /// Add a folder to input folders
        /// Returns true if the folder already exists.
        bool add_inputfolder(const fs::path& path);

        /// Remove a folder from input folders
        /// Returns true if the folder does not exist.
        bool remove_inputfolder(const fs::path& path);

        /// Get a list of input folders.
        std::vector<fs::path> list_input_folders();

        /// Get this project's path.
        const fs::path& get_path() const;

        /// Check if a file exists.
        bool has_file(const fs::path& path);

        /// Register a file into database.
        /// Returns true if the file already exists.
        /// Don't use this function for bulk importing, use Project::import
        /// instead! This function can be slow when used with many files.
        bool register_file(const fs::path& path);

        /// Checks registered files.
        /// Checks all files in the project directory with registered files,
        /// and remove registered files that no longer exist.
        /// Returns a list of all files that were removed.
        std::vector<fs::path> check();

        /// Import files into export_folder.
        /// The optional argument import_folder specifies that only that folder
        /// should be imported.
        Result import(fs::path export_folder,
            boost::optional<fs::path> import_folder);

        /// Export all registered files into a given folder.
        Result export_to_folder(fs::path export_folder);

        /// Add a filter to this project.
        void add_filter(filter_t type, const Filter& filter);

        /// Get a list of filters
        std::list<FilterData> get_filters();

        /// Remove a filter from this project.
        /// Returns true if filter of id does not exist
        bool remove_filter(int id);
    };

    /// Get the project.
    /// Searches the current directory and all parents of the current directory
    /// for a valid project, and return none if a project could not be found.
    /// This function will perform a sanity check after opening the project.
    boost::optional<Project> get_project(bool force);
}
