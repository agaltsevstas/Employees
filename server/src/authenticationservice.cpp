#include "authenticationservice.h"


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


    AuthenticationService::AuthenticationService(QObject* parent) :
        QObject(parent)
    {

    }

    void AuthenticationService::authentication(const QString &iID, const QString &iUserName, const QString &iRole)
    {
        createToken(iID, iUserName, iRole);
    }

    void AuthenticationService::logout()
    {
        removeToken();
    }

    qint64 AuthenticationService::getID() const
    {
        if (_selectedIndex < 0)
            return {};

        return _tokens[_selectedIndex].getID();
    }

    const QByteArray AuthenticationService::getUserName() const
    {
        if (_selectedIndex < 0)
            return {};

        return _tokens[_selectedIndex].getUserName().toUtf8();
    }

    const QByteArray AuthenticationService::getRole() const
    {
        if (_selectedIndex < 0)
            return {};

        return _tokens[_selectedIndex].getRole().toUtf8();
    }

    const QByteArray AuthenticationService::getCookie()
    {
        return HttpCookie("refreshToken",
                          _selectedIndex < 0 ? "" : _tokens[_selectedIndex].getToken(),
                          _selectedIndex < 0 ? 0 :_tokens[_selectedIndex].getExp() / 1000,
                          "/",
                          "jwt",
                          "",
                          true,
                          false,
                          "Lax").toByteArray();
    }

    bool AuthenticationService::checkAuthentication(QByteArray &ioToken, qint64& oExp)
    {
        for (decltype(_tokens.size()) i = 0; i < _tokens.size(); ++i)
        {
            if (_tokens[i].getToken() == ioToken)
            {
                oExp = _tokens[i].getExp();
                _selectedIndex = i;
                return true;
            }
        }

        return false;
    }

    QJsonWebToken AuthenticationService::createToken(const QString &iID, const QString &iUserName, const QString &iRole)
    {
        QJsonWebToken newToken;
        newToken.appendClaim("id", iID);
        newToken.appendClaim("username", iUserName);
        newToken.appendClaim("role", iRole);
        newToken.appendClaim("iat", QString::number(QDateTime::currentSecsSinceEpoch()));
        newToken.appendClaim("exp", QString::number(QDateTime::currentDateTime().addDays(7).toMSecsSinceEpoch()));
        newToken.setRandomSecret();
        _selectedIndex = _tokens.size();
        return _tokens.emplaceBack(newToken);
    }

    void AuthenticationService::removeToken()
    {
        if (_selectedIndex < 0)
        {
            return;
        }
        else if (_selectedIndex == 0 && _tokens.size() == 1)
        {
            _tokens.clear();
            _selectedIndex = -1;
        }
        else
        {
            _tokens.remove(_selectedIndex);
        }
    }
}
