#include "platform/file_system_service.h"

#include <QFileInfo>

namespace ogc::platform {

QString FileSystemService::displayNameForPath(const QString& path) {
    const QFileInfo info(path);
    return info.fileName().isEmpty() ? path : info.fileName();
}

}  // namespace ogc::platform
