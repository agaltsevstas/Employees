#include "utils.h"

#include <QDateTime>
#include <QRegularExpression>


namespace Utils
{
    // Карта перевода из кириллицы в латиницу
    static const QMap<QString, QString> translitSymbols =
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

    QString Translit(const QString &iTextCyrillic) noexcept
    {
        QString textLatin;
        for (const auto& letter : iTextCyrillic)
        {
            if (auto latinLetter = translitSymbols.find(letter); latinLetter != translitSymbols.end())
                textLatin += latinLetter.value();
        }
        return textLatin;
    }
    
    QString CreateEmail(const QVector<QString> &iAnthroponym) noexcept
    {
        QString email;
        for (auto part: iAnthroponym) // Фамилия, имя, отчество
        {
            ToUpperandtolower(part, 0);
            email += Translit(part) + ".";
        }
        email.chop(1);
        return email += "@tradingcompany.ru";
    }
    
    QStringList SplitString(QString iSource, const QString &iDelim) noexcept
    {
        return iSource.simplified().split(QRegularExpression("[" + iDelim + "]"));
    }

    QString LocalTime() noexcept
    {
        return QDateTime::currentDateTime().toString("yyyy.MM.dd_hh:mm:ss");
    }

    QString Date() noexcept
    {
        return QDateTime::currentDateTime().toString("yyyy.mm.dd");
    }

    QVector<int> FindAge(const QVector<QString> &iData, const QVector<QString> &iDateOfBirth)
    {
        uint currentYear = iData.at(0).toUInt();
        uint currentMonth = iData.at(1).toUInt();
        uint currentDay = iData.at(2).toUInt();
        uint birthDay = iDateOfBirth.at(0).toUInt();
        uint birthMonth = iDateOfBirth.at(1).toUInt();
        uint birthYear = iDateOfBirth.at(2).toUInt();

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
        const QVector<int> age = { calculatedYear, calculatedMonth, calculatedDay };
        return age;
    }

    QString ToUpperAndToLower(const QString& iSource, uint iNumberUpper) noexcept
    {
        if (iSource.isEmpty())
            return {};

        return iSource.left(iNumberUpper) + iSource.right(iNumberUpper + 1).toLower();
    }

    void ToUpperandtolower(QString &iSource, uint iNumberUpper) noexcept
    {
        const auto size = iSource.size();
        if (!size)
            return;
        
        iSource = iSource.left(iNumberUpper).toUpper() + iSource.right(size - iNumberUpper).toLower();
    }

    void FormatDateToPostgres(QString &iData)
    {
        if (iData.isEmpty() || iData.size() == 4)
            return;

        QStringList result = iData.split(QRegularExpression("[./-]"));
        if (result.size() != 3)
            return;

        iData = result[2] + "-" + result[1] + "-" + result[0];
    }
}
