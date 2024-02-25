#ifndef COOKIE_H
#define COOKIE_H

#include <QMap>


class Cookie final
{
    Q_DISABLE_COPY_MOVE(Cookie);
    friend class Session;

public:
    /*!
     * \brief Проверка одного из токенов access или refresh на валидацию
     * \return true - хотя бы один из токенов валиден, false - все токены невалидны
     */
    bool isValid() const;

    /*!
     * \brief Получение хотя бы одного валидного токена в приоритете: 1 access token, 2 refresh token
     * \return Значение валидного токена, иначе empty
     */
    [[nodiscard("getValidToken")]] QString getValidToken();

    /*!
     * \brief Добавление refresh или access token
     * \param iKey - Название: refresh или acess token
     * \param iValue - Значение: refresh или acess token
     */
    void addToken(const QString& iKey, const QString& iValue) noexcept;

    /*!
     * \brief Получение username из токена
     * \return username
     */
    [[nodiscard("getUserName")]] QString getUserName() const;

    /*!
     * \brief Получить данные Coockie
     * \return данные Coockie
     */
    [[nodiscard("get")]] inline const QMap<QString, QString>& get() const noexcept { return _data; }

    /*!
     * \brief Очистить Coockie
     */
    void clear() noexcept;

    /*!
     * \brief Проверить Cookie на пустоту
     * \return true - Cookie пустые, иначе нет
     */
    bool isEmpty() const noexcept;

private:
    static Cookie& Instance() noexcept;
    Cookie();
    ~Cookie();

    QMap<QString, QString> _data;
};

#endif // COOKIE_H
