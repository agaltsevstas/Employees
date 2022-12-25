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
        static QString ID()           { return "id"; }
        static QString Role()         { return "role"; }
        static QString Surname()      { return "surname"; }
        static QString Name()         { return "name"; }
        static QString Patronymic()   { return "patronymic"; }
        static QString Sex()          { return "sex"; }
        static QString DateOfBirth()  { return "date_of_birth"; }
        static QString Passport()     { return "passport"; }
        static QString Phone()        { return "phone"; }
        static QString Email()        { return "email"; }
        static QString DateOfHiring() { return "date_of_hiring"; }
        static QString WorkingHours() { return "working_hours"; }
        static QString Salary()       { return "salary"; }
        static QString Password()     { return "password"; }

        static QList<QString> getTables()
        {
            return {"employee",
                    "permission",
                    "personal_data_permission"
                    "database_permission"};
        }

        static QList<QPair<QString, QString> > getFileds()
        {
            return
            {
                { ID(),           "ID: "},
                { Role(),         "Должность: "},
                { Surname(),      "Фамилия: "},
                { Name(),         "Имя: "},
                { Patronymic(),   "Отчество: "},
                { Sex(),          "Пол: "},
                { DateOfBirth(),  "Дата рождения: "},
                { Passport(),     "Паспорт: "},
                { Phone(),        "Телефон: "},
                { Email(),        "Почта: "},
                { DateOfHiring(), "Дата принятия на работу: "},
                { WorkingHours(), "Часы работы: "},
                { Salary(),       "Зарплата: "},
                { Password(),     "Пароль: "}
            };
        }
    };
}

#endif // CLIENT_H
