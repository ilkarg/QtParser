#include "AppSystem.h"

bool AppSystem::isEmpty(QString str) {
    return str.trimmed().size() <= 0;
}

QString AppSystem::getSiteDomainName(QString url) {
    QString domainName = url;

    if (url.startsWith("http://")) {
        domainName = domainName.replace("http://", "");
    } else if (url.startsWith("https://")) {
        domainName = domainName.replace("https://", "");
    }

    if (domainName.indexOf("/") != -1) {
        domainName.truncate(domainName.indexOf("/"));
    }

    return domainName;
}
