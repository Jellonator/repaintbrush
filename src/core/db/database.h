#pragma once

#include <sqlite3.h>
#include <string>
#include <memory>
#include <boost/filesystem.hpp>
#include "statement.h"
namespace fs = boost::filesystem;

namespace database {
    class Database {
        std::unique_ptr<sqlite3, decltype(&sqlite3_close_v2)> m_database;
    public:
        // May move a database
        Database(Database&& other) = default;
        Database& operator=(Database&& other) = default;
        // May NOT copy a database
        Database(const Database& other) = delete;
        Database& operator=(const Database& other) = delete;

        /// Create a new connection to a database.
        Database(const fs::path& path, int flags);

        /// Prepare an SQL statement.
        Statement prepare(const std::string& statement);

        /// Get a pointer to this Database's underlying database.
        sqlite3* get_ptr();
    };
}
