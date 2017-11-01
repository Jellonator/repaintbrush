#pragma once

#include <string>
#include <memory>
#include <map>
#include <functional>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace core {
    /// An interface for defining filters.
    class IFilter {
    public:
        /// Return false if path should be ignored.
        virtual bool filter(const fs::path& base, const fs::path& path) const = 0;
        /// Serialize this filter into a string.
        virtual std::string serialize() const = 0;
        // All filters require a static deserialize function that return a unique
        // pointer to itself.
    };

    /// Filter out images whose name contains the word mipmap
    class FilterMipMap : public IFilter {
    public:
        static std::unique_ptr<IFilter> deserialize(const std::string&);
        bool filter(const fs::path& base, const fs::path& path) const;
        std::string serialize() const;
    };

    /// Filter out images of a given type
    class FilterType : public IFilter {
        std::string m_path;
    public:
        static std::unique_ptr<IFilter> deserialize(const std::string&);
        bool filter(const fs::path& base, const fs::path& path) const;
        std::string serialize() const;
    };

    /// Filter out images that are inside a given path. Note that this path
    /// should be relative. In the case of input filters, this path is relative
    /// to the input folder. In the case of output filters, this path is
    /// relative to the project base path.
    class FilterPath : public IFilter {
        fs::path m_path;
    public:
        static std::unique_ptr<IFilter> deserialize(const std::string&);
        bool filter(const fs::path& base, const fs::path& path) const;
        std::string serialize() const;
    };

    /// Definition for a filter in a filterfactory.
    struct FilterDef {
        /// Function used for creating a filter from a string.
        std::function<std::unique_ptr<IFilter>(const std::string&)> deserialize;
        bool has_arg;
        std::string description;
    };

    /// A filter.
    /// Analyzes files and returns if said file should be used.
    class Filter {
        friend class FilterFactory;
        std::unique_ptr<IFilter> m_filter;
        std::string m_name;
        bool m_valid_name;
        Filter(std::unique_ptr<IFilter> ptr, const std::string& name);
    public:
        /// Filter a path. Returns false if the path should be ignored.
        bool filter(const fs::path& base, const fs::path& path) const;
        bool operator()(const fs::path& base, const fs::path& path) const;

        /// Get the name of this filter.
        const std::string& get_name() const;

        /// Serialize this filter's content into a string.
        std::string serialize() const;

        /// Returns true if this filter is valid.
        bool valid() const;
        operator bool() const;

        /// Returns whether this filter has a valid name.
        bool is_name_valid() const;
    };

    /// Factory for creating filters.
    class FilterFactory {
        std::map<std::string, FilterDef> m_filters;
    public:
        FilterFactory();

        /// Create a new filter.
        /// This function will throw an exception
        /// if a filter could not be created.
        Filter create(const std::string& name, const std::string& str) const;

        const std::map<std::string, FilterDef>& get_filters() const;
    };
}
