#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <QValidator>


class TextValidator final : public QValidator
{
    Q_OBJECT
public:
    explicit TextValidator(QObject *parent = nullptr);
    QValidator::State validate(QString &input, int&) const override;
};

class UInt64Validator final : public QValidator
{
    Q_OBJECT
public:
    enum class Mode
    {
        None = 0,
        Date,
        Passport,
        Phone
    };

    UInt64Validator(quint64 min, quint64 max, Mode mode = Mode::None, QObject *parent = nullptr);

    /*!
     * \brief Получить нижний предел
     * \return Нижний предел
     */
    inline quint64 bottom() const noexcept { return _min; }

    /*!
     * \brief Получить верхний предел
     * \return Верхний предел
     */
    inline quint64 top() const noexcept { return _max; }

    /*!
     * \brief Установить нижний предел
     * \param min - Нижний предел
     */
    void setBottom(quint64 min) noexcept;

    /*!
     * \brief Установить верхний предел
     * \param min - Верхний предел
     */
    void setTop(quint32 max) noexcept;

    /*!
     * \brief Устноавить пределы
     * \param min - Нижний предел
     * \param max - Верхний предел
     */
    void setRange(quint32 min, quint32 max) noexcept;

    /*!
     * \brief Проверка на валидность
     * \param input - Значение
     * \return true - значение валидно, иначе false
     */
    QValidator::State validate(QString &input, int&) const override;
private:
    Mode _mode;
    quint64 _min;
    quint64 _max;
    mutable QString _cache;
};

#endif // VALIDATOR_H
