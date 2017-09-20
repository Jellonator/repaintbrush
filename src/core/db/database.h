#pragma once
#include <sqlite3.h>
#include <string>
#include <memory>
#include "statement.h"

namespace database {
    class Database {
        std::unique_ptr<sqlite3, decltype(&sqlite3_close_v2)> m_database;
    public:
        Database(const std::string& path, int flags);
        // May move a database
        Database(Database&& other) = default;
        Database& operator=(Database&& other) = default;
        // May NOT copy a database
        Database(const Database& other) = delete;
        Database& operator=(const Database& other) = delete;

        Statement prepare(const std::string& statement);
    };
}
