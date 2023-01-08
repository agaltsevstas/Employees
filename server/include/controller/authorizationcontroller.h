#ifndef AUTHORIZATIONCONTROLLER_H
#define AUTHORIZATIONCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"


using namespace stefanfrings;

namespace Server
{
    class AuthenticationService;
    class AuthorizationController : public HttpRequestHandler
    {
        Q_OBJECT
        Q_DISABLE_COPY(AuthorizationController)

    public:

        /** Constructor */
        AuthorizationController(QObject* parent);

        /** Generates the response */
        void service(HttpRequest &request, HttpResponse &response);

    private:
        bool authorization(HttpRequest &iRequest, HttpResponse &iResponse);
        void login(HttpResponse &iResponse);
        void logout(HttpResponse &iResponse);
        void updatePersonalData(HttpResponse &iResponse);
        void showDatabase(HttpResponse &iResponse);

    private:
        AuthenticationService *_authenticationService;
        template <class TFunction> friend class Permission;
    };
}

#endif // AUTHORIZATIONCONTROLLER_H
