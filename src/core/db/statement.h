#pragma once
#include <sqlite3.h>
#include <string>
#include <memory>

namespace database {
    class Statement {
        std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> m_statement;
        int m_status;
    public:
        Statement(sqlite3* db, const std::string& statement);
        int step();
        bool done();
        void finish();
        void reset();
        bool bind_null(int key);
        bool bind_null(const std::string& key);
        template<typename V>
        bool bind(int key, const V& value);
        template<typename V>
        bool bind(const std::string& key, const V& value)
        {
            int ikey = sqlite3_bind_parameter_index(this->m_statement.get(),
                key.c_str());
            return this->bind(ikey, value);
        }
        sqlite3* database_ptr();
        sqlite3_stmt* stmt_ptr();
    };
    template<>
    bool Statement::bind<int32_t>(int key, const int32_t& value);
    template<>
    bool Statement::bind<int64_t>(int key, const int64_t& value);
    template<>
    bool Statement::bind<double>(int key, const double& value);
    template<>
    bool Statement::bind<std::string>(int key, const std::string& value);
}
