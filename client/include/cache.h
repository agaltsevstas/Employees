#ifndef CACHE_H
#define CACHE_H

#include "QString"
#include "QScopedPointer"

class QJsonDocument;

class Cache
{
public:

    [[nodiscard]] QStringList getLogins() const;
    [[nodiscard]] QString getPassword(const QString &iLogin) const;
    bool findUser(const QString &iLogin, const QString &iPassword) const;
    void addUser(const QString &iLogin, const QString &iPassword);
    [[nodiscard]] QStringList getSearchWords() const;
    void addSearchWord(const QString &iWord);
    void addSearchWords(const QStringList &iWords);

private:
    static Cache &Instance() noexcept;
    Cache();
    ~Cache();

    /*!
     * @brief Запрет копирования
     */
    Cache(const Cache&) = delete;

    /*!
     * @brief Запрет оператора присваивания
     */
    Cache& operator=(Cache&) = delete;

    bool findValue(const QString &iTable, const QString iKey, const QString &iValue) const;
    bool findValue(const QString &iTable, const QString &iValue) const;
    [[nodiscard]] QString getValue(const QString &iTable, const QString iFindKey, const QString &iFindValue) const;
    [[nodiscard]] QStringList getList(const QString &iTable, const QString &iKey) const;
    [[nodiscard]] QStringList getList(const QString &iTable) const;
    void insert(const QString &iTable, const QList<QPair<QString, QString>> &iUserData);
    void insert(const QString &iTable, const QString &iValue);

private:
    QScopedPointer<QJsonDocument> _cache;

    friend class Session;
};

#endif // CACHE_H
