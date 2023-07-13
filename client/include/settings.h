#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
    Q_OBJECT

private:
    static Settings &Instance() noexcept;
    Settings(const QString &fileName, Format format, QObject *parent = nullptr);
    ~Settings();

private:
    Q_DISABLE_COPY(Settings)

    friend class Session;
};

#endif // SETTINGS_H