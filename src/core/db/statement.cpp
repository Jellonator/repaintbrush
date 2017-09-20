#include "statement.h"

namespace database {
    Statement::Statement(sqlite3* db, const std::string& statement)
    : m_statement(nullptr, &sqlite3_finalize)
    , m_status(SQLITE_OK)
    {
        sqlite3_stmt* stmt;
        int ok = sqlite3_prepare_v2(db,
            statement.c_str(), statement.size(), &stmt, nullptr);
        if (ok != SQLITE_OK) {
            auto err = std::runtime_error(sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            throw err;
        }
        this->m_statement.reset(stmt);
    }

    int Statement::step()
    {
        this->m_status = sqlite3_step(this->m_statement.get());
        if (this->m_status != SQLITE_DONE && this->m_status != SQLITE_ROW) {
            //note: May need to handle SQLITE_BUSY at some point.
            sqlite3* db = sqlite3_db_handle(this->m_statement.get());
            auto err = std::runtime_error(sqlite3_errmsg(db));
            throw err;
        }
        return this->m_status;
    }

    bool Statement::done()
    {
        return this->m_status == SQLITE_DONE;
    }

    void Statement::finish()
    {
        while (!this->done()) {
            this->step();
        }
    }

    bool Statement::bind_null(int key)
    {
        int ok = sqlite3_bind_null(this->m_statement.get(), key);
        return ok == SQLITE_OK;
    }

    bool Statement::bind_null(const std::string& key)
    {
        int ikey = sqlite3_bind_parameter_index(this->m_statement.get(),
            key.c_str());
        return this->bind_null(ikey);
    }

    template<>
    bool Statement::bind<int32_t>(int key, const int32_t& value)
    {
        int ok = sqlite3_bind_int(this->m_statement.get(), key, value);
        return ok == SQLITE_OK;
    }
    
    template<>
    bool Statement::bind<int64_t>(int key, const int64_t& value)
    {
        int ok = sqlite3_bind_int64(this->m_statement.get(), key, value);
        return ok == SQLITE_OK;
    }

    template<>
    bool Statement::bind<double>(int key, const double& value)
    {
        int ok = sqlite3_bind_double(this->m_statement.get(), key, value);
        return ok == SQLITE_OK;
    }

    template<>
    bool Statement::bind<std::string>(int key, const std::string& value)
    {
        int ok = sqlite3_bind_text(this->m_statement.get(), key, value.c_str(),
            value.size(), SQLITE_TRANSIENT);
        return ok == SQLITE_OK;
    }
}
