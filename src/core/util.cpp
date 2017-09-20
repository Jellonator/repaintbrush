#include "util.h"
#include <glibmm/fileutils.h>

namespace core {
    boost::optional<Glib::RefPtr<Gio::File>> get_project_directory(
        const std::string& current_directory)
    {
        auto file = Gio::File::create_for_path(current_directory);
        while (true) {
            auto child = file->get_child(rbrush_folder_name);
            auto childtype = child->query_file_type();
            if (childtype == Gio::FileType::FILE_TYPE_DIRECTORY) {
                return file;
            }
            if (!file->has_parent()) {
                break;
            }
            file = file->get_parent();
        }
        return boost::none;
    }

    ProjectFolderLock::ProjectFolderLock(Glib::RefPtr<Gio::File>& fh,
        bool force)
    : m_lockpath(fh->get_child(rbrush_folder_name)->get_child(rbrush_lock_name))
    {
        if (force) {
            try {
                this->m_lockpath->create_file();
            } catch (Gio::Error& e) {
                // This is fine
            }
        } else {
            this->m_lockpath->create_file();
        }
    }

    ProjectFolderLock::~ProjectFolderLock()
    {
        if (this->m_lockpath) {
            try {
                this->m_lockpath->remove();
            } catch (Glib::FileError& error) {
                // Do nothing, doesn't matter
            }
        }
    }

    ProjectFolderLock::ProjectFolderLock(ProjectFolderLock&& other)
    {
        this->m_lockpath = other.m_lockpath;
        other.m_lockpath.reset();
        other.m_valid = false;
    }

    ProjectFolderLock& ProjectFolderLock::operator=(ProjectFolderLock&& other)
    {
        this->m_lockpath = other.m_lockpath;
        other.m_lockpath.reset();
        other.m_valid = false;
        return *this;
    }
}
