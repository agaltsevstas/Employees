#ifndef Utils_h
#define Utils_h

#include <QVector>


namespace Utils
{  
    /*!
     * @brief Перевод кирилицы латинскими буквами
     * @param iTextCyrillic - Кириллица
     * @return Латиница
     */
    [[nodiscard]] QString Translit(const QString &iTextCyrillic) noexcept;
    
    /*!
     * @brief Создание почты, используя фамилию, имя и отчество
     * @param iAnthroponym - Вектор антропононимов
     * @return Готовая почта
     */
    [[nodiscard]] QString CreateEmail(const QVector<QString> &iAnthroponym) noexcept;

    /*!
     * @brief Разбиение текста на части
     * @param iSource - Исходный текст
     * @param iDelim - Исходный текст
     */
    [[nodiscard]] QStringList SplitString(QString iSource, const QString &iDelim) noexcept;

    /*!
     * @brief Получение локального времени
     * @return - Локальное время
     */
    [[nodiscard]] QString LocalTime() noexcept;

    /*!
     * @brief Получение текущей даты
     * @return - Текущая дата
     */
    [[nodiscard]] QString Date() noexcept;
    
    /*!
     * @brief Получение возраста
     * @param iData - Вектор, хранящий текущую дату (день, месяц, год)
     * @param iDateOfBirth - Вектор, хранящий дату рождения (день, месяц, год)
     * @return - Найденный возраст
     */
    [[nodiscard]] QVector<int> FindAge(const QVector<QString> &iData, const QVector<QString> &iDateOfBirth);

    /*!
     * @brief Конвертирование текста в прописные и строчные буквы
     * @param iSource - Исходный текст
     * @param iNumberUpper - Кол-во прописных букв в начале текста, остальные буквы переводятся в строчные
     * @return Готовый текст
     */
    [[nodiscard]] QString ToUpperAndToLower(const QString& iSource, uint iNumberUpper = 1) noexcept;

    /*!
     * @brief Конвертирование текста в прописные и строчные буквы
     * @param iSource - Исходный текст
     * @param iNumberUpper - Кол-во прописных букв в начале текста, остальные буквы переводятся в строчные
     */
    void ToUpperandtolower(QString &iSource, uint iNumberUpper = 1) noexcept;

    /*!
     * @brief Форматирование текста в формат Postgres
     * @param iSource - Исходная дата
     * @param iNumberUpper - Кол-во прописных букв в начале текста, остальные буквы переводятся в строчные
     */
    void FormatDateToPostgres(QString &iData);
}

#endif // Utils_h
