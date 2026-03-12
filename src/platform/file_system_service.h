#pragma once

#include <string>

namespace ogc::platform {

class FileSystemService {
public:
    [[nodiscard]] static std::string displayNameForPath(const std::string& path);
};

}  // namespace ogc::platform
