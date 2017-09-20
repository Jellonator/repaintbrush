#pragma once

#include <string>
#include <functional>

namespace core {
    class IFileFilter {
    public:
        // Return false if path should be ignored.
        virtual bool filter(const std::string& path) = 0;
    };
}
