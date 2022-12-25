#include "controller/authenticationservice.h"

#include "QtWebApp/httpserver/httpcookie.h"


namespace Server
{
    AuthenticationService::AuthenticationService(QObject* parent) :
        QObject(parent)
    {

    }

    stefanfrings::HttpCookie AuthenticationService::authentication(const QString& iID, const QString& iUserName, const QString& iRole)
    {
        QJsonWebToken newToken = createToken(iID, iUserName, iRole);
        return stefanfrings::HttpCookie("refreshToken",
                                        newToken.getToken(),
                                        newToken.getExp() / 1000,
                                        "/",
                                        "jwt",
                                        "",
                                        true,
                                        false,
                                        "Lax");
    }

    void AuthenticationService::logout()
    {
        removeToken();
    }

    QString AuthenticationService::getRole()
    {
        if (_selectedIndex < 0)
            return {};

        return _tokens[_selectedIndex].getRole();
    }

    bool AuthenticationService::checkAuthentication(const QByteArray &iToken)
    {
        for (decltype(_tokens.size()) i = 0; i < _tokens.size(); ++i)
        {
            if (_tokens[i].getToken() == iToken)
            {
                _selectedIndex = i;
                return true;
            }
        }

        return false;
    }

    QJsonWebToken AuthenticationService::createToken(const QString& iID, const QString& iUserName, const QString& iRole)
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
            return;

        _tokens.remove(_selectedIndex);
    }
}
