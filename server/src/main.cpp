#include "database.h"

#include "httplistener.h"
#include "httpsessionstore.h"
#include "controller/requestmapper.h"

#include <QApplication>
#include <QTextCodec>

#define SERVER_HOSTNAME "127.0.0.1" // Хост
#define SERVER_PORT      5433       // Порт

HttpSessionStore* sessionStore;
Server::DataBase* db;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTextCodec *utfCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(utfCodec);

    db = new Server::DataBase();
    db->connectToDataBase();

    QSettings* lisnterSettings = new QSettings(&app);
    lisnterSettings->setValue("host", SERVER_HOSTNAME);
    lisnterSettings->setValue("port", SERVER_PORT);
    lisnterSettings->setValue("minThreads", "4");
    lisnterSettings->setValue("maxThreads", "100");
    lisnterSettings->setValue("cleanupInterval", "60000");
    lisnterSettings->setValue("readTimeout", "60000");
    lisnterSettings->setValue("maxRequestSize", "16000");
    lisnterSettings->setValue("maxMultiPartSize", "10000000");

    new HttpListener(lisnterSettings, new Server::RequestMapper(&app), &app);

    QSettings* sessionSettings = new QSettings(&app);
    lisnterSettings->setValue("expirationTime", "3600000");
    lisnterSettings->setValue("cookieName", "sessionid");
    lisnterSettings->setValue("cookiePath", "/");
    lisnterSettings->setValue("cookieComment", "Identifies the user");

    sessionStore = new HttpSessionStore(sessionSettings, &app);

    return app.exec();
}
