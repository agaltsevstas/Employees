#include "dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Client::Dialog dialog;
    dialog.show();
    return a.exec();
}
