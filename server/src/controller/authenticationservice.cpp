#include "controller/authenticationservice.h"

#include "QtWebApp/httpserver/httpcookie.h"


namespace Server
{
    AuthenticationService::AuthenticationService(QObject* parent) :
        QObject(parent)
    {

    }

    void AuthenticationService::authentication(const QString &iID, const QString &iUserName, const QString &iRole, QByteArray &oToken, qint64& oExp)
    {
        QJsonWebToken token = createToken(iID, iUserName, iRole);
        oToken = token.getToken();
        oExp = token.getExp();
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
            return;

        _tokens.remove(_selectedIndex);
    }
}
