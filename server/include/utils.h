#ifndef Utils_h
#define Utils_h

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <codecvt>


#define LOGIN(object) Logger::info << "Вход в аккаунт >> " << object->GetPosition()   << " "        \
                                                           << object->GetSurname()    << " "        \
                                                           << object->GetName()       << " "        \
                                                           << object->GetPatronymic() << std::endl; \
                      std::cout << "Вы зашли за " << object->GetPosition() << "'a. " << std::endl;  \
                      std::cout << "Приветствуем вас, " << object->GetSurname()    << " "           \
                                                        << object->GetName()       << " "           \
                                                        << object->GetPatronymic() << "!"           \
                                                        << std::endl;
 
#define LOGOUT(object) Logger::info << "Выход из аккаунта << " << object->GetPosition()   << " "        \
                                                               << object->GetSurname()    << " "        \
                                                               << object->GetName()       << " "        \
                                                               << object->GetPatronymic() << std::endl; \
                       std::cout << std::endl << "Вы вышли из аккаунта" << std::endl;

#define EXIT(object) Logger::info << "Выход из аккаунта << " << object->GetPosition()   << " "        \
                                                             << object->GetSurname()    << " "        \
                                                             << object->GetName()       << " "        \
                                                             << object->GetPatronymic() << std::endl; \
                     Logger::info << "Выход из программы" << std::endl;                               \
                     std::cout << std::endl;                                                          \
                     std::cout << "Вы вышли из аккаунта" << std::endl;                                \
                     std::cout << "Вы вышли из программы" << std::endl;                               \
                     exit(0);

namespace Utils
{  
    /*!
     * @brief Перевод кирилицы латинскими буквами
     * @param iTextCyrillic - Кириллица
     * @return Латиница
     */
    std::string Translit(const std::string &iTextCyrillic);
    
    /*!
     * @brief Создание почты, используя фамилию, имя и отчество
     * @param iAnthroponym - Вектор антропононимов
     * @return Готовая почта
     */
    std::string CreateEmail(const std::vector<std::string> &iAnthroponym);
    
    /*!
     * @brief Хэширование текста
     * @param iSource - Исходный текст
     * @return Готовый текст
     */
    inline unsigned constexpr Hash(const char *iSource)
    {
        return *iSource ? static_cast<unsigned int>(*iSource) + 33 * Hash(iSource + 1) : 5381;
    }

    /*!
     * @brief Конвертирование прописных букв в строчные
     * @param iSource - Исходный текст
     * @return Готовый текст
     */
    inline std::string toLower(std::string iSource)
    {
        std::transform(iSource.begin(), iSource.end(), iSource.begin(), ::tolower);
        return iSource;
    }

    /*!
     * @brief Конвертирование прописных букв в строчные
     * @param iSource - Исходный текст
     */
    inline void tolower(std::string &iSource)
    {
        std::transform(iSource.begin(), iSource.end(), iSource.begin(), ::tolower);
    }

    /*!
     * @brief Разбиение текста на части
     * @param iSource - Исходный текст
     * @param iDelim - Исходный текст
     */
    std::vector<std::string> SplitString(std::string iSource, const std::string &iDelim);

    /*!
     * @brief Получение локального времени
     * @return - Локальное время
     */
    std::string LocalTime();

    /*!
     * @brief Получение текущей даты
     * @return - Текущая дата
     */
    std::string Date();
    
    /*!
     * @brief Получение возраста
     * @param iData - Вектор, хранящий текущую дату (день, месяц, год)
     * @param iDateOfBirth - Вектор, хранящий дату рождения (день, месяц, год)
     * @return - Найденный возраст
     */
    std::vector<int> FindAge(const std::vector<std::string> &iData, const std::vector<std::string> &iDateOfBirth);

    /*!
     * @brief Конвертирование wstd::string в UTF-8 std::string
     * @param iSource - Исходный текст
     * @return Готовый текст
     */
    inline std::string WstringToUtf8(const std::wstring &iSource)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
        return convert.to_bytes(iSource);
    }

    /*!
     * @brief Конвертирование UTF-8 std::string в wstd::string
     * @param iSource - Исходный текст
     * @return Готовый текст
     */
    inline std::wstring UTF8ToWstring(const std::string &iSource)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
        return convert.from_bytes(iSource);
    }

    /*!
     * @brief Конвертирование текста в прописные и строчные буквы
     * @param iSource - Исходный текст
     * @param iNumberUpper - Кол-во прописных букв в начале текста, остальные буквы переводятся в строчные
     * @return Готовый текст
     */
    std::string ToUpperAndToLower(std::string iSource, uint iNumberUpper = 1);

    /*!
     * @brief Конвертирование текста в прописные и строчные буквы
     * @param iSource - Исходный текст
     * @param iNumberUpper - Кол-во прописных букв в начале текста, остальные буквы переводятся в строчные
     */
    void ToUpperandtolower(std::string &iSource, uint iNumberUpper = 1);

    /*!
     * @brief Форматирование текста в формат Postgres
     * @param iSource - Исходная дата
     * @param iNumberUpper - Кол-во прописных букв в начале текста, остальные буквы переводятся в строчные
     */
    void FormatDateToPostgres(std::string &iData);

    /*!
     * @brief Получение имени класса в строковом виде
     * @param iObject - Объект класса
     * @return Имя класса
     */
    template <class C>
    inline std::string GetClassName(const C &iObject)
    {
        std::string source = typeid(iObject).name();
        // Удаление числа, которое указывает на кол-во символов
        source.erase(std::remove_if(std::begin(source), std::end(source), [](auto c) { return std::isdigit(c); }), source.end());
        return source;
    }
}

#endif // Utils_h
