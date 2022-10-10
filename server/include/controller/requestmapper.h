/**
  @file
  @author Stefan Frings
*/

#ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include "httprequesthandler.h"
#include "database.h"

using namespace stefanfrings;

namespace Server
{
    /**
      The request mapper dispatches incoming HTTP requests to controller classes
      depending on the requested path.
    */

    class RequestMapper : public HttpRequestHandler {
        Q_OBJECT
        Q_DISABLE_COPY(RequestMapper)
    public:

        /**
          Constructor.
          @param parent Parent object
        */
        RequestMapper(DataBase &iDB, QObject* parent=0);

        /**
          Destructor.
        */
        ~RequestMapper();

        /**
          Dispatch incoming HTTP requests to different controllers depending on the URL.
          @param request The received HTTP request
          @param response Must be used to return the response
        */
        void service(HttpRequest& request, HttpResponse& response);

    private:
        void authorization(HttpRequest &request, HttpResponse &response);
        void showDatabase(HttpRequest &request, HttpResponse &response);

    private:
        DataBase& _db;
        QByteArray _token;
    };
}

#endif // REQUESTMAPPER_H
