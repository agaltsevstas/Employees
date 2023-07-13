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

        static QString id()           { return "id"; }
        static QString role()         { return "role"; }
        static QString surname()      { return "surname"; }
        static QString name()         { return "name"; }
        static QString patronymic()   { return "patronymic"; }
        static QString sex()          { return "sex"; }
        static QString dateOfBirth()  { return "date_of_birth"; }
        static QString passport()     { return "passport"; }
        static QString phone()        { return "phone"; }
        static QString email()        { return "email"; }
        static QString dateOfHiring() { return "date_of_hiring"; }
        static QString workingHours() { return "working_hours"; }
        static QString salary()       { return "salary"; }
        static QString password()     { return "password"; }

        /*!
         * @brief Проверка полей на повреждение/перезапись данных
         * @param iWarning - Предупреждение о невалидности данных поля
         */

        static bool checkField(const QString &iField, QString &iValue);
        static QString employeeTable() { return "employee"; }
        static QString permissionTable() { return "permission"; }
        static QString personalDataPermissionTable() { return "personal_data_permission"; }
        static QString databasePermissionTable() { return "database_permission"; }

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

        /// Карта параметров и оберток инициализаций полей
        const QMap<QString, std::function<void(Employee&, const QString&)>> _setParameters =
        {
            {id(),           nullptr},
            {role(),         &Employee::SetRole},
            {surname(),      &Employee::SetSurname},
            {name(),         &Employee::SetName},
            {patronymic(),   &Employee::SetPatronymic},
            {sex(),          &Employee::SetSex},
            {dateOfBirth(),  &Employee::SetDateOfBirth},
            {passport(),     &Employee::SetPassport},
            {phone(),        &Employee::SetPhone},
            {email(),        &Employee::SetEmail},
            {dateOfHiring(), &Employee::SetDateOfHiring},
            {workingHours(), &Employee::SetWorkingHours},
            {salary(),       &Employee::SetSalary},
            {password(),     &Employee::SetPassword}
        };

        /// Карта полей с их статусом
        QMap<Field, Status> _fieldStatus
        {
            {FIELD_ID,             ST_EMPTY},
            {FIELD_ROLE,           ST_EMPTY},
            {FIELD_SURNAME,        ST_EMPTY},
            {FIELD_NAME,           ST_EMPTY},
            {FIELD_PATRONYMIC,     ST_EMPTY},
            {FIELD_SEX,            ST_EMPTY},
            {FIELD_DATE_OF_BIRTH,  ST_EMPTY},
            {FIELD_PASSPORT,       ST_EMPTY},
            {FIELD_PHONE,          ST_EMPTY},
            {FIELD_EMAIL,          ST_EMPTY},
            {FIELD_DATE_OF_HIRING, ST_EMPTY},
            {FIELD_WORKING_HOURS,  ST_EMPTY},
            {FIELD_SALARY,         ST_EMPTY},
            {FIELD_PASSWORD,       ST_EMPTY},
        };

    public:
        virtual ~Employee() {}

        /*!
         * @brief Перегрузка оператора = (присваивания).
         * Запись данных из одного объекта в другой
         * @param object - объект
         * @return Измененный объект
         */
        const Employee& operator = (const Employee &object) noexcept;

        /*!
         * @brief Перегрузка оператора << (вывода).
         * Вывод полей объекта
         * @param ioOut - поток вывода
         * @param object - объект
         * @return Поток вывода
         */
        friend QDataStream& operator << (QDataStream &ioOut, const Employee &object) noexcept;

        /*!
         * @brief Перегрузка оператора == (сравнения).
         * Сравнение двух объектов на равенство
         * @param first - первый объект
         * @param second - второй объект
         * @return Логическое значение
         */
        friend bool operator == (const Employee &first, const Employee &second) noexcept;

    private:
        uint32_t _id = 0;       /// ID
        QString _role;         /// Должность
        QString _surname;      /// Фамилия
        QString _name;         /// Имя
        QString _patronymic;   /// Отчество
        QString _sex;          /// Пол
        QString _dateOfBirth;  /// Дата рождения
        uint64_t _passport = 0; /// Паспорт
        uint64_t _phone = 0;    /// Телефон
        QString _email;        /// Почта
        QString _dateOfHiring; /// Дата принятия на работу
        QString _workingHours; /// Часы работы
        uint32_t _salary = 0;   /// Зарплата
        QString _password;     /// Пароль

        /*!
         * @brief Инициализация полей
         * @param Значение поля
         */
        void SetId(const QString &iID) noexcept;
        void SetRole(const QString &iRole) noexcept;
        void SetSurname(const QString &iSurname) noexcept;
        void SetName(const QString &iName) noexcept;
        void SetPatronymic(const QString &iPatronymic) noexcept;
        void SetSex(const QString &iSex) noexcept;
        void SetDateOfBirth(const QString &iDateOfBirth) noexcept;
        void SetPhone(const QString &iPhone) noexcept;
        void SetEmail(const QString &iEmail) noexcept;
        void SetDateOfHiring(const QString &iDateOfHiring) noexcept;
        void SetWorkingHours(const QString &iWorkingHours) noexcept;
        void SetPassport(const QString &iPassport) noexcept;
        void SetSalary(const QString &iSalary) noexcept;
        void SetPassword(const QString &iPassword) noexcept;

        /*!
         * @brief Изменение статуса полей на перезапись/дублирование данных
         */
        void ChangeStatusRole() noexcept;
        void ChangeStatusSurname() noexcept;
        void ChangeStatusName() noexcept;
        void ChangeStatusPatronymic() noexcept;
        void ChangeStatusSex() noexcept;
        void ChangeStatusDateOfBirth() noexcept;
        /// @param canOverwrite - true-перезапись/false-дублирование
        void ChangeStatusPassport(const bool canOverwrite = false) noexcept;
        /// @param canOverwrite - true-перезапись/false-дублирование
        void ChangeStatusPhone(const bool canOverwrite = false) noexcept;
        /// @param canOverwrite - true-перезапись/false-дублирование
        void ChangeStatusEmail(const bool canOverwrite) noexcept;
        void ChangeStatusDateOfHiring() noexcept;
        void ChangeStatusWorkingHours() noexcept;
        void ChangeStatusSalary() noexcept;
        void ChangeStatusPassword() noexcept;

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
