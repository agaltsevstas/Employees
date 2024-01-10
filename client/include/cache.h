#ifndef CACHE_H
#define CACHE_H

#include "QScopedPointer"


class QJsonDocument;

class Cache
{
    Q_DISABLE_COPY(Cache);
    friend class Session;

public:
    /*!
     * \brief Получение logins
     * \return logins
     */
    [[nodiscard]] QStringList getLogins() const;
    [[nodiscard]] QString getPassword(const QString &iLogin) const;

    /*!
     * \brief Поиск пользователя
     * \param iLogin - Логин
     * \param iPassword - Пароль
     * \return true - пользователь найден, false - пользователь не найден
     */
    bool findUser(const QString &iLogin, const QString &iPassword) const;

    /*!
     * \brief Добавление пользователя
     * \param iLogin - логин
     * \param iPassword - пароль
     */
    void addUser(const QString &iLogin, const QString &iPassword);

    /*!
     * \brief Получить слова для поиска
     * \return Слова для поиска
     */
    [[nodiscard]] QStringList getSearchWords() const;

    /*!
     * \brief Добавить слово для поиска
     * \param iWord - Слово для поиска
     */
    void addSearchWord(const QString &iWord);

    /*!
     * \brief Добавить слова для поиска
     * \param iWords - Слова для поиска
     */
    void addSearchWords(const QStringList &iWords);

private:
    static Cache &Instance() noexcept;
    Cache();
    ~Cache();

    bool findValue(const QString &iTable, const QString iKey, const QString &iValue) const;
    bool findValue(const QString &iTable, const QString &iValue) const;
    [[nodiscard]] QString getValue(const QString &iTable, const QString iFindKey, const QString &iFindValue) const;
    [[nodiscard]] QStringList getList(const QString &iTable, const QString &iKey) const;
    [[nodiscard]] QStringList getList(const QString &iTable) const;
    void insert(const QString &iTable, const QList<QPair<QString, QString>> &iUserData);
    void insert(const QString &iTable, const QString &iValue);

private:
    QScopedPointer<QJsonDocument> _cache;
};

#endif // CACHE_H
