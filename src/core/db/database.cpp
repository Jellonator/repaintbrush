#include "database.h"
#include <stdexcept>
#include <iostream>

namespace database {
    Transaction::Transaction(Database* db)
    : m_db(db)
    {
        m_db->execute("BEGIN TRANSACTION");
        // for simplicity, end transaction is just another cleanup call.
        // since it is first it is always called last.
        m_cleanup.push_back("END TRANSACTION");
    }

    Transaction::~Transaction()
    {
        for (auto iter = m_cleanup.rbegin(); iter != m_cleanup.rend(); ++iter){
            try {
                m_db->execute(*iter);
            } catch(std::exception& e) {
                std::cout << e.what() << std::endl;
            } catch(...) {
                std::cout << "Warning: Unknown error occured during cleanup."
                          << std::endl;
                // doesn't matter since it's only cleanup
            }
        }
    }

    void Transaction::push(const std::string& exec, const std::string& cleanup)
    {
        m_db->execute(exec);
        // Note that cleanup is not pushed if exec fails
        m_cleanup.push_back(cleanup);
    }

    Database::Database(const fs::path& path, int flags)
    : m_database(nullptr, &sqlite3_close_v2)
    {
        sqlite3* db;
        int ok = sqlite3_open_v2(path.c_str(), &db, flags, nullptr);
        if (ok != SQLITE_OK) {
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

    void Database::execute(const std::string& statement)
    {
        int ok = sqlite3_exec(this->get_ptr(), statement.c_str(),
            nullptr, nullptr, nullptr);
        if (ok != SQLITE_OK) {
            auto err = std::runtime_error(sqlite3_errmsg(this->get_ptr()));
            throw err;
        }
    }

    Transaction Database::create_transaction()
    {
        return Transaction(this);
    }
}
