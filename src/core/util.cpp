#include "util.h"
#include <iostream>

namespace core {
    boost::optional<fs::path> get_project_directory(
        const fs::path& current_directory)
    {
        fs::path ret = current_directory;
        while (true) {
            fs::path child = ret / rbrush_folder_name;
            if (fs::is_directory(child)) {
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

    fs::path resolve_path(const fs::path& p)
    {
        if (p == "") {
            return fs::current_path();
        }
        if (!p.is_absolute()) {
            return fs::weakly_canonical(fs::absolute(p));
        }
        return fs::weakly_canonical(p);
    }

    bool is_path_within_path(const fs::path& a, const fs::path& b)
    {
        fs::path::const_iterator a_iter = a.begin();
        fs::path::const_iterator b_iter = b.begin();
        // look for elements that exist in B but not in A
        while (b_iter != b.end() && a_iter != a.end()) {
            if (*b_iter != *a_iter) {
                return false;
            }
            ++a_iter;
            ++b_iter;
        }
        // If B is not at the end, then B has
        // extra elements that A does not have
        return b_iter == b.end();
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
