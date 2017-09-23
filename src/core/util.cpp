#include "util.h"
#include <glibmm/fileutils.h>
#include <iostream>

namespace core {
    boost::optional<fs::path> get_project_directory(
        const fs::path& current_directory)
    {
        fs::path ret = current_directory;
        while (true) {
            fs::path child = ret / rbrush_folder_name;
            if (fs::is_directory(ret)) {
                return ret;
            }
            if (ret.has_parent_path()) {
                ret = ret.parent_path();
            } else {
                break;
            }
        }
        return {};
    }

    ProjectFolderLock::ProjectFolderLock(const fs::path& path, bool force)
    : m_lockpath(path / rbrush_lock_name)
    {
        fs::fstream f;
        f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            f.open(this->m_lockpath, std::ios::out);
        } catch (const std::ifstream::failure& e) {
            if (!force) {
                throw e;
            } else {
                std::cout << "Could not aquire lock on project, forcing lock."
                    << std::endl;
            }
        }
    }

    ProjectFolderLock::~ProjectFolderLock()
    {
        // TODO
        if (this->m_valid) {
            try {
                fs::remove(this->m_lockpath);
            } catch (...) {
                // Do nothing, doesn't matter
            }
        }
    }

    ProjectFolderLock::ProjectFolderLock(ProjectFolderLock&& other)
    {
        this->m_lockpath = other.m_lockpath;
        other.m_valid = false;
    }

    ProjectFolderLock& ProjectFolderLock::operator=(ProjectFolderLock&& other)
    {
        this->m_lockpath = other.m_lockpath;
        other.m_valid = false;
        return *this;
    }
}
