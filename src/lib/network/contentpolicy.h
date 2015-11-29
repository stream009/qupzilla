#ifndef QUPZILLA_CONTENT_POLICY_H
#define QUPZILLA_CONTENT_POLICY_H

#include "qzcommon.h"

#include <QtNetwork/QNetworkRequest>

class WebPage;

class QUPZILLA_EXPORT ContentPolicy
{
public:
    virtual bool shouldLoad(
        const QNetworkRequest &aRequest,
        const WebPage* const aOriginPage,
        const bool fromObject) const = 0;

    virtual QString reason() const = 0;
};

#endif // QUPZILLA_CONTENT_POLICY_H
