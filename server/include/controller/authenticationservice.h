#ifndef AUTHENTICATIONSERVICE_H
#define AUTHENTICATIONSERVICE_H

#include "httprequest.h"
#include "httpresponse.h"
#include "qjsonwebtoken.h"

#include <QSet>

namespace Server
{
    class AuthenticationService : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(AuthenticationService)
    public:

        AuthenticationService(QObject* parent);

        stefanfrings::HttpCookie authentication(const QString& iID, const QString& iUserName, const QString& iRole);

        void logout();
        QString getRole();
        bool checkAuthentication(const QByteArray& iToken);

    private:
        QJsonWebToken createToken(const QString& iID, const QString& iUserName, const QString& iRole);
        void removeToken();

    private:
        int _selectedIndex = -1;
        QVector<QJsonWebToken> _tokens;
    };
}

#endif // AUTHENTICATIONSERVICE_H
