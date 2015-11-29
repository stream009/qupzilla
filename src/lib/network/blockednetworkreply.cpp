#include "blockednetworkreply.h"

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkRequest>

BlockedNetworkReply::
BlockedNetworkReply(const QNetworkRequest &request, const QString &reason)
{
    Base::setRequest(request);
    Base::setUrl(request.url());
    Base::setError(QNetworkReply::ContentAccessDenied, reason);

    Base::open(QIODevice::ReadOnly);

    QTimer::singleShot(0, this, SLOT(delayedFinished()));
}

qint64 BlockedNetworkReply::
readData(char* data, qint64 maxSize)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    return -1;
}

void BlockedNetworkReply::
delayedFinished()
{
    Q_EMIT error(QNetworkReply::ContentAccessDenied);
    Q_EMIT finished();
}
