/**
  @file
  @author Stefan Frings
*/

#include <QCoreApplication>
#include "controller/requestmapper.h"

namespace Server
{
    RequestMapper::RequestMapper(DataBase &iDB, QObject* parent) : HttpRequestHandler(parent), _db(iDB)
    {
        qDebug("RequestMapper: created");
    }


    RequestMapper::~RequestMapper()
    {
        qDebug("RequestMapper: deleted");
    }


    void RequestMapper::service(HttpRequest& request, HttpResponse& response)
    {
        QByteArray path = request.getPath();
        qDebug("RequestMapper: path=%s", path.data());

        if (path.startsWith("/login"))
        {
            Authorization(request, response);
        }
        // All other pathes are mapped to the static file controller.
        // In this case, a single instance is used for multiple requests.
        else
        {
    //        staticFileController->service(request, response);
        }

        qDebug("RequestMapper: finished request");
    }

    void RequestMapper::Authorization(HttpRequest &request, HttpResponse &response)
    {
        QByteArray authentication = request.getHeader("Authorization");
        if (authentication.isNull())
        {
            qInfo("User is not logged in");
            response.setStatus(401, "Unauthorized");
            response.setHeader("WWW-Authenticate", "Basic realm=Please login with any name and password");
        }
        else
        {
//            QByteArray decoded = QByteArray::fromBase64(authentication.mid(6)); // Skip the first 6 characters ("Basic ")
            QList<QByteArray> parts = authentication.mid(6).split(':');
            if (parts.size() == 2)
            {
                QByteArray userName = parts[0];
                QByteArray password = parts[1];
                QByteArray data;
                if (_db.authentication(userName, password, data))
                {
                    response.setHeader("Content-Type", "application/json");
                    response.write(data, true);
                }
            }
        }
    }
}
