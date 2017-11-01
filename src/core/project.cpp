#include "project.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace core {
    const std::string TYPE_INPUT = "input";
    const std::string TYPE_OUTPUT = "output";
    const std::string TYPE_NONE = "";
    const std::string& get_ftype_name(core::Project::filter_t type)
    {
        switch (type) {
        case core::Project::FILTER_INPUT:
            return TYPE_INPUT;
            break;
        case core::Project::FILTER_OUTPUT:
            return TYPE_OUTPUT;
            break;
        }
        return TYPE_NONE;
    }

    void check_project_can_create(const fs::path& path) {
        if (!fs::is_directory(path)) {
            std::stringstream s;
            s << "No such directory " << path;
            throw std::runtime_error(s.str());
        }
        if (fs::is_directory(path/rbrush_folder_name) &&
        fs::is_regular_file(path/rbrush_folder_name/rbrush_db_name)) {
            std::stringstream s;
            s << "The project directory " << path << " already exists";
            throw std::runtime_error(s.str());
        }
    }

    void check_project_is_valid(const fs::path& path) {
        if (!fs::is_directory(path)) {
            std::stringstream s;
            s << "No such directory " << path;
            throw std::runtime_error(s.str());
        }
        if (!fs::is_directory(path/rbrush_folder_name)) {
            std::stringstream s;
            s << "Directory " << path << " is not a valid project folder";
            throw std::runtime_error(s.str());
        }
        if (!fs::is_regular_file(path/rbrush_folder_name/rbrush_db_name)) {
            std::stringstream s;
            s << "The project directory " << path << " may be corrupted";
            throw std::runtime_error(s.str());
        }
    }

    Project::Project(const fs::path& path, bool force, int flags)
    : m_lock(path / rbrush_folder_name, force)
    , m_path(path)
    , m_database(path / rbrush_folder_name / rbrush_db_name, flags) {}

    Project::Result::Result() : files(0), folders(0), filtered(0) {}

    Project Project::connect(const fs::path& path, bool force)
    {
        check_project_is_valid(path);
        return Project(path, force, SQLITE_OPEN_READWRITE);
    }

    Project Project::create(const fs::path& path, bool force)
    {
        check_project_can_create(path);
        fs::create_directories(path/rbrush_folder_name);
        Project project(path, force, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
        auto& db = project.get_database();
        db.execute(R"(
            CREATE TABLE IF NOT EXISTS images(
                name TEXT NOT NULL,
                alias TEXT
            )
        )");
        db.execute(R"(
            CREATE TABLE IF NOT EXISTS inputfolders(
                name TEXT NOT NULL UNIQUE
            )
        )");
        db.execute(R"(
            CREATE TABLE IF NOT EXISTS filters(
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                type INTEGER NOT NULL,
                name TEXT NOT NULL,
                arg TEXT NOT NULL
            )
        )");
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

    Project::Result Project::import(fs::path export_folder,
        boost::optional<fs::path> import_folder)
    {
        Result ret;
        // make sure export folder exists
        export_folder = core::resolve_path(export_folder);
        if (!core::is_path_within_path(export_folder, get_path())) {
            throw std::runtime_error("Selected export folder must be\n"
                "within project folder.");
        }
        fs::create_directories(export_folder);
        auto& db = this->get_database();
        {
            // Get all filters
            auto filters = this->get_filters();
            filters.remove_if([](const auto& filter) {
                return filter.type != FILTER_INPUT;
            });
            // Put all images that can be imported into a table
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
                    ++ ret.folders;
                    auto fileiter = fs::recursive_directory_iterator(folder);
                    for (const fs::path& file : fileiter) {
                        bool should_continue = false;
                        for (const auto& filter : filters) {
                            if (filter.filter(folder, file)) {
                                // Yeah, yeah, I know. I'm building the
                                // pyramids of Egypt over here. But you know
                                // what? I just don't care. If it works, and it
                                // looks alright, it's alright.
                                should_continue = true;
                                break;
                            }
                        }
                        if (should_continue) {
                            ++ ret.filtered;
                            continue;
                        }
                        insertstmt.reset();
                        insertstmt.bind(1, file.filename().string());
                        insertstmt.bind(2, file.string());
                        insertstmt.finish();
                    }
                }
            }
            // find all files that exist in imglist but not in images, insert
            // those files into images, and copy the files into export_folder.
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
                ++ ret.files;
                fs::path name = selectstmt.column_value<std::string>(1);
                fs::path path = selectstmt.column_value<std::string>(2);
                fs::path outfile = export_folder/name;
                fs::copy_file(path, outfile);
                insertfilestmt.reset();
                insertfilestmt.bind(1, name.string());
                insertfilestmt.finish();
            }
        }
        return ret;
    }

    Project::Result Project::export_to_folder(fs::path export_folder)
    {
        Result ret;
        auto& db = this->get_database();
        auto transaction = db.create_transaction();

        auto filters = this->get_filters();
        filters.remove_if([](const auto& filter) {
            return filter.type != FILTER_OUTPUT;
        });

        // Create a temporary table that is a list of files in the project
        transaction.push(R"(
            CREATE TEMP TABLE imglist(
                name NTEXT NOT NULL,
                path NTEXT NOT NULL)
            )",
            R"(DROP TABLE imglist)"
        );
        auto insertstmt = db.prepare(R"(
            INSERT INTO imglist(name, path)
            VALUES (?, ?)
        )");
        auto fileiter = fs::recursive_directory_iterator(this->get_path());
        for (const fs::path& file : fileiter) {
            insertstmt.reset();
            insertstmt.bind(1, file.filename().string());
            insertstmt.bind(2, file.string());
            insertstmt.finish();
        }
        auto selectstmt = db.prepare(R"(
            SELECT imglist.name, imglist.path FROM images
            INNER JOIN imglist ON images.name = imglist.name
        )");
        while (selectstmt.step() == SQLITE_ROW) {
            std::string name = selectstmt.column_value<std::string>(1);
            fs::path path = selectstmt.column_value<std::string>(2);
            bool should_copy = true;
            for (const auto& filter : filters) {
                if (filter.filter(this->get_path(), path)) {
                    should_copy = false;
                    break;
                }
            }
            if (should_copy) {
                fs::copy_file(path, export_folder/name,
                    fs::copy_option::overwrite_if_exists);
                ++ ret.files;
            } else {
                ++ ret.filtered;
            }
        }
        ++ ret.folders;
        return ret;
    }

    void Project::add_filter(filter_t type, const Filter& filter)
    {
        auto& db = this->get_database();
        auto insertstmt = db.prepare(R"(
            INSERT INTO filters(type, name, arg)
            VALUES(?1, ?2, ?3)
        )");
        insertstmt.bind(1, static_cast<int>(type));
        insertstmt.bind(2, filter.get_name());
        insertstmt.bind(3, filter.serialize());
        insertstmt.finish();
    }

    std::list<Project::FilterData> Project::get_filters()
    {
        FilterFactory factory;
        std::list<FilterData> ret;
        auto& db = this->get_database();
        auto selectstmt = db.prepare(R"(
            SELECT id, type, name, arg
            FROM filters
        )");
        while (SQLITE_ROW == selectstmt.step()) {
            int id = selectstmt.column_value<int>(1);
            int type = selectstmt.column_value<int>(2);
            std::string name = selectstmt.column_value<std::string>(3);
            std::string arg = selectstmt.column_value<std::string>(4);
            ret.push_back(Project::FilterData {
                factory.create(name, arg),
                static_cast<filter_t>(type),
                id
            });
        }

        return ret;
    }

    bool Project::remove_filter(int id)
    {
        auto& db = this->get_database();
        auto deletestmt = db.prepare(R"(
            DELETE FROM filters
            WHERE id = ?
        )");
        deletestmt.bind(1, id);
        deletestmt.finish();
        return sqlite3_changes(db.get_ptr()) == 0;
    }

    boost::optional<Project> open_project(const fs::path& path, bool force)
    {
        core::Project project = core::Project::connect(path, force);
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

    boost::optional<Project> get_project(bool force)
    {
        auto path = core::get_project_directory(fs::current_path());
        if (!path) {
            std::cout << "Could not find repaintbrush project folder." << std::endl;
            return {};
        }
        return open_project(*path, force);
    }
}
