#include "database.h"

#include "httplistener.h"
#include "controller/requestmapper.h"

#include <QApplication>

#define SERVER_HOSTNAME "127.0.0.1" // Хост
#define SERVER_PORT      5433       // Порт

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Server::DataBase db;
    db.connectToDataBase();

    QSettings* settings = new QSettings(&app);
    settings->setValue("host", SERVER_HOSTNAME);
    settings->setValue("port", SERVER_PORT);
    settings->setValue("minThreads", "4");
    settings->setValue("maxThreads", "100");
    settings->setValue("cleanupInterval", "60000");
    settings->setValue("readTimeout", "60000");
    settings->setValue("maxRequestSize", "16000");
    settings->setValue("maxMultiPartSize", "10000000");

    new HttpListener(settings, new Server::RequestMapper(db, &app), &app);

    return app.exec();
}
