#include "settings.h"

#define DIRECTORY "../settings/"


Settings::Settings(const QString &fileName, Format format, QObject *parent) :
    QSettings(fileName, format, parent)
{

}

Settings::~Settings()
{

}

Settings &Settings::Instance() noexcept
{
    static Settings data(DIRECTORY + QString("settings.ini"), QSettings::IniFormat); // Объект-одиночка
    return data;
}
