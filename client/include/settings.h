#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>


class Settings final : public QSettings
{
    Q_OBJECT
    Q_DISABLE_COPY(Settings)
    friend class Session;

private:
    static Settings& Instance() noexcept;
    Settings(const QString& fileName, const Format& format, QObject* parent = nullptr);
    ~Settings();
};

#endif // SETTINGS_H
