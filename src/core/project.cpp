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
            auto stmt = project.get_database().prepare(R"(
                CREATE TABLE IF NOT EXISTS images(
                    name NTEXT NOT NULL,
                    alias NTEXT
                )
            )");
            stmt.finish();
        }
        {
            auto stmt = project.get_database().prepare(R"(
                CREATE TABLE IF NOT EXISTS inputfolders(
                    name TEXT NOT NULL UNIQUE
                )
            )");
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
        auto stmt = db.prepare(R"(
            INSERT INTO inputfolders(name)
            VALUES (?)
        )");
        stmt.bind(1, fs::canonical(path).string());
        try {
            stmt.finish();
        } catch (std::exception& e) {
            if (std::string(e.what()) ==
                    "UNIQUE constraint failed: inputfolders.name") {
                return true;
            } else {
                throw;
            }
        }
        return false;
    }

    bool Project::remove_inputfolder(const fs::path& path)
    {
        auto& db = this->get_database();
        auto stmt = db.prepare(R"(
            DELETE FROM inputfolders
            WHERE name = ?
        )");
        stmt.bind(1, fs::canonical(path).string());
        stmt.finish();
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

    const fs::path& Project::get_path() const
    {
        return this->m_path;
    }

    bool Project::has_file(const fs::path& path)
    {
        std::vector<fs::path> ret;
        auto& db = this->get_database();
        auto stmt = db.prepare(R"(
            SELECT * FROM images WHERE name = ?
        )");
        stmt.bind(1, path.filename().string());
        while (!stmt.done()) {
            if (stmt.step() == SQLITE_ROW) {
                return true;
            }
        }
        return false;
    }

    bool Project::register_file(const fs::path& path)
    {
        if (!fs::is_regular_file(path)) {
            std::stringstream s;
            s << "Error: '" << path.string() << "' is not a valid file!";
            throw std::runtime_error(s.str());
        }
        auto& db = this->get_database();
        auto stmt = db.prepare(R"(
            INSERT INTO images(name)
            SELECT ?1
            WHERE NOT EXISTS (
                SELECT name FROM images WHERE name = ?1
            )
        )");
        stmt.bind(1, path.filename().string());
        stmt.finish();
        return sqlite3_changes(db.get_ptr()) == 0;
    }

    std::vector<fs::path> Project::check()
    {
        std::vector<fs::path> ret;
        auto& db = this->get_database();
        // insert every image into a temporary database
        // since many values are being inserted at one time, a transaction will
        // be made to speed up the process.
        {
            auto transaction = db.create_transaction();
            transaction.push(R"(
                CREATE TEMP TABLE imglist(
                    name NTEXT NOT NULL)
                )",
                R"(DROP TABLE imglist)"
            );
            auto insertstmt = db.prepare(R"(
                INSERT INTO imglist(name)
                VALUES (?)
            )");
            auto fileiter = fs::recursive_directory_iterator(this->get_path());
            for (const fs::path& file : fileiter) {
                insertstmt.reset();
                insertstmt.bind(1, file.filename().string());
                insertstmt.finish();
            }
            // Find all files that are in the table 'images' but not in the
            // temporary table 'imglist'. Note that it is fine for files to
            // exist but not be registered.
            auto findstmt = db.prepare(R"(
                SELECT images.name FROM images
                LEFT OUTER JOIN imglist
                ON images.name = imglist.name
                WHERE imglist.name IS NULL
            )");
            while (SQLITE_ROW == findstmt.step()) {
                std::string value = findstmt.column_value<std::string>(1);
                ret.push_back(value);
            }
            auto deletestmt = db.prepare(R"(
                DELETE FROM images
                WHERE images.name IN (
                    SELECT images.name FROM images
                    LEFT OUTER JOIN imglist
                    ON images.name = imglist.name
                    WHERE imglist.name IS NULL
                )
            )");
            deletestmt.finish();
        }
        return ret;
    }

    std::pair<int, int> Project::import(fs::path export_folder,
        boost::optional<fs::path> import_folder)
    {
        int num_folders = 0;
        int num_files = 0;
        // make sure export folder exists
        export_folder = fs::current_path() / export_folder;
        fs::create_directories(export_folder);
        auto& db = this->get_database();
        {
            // Put all images into a database
            auto folders = this->list_input_folders();
            auto transaction = db.create_transaction();
            transaction.push(R"(
                CREATE TEMP TABLE imglist(
                    name NTEXT NOT NULL,
                    path NTEXT NOT NULL
                ))",
                R"(DROP TABLE imglist)"
            );
            auto insertstmt = db.prepare(R"(
                INSERT INTO imglist(name, path)
                VALUES (?1, ?2)
            )");
            for (const fs::path& folder : folders) {
                if (!import_folder || fs::equivalent(*import_folder, folder)) {
                    ++num_folders;
                    auto fileiter = fs::recursive_directory_iterator(folder);
                    for (const fs::path& file : fileiter) {
                        insertstmt.reset();
                        insertstmt.bind(1, file.filename().string());
                        insertstmt.bind(2, file.string());
                        insertstmt.finish();
                    }
                }
            }
            auto selectstmt = db.prepare(R"(
                SELECT imglist.name, imglist.path FROM imglist
                LEFT OUTER JOIN images
                ON imglist.name = images.name
                WHERE images.name IS NULL
            )");
            auto insertfilestmt = db.prepare(R"(
                INSERT INTO images(name)
                VALUES (?)
            )");
            while (SQLITE_ROW == selectstmt.step()) {
                num_files ++;
                fs::path name = selectstmt.column_value<std::string>(1);
                fs::path path = selectstmt.column_value<std::string>(2);
                fs::path outfile = export_folder/name;
                fs::copy(path, outfile);
                insertfilestmt.reset();
                insertfilestmt.bind(1, name.string());
                insertfilestmt.finish();
            }
        }
        return std::make_pair(num_folders, num_files);
    }

    boost::optional<Project> get_project(bool force)
    {
        auto path = core::get_project_directory(fs::current_path());
        if (!path) {
            std::cout << "Could not find repaintbrush project folder." << std::endl;
            return {};
        }
        core::Project project = core::Project::connect(*path, force);
        auto removedpaths = project.check();
        if (removedpaths.size() > 0) {
            std::cout << "Warning: the following files were removed since "
                         "last execution." << std::endl;
        }
        for (auto& path : removedpaths) {
            std::cout << "\t" << path.string() << std::endl;
        }
        if (removedpaths.size() > 0) {
            std::cout << std::endl;
        }
        return project;
    }
}
