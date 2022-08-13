#include "AppSystem.h"

bool AppSystem::isEmpty(QString str) {
    return str.trimmed().size() <= 0;
}

QString getSiteDomainName(QString url) {
    return "";
}
