#ifndef CLIENT_H
#define CLIENT_H

#pragma once

#include <QMap>
#include <QPair>
#include <QList>
#include <QString>


namespace Client
{
    class Employee
    {
        /// Номера полей класса Employee
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

        /// Статус данных
        enum Status
        {
            /// Успех
            ST_OK = 0,

            /// Перезаписывание данных
            ST_OVERWRITEDATA,

            /// Пусто
            ST_EMPTY,

            /// Проблема с данными
            ST_WRONGDATA,

            /// Дублирование данных
            ST_DUBLICATE
        };

    public:

        [[nodiscard]] inline static QList<QPair<QString, QString>> getFieldNames() noexcept
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

        [[nodiscard]] inline static QMap<QString, QString> helpFields() noexcept
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

        [[nodiscard]] inline static QStringList getRoles() noexcept
        {
            return
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
        }

        [[nodiscard]] inline static QStringList getSex() noexcept
        {
            return
            {
                "Муж",
                "Жен"
            };
        }

        static QString id() noexcept           { return "id"; }
        static QString role() noexcept         { return "role"; }
        static QString surname() noexcept      { return "surname"; }
        static QString name() noexcept         { return "name"; }
        static QString patronymic() noexcept   { return "patronymic"; }
        static QString sex() noexcept          { return "sex"; }
        static QString dateOfBirth() noexcept  { return "date_of_birth"; }
        static QString passport() noexcept     { return "passport"; }
        static QString phone() noexcept        { return "phone"; }
        static QString email() noexcept        { return "email"; }
        static QString dateOfHiring() noexcept { return "date_of_hiring"; }
        static QString workingHours() noexcept { return "working_hours"; }
        static QString salary() noexcept       { return "salary"; }
        static QString password() noexcept     { return "password"; }

        /*!
         * @brief Проверка полей на повреждение/перезапись данных
         * @param iWarning - Предупреждение о невалидности данных поля
         */

        static bool checkField(const QString &iField, QString &iValue);
        static QString employeeTable() noexcept { return "employee"; }
        static QString permissionTable() noexcept { return "permission"; }
        static QString personalDataPermissionTable() noexcept { return "personal_data_permission"; }
        static QString databasePermissionTable() noexcept { return "database_permission"; }

        [[nodiscard]] static QList<QString> getTables() noexcept
        {
            return
            {
                employeeTable(),
                permissionTable(),
                personalDataPermissionTable(),
                databasePermissionTable()
            };
        }

    private:
        /*!
         * @brief Структура данных
         */
        struct Type
        {
            /// Статус данных
            Status status;

            /*!
             * @brief Тип данных
             */

            /// 32-разрядное целое значение без знака
            uint32_t uintValue = 0;

            /// 64-разрядное целое значение без знака
            uint64_t uint64Value = 0;

            /// Строковое значение
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
