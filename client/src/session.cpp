#include "session.h"
#include "cache.h"
#include "cookie.h"
#include "settings.h"
#include "logger.h"


Session::Session()
{
    Cache::Instance();
    Cookie::Instance();
    Settings::Instance();
    Logger::Instance();
    qInstallMessageHandler(Logger::messageHandler);
}

Session::~Session()
{

}

Session &Session::getSession()
{
    static Session data;
    return data;
}

Cache& Session::Cache()
{
    return Cache::Instance();
}

Cookie& Session::Cookie()
{
    return Cookie::Instance();
}

Settings& Session::Settings()
{
    return Settings::Instance();
}
