#ifndef Utils_h
#define Utils_h

#pragma once

#include <QString>
#include <QVector>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <codecvt>

namespace Utils
{  
    /*!
     * @brief Перевод кирилицы латинскими буквами
     * @param iTextCyrillic - Кириллица
     * @return Латиница
     */
    QString Translit(const QString &iTextCyrillic);
    
    /*!
     * @brief Создание почты, используя фамилию, имя и отчество
     * @param iAnthroponym - Вектор антропононимов
     * @return Готовая почта
     */
    QString CreateEmail(const QVector<QString> &iAnthroponym);

    /*!
     * @brief Разбиение текста на части
     * @param iSource - Исходный текст
     * @param iDelim - Исходный текст
     */
    QStringList SplitString(QString iSource, const QString &iDelim);

    /*!
     * @brief Получение локального времени
     * @return - Локальное время
     */
    QString LocalTime();

    /*!
     * @brief Получение текущей даты
     * @return - Текущая дата
     */
    QString Date();
    
    /*!
     * @brief Получение возраста
     * @param iData - Вектор, хранящий текущую дату (день, месяц, год)
     * @param iDateOfBirth - Вектор, хранящий дату рождения (день, месяц, год)
     * @return - Найденный возраст
     */
    QVector<int> FindAge(const QVector<QString> &iData, const QVector<QString> &iDateOfBirth);

    /*!
     * @brief Конвертирование текста в прописные и строчные буквы
     * @param iSource - Исходный текст
     * @param iNumberUpper - Кол-во прописных букв в начале текста, остальные буквы переводятся в строчные
     * @return Готовый текст
     */
    QString ToUpperAndToLower(const QString& iSource, uint iNumberUpper = 1);

    /*!
     * @brief Конвертирование текста в прописные и строчные буквы
     * @param iSource - Исходный текст
     * @param iNumberUpper - Кол-во прописных букв в начале текста, остальные буквы переводятся в строчные
     */
    void ToUpperandtolower(QString &iSource, uint iNumberUpper = 1);

    /*!
     * @brief Форматирование текста в формат Postgres
     * @param iSource - Исходная дата
     * @param iNumberUpper - Кол-во прописных букв в начале текста, остальные буквы переводятся в строчные
     */
    void FormatDateToPostgres(QString &iData);
}

#endif // Utils_h
