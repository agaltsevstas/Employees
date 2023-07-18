#ifndef AUTHENTICATIONSERVICE_H
#define AUTHENTICATIONSERVICE_H

#include "qjsonwebtoken.h"

#include <QSet>

namespace Server
{
    class AuthenticationService : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(AuthenticationService)
    public:

        AuthenticationService(QObject* parent = nullptr);

        void authentication(const QString &iID, const QString &iUserName, const QString &iRole);
        void logout();

        qint64 getID() const;
        const QByteArray getUserName() const;
        const QByteArray getRole() const;
        const QByteArray getCookie();
        bool checkAuthentication(QByteArray &ioToken, qint64& oExp);

    private:
        QJsonWebToken createToken(const QString &iID, const QString &iUserName, const QString &iRole);
        void removeToken();

    private:
        int _selectedIndex = -1;
        QVector<QJsonWebToken> _tokens;
    };
}

#endif // AUTHENTICATIONSERVICE_H
