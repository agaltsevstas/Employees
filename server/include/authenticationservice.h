#ifndef AUTHENTICATIONSERVICE_H
#define AUTHENTICATIONSERVICE_H

#include "qjsonwebtoken.h"


namespace Server
{
    class AuthenticationService
    {
        Q_DISABLE_COPY(AuthenticationService)
    public:

        AuthenticationService();

        void authentication(const QString &iID, const QString &iUserName, const QString &iRole);
        void logout();

        [[nodiscard]] qint64 getID() const;
        [[nodiscard]] const QByteArray getUserName() const;
        [[nodiscard]] const QByteArray getRole() const;
        [[nodiscard]] const QByteArray getCookie();
        bool checkAuthentication(QByteArray &ioToken, qint64& oExp);

    private:
        [[nodiscard]] QJsonWebToken createToken(const QString &iID, const QString &iUserName, const QString &iRole);
        void removeToken();

    private:
        int _selectedIndex = -1;
        QVector<QJsonWebToken> _tokens;
    };
}

#endif // AUTHENTICATIONSERVICE_H
