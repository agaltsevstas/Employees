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
        ~AuthenticationService() = default;

        /*!
         * \brief Выход
         */
        void logout();

        /*!
         * \brief Устнановить id пользователя
         * \param iID пользователя
         */
        void setID(qint64 iID);

        /*!
         * \brief Получить id пользователя
         * \return ID пользователя
         */
        [[nodiscard]] qint64 getID() const;

        /*!
         * \brief Устноавить имя пользователя
         * \param iUserName - Имя пользователя
         */
        void setUserName(const QString &iUserName);

        /*!
         * \brief Получить имя пользователя
         * \return Имя пользователя
         */
        [[nodiscard]] const QByteArray getUserName() const;

        /*!
         * \brief Установить роль пользователя
         * \param iRole - Роль пользователя
         */
        void setRole(const QString &iRole);

        /*!
         * \brief Получить роль пользователя
         * \return Роль пользователя
         */
        [[nodiscard]] const QByteArray getRole() const;

        /*!
         * \brief Получить access token
         * \return access token
         */
        [[nodiscard]] const QByteArray getAccessToken();

        /*!
         * \brief Получить refresh token
         * \return
         */
        [[nodiscard]] const QByteArray getRefreshToken();

        /*!
         * \brief Проверка аутентификации
         * \param ioToken - access/refresh token
         * \return
         */
        bool checkAuthentication(QByteArray &ioToken);

    private:
        qint64 _id;
        QString _userName;
        QString _role;
    };
}

#endif // AUTHENTICATIONSERVICE_H
