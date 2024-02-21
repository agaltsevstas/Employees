#include "dialog.h"
#include "session.h"

#include <QApplication>
#include <QTextCodec>
#include <Requester>

Client::Requester* requester;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(utfCodec);

    Session::getSession();

    requester = new Client::Requester();

    Client::Dialog dialog;
    dialog.show();
    return app.exec();
}
