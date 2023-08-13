#ifndef SESSION_H
#define SESSION_H

#include "qglobal.h"


class Session
{
    Q_DISABLE_COPY(Session);

public:
    static Session &getSession();
    class Cache& Cache();
    class Cookie& Cookie();
    class Settings& Settings();

private:
    Session();
    ~Session();
};

#endif // SESSION_H
