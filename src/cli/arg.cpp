#include "arg.h"
#include <algorithm>
#include <stdexcept>
#include <sstream>

namespace cli {
    bool check_full_arg(const std::string& val)
    {
        if (val.size() >= 2) {
            return val[0] == '-' && val[1] == '-';
        }
        return false;
    }

    char check_short_arg(const std::string& val)
    {
        if (val.size() >= 1) {
            if (val.size() >= 3) {
                std::stringstream s;
                s << "Error parsing '" << val << "': ";
                s << "Shorthand argument should only have one character.";
                throw std::runtime_error(s.str());
            }
            if (val[0] == '-') {
                return val[1];
            } else {
                return 0;
            }
        }
        return 0;
    }

    ArgChain::ArgChain(const std::vector<std::string>& arguments)
    : m_args() {
        for (auto& str : arguments) {
            this->m_args.push(str);
        }
    }

    std::string get_full_name(const std::string& name,
        const std::map<char, std::string>& shorthand)
    {
        if (check_full_arg(name)) {
            return name.substr(2);
        } else {
            char c = check_short_arg(name);
            if (shorthand.count(c) == 0) {
                std::stringstream s;
                s << "Error parsing '" << name << "': ";
                s << "Argument does not exist.";
                throw std::runtime_error(s.str());
            }
            return shorthand.at(c);
        }
    }

    bool is_argument(const std::string& name)
    {
        return name.size() > 0 && name[0] == '-';
    }

    void assert_arg(bool expr, const std::string& arg, const std::string& msg)
    {
        if (expr) {
            std::stringstream s;
            s << "Error parsing '" << arg << "': ";
            s << msg;
            throw std::runtime_error(s.str());
        }
    }

    ArgBlock ArgChain::parse(int nargs, const std::vector<ArgParse> &argdefs)
    {
        ArgBlock block;
        block.m_nargs = nargs;
        std::map<std::string, const ArgParse&> longhand;
        std::map<char, std::string> shorthand;
        for (auto& def : argdefs) {
            longhand.emplace(def.m_name, def);
            if (def.m_short) {
                shorthand.emplace(*def.m_short, def.m_name);
            }
            block.m_valid.insert(def.m_name);
        }
        while (!this->m_args.empty()) {
            std::string arg = this->m_args.front();
            if (is_argument(arg)) {
                auto fullname = get_full_name(arg, shorthand);
                assert_arg(longhand.count(fullname) == 0, arg,
                    "Argument does not exist.");
                assert_arg(block.m_values.count(fullname) != 0, arg,
                    "Duplicate argument");
                const ArgParse& parse = longhand.at(fullname);
                if (parse.m_hasarg) {
                    this->m_args.pop();
                    assert_arg(this->m_args.empty(), arg,
                    "Expected argument.");
                    const std::string& newarg = this->m_args.front();
                    assert_arg(is_argument(newarg), arg,
                    "Expected argument.");
                    block.m_values[fullname] = newarg;
                } else {
                    block.m_values[fullname] = "";
                }
            } else {
                if (nargs <= 0) {
                    break;
                }
                nargs --;
                block.m_args.push_back(arg);
            }
            this->m_args.pop();
        }
        return block;
    }

    void ArgChain::assert_finished() const {
        if (!this->m_args.empty()) {
            std::stringstream s;
            s << "Error: ";
            s << "Too many arguments.";
            throw std::runtime_error(s.str());
        }
    }

    void ArgBlock::assert_all_args() const
    {
        if (this->m_nargs != int(this->size())) {
            std::stringstream s;
            s << "Error: ";
            s << "Expected " << this->m_nargs << " arguments.";
            throw std::runtime_error(s.str());
        }
    }

    void ArgBlock::assert_num_args(int n) const
    {
        if (n != int(this->size())) {
            std::stringstream s;
            s << "Error: ";
            s << "Expected " << n << " arguments.";
            throw std::runtime_error(s.str());
        }
    }

    void ArgBlock::assert_options(
        const std::set<std::string>& exceptions) const
    {
        for (auto& namepair : this->m_values) {
            if (exceptions.count(namepair.first) == 0) {
                std::stringstream s;
                s << "Unexpected option '--" << namepair.first << "'";
                throw std::runtime_error(s.str());
            }
        }
    }

    size_t ArgBlock::size() const
    {
        return this->m_args.size();
    }

    bool ArgBlock::match_arg(int n, const std::string& value) const
    {
        if (n >= 0 && n < int(this->size())) {
            return this->m_args[n] == value;
        }
        return false;
    }

    const std::vector<std::string>& ArgBlock::get_arguments() const
    {
        return this->m_args;
    }

    bool ArgBlock::has_option(const std::string& name) const
    {
        return this->m_values.count(name) != 0;
    }

    ArgResult ArgBlock::get_option(const std::string& name) const
    {
        ArgResult ret;
        if (this->m_values.count(name) != 0) {
            ret.m_valid = true;
            ret.m_value = this->m_values.at(name);
        } else {
            if (this->m_valid.count(name) == 0) {
                std::stringstream s;
                s << "Error: Attempt to get non-defined argument '";
                s << name << "'";
                throw std::runtime_error(s.str());
            }
            ret.m_valid = false;
            ret.m_value = "";
        }
        return ret;
    }

    const std::string& ArgBlock::operator[](size_t i) const
    {
        return this->m_args[i];
    }

    ArgResult ArgBlock::operator[](const std::string& key) const
    {
        return this->get_option(key);
    }

    ArgResult::operator bool() const
    {
        return this->m_valid;
    }

    const std::string& ArgResult::get()
    {
        return this->m_value;
    }
}
