#ifndef CACHE_H
#define CACHE_H

#include "QString"
#include "QScopedPointer"

class QJsonDocument;

class Cache
{
public:
    static Cache &Instance() noexcept;

    QStringList getLogins() const;
    QString getPassword(const QString &iLogin) const;
    bool findUser(const QString &iLogin, const QString &iPassword) const;
    void addUser(const QString &iLogin, const QString &iPassword);
    QStringList getSearchWords() const;
    void addSearchWord(const QString &iWord);
    void addSearchWords(const QStringList &iWords);

private:
    Cache();
    ~Cache();

    bool findValue(const QString &iTable, const QString iKey, const QString &iValue) const;
    bool findValue(const QString &iTable, const QString &iValue) const;
    QString getValue(const QString &iTable, const QString iFindKey, const QString &iFindValue) const;
    QStringList getList(const QString &iTable, const QString &iKey) const;
    QStringList getList(const QString &iTable) const;
    void insert(const QString &iTable, const QList<QPair<QString, QString>> &iUserData);
    void insert(const QString &iTable, const QString &iValue);

    /*!
     * @brief Запрет копирования
     */
    Cache(const Cache&) = delete;

    /*!
     * @brief Запрет оператора присваивания
     */
    Cache& operator=(Cache&) = delete;

private:
    QScopedPointer<QJsonDocument> _cache;
};

#endif // CACHE_H
