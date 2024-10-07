#ifndef CLIENT_H
#define CLIENT_H

#include <QMap>


namespace Client
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

        [[nodiscard("getFieldNames")]] inline static const QList<QPair<QString, QString>>& getFieldNames() noexcept
        {
            static QList<QPair<QString, QString>> fieldNames =
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

            return fieldNames;
        }

        [[nodiscard("helpFields")]] inline static QMap<QString, QString>& helpFields() noexcept
        {
            static QMap<QString, QString> helpFields =
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

            return helpFields;
        };

        [[nodiscard("getRoles")]] inline static const QStringList& getRoles() noexcept
        {
            static QStringList roles =
            {
                "Бухгалтер",
                "Водитель",
                "Главный бухгалтер",
                "Главный юрист-консультант",
                "Грузчик",
                "Директор",
                "Кассир",
                "Логист",
                "Менеджер по закупкам",
                "Менеджер по продажам",
                "Начальник отдела закупок",
                "Начальник склада",
                "Юрист",
                "Менеджер по персоналу"
            };

            return roles;
        }

        [[nodiscard("getSex")]] inline static const QStringList& getSex() noexcept
        {
            static QStringList sex = {"Муж", "Жен"};
            return sex;
        }

        inline static const QString id() noexcept           { static QString id = "id"; return id; }
        inline static const QString role() noexcept         { static QString role = "role"; return role; }
        inline static const QString surname() noexcept      { static QString surname = "surname"; return surname; }
        inline static const QString name() noexcept         { static QString name = "name"; return name; }
        inline static const QString patronymic() noexcept   { static QString patronymic = "patronymic"; return patronymic; }
        inline static const QString sex() noexcept          { static QString sex = "sex"; return sex; }
        inline static const QString dateOfBirth() noexcept  { static QString date_of_birth = "date_of_birth"; return date_of_birth; }
        inline static const QString passport() noexcept     { static QString passport = "passport"; return passport; }
        inline static const QString phone() noexcept        { static QString phone = "phone"; return phone; }
        inline static const QString email() noexcept        { static QString email = "email"; return email; }
        inline static const QString dateOfHiring() noexcept { static QString date_of_hiring = "date_of_hiring"; return date_of_hiring; }
        inline static const QString workingHours() noexcept { static QString working_hours = "working_hours"; return working_hours; }
        inline static const QString salary() noexcept       { static QString salary = "salary"; return salary; }
        inline static const QString password() noexcept     { static QString password = "password"; return password; }

        static bool checkField(const QString& iField, QString& iValue);
        inline static const QString& employeeTable() noexcept { static QString employee = "employee"; return employee; }
        inline static const QString& permissionTable() noexcept { static QString permission = "permission"; return permission; }
        inline static const QString& personalDataPermissionTable() noexcept { static QString personal_data_permission = "personal_data_permission"; return personal_data_permission; }
        inline static const QString& databasePermissionTable() noexcept { static QString database_permission = "database_permission"; return database_permission; }

        [[nodiscard("getTables")]] inline static const QList<QString>& getTables() noexcept
        {
            static QList<QString> tables =
            {
                employeeTable(),
                permissionTable(),
                personalDataPermissionTable(),
                databasePermissionTable()
            };

            return tables;
        }

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
        static const Type _checkField(QString& iValue, const Field iField);
    };
}

#endif // CLIENT_H
