#include "database.h"
#include <stdexcept>
#include <iostream>

namespace database {
    Database::Database(const fs::path& path, int flags)
    : m_database(nullptr, &sqlite3_close_v2)
    {
        sqlite3* db;
        int ok = sqlite3_open_v2(path.c_str(), &db, flags, nullptr);
        if (ok != SQLITE_OK) {
            std::cout << path.string() << std::endl;
            auto err = std::runtime_error(sqlite3_errmsg(db));
            sqlite3_close(db);
            throw err;
        }
        this->m_database.reset(db);
    }

    Statement Database::prepare(const std::string& statement)
    {
        return Statement(this->m_database.get(), statement);
    }

    sqlite3* Database::get_ptr()
    {
        return this->m_database.get();
    }
}
