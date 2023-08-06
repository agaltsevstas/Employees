#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>


namespace Server
{
    class HttpServer : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(HttpServer)

    public:
        static void Start(QObject* parent = nullptr);
    private:
        HttpServer(QObject* parent = nullptr);
        ~HttpServer();
    private:
        class HttpServerImpl;
        QScopedPointer<HttpServerImpl> _server;
        friend class HttpServerImpl;
        template <class TCallBack> friend class AuthorizationService;
    };
}

#endif // HTTPSERVER_H
