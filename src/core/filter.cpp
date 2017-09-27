#include "filter.h"
#include <glibmm.h>
#include <boost/algorithm/string.hpp>
#include <regex>

#define FILTER(name, classname) \
    {name, {classname::deserialize}}

// ends with _mip and one or more numbers
const std::regex match_mipmap("^.*_mip[0-9]+$");
bool filter_mipmaps(const fs::path& path)
{
    if (!path.has_stem()) {
        return false;
    }
    std::string stem = path.stem().string();
    return std::regex_match(stem, match_mipmap);
}

namespace core {
    // Filter Factory
    std::unique_ptr<IFilter> FilterMipMap::deserialize(const std::string& arg)
    {
        return std::make_unique<FilterMipMap>();
    }

    bool FilterMipMap::filter(const fs::path& path) const
    {
        return filter_mipmaps(path);
    }

    std::string FilterMipMap::serialize() const
    {
        return "";
    }

    // Filter Type
    std::unique_ptr<IFilter> FilterType::deserialize(const std::string& arg)
    {
        auto ret = std::make_unique<FilterType>();
        ret->m_path = arg;
        return ret;
    }

    bool FilterType::filter(const fs::path& path) const
    {
        if (!path.has_extension() && this->m_path == "") {
            return true;
        } else {
            return path.extension() == "." + this->m_path;
        }
    }

    std::string FilterType::serialize() const
    {
        return this->m_path;
    }

    // Filter
    Filter::Filter(std::unique_ptr<IFilter> ptr, const std::string& name)
    : m_filter(std::move(ptr)), m_name(name) {}

    bool Filter::filter(const fs::path& path) const
    {
        return this->m_filter->filter(path);
    }

    bool Filter::operator()(const fs::path& path) const
    {
        return this->filter(path);
    }

    const std::string& Filter::get_name() const
    {
        return this->m_name;
    }

    std::string Filter::serialize() const
    {
        return this->m_filter->serialize();
    }

    bool Filter::valid() const
    {
        return bool(this->m_filter);
    }

    Filter::operator bool() const
    {
        return this->valid();
    }
    
    // Filter Factory
    FilterFactory::FilterFactory()
    : m_filters({
        FILTER("mipmap", FilterMipMap),
        FILTER("filetype", FilterType)
    }) {}

    Filter FilterFactory::create(const std::string& name, const std::string& str) const
    {
        if (this->m_filters.count(name) == 0) {
            // TODO: throw something useful
            throw 1;
        }
        const auto& def = this->m_filters.at(name);
        return Filter(def.deserialize(str), name);
    }
}
