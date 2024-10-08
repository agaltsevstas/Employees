#include "cookie.h"
#include "qjsonwebtoken.h"

#include <QDir>


namespace
{
    constexpr auto FILENAME = "cookies.txt";
    constexpr auto DIRECTORY = "../settings/";
}


Cookie::Cookie()
{
    QDir directory(DIRECTORY);
    if (!directory.exists())
        directory.mkpath(".");

    QFile file(QString(DIRECTORY) + FILENAME);
    file.open(QFile::ReadOnly | QFile::Text);
    if (file.isOpen())
    {
        qInfo() << "Чтение Cookie";
        QTextStream stream(&file);
        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            QStringList tokens = line.split(" ");
            if (tokens.size() == 2)
            {
                QString key = tokens.at(0).left(tokens.at(0).length() - 1);
                QString value = tokens.at(1).mid(1, tokens.at(1).length() - 2);
                _data[key] = value;
            }
        }
        file.close();
    }
    else
    {
        qWarning() << "Файл Cookie не открыт";
    }
}

Cookie::~Cookie()
{
    if (_data.empty())
        return;

    QFile file(QString(DIRECTORY) + FILENAME);
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
    {
        qWarning() << "Файл Cookie не открыт";
        return;
    }

    qInfo() << "Запись Cookie";
    QTextStream out(&file);
    for (const auto& name : _data.keys())
        out << name << ": \"" << _data.value(name) << "\"" << Qt::endl;

    file.close();
}

Cookie& Cookie::Instance() noexcept
{
    static Cookie data;
    return data;
}

bool Cookie::isValid() const
{
    QReadLocker lock(&_mutex);
    for (const auto& name : _data.keys())
    {
        if (QJsonWebToken token = QJsonWebToken::fromToken(_data.value(name)); token.isValid())
        {
            return true;
        }
    }

    return false;
}

QString Cookie::getValidToken()
{
    QReadLocker lock(&_mutex);
    for (const auto& name : _data.keys())
    {
        if (QJsonWebToken token = QJsonWebToken::fromToken(_data.value(name)); token.isValid())
        {
            return token.getToken();
        }
    }

    return {};
}

void Cookie::addToken(const QString& iKey, const QString& iValue) noexcept
{
    QWriteLocker lock(&_mutex);
    _data[iKey] = iValue;
}

QString Cookie::getUserName() const
{
    QReadLocker lock(&_mutex);
    for (const auto& name : _data.keys())
    {
        if (QJsonWebToken token = QJsonWebToken::fromToken(_data.value(name)); token.isValid())
        {
            return token.getUserName();
        }
    }

    return {};
}

void Cookie::clear() noexcept
{
    QWriteLocker lock(&_mutex);
    _data.clear();
}

bool Cookie::isEmpty() const noexcept
{
    QReadLocker lock(&_mutex);
    return _data.empty();
}
