#pragma once
#include <sqlite3.h>
#include <string>
#include <memory>

namespace database {
    class Statement {
        std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> m_statement;
        int m_status;
    public:
        /// Create a new statement.
        /// It is recommended to use Database::prepare instead.
        Statement(sqlite3* db, const std::string& statement);

        /// Run a single step of this statement and return its code.
        /// This function will only return non-error codes. Any actual errors
        /// will throw an exception instead.
        int step();

        /// Returns true if this statement has completely finished execution.
        bool done();

        /// Repeatedly step this statement until it is done.
        void finish();

        /// Reset this statement so it can be ran again.
        void reset();

        /// Bind a null value to the given key.
        /// Note that keys are 1-indexed.
        /// Returns false if a valid binding could not be made.
        bool bind_null(int key);

        /// Bind a null value to the given key.
        /// The key should match a table parameter.
        /// Returns false if a valid binding could not be made.
        bool bind_null(const std::string& key);

        /// Bind a value to the given key.
        /// Valid data types are integers, doubles, and strings.
        /// Note that keys are 1-indexed.
        /// Returns false if a valid binding could not be made.
        template<typename V>
        bool bind(int key, const V& value);

        /// Bind a value to the given key.
        /// Valid data types are integers, doubles, and strings.
        /// The key should match a table parameter.
        /// Returns false if a valid binding could not be made.
        template<typename V>
        bool bind(const std::string& key, const V& value)
        {
            int ikey = sqlite3_bind_parameter_index(this->m_statement.get(),
                key.c_str());
            return this->bind(ikey, value);
        }

        /// Return a pointer to this statement's underlying statement.
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
