#ifndef AUTHENTICATIONSERVICE_H
#define AUTHENTICATIONSERVICE_H

#include <QString>


namespace Server
{
    class AuthenticationService
    {
        Q_DISABLE_COPY(AuthenticationService)

    public:

        AuthenticationService() = default;
        ~AuthenticationService() = default;

        /*!
         * \brief Выход
         */
        void logout() noexcept;

        /*!
         * \brief Устнановить id пользователя
         * \param iID пользователя
         */
        void setID(qint64 iID) noexcept;

        /*!
         * \brief Получить id пользователя
         * \return ID пользователя
         */
        [[nodiscard("getID")]] qint64 getID() const noexcept;

        /*!
         * \brief Устноавить имя пользователя
         * \param iUserName - Имя пользователя
         */
        void setUserName(const QString& iUserName) noexcept;

        /*!
         * \brief Получить имя пользователя
         * \return Имя пользователя
         */
        [[nodiscard("getUserName")]] const QByteArray getUserName() const noexcept;

        /*!
         * \brief Установить роль пользователя
         * \param iRole - Роль пользователя
         */
        void setRole(const QString& iRole) noexcept;

        /*!
         * \brief Получить роль пользователя
         * \return Роль пользователя
         */
        [[nodiscard("getRole")]] const QByteArray getRole() const noexcept;

        /*!
         * \brief Получить access token
         * \return access token
         */
        [[nodiscard("getAccessToken")]] const QByteArray getAccessToken();

        /*!
         * \brief Получить refresh token
         * \return
         */
        [[nodiscard("getRefreshToken")]] const QByteArray getRefreshToken();

        /*!
         * \brief Проверка аутентификации
         * \param ioToken - access/refresh token
         * \return
         */
        bool checkAuthentication(QByteArray& ioToken);

    private:
        qint64 _id;
        QString _userName;
        QString _role;
    };
}

#endif // AUTHENTICATIONSERVICE_H
