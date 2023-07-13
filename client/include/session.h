#ifndef SESSION_H
#define SESSION_H


class Session
{
public:
    static Session &getSession();
    class Cache& Cache();
    class Settings& Settings();

private:
    Session();
    ~Session();

    /*!
     * @brief Запрет копирования
     */
    Session(const Session&) = delete;

    /*!
     * @brief Запрет оператора присваивания
     */
    Session& operator=(Session&) = delete;
};

#endif // SESSION_H
