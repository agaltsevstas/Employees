#include "cache.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#define FILENAME  "cache.json"
#define DIRECTORY "../settings/"


Cache::Cache() : _cache(new QJsonDocument())
{
    QDir directory(DIRECTORY);
    if (!directory.exists())
        directory.mkpath(".");

    QFile file(QString(DIRECTORY) + FILENAME);
    file.open(QFile::ReadOnly | QFile::Text);
    if (file.isOpen())
    {
        const QJsonDocument document = _cache->fromJson(file.readAll());
        if (document.isArray())
            _cache->setArray(document.array());
        file.close();
    }
    else
    {
        qDebug() << "File Cache is not open";
    }
}

Cache::~Cache()
{
    QFile file(QString(DIRECTORY) + FILENAME);
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
    {
        qDebug() << "File Cache is not open";
        return;
    }

    file.write(_cache->toJson());
    file.close();
}

Cache &Cache::Instance() noexcept
{
    static Cache data; // Объект-одиночка
    return data;
}

QStringList Cache::getLogins() const
{
    return getList("Authorization", "Login");
}

QString Cache::getPassword(const QString &iLogin) const
{
    return getValue("Authorization", "Password", iLogin);
}

bool Cache::findUser(const QString &iLogin, const QString &iPassword) const
{
    return findValue("Authorization", "Login", iLogin) && !findValue("Authorization", "Password", iPassword);
}

void Cache::addUser(const QString &iLogin, const QString &iPassword)
{
    QList<QPair<QString, QString>> userData = {{"Login",    iLogin},
                                               {"Password", iPassword}};
    insert("Authorization", userData);
}

QStringList Cache::getSearchWords() const
{
    return getList("Search");
}

void Cache::addSearchWord(const QString &iWord)
{
    if (!findValue("Search", iWord))
        insert("Search", iWord);
}

void Cache::addSearchWords(const QStringList &iWords)
{
    for (const auto &word : iWords)
        if (!findValue("Search", word))
            addSearchWord(word);
}

void Cache::insert(const QString &iTable, const QList<QPair<QString, QString>> &iUserData)
{
    if (iUserData.size() != 2)
        return;

    if (_cache->isArray())
    {
        QJsonArray newArray = _cache->array();
        for (qsizetype i = 0, I = newArray.count(); i < I; ++i)
        {
            if (newArray[i].isObject() && newArray[i].toObject().contains(iTable))
            {
                QJsonObject newObject = newArray[i].toObject();
                for (const auto &values : newObject)
                {
                    if (values.isArray())
                    {
                        bool isFound = false;
                        QJsonArray newValues = values.toArray();
                        for (qsizetype j = 0, J = newValues.count(); j < J; ++j)
                        {
                            if (newValues[j].isObject())
                            {
                                QList<QPair<QString, QString>> userData;
                                QJsonObject newValue = newValues[j].toObject();
                                for (auto itNewValue = newValue.begin(); itNewValue != newValue.end(); ++itNewValue)
                                    userData.emplace_back(itNewValue.key(), itNewValue.value().toString());

                                if (userData == iUserData)
                                    return;
                                else if (userData.size() == iUserData.size() &&
                                         userData.front() == iUserData.front())
                                {
                                    QJsonObject newValue;
                                    for (const auto &[key, value] : iUserData)
                                        newValue.insert(key, value);

                                    newValues.replace(j, std::move(newValue));
                                    newObject = {{ std::move(iTable), std::move(newValues)}};
                                    newArray.replace(i, std::move(newObject));
                                    _cache->setArray(std::move(newArray));
                                    return;
                                }
                            }
                        }

                        if (!isFound)
                        {
                            QJsonObject newValue;
                            for (const auto &[key, value] : iUserData)
                                newValue.insert(key, value);

                            newValues.append(std::move(newValue));
                            newObject = {{ std::move(iTable), std::move(newValues)}};
                            newArray.replace(i, std::move(newObject));
                            _cache->setArray(std::move(newArray));
                            return;
                        }
                    }
                }
            }
        }

        QJsonObject newValue;
        for (const auto &[key, value] : iUserData)
            newValue.insert(key, value);

        QJsonArray newValues = { std::move(newValue) };
        QJsonObject newObject = {{ std::move(iTable), std::move(newValues)}};
        newArray.append(std::move(newObject));

        _cache->setArray(std::move(newArray));
    }
}

void Cache::insert(const QString &iTable, const QString &iValue)
{
    if (_cache->isArray())
    {
        bool isFound = false;
        QJsonArray newArray = _cache->array();
        for (qsizetype i = 0, I = newArray.count(); i < I; ++i)
        {
            if (newArray[i].isObject() && newArray[i].toObject().contains(iTable))
            {
                isFound = true;
                QJsonObject newObject = newArray[i].toObject();
                for (const auto &values : newObject)
                {
                    if (values.isArray())
                    {
                        QJsonArray newValues = values.toArray();
                        if (!newValues.contains(iValue))
                        {
                            newValues.append(std::move(iValue));
                            newObject = {{std::move(iTable), std::move(newValues)}};
                            newArray.replace(i, std::move(newObject));
                            _cache->setArray(std::move(newArray));
                            return;
                        }
                    }
                }
            }
        }

        if (!isFound)
        {
            QJsonArray values = {std::move(iValue)};
            QJsonObject table = {{std::move(iTable), std::move(values)}};
            newArray.append(std::move(table));
        }

        _cache->setArray(std::move(newArray));
    }
}

bool Cache::findValue(const QString &iTable, const QString iKey, const QString &iValue) const
{
    if (_cache->isArray())
    {
        for (const auto &table: _cache->array())
        {
            if (table.isObject() && table.toObject().contains(iTable))
            {
                for (const auto &array : table.toObject())
                {
                    if (array.isArray())
                    {
                        for (const auto &value: array.toArray())
                        {
                            if (value.isObject() && value.toObject().contains(iKey))
                            {
                                if (value.toObject().value(iKey).toString() == iValue)
                                    return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool Cache::findValue(const QString &iTable, const QString &iValue) const
{
    if (_cache->isArray())
    {
        for (const auto &table: _cache->array())
        {
            if (table.isObject() && table.toObject().contains(iTable))
            {
                for (const auto &array : table.toObject())
                {
                    if (array.isArray())
                    {
                        for (const auto &value: array.toArray())
                        {
                            if (value.isString() && value.toString() == iValue)
                                return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

QString Cache::getValue(const QString &iTable, const QString iFindKey, const QString &iFindValue) const
{
    if (_cache->isArray())
    {
        for (const auto &table: _cache->array())
        {
            if (table.isObject() && table.toObject().contains(iTable))
            {
                for (const auto &array : table.toObject())
                {
                    if (array.isArray())
                    {
                        for (const auto &value: array.toArray())
                        {
                            if (value.isObject())
                            {
                                for (const auto &val : value.toObject())
                                {
                                    if (val.isString() && val.toString() == iFindValue)
                                        return value.toObject().value(iFindKey).toString();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return {};
}

QStringList Cache::getList(const QString &iTable, const QString &iKey) const
{
    QStringList list;
    if (_cache->isArray())
    {
        for (const auto &object: _cache->array())
        {
            if (object.isObject() && object.toObject().contains(iTable))
            {
                for (const auto &array : object.toObject())
                {
                    if (array.isArray())
                    {
                        for (const auto &value: array.toArray())
                        {
                            if (value.isObject() && value.toObject().contains(iKey))
                                list.emplace_back(value.toObject().value(iKey).toString());
                        }
                    }
                }
            }
        }
    }

    return list;
}

QStringList Cache::getList(const QString &iTable) const
{
    QStringList list;
    if (_cache->isArray())
    {
        for (const auto &object: _cache->array())
        {
            if (object.isObject() && object.toObject().contains(iTable))
            {
                for (const auto &array : object.toObject())
                {
                    if (array.isArray())
                    {
                        for (const auto &value: array.toArray())
                        {
                            if (value.isString())
                                list.emplace_back(value.toString());
                        }
                    }
                }
            }
        }
    }

    return list;
}


