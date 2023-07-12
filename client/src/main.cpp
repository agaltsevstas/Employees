#include "dialog.h"
#include "cache.h"

#include <QApplication>
#include <QTextCodec>
#include <Settings>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *utfCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(utfCodec);

    Cache::Instance();
    Settings::Instance();

    Client::Dialog dialog;
    dialog.show();
    return a.exec();
}
