#include "dialog.h"

#include <QApplication>
//#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QTextCodec *utfcodec = QTextCodec::codecForName("UTF-8");
//    QTextCodec::setCodecForLocale(utfcodec);

    Client::Dialog dialog;
    dialog.show();
    return a.exec();
}
