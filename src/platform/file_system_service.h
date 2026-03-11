#pragma once

#include <QString>

namespace ogc::platform {

class FileSystemService {
public:
    [[nodiscard]] static QString displayNameForPath(const QString& path);
};

}  // namespace ogc::platform
