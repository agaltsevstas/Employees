#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QPair>
#include <QList>

namespace Client
{
    class Employee
    {
    public:
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

        static QList<QPair<QString, QString>> getFileds()
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

        static QString employeeTable() { return "employee"; }
        static QString permissionTable() { return "permission"; }
        static QString personalDataPermissionTable() { return "personal_data_permission"; }
        static QString databasePermissionTable() { return "database_permission"; }

        static QList<QString> getTables()
        {
            return
            {
                employeeTable(),
                permissionTable(),
                personalDataPermissionTable(),
                databasePermissionTable()};
        }
    };
}

#endif // CLIENT_H
