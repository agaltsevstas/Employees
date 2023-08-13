#ifndef AUTHENTICATIONSERVICE_H
#define AUTHENTICATIONSERVICE_H

#include <QString>

namespace Server
{
    class AuthenticationService
    {
        Q_DISABLE_COPY(AuthenticationService)
    public:

        AuthenticationService();
        void logout();

        void setID(qint64 iID);
        [[nodiscard]] qint64 getID() const;
        void setUserName(const QString &iUserName);
        [[nodiscard]] const QByteArray getUserName() const;
        void setRole(const QString &iRole);
        [[nodiscard]] const QByteArray getRole() const;
        [[nodiscard]] const QByteArray getAccessToken();
        [[nodiscard]] const QByteArray getRefreshToken();
        bool checkAuthentication(QByteArray &ioToken);

    private:
        qint64 _id;
        QString _userName;
        QString _role;
    };
}

#endif // AUTHENTICATIONSERVICE_H
