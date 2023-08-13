#include "cookie.h"
#include "qjsonwebtoken.h"

#include <QDir>
#include <QFile>

#define FILENAME  "cookies.txt"
#define DIRECTORY "../settings/"


Cookie::Cookie()
{
    QDir directory(DIRECTORY);
    if (!directory.exists())
        directory.mkpath(".");

    QFile file(QString(DIRECTORY) + FILENAME);
    file.open(QFile::ReadOnly | QFile::Text);
    if (file.isOpen())
    {
        qDebug() << "File Cookie is open";
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
        qDebug() << "File Cache is not open";
    }
}

Cookie::~Cookie()
{
    QFile file(QString(DIRECTORY) + FILENAME);
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
    {
        qDebug() << "File Cache is not open";
        return;
    }

    QTextStream out(&file);
    for (const auto& name : _data.keys())
        out << name << ": \"" << _data.value(name) << "\"" << Qt::endl;

    file.close();
}

Cookie &Cookie::Instance() noexcept
{
    static Cookie data; // Объект-одиночка
    return data;
}

bool Cookie::isValid() const
{
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
    for (const auto& name : _data.keys())
    {
        if (QJsonWebToken token = QJsonWebToken::fromToken(_data.value(name)); token.isValid())
        {
            return token.getToken();
        }
    }

    return {};
}

void Cookie::add(const QString &iKey, const QString &iValue) noexcept
{
    _data[iKey] = iValue;
}

QString Cookie::getUserName() const
{
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
    _data.clear();
}

bool Cookie::isEmpty() const noexcept
{
    return _data.empty();
}
