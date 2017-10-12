#pragma once

#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace core {
    const std::string version = "1.0";
    const fs::path rbrush_folder_name = ".rbrush";
    const fs::path rbrush_version_name = "VERSION";
    const fs::path rbrush_db_name = "data.db";
    const fs::path rbrush_lock_name = "LOCK";

    /// Recursively search for a project directory, going though parents.
    /// This function returns the base folder, NOT the .rbrush folder!
    boost::optional<fs::path> get_project_directory(
        const fs::path& current_directory);

    fs::path resolve_path(const fs::path& p);

    /// Determine if path B does not contain elements that are contained
    /// in path A. If both A and B are canonical, this function checks if
    /// path A is contained within path B.
    /// Returns true if A is within B.
    bool is_path_within_path(const fs::path& a, const fs::path& b);

    /// Represents a lock on a directory.
    /// Note that the constructor for this class must take a directory which
    /// must be locked, not the name of the lockfile itself.
    class ProjectFolderLock {
        fs::path m_lockpath;
        bool m_valid = true;
    public:
        ProjectFolderLock(const fs::path& path, bool force=false);
        ~ProjectFolderLock();
        ProjectFolderLock(ProjectFolderLock&& other);
        ProjectFolderLock& operator=(ProjectFolderLock&& other);
        // Can NOT copy a lock
        ProjectFolderLock(const ProjectFolderLock& other) = delete;
        ProjectFolderLock& operator=(const ProjectFolderLock& other) = delete;
    };
}
