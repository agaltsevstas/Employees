#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
    Q_OBJECT

public:
    static Settings &Instance() noexcept;

private:
    Settings(const QString &fileName, Format format, QObject *parent = nullptr);
    ~Settings();

private:
    Q_DISABLE_COPY(Settings)
};

#endif // SETTINGS_H
