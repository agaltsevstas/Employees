#ifndef CLIENT_H
#define CLIENT_H

#include <QMap>


namespace Server
{
    class Employee
    {
        enum Field
        {
            FIELD_ID = 0,
            FIELD_ROLE,
            FIELD_SURNAME,
            FIELD_NAME,
            FIELD_PATRONYMIC,
            FIELD_SEX,
            FIELD_DATE_OF_BIRTH,
            FIELD_PASSPORT,
            FIELD_PHONE,
            FIELD_EMAIL,
            FIELD_DATE_OF_HIRING,
            FIELD_WORKING_HOURS,
            FIELD_SALARY,
            FIELD_PASSWORD,
        };

        /// Карта полей с их доступом
        static const QMap<QString, Field> checkParameters;

        enum Status
        {
            ST_OK = 0,
            ST_OVERWRITEDATA,
            ST_EMPTY,
            ST_WRONGDATA,
            ST_DUBLICATE
        };

    public:

        [[nodiscard]] inline static const QList<QPair<QString, QString>> getFieldNames() noexcept
        {
            return
            {
                { id(),           "ID: "},
                { role(),         "Должность: "},
                { surname(),      "Фамилия: "},
                { name(),         "Имя: "},
                { patronymic(),   "Отчество: "},
                { sex(),          "Пол: "},
                { dateOfBirth(),  "Дата рождения: "},
                { passport(),     "Паспорт: "},
                { phone(),        "Телефон: "},
                { email(),        "Почта: "},
                { dateOfHiring(), "Дата принятия на работу: "},
                { workingHours(), "Часы работы: "},
                { salary(),       "Зарплата: "},
                { password(),     "Пароль: "}
            };
        }

        [[nodiscard]] inline static const QMap<QString, QString> helpFields() noexcept
        {
            return
            {
                { id(),           "Введите ID (например, 100): "},
                { role(),         "Введите должность: "},
                { surname(),      "Введите фамилию (буквы кириллицы алфавит): "},
                { name(),         "Введите имя (буквы кириллицы алфавит): "},
                { patronymic(),   "Введите отчество (буквы кириллицы алфавит): "},
                { sex(),          "Введите пол: "},
                { dateOfBirth(),  "Введите дату рождения (например, 1995-12-16): "},
                { passport(),     "Введите номер паспорта (например, 4516-561974): "},
                { phone(),        "Введите номер телефона (например, 903-269-79-63): "},
                { email(),        "Введите почту (например, surname.name.patronymic@tradingcompany.ru): "},
                { dateOfHiring(), "Введите дату принятия на работу (например, 2018-04-16): "},
                { workingHours(), "Введите время работы\n(например, "
                                  "Понедельник=09:00-18:00 или "
                                  "Понедельник-Пятница=09:00-18:00 или "
                                  "Понедельник=09:00-18:00;Пятница=09:00-18:00): "},
                { salary(),       "Введите зарплату (в рублях): "},
                { password(),     "Введите новый пароль\n(пароль должен содержать:\n-не менее 6 символов\n"
                                  "-хотя бы одну прописную латинскую букву\n"
                                  "-хотя бы одну строчную латинскую букву\n"
                                  "-хотя бы одну цифру): "}
            };
        };

        static const QString id() noexcept           { return "id"; }
        static const QString role() noexcept         { return "role"; }
        static const QString surname() noexcept      { return "surname"; }
        static const QString name() noexcept         { return "name"; }
        static const QString patronymic() noexcept   { return "patronymic"; }
        static const QString sex() noexcept          { return "sex"; }
        static const QString dateOfBirth() noexcept  { return "date_of_birth"; }
        static const QString passport() noexcept     { return "passport"; }
        static const QString phone() noexcept        { return "phone"; }
        static const QString email() noexcept        { return "email"; }
        static const QString dateOfHiring() noexcept { return "date_of_hiring"; }
        static const QString workingHours() noexcept { return "working_hours"; }
        static const QString salary() noexcept       { return "salary"; }
        static const QString password() noexcept     { return "password"; }

        static bool checkField(const QString &iField, QString &iValue);
        static const QString employeeTable() noexcept { return "employee"; }
        static const QString roleTable() noexcept { return "role"; }
        static const QString permissionTable() noexcept { return "permission"; }
        static const QString personalDataPermissionTable() noexcept { return "personal_data_permission"; }
        static const QString databasePermissionTable() noexcept { return "database_permission"; }

    private:

        struct Type
        {
            Status status;
            uint32_t uintValue = 0;
            uint64_t uint64Value = 0;
            QString stringValue;
        };

    private:

        /*!
         * @details Валидация данных.
         * Проверка данных каждого поля в соответствии с требованиями
         * в зависимости от номера поля. Значение поля устанавливается
         * в одно из полей струтуры данных (uint32_t/uint64_t/string).
         * Устанавливается статус поля (ST_EMPTY/ST_WRONGDATA/ST_OK)
         * @param iValue - Значение поля
         * @param iField - Номер поля
         * @return Структура данных
         */
        static const Type _checkField(QString &iValue, const Field iField);
    };
}

#endif // CLIENT_H
