#include "authenticationservice.h"
#include "qjsonwebtoken.h"

#define JWT_ACCESS_SECRET "jwt-access-secret-key"
#define JWT_REFRESH_SECRET "jwt-refresh-secret-key"

#define INFO(object, str) qInfo() << "[" + QString::number(object.getID()) + " " + object.getUserName() + " " + object.getRole() + "] " + str;
#define WARNING(object, str) qWarning() << "[" + QString::number(object.getID()) + " " + object.getUserName() + " " + object.getRole() + "] " + str;
#define CRITICAL(object, str) qCritical() << "[" + QString::number(object.getID()) + " " + object.getUserName() + " " + object.getRole() + "] " + str;


namespace Server
{
    class HttpCookie
    {
    public:
    /*!
     * @brief создание Coockie
     * @param name - Имя куки
     * @param value - Значение файла cookie
     * @param maxAge - Максимальный срок жизни cookie в секундах, по-умолчанию 0
     * @param path - Путь, по которому будет отправлен файл cookie, по умолчанию = "/", что означает весь домен.
     * @param comment - Необязательный комментарий, который может где-то отображаться веб-браузером
     * @param domain - Необязательный домен, для которого будет отправлен файл cookie. По умолчанию текущий домен
     * @param secure - Если true, браузер будет отправлять cookie на сервер только при безопасном соединении.
     * @param httpOnly - Если установлено значение true, браузер не разрешает клиентским сценариям доступ к файлу cookie.
     * @param sameSite - Может ли файл cookie быть прочитан только одним и тем же сайтом, что является более сильным ограничением, чем домен. Допустимые значения: «Нестрогий» и «Строгий».
     */
        HttpCookie(const QByteArray iName, const QByteArray iValue, const int iMaxAge,
                   const QByteArray iPath = "/", const QByteArray iComment = QByteArray(),
                   const QByteArray iDomain = QByteArray(), const bool iSecure = false,
                   const bool iHttpOnly = false, const QByteArray iSameSite = QByteArray())
        {
            _name = iName;
            _value = iValue;
            _maxAge = iMaxAge;
            _path = iPath;
            _comment = iComment;
            _domain = iDomain;
            _secure = iSecure;
            _httpOnly = iHttpOnly;
            _sameSite = iSameSite;
            _version = 1;
        }

        QByteArray toByteArray(const AuthenticationService &parent) const
        {
            INFO(parent, "Создание access token");
            QByteArray buffer(_name);
            buffer.append('=');
            buffer.append(_value);
            if (!_comment.isEmpty())
            {
                buffer.append("; Comment=");
                buffer.append(_comment);
            }
            if (!_domain.isEmpty())
            {
                buffer.append("; Domain=");
                buffer.append(_domain);
            }
            if (_maxAge != 0)
            {
                buffer.append("; Max-Age=");
                buffer.append(QByteArray::number(_maxAge));
            }
            if (!_path.isEmpty())
            {
                buffer.append("; Path=");
                buffer.append(_path);
            }
            if (_secure)
            {
                buffer.append("; Secure");
            }
            if (_httpOnly)
            {
                buffer.append("; HttpOnly");
            }
            if (!_sameSite.isEmpty())
            {
                buffer.append("; SameSite=");
                buffer.append(_sameSite);
            }
            buffer.append("; Version=");
            buffer.append(QByteArray::number(_version));
            return buffer;
        }

    private:
        QByteArray _name;
        QByteArray _value;
        QByteArray _comment;
        QByteArray _domain;
        int _maxAge;
        QByteArray _path;
        bool _secure;
        bool _httpOnly;
        QByteArray _sameSite;
        int _version;
    };

    void AuthenticationService::logout() noexcept
    {
        INFO((*this), "Выход");
        _id = 0;
        _userName.clear();
        _role.clear();
    }

    void AuthenticationService::setID(qint64 iID) noexcept
    {
        _id = iID;
    }

    qint64 AuthenticationService::getID() const noexcept
    {
        return _id;
    }

    void AuthenticationService::setUserName(const QString &iUserName) noexcept
    {
        _userName = iUserName;
    }

    const QByteArray AuthenticationService::getUserName() const noexcept
    {
        return _userName.toUtf8();
    }

    void AuthenticationService::setRole(const QString &iRole) noexcept
    {
        _role = iRole;
    }

    const QByteArray AuthenticationService::getRole() const noexcept
    {
        return _role.toUtf8();
    }

    const QByteArray AuthenticationService::getAccessToken()
    {
        INFO((*this), "Создание access token");
        QJsonWebToken accessToken;
        accessToken.appendClaim("id", QString::number(_id));
        accessToken.appendClaim("username", _userName);
        accessToken.appendClaim("role", _role);
        accessToken.appendClaim("iat", QString::number(QDateTime::currentDateTime().toSecsSinceEpoch()));
        accessToken.appendClaim("exp", QString::number(QDateTime::currentDateTime().addSecs(60).toSecsSinceEpoch()));
        accessToken.setSecret(JWT_ACCESS_SECRET);

        return HttpCookie("accessToken",
                          accessToken.getToken(),
                          accessToken.getExp(),
                          "/",
                          "jwt",
                          "",
                          true,
                          true,
                          "Lax").toByteArray(*this);
    }

    const QByteArray AuthenticationService::getRefreshToken()
    {
        INFO((*this), "Создание refresh token");
        QJsonWebToken refreshToken;
        refreshToken.appendClaim("id", QString::number(_id));
        refreshToken.appendClaim("username", _userName);
        refreshToken.appendClaim("role", _role);
        refreshToken.appendClaim("iat", QString::number(QDateTime::currentDateTime().toSecsSinceEpoch()));
        refreshToken.appendClaim("exp", QString::number(QDateTime::currentDateTime().addSecs(360).toSecsSinceEpoch()));
        refreshToken.setSecret(JWT_REFRESH_SECRET);

        return HttpCookie("refreshToken",
                          refreshToken.getToken(),
                          refreshToken.getExp(),
                          "/",
                          "jwt",
                          "",
                          true,
                          true,
                          "Lax").toByteArray(*this);
    }

    bool AuthenticationService::checkAuthentication(QByteArray &ioToken)
    {
        if (QJsonWebToken accessToken = QJsonWebToken::fromTokenAndSecret(ioToken, JWT_ACCESS_SECRET); accessToken.isValid())
        {
            INFO((*this), "Проверка access token");
            _id = accessToken.getID();
            _userName = accessToken.getUserName();
            _role = accessToken.getRole();
            return true;
        }
        else if (QJsonWebToken refreshToken = QJsonWebToken::fromTokenAndSecret(ioToken, JWT_REFRESH_SECRET); refreshToken.isValid())
        {
            INFO((*this), "Проверка access token");
            _id = refreshToken.getID();
            _userName = refreshToken.getUserName();
            _role = refreshToken.getRole();
            return true;
        }

        CRITICAL((*this), "Аутентификации не пройдена");
        return false;
    }
}
