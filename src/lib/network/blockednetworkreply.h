#ifndef QUPZILLA_BLOCKED_NETWORK_REPLY_H
#define QUPZILLA_BLOCKED_NETWORK_REPLY_H

#include "qzcommon.h"

#include <QtNetwork/QNetworkReply>

class QNetworkRequest;
class QString;

class QUPZILLA_EXPORT BlockedNetworkReply : public QNetworkReply
{
    Q_OBJECT
    typedef QNetworkReply Base;
public:
    BlockedNetworkReply(const QNetworkRequest&, const QString &reason);

    //@overide QNetworkReply
    virtual void abort() {}

protected:
    //@overide QNetworkReply
    virtual qint64 readData(char* data, qint64 maxSize);

private Q_SLOTS:
    void delayedFinished();
};

#endif // QUPZILLA_BLOCKED_NETWORK_REPLY_H
