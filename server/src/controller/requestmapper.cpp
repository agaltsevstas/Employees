#include "controller/requestmapper.h"

#include "controller/authorizationcontroller.h"
#include "httpsessionstore.h"

extern HttpSessionStore* sessionStore;

namespace Server
{
    RequestMapper::RequestMapper(QObject* parent) :
        HttpRequestHandler(parent),
        _authorizationController(new AuthorizationController(parent))
    {
        qDebug("RequestMapper: created");
    }

    RequestMapper::~RequestMapper()
    {
        qDebug("RequestMapper: deleted");
    }

    void RequestMapper::service(HttpRequest& request, HttpResponse& response)
    {
        HttpSession session = sessionStore->getSession(request, response);
        qDebug() << "Session ID: " << session.getId();

        _authorizationController->service(request, response);

        qDebug("RequestMapper: finished request");
    }
}
