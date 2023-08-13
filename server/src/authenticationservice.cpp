#include "authenticationservice.h"
#include "qjsonwebtoken.h"

#define JWT_ACCESS_SECRET "jwt-access-secret-key"
#define JWT_REFRESH_SECRET "jwt-refresh-secret-key"


namespace Server
{
    class HttpCookie
    {
    public:
        /**
          Create a cookie and set name/value pair.
          @param name name of the cookie
          @param value value of the cookie
          @param maxAge maximum age of the cookie in seconds. 0=discard immediately
          @param path Path for that the cookie will be sent, default="/" which means the whole domain
          @param comment Optional comment, may be displayed by the web browser somewhere
          @param domain Optional domain for that the cookie will be sent. Defaults to the current domain
          @param secure If true, the cookie will be sent by the browser to the server only on secure connections
          @param httpOnly If true, the browser does not allow client-side scripts to access the cookie
          @param sameSite Declare if the cookie can only be read by the same site, which is a stronger
                 restriction than the domain. Allowed values: "Lax" and "Strict".
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

        QByteArray toByteArray()
        {
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

    AuthenticationService::AuthenticationService()
    {

    }

    void AuthenticationService::logout()
    {
        _id = 0;
        _userName.clear();
        _role.clear();
    }

    void AuthenticationService::setID(qint64 iID)
    {
        _id = iID;
    }

    qint64 AuthenticationService::getID() const
    {
        return _id;
    }

    void AuthenticationService::setUserName(const QString &iUserName)
    {
        _userName = iUserName;
    }

    const QByteArray AuthenticationService::getUserName() const
    {
        return _userName.toUtf8();
    }

    void AuthenticationService::setRole(const QString &iRole)
    {
        _role = iRole;
    }

    const QByteArray AuthenticationService::getRole() const
    {
        return _role.toUtf8();
    }

    const QByteArray AuthenticationService::getAccessToken()
    {
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
                          "Lax").toByteArray();
    }

    const QByteArray AuthenticationService::getRefreshToken()
    {
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
                          "Lax").toByteArray();
    }

    bool AuthenticationService::checkAuthentication(QByteArray &ioToken)
    {
        if (QJsonWebToken accessToken = QJsonWebToken::fromTokenAndSecret(ioToken, JWT_ACCESS_SECRET); accessToken.isValid())
        {
            _id = accessToken.getID();
            _userName = accessToken.getUserName();
            _role = accessToken.getRole();
            return true;
        }
        else if (QJsonWebToken refreshToken = QJsonWebToken::fromTokenAndSecret(ioToken, JWT_REFRESH_SECRET); refreshToken.isValid())
        {
            _id = refreshToken.getID();
            _userName = refreshToken.getUserName();
            _role = refreshToken.getRole();
            return true;
        }

        return false;
    }
}
