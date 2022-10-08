#include "utils.h"

#include <boost/algorithm/string.hpp>

namespace Utils
{
    // Карта перевода из кириллицы в латиницу
    const std::map<std::string, std::string> translitSymbols =
    {
        {"а", "a"},
        {"б", "b"},
        {"в", "v"},
        {"г", "g"},
        {"д", "d"},
        {"е", "e"},
        {"ё", "e"},
        {"ж", "zh"},
        {"з", "z"},
        {"и", "i"},
        {"й", "j"},
        {"к", "k"},
        {"л", "l"},
        {"м", "m"},
        {"н", "n"},
        {"о", "o"},
        {"п", "p"},
        {"р", "r"},
        {"с", "s"},
        {"т", "t"},
        {"у", "u"},
        {"ф", "f"},
        {"х", "h"},
        {"ц", "ts"},
        {"ч", "ch"},
        {"ш", "sh"},
        {"щ", "sch"},
        {"ъ", ""},
        {"ы", "j"},
        {"ь", ""},
        {"э", "e"},
        {"ю", "ju"},
        {"я", "ja"},
    };

    std::string Translit(const std::string &iTextCyrillic)
    {
        std::string textLatin;
        for (size_t i = 0; i <= iTextCyrillic.length(); ++i)
        {
            char space = iTextCyrillic[i];
            std::string letter = iTextCyrillic.substr(i, 2); // Кириллица = 2 байта
            // Пробелы по-особенному считывает
            if (space == ' ')
                textLatin += " ";
            else if (translitSymbols.find(letter) != translitSymbols.end())
                textLatin += translitSymbols.find(letter)->second;
        }
        return textLatin;
    }
    
    std::string CreateEmail(const std::vector<std::string> &iAnthroponym)
    {
        std::string email;
        for (auto part: iAnthroponym) // Фамилия, имя, отчество
        {
            ToUpperandtolower(part, 0);
            email += Translit(part) + ".";
        }
        email.pop_back(); // Удаление лишней точки в конце
        return email += "@tradingcompany.ru";
    }
    
    std::vector<std::string> SplitString(std::string iSource, const std::string &iDelim)
    {
        std::vector<std::string> result;
        iSource.erase(remove(iSource.begin(), iSource.end(), ' '), iSource.end()); // Удаление пробелов
        boost::split(result, iSource, boost::is_any_of(iDelim));
        return result;
    }

    std::string LocalTime()
    {
        std::stringstream ss;
        time_t t = std::time(nullptr);
        auto tm = *localtime(&t);
        ss << std::put_time(&tm, "%d.%m.%Y_%H:%M:%S");
        return ss.str();
    }

    std::string Date()
    {
        std::stringstream ss;
        time_t t = std::time(nullptr);
        auto tm = *localtime(&t);
        ss << std::put_time(&tm, "%d.%m.%Y");
        return ss.str();
    }

    std::vector<int> FindAge(const std::vector<std::string> &iData, const std::vector<std::string> &iDateOfBirth)
    {
        uint currentDay = atoi(iData[0].c_str());
        uint currentMonth = atoi(iData[1].c_str());
        uint currentYear = atoi(iData[2].c_str());
        uint birthDay = atoi(iDateOfBirth[0].c_str());
        uint birthMonth = atoi(iDateOfBirth[1].c_str());
        uint birthYear = atoi(iDateOfBirth[2].c_str());

        const uint february = ((birthYear % 4) == 0) && (((birthYear % 100) != 0) || ((birthYear % 400) == 0)) ? 29 : 28;
        uint month[12] = { 31, february, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // Кол-во дней в 12 месяцах
        if (birthDay > currentDay)
        {
            currentDay += month[birthMonth - 1];
            currentMonth -= 1;
        }
        if (birthMonth > currentMonth)
        {
            currentYear -= 1;
            currentMonth += 12;
        }
        int calculatedDay = currentDay - birthDay;
        int calculatedMonth = currentMonth - birthMonth;
        int calculatedYear = currentYear - birthYear;
        const std::vector<int> age = { calculatedYear, calculatedMonth, calculatedDay };
        return age;
    }

    std::string ToUpperAndToLower(std::string iSource, uint iNumberUpper)
    {
        if (iSource.empty())
            return {};
        
        // Конвертирование кириллицы в wstd::string, чтобы перевести буквы нижнего регистра в верхний или обратно
        std::wstring wstr = UTF8ToWstring(iSource);
        for (size_t i = 0; i < iNumberUpper; ++i)
            wstr[i] = towupper(wstr[i]);
        transform(wstr.begin() + iNumberUpper, wstr.end(), wstr.begin() + iNumberUpper, std::towlower);
        // Обратное конвертирование кириллицы в std::string
        iSource = WstringToUtf8(wstr);
        return iSource;
    }

    void ToUpperandtolower(std::string &iSource, uint iNumberUpper)
    {
        if (iSource.empty())
            return;
        
        // Конвертирование кириллицы в wstd::string, чтобы перевести буквы нижнего регистра в верхний или обратно
        std::wstring wstr = UTF8ToWstring(iSource);
        for (size_t i = 0; i < iNumberUpper; ++i)
            wstr[i] = towupper(wstr[i]);
        transform(wstr.begin() + iNumberUpper, wstr.end(), wstr.begin() + iNumberUpper, std::towlower);
        // Обратное конвертирование кириллицы в std::string
        iSource = WstringToUtf8(wstr);
    }

    void FormatDateToPostgres(std::string &iData)
    {
        if (iData.empty() || iData.size() == 4)
            return;

        const char* delimiters = "./-";
        std::string day = std::strtok(const_cast<std::string&>(iData).data(), delimiters);
        std::string month = std::strtok(nullptr, delimiters);
        std::string year = std::strtok(nullptr, delimiters);

        iData = year + "-" + month + "-" + day;
    }
}
