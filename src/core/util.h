#pragma once

#include <boost/optional.hpp>
#include <giomm/file.h>

namespace core {
    const std::string rbrush_folder_name = ".rbrush";
    const std::string rbrush_version_name = "VERSION";
    const std::string rbrush_db_name = "data.db";
    const std::string rbrush_lock_name = "LOCK";

    /// Recursively search for a project directory, going though parents.
    /// This function returns the base folder, NOT the .rbrush folder!
    boost::optional<Glib::RefPtr<Gio::File>> get_project_directory(
        const std::string& current_directory);

    /// Represents a lock on a directory.
    class ProjectFolderLock {
        Glib::RefPtr<Gio::File> m_lockpath;
        bool m_valid = true;
    public:
        ProjectFolderLock(Glib::RefPtr<Gio::File>& fh, bool force=false);
        ~ProjectFolderLock();
        ProjectFolderLock(ProjectFolderLock&& other);
        ProjectFolderLock& operator=(ProjectFolderLock&& other);
        // Can NOT copy a lock
        ProjectFolderLock(const ProjectFolderLock& other) = delete;
        ProjectFolderLock& operator=(const ProjectFolderLock& other) = delete;
    };
}
