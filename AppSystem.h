#ifndef APPSYSTEM_H
#define APPSYSTEM_H

#include "QString"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

class AppSystem {
  public:
    bool isEmpty(QString str);
    QString getSiteDomainName(QString url);
};

#endif // APPSYSTEM_H
