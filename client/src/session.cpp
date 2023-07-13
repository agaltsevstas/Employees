#include "session.h"
#include "cache.h"
#include "settings.h"
#include "logger.h"

Session::Session()
{
    Cache::Instance();
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

class Cache& Session::Cache()
{
    return Cache::Instance();
}

class Settings& Session::Settings()
{
    return Settings::Instance();
}
