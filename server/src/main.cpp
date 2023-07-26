#include "database.h"
#include "httpserver.h"
#include "logger.h"

#include <QApplication>
#include <QTextCodec>

#define SERVER_HOSTNAME "127.0.0.1" // Хост
#define SERVER_PORT      5433       // Порт

Server::DataBase* db;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTextCodec *utfCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(utfCodec);

    Logger::Instance();
    qInstallMessageHandler(Logger::messageHandler);

    db = new Server::DataBase();
    db->connect();

    Server::HttpServer::Start(&app);

    int exec = app.exec();

    delete db;
    return exec;
}
