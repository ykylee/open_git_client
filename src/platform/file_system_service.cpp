#include "platform/file_system_service.h"

#include <filesystem>

namespace ogc::platform {

std::string FileSystemService::displayNameForPath(const std::string& path) {
    const std::filesystem::path filePath(path);
    return filePath.filename().empty() ? path : filePath.filename().string();
}

}  // namespace ogc::platform
