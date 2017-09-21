#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <boost/optional.hpp>

namespace cli {
    class ArgChain;
    class ArgBlock;
    struct ArgParse;
    class ArgResult;

    class ArgChain {
        std::queue<std::string> m_args;
    public:
        ArgChain(const std::vector<std::string>& arguments);
        ArgBlock parse(int nargs, const std::vector<ArgParse>& argdefs);
        void assert_finished() const;
    };

    struct ArgParse {
        std::string m_name;
        bool m_hasarg;
        boost::optional<char> m_short;
    };

    class ArgBlock {
        friend class ArgChain;
        std::vector<std::string> m_args;
        std::map<std::string, std::string> m_values;
        std::set<std::string> m_valid;
        int m_nargs;
    public:
        void assert_all_args() const;
        void assert_num_args(int n) const;
        void assert_options(const std::set<std::string>& exceptions) const;
        size_t size() const;
        bool match_arg(int n, const std::string& value) const;

        const std::vector<std::string>& get_arguments() const;
        bool has_option(const std::string& name) const;
        ArgResult get_option(const std::string& name) const;
        const std::string& operator[](size_t i) const;
        ArgResult operator[](const std::string& key) const;
    };

    class ArgResult {
        friend class ArgBlock;
        friend class ArgChain;
        std::string m_value;
        bool m_valid;
    public:
        const std::string& get();
        operator bool() const;
    };
}
