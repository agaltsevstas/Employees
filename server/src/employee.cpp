#include "employee.h"
#include "utils.h"

#include <regex>
#include <QSqlError>
#include <QSqlRecord>
#include <QRegularExpression>

namespace Server
{
    const QMap<QString, Employee::Field> Employee::checkParameters =
    {
        { Employee::id(),           Employee::Field::FIELD_ID},
        { Employee::role(),         Employee::Field::FIELD_ROLE},
        { Employee::surname(),      Employee::Field::FIELD_SURNAME},
        { Employee::name(),         Employee::Field::FIELD_NAME},
        { Employee::patronymic(),   Employee::Field::FIELD_PATRONYMIC},
        { Employee::sex(),          Employee::Field::FIELD_SEX},
        { Employee::dateOfBirth(),  Employee::Field::FIELD_DATE_OF_BIRTH},
        { Employee::passport(),     Employee::Field::FIELD_PASSPORT},
        { Employee::phone(),        Employee::Field::FIELD_PHONE},
        { Employee::email(),        Employee::Field::FIELD_EMAIL},
        { Employee::dateOfHiring(), Employee::Field::FIELD_DATE_OF_HIRING},
        { Employee::workingHours(), Employee::Field::FIELD_WORKING_HOURS},
        { Employee::salary(),       Employee::Field::FIELD_SALARY},
        { Employee::password(),     Employee::Field::FIELD_PASSWORD}
    };

    const Employee& Employee::operator = (const Employee &object) noexcept
    {
        _surname = object._surname;
        _name = object._name;
        _patronymic = object._patronymic;
        _sex = object._sex;
        _dateOfBirth = object._dateOfBirth;
        _passport = object._passport;
        _phone = object._phone;
        _email = object._email;
        _dateOfHiring = object._dateOfHiring;
        _workingHours = object._workingHours;
        _salary = object._salary;
        _password = object._password;

        // Передача статусов полей от объекта к *this
        for(const auto& key : _fieldStatus.keys())
            this->_fieldStatus[key] = object._fieldStatus[key];

        return *this;
    }

    QDataStream& operator << (QDataStream &ioOut, const Employee &object) noexcept
    {
        ioOut << "ID: "                      << object._id           << ", ";
        ioOut << "Должность: "               << object._role         << ", ";
        ioOut << "Фамилия: "                 << object._surname      << ", ";
        ioOut << "Имя: "                     << object._name         << ", ";
        ioOut << "Отчество: "                << object._patronymic   << ", ";
        ioOut << "Пол: "                     << object._sex          << ", ";
        ioOut << "Дата рождения: "           << object._dateOfBirth  << ", ";
        ioOut << "Паспорт: "                 << object._passport     << ", ";
        ioOut << "Телефон: "                 << object._phone        << ", ";
        ioOut << "Почта: "                   << object._email        << ", ";
        ioOut << "Дата принятия на работу: " << object._dateOfHiring << ", ";
        ioOut << "Часы работы: "             << object._workingHours << ", ";
        ioOut << "Зарплата: "                << object._salary       << ", ";
        ioOut << "Пароль: "                  << object._password;
        return ioOut;
    }

    bool operator == (const Employee &first, const Employee &second) noexcept
    {
        return (first._email == second._email) &&
               (first._password == second._password);
    }

    void Employee::SetId(const QString &iID) noexcept
    {
        QString ID = iID;
        Employee::Type type = _checkField(ID, FIELD_ID);
        _id = type.uintValue;
        _fieldStatus[FIELD_ID] = type.status;
    }

    void Employee::SetRole(const QString &iRole) noexcept
    {
        QString role = iRole;
        Employee::Type type = _checkField(role, FIELD_ROLE);
        _role = type.stringValue;
        _fieldStatus[FIELD_ROLE] = type.status;
    }

    void Employee::SetSurname(const QString &iSurname) noexcept
    {
        QString surname = iSurname;
        Employee::Type type = _checkField(surname, FIELD_SURNAME);
        _surname = type.stringValue;
        _fieldStatus[FIELD_SURNAME] = type.status;
    }

    void Employee::SetName(const QString &iName) noexcept
    {
        QString name = iName;
        Employee::Type type = _checkField(name, FIELD_NAME);
        _name = type.stringValue;
        _fieldStatus[FIELD_NAME] = type.status;
    }

    void Employee::SetPatronymic(const QString &iPatronymic) noexcept
    {
        QString patronymic = iPatronymic;
        Employee::Type type = _checkField(patronymic, FIELD_PATRONYMIC);
        _patronymic = type.stringValue;
        _fieldStatus[FIELD_PATRONYMIC] = type.status;
    }

    void Employee::SetSex(const QString &iSex) noexcept
    {
        QString sex = iSex;
        Employee::Type type = _checkField(sex, FIELD_SEX);
        _sex = type.stringValue;
        _fieldStatus[FIELD_SEX] = type.status;
    }

    void Employee::SetDateOfBirth(const QString &iDateOfBirth) noexcept
    {
        QString dateOfBirth = iDateOfBirth;
        Employee::Type type = _checkField(dateOfBirth, FIELD_DATE_OF_BIRTH);
        _dateOfBirth = type.stringValue;
        _fieldStatus[FIELD_DATE_OF_BIRTH] = type.status;
    }

    void Employee::SetPassport(const QString &iPassport) noexcept
    {
        QString passport = iPassport;
        Employee::Type type = _checkField(passport, FIELD_PASSPORT);
        _passport = type.uint64Value;
        _fieldStatus[FIELD_PASSPORT] = type.status;
    }

    void Employee::SetPhone(const QString &iPhone) noexcept
    {
        QString phone = iPhone;
        Employee::Type type = _checkField(phone, FIELD_PHONE);
        _phone = type.uint64Value;
        _fieldStatus[FIELD_PHONE] = type.status;
    }

    void Employee::SetEmail(const QString &iEmail) noexcept
    {
        QString email = iEmail;
        Employee::Type type = _checkField(email, FIELD_EMAIL);
        _email = type.stringValue;
        _fieldStatus[FIELD_EMAIL] = type.status;
    }

    void Employee::SetDateOfHiring(const QString &iDateOfHiring) noexcept
    {
        QString dateOfHiring = iDateOfHiring;
        Employee::Type type = _checkField(dateOfHiring, FIELD_DATE_OF_HIRING);
        _dateOfHiring = type.stringValue;
        _fieldStatus[FIELD_DATE_OF_HIRING] = type.status;
    }

    void Employee::SetWorkingHours(const QString &iWorkingHours) noexcept
    {
        QString workingHours = iWorkingHours;
        Employee::Type type = _checkField(workingHours, FIELD_WORKING_HOURS);
        _workingHours = type.stringValue;
        _fieldStatus[FIELD_WORKING_HOURS] = type.status;
    }

    void Employee::SetSalary(const QString &iSalary) noexcept
    {
        QString salary = iSalary;
        Employee::Type type = _checkField(salary, FIELD_SALARY);
        _salary = type.uintValue;
        _fieldStatus[FIELD_SALARY] = type.status;
    }

    void Employee::SetPassword(const QString &iPassword) noexcept
    {
        QString password = iPassword;
        Employee::Type type = _checkField(password, FIELD_PASSWORD);
        _password = type.stringValue;
        _fieldStatus[FIELD_PASSWORD] = type.status;
    }

    bool Employee::checkField(const QString &iField, QString &iValue)
    {
        Field field = checkParameters.value(iField);
        return _checkField(iValue, field).status == Status::ST_OK;
    }

    const Employee::Type Employee::_checkField(QString &iValue, const Field iField)
    {
        Type type;

        try
        {
            switch (iField)
            {
                case FIELD_ID :
                {
                    QRegularExpression regular("^[0-9]{1,4}$");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустой ID >> " + iValue + ", " + helpFields()[id()];
                        type.status = ST_EMPTY;
    //                    Logger::error << "Пустой ID >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректный ID >> " + iValue + ", " + helpFields()[id()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорректный ID >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    !_id ? Logger::info << "ID >> " << iValue << std::endl :
    //                          (Logger::info << "ID << " << _id << " >> изменен на >> " << iValue << std::endl,
    //                           std::cout << "ID успешно изменен" << std::endl);
                    }
                    type.uintValue = iValue.toUInt();
                    return type;
                }

                case FIELD_ROLE :
                {
                    Utils::ToUpperandtolower(iValue);
                    QRegularExpression regular("(Бухгалтер|Водитель|Главный бухгалтер|Главный юрист-консультант|Грузчик|Директор|Кассир|Логист|"
                                          "Менеджер по закупкам|Менеджер по продажам|Начальник отдела закупок|Начальник склада|Юрист|Менеджер по персоналу)");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустая должность >> " + iValue + ", " + helpFields()[role()];
                        type.status = ST_EMPTY;
    //                    Logger::error << "Пустая должность >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректная должность >> " + iValue + ", " + helpFields()[role()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорректная должность >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    _role.empty() ? Logger::info << "Должность >> " << iValue << std::endl :
    //                                       (Logger::info << "Должность << " << _role << " >> изменена на >> " << iValue << std::endl,
    //                                        std::cout << "Должность успешно изменена" << std::endl);
                    }
                    type.stringValue = iValue;
                    return type;
                }

                case FIELD_SURNAME :
                {
                    Utils::ToUpperandtolower(iValue);
                    QRegularExpression regular("[А-Яабвгдеёжзийклмнопрстуфхцчшщъыьэюя]+");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустая фамилия >> " + iValue + ", " + helpFields()[surname()];
                        type.status = ST_EMPTY;
    //                    Logger::error << "Пустая фамилия >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректная фамилия >> " + iValue + ", " + helpFields()[surname()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорректная фамилия >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    _surname.empty() ? Logger::info << "Фамилия >> " << iValue << std::endl :
    //                                      (Logger::info << "Фамилия << " << _surname << " >> изменена на >> " << iValue << std::endl,
    //                                       std::cout << "Фамилия успешно изменена" << std::endl);
                    }
                    type.stringValue = iValue;
                    return type;
                }

                case FIELD_NAME :
                {
                    Utils::ToUpperandtolower(iValue);
                    QRegularExpression regular("[А-Яабвгдеёжзийклмнопрстуфхцчшщъыьэюя]+");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустое имя >> " + iValue + ", " + helpFields()[name()];
                        type.status = ST_EMPTY;
    //                    Logger::info << "Пустое имя >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорретное имя >> " + iValue + ", " + helpFields()[name()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорретное имя >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    _name.empty() ? Logger::info << "Имя >> " << iValue << std::endl :
    //                                   (Logger::info << "Имя << " << _name << " >> изменено на >> " << iValue << std::endl,
    //                                    std::cout << "Имя успешно изменено" << std::endl);
                    }
                    type.stringValue = iValue;
                    return type;
                }

                case FIELD_PATRONYMIC :
                {
                    Utils::ToUpperandtolower(iValue);
                    QRegularExpression regular("[А-Яабвгдеёжзийклмнопрстуфхцчшщъыьэюя]+");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустое отчество >> " + iValue + ", " + helpFields()[patronymic()];
                        type.status = ST_EMPTY;
    //                    Logger::error << "Пустое отчество >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорретное отчество >> " + iValue + ", " + helpFields()[patronymic()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорретное отчество >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    _patronymic.empty() ? Logger::info << "Отчество >> " << iValue << std::endl :
    //                                         (Logger::info << "Отчество << " << _patronymic << " >> изменено на >> " << iValue << std::endl,
    //                                          std::cout << "Отчество успешно изменено" << std::endl);
                    }
                    type.stringValue = iValue;
                    return type;
                }

                case FIELD_SEX :
                {
                    Utils::ToUpperandtolower(iValue);
                    QRegularExpression regular("^(Муж|Жен)$");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустой пол >> " + iValue + ", " + helpFields()[sex()];
                        type.status = ST_EMPTY;
    //                    Logger::error << "Пустой пол >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректный пол >> " + iValue + ", " + helpFields()[sex()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорректный пол >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    _sex.empty() ? Logger::info << "Пол >> " << iValue << std::endl :
    //                                  (Logger::info << "Пол << " << _sex << " >> изменен на >> " << iValue << std::endl,
    //                                   std::cout << "Пол успешно изменен" << std::endl);
                    }
                    type.stringValue = iValue;
                    return type;
                }

                case FIELD_DATE_OF_BIRTH :
                {
                    Utils::FormatDateToPostgres(iValue);
                    QRegularExpression regular("^(((((0[1-9]|[12][0-9]|30)[-/.])?(0[13-9]|1[012])|(31[-/.])?(0[13578]|1[02])|((0[1-9]|"
                                          "1[0-9]|2[0-8])[-/.])?02)[-/.])?[0-9]{4}|(29[-/.])?(02[-/.])?([0-9]{2}(([2468][048]|"
                                          "[02468][48])|[13579][26])|([13579][26]|[02468][048]|0[0-9]|1[0-6])00))$");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустая дата рождения >> " + iValue + ", " + helpFields()[dateOfBirth()];
                        type.status = ST_EMPTY;
    //                    Logger::error << "Пустая дата рождения >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректная дата рождения >> " + iValue + ", " + helpFields()[dateOfBirth()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорректная дата рождения >> " << iValue << std::endl;
                    }
                    else
                    {
                        // Нахождение возраста
                        QStringList data = Utils::SplitString(Utils::Date(), " .-");
                        QStringList dateofbirth = Utils::SplitString(iValue, " .-");
                        auto age = Utils::FindAge(data, dateofbirth);
                        int year = age[0];
                        int month = age[1];
                        int day = age[2];
                        if (year < 18)
                        {
                            iValue = "Неудовлетворительный возраст >> " + iValue + ", " + helpFields()[dateOfBirth()];
                            type.status = ST_WRONGDATA;
    //                        Logger::warning << "Неудовлетворительный возраст >> " << "Лет: " << year << "Месяцев: " << month << "Дней: " << day << std::endl;
                        }
                        else
                        {
                            type.status = ST_OK;
    //                        _dateOfBirth.empty() ? Logger::info << "Дата рождения >> " << iValue << std::endl :
    //                                              (Logger::info << "Дата рождения << " << _dateOfBirth << " >> изменена на >> " << iValue << std::endl,
    //                                               std::cout << "Дата рождения успешно изменена" << std::endl);
                        }
                    }
                    type.stringValue = iValue;
                    return type;
                }

                case FIELD_PASSPORT :
                {
                    iValue.replace("-", "");
                    QRegularExpression regular("^[0-9]{10}$");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустой паспорт >> " + iValue + ", " + helpFields()[passport()];
                        type.status = ST_EMPTY;
    //                    Logger::error << "Пустой паспорт >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректный паспорт >> " + iValue + ", " + helpFields()[passport()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорректный паспорт >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    !_passport ? Logger::info << "Паспорт >> " << iValue << std::endl :
    //                                (Logger::info << "Паспорт << " << _passport << " >> изменен на >> " << iValue << std::endl,
    //                                 std::cout << "Паспорт успешно изменен" << std::endl);
                    }
                    type.uint64Value = iValue.toULongLong();
                    return type;
                }

                case FIELD_PHONE :
                {
                    iValue.replace("-", "");
                    QRegularExpression regular("^[0-9]{10}$");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустой телефон >> " + iValue + ", " + helpFields()[phone()];
                        type.status = ST_EMPTY;
    //                    Logger::error << "Пустой телефон >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректный телефон >> " + iValue + ", " + helpFields()[phone()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорректный телефон >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    !_phone ? Logger::info << "Телефон >> " << iValue << std::endl :
    //                             (Logger::info << "Телефон << " << _phone << " >> изменен на >> " << iValue << std::endl,
    //                              std::cout << "Телефон успешно изменен" << std::endl);
                    }
                    type.uint64Value = iValue.toULongLong();
                    return type;
                }

                case FIELD_EMAIL :
                {
                    iValue = iValue.toLower();
                    QRegularExpression regular("^([a-z0-9]+)(\\.)([a-z0-9]+)(\\.)([a-z0-9]+)(@)(tradingcompany)(\\.)(ru)$");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустая почта >> " + iValue + ", " + helpFields()[email()];
                        type.status = ST_EMPTY;
//                        Logger::error << "Пустая почта >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректная почта >> " + iValue + ", " + helpFields()[email()];
                        type.status = ST_WRONGDATA;
//                        Logger::warning << "Некорректная почта >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    _email.empty() ? Logger::info << "Почта >> " << iValue << std::endl :
    //                                    (Logger::info << "Почта << " << _email << " >> изменена на >> " << iValue << std::endl,
    //                                     std::cout << "Почта успешно изменена" << std::endl);
                    }
                    type.stringValue = iValue;
                    return type;
                }

                case FIELD_DATE_OF_HIRING :
                {
                    Utils::FormatDateToPostgres(iValue);
                    QRegularExpression regular("^(((((0[1-9]|[12][0-9]|30)[-/.])?(0[13-9]|1[012])|(31[-/.])?(0[13578]|1[02])|((0[1-9]|"
                                          "1[0-9]|2[0-8])[-/.])?02)[-/.])?[0-9]{4}|(29[-/.])?(02[-/.])?([0-9]{2}(([2468][048]|"
                                          "[02468][48])|[13579][26])|([13579][26]|[02468][048]|0[0-9]|1[0-6])00))$");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустая дата принятия на работу >> " + iValue + ", " + helpFields()[dateOfHiring()];
                        type.status = ST_EMPTY;
    //                    Logger::error << "Пустая дата принятия на работу >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректная дата принятия на работу >> " + iValue + ", " + helpFields()[dateOfHiring()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорректная дата принятия на работу >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    _dateOfHiring.empty() ? Logger::info << "Дата принятия на работу >> " << iValue << std::endl :
    //                                           (Logger::info << "Дата принятия на работу << " << _dateOfHiring << " >> изменена на >> " << iValue << std::endl,
    //                                            std::cout << "Дата принятия на работу успешно изменена" << std::endl);
                    }
                    type.stringValue = iValue;
                    return type;
                }

                case FIELD_WORKING_HOURS :
                {
                    QRegularExpression regular("(((((Понедельник|Вторник|Среда|Четверг|Пятница|Суббота|Воскресенье),*)+)|"
                                                  "(Понедельник|Вторник|Среда|Четверг|Пятница|Суббота|Воскресенье)-"
                                                  "(Понедельник|Вторник|Среда|Четверг|Пятница|Суббота|Воскресенье))="
                                                  "([0-1]?[0-9]|[2][0-3]):([0-5][0-9])-([0-1]?[0-9]|[2][0-3]):([0-5][0-9]);*)+");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустые часы работы >> " + iValue + ", " + helpFields()[workingHours()];
                        type.status = ST_EMPTY;
    //                    Logger::error << "Пустые часы работы >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректные часы работы >> " + iValue + ", " + helpFields()[workingHours()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорректные часы работы >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    _workingHours.empty() ? Logger::info << "Часы работы >> " << iValue << std::endl :
    //                                           (Logger::info << "Часы работы << " << _workingHours << " >> изменены на >> " << iValue << std::endl,
    //                                            std::cout << "Часы работы успешно изменены" << std::endl);
                    }
                    type.stringValue = iValue;
                    return type;
                }

                case FIELD_SALARY :
                {
                    QRegularExpression regular("[0-9]+");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустая зарплата >> " + iValue + ", " + helpFields()[salary()];
                        type.status = ST_EMPTY;
    //                    Logger::error << "Пустая зарплата >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректная зарплата >> " + iValue + ", " + helpFields()[salary()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорректная зарплата >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    !_salary ? Logger::info << "Зарплата >> " << iValue << std::endl :
    //                              (Logger::info << "Зарплата << " << _salary << " >> изменена на >> " << iValue << std::endl,
    //                               std::cout << "Зарплата успешно изменена" << std::endl);
                    }
                    type.uintValue = iValue.toUInt();
                    return type;
                }

                case FIELD_PASSWORD :
                {
                    QRegularExpression regular("(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)[a-zA-Z\\d]{6,}");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустая пароль >> " + iValue + ", " + helpFields()[password()];
                        type.status = ST_EMPTY;
    //                    Logger::error << "Пустой пароль >> " << iValue << std::endl;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректный пароль >> " + iValue + ", " + helpFields()[password()];
                        type.status = ST_WRONGDATA;
    //                    Logger::warning << "Некорректный пароль >> " << iValue << std::endl;
                    }
                    else
                    {
                        type.status = ST_OK;
    //                    _password.empty() ? Logger::info << "Пароль >> " << iValue << std::endl :
    //                                       (Logger::info << "Пароль << " << _password << " >> изменен на >> " << iValue << std::endl,
    //                                        std::cout << "Пароль успешно изменен" << std::endl);
                    }
                    type.stringValue = iValue;
                    return type;
                }

                default:
                    throw iField;
            }
        }
        catch (const std::string &exception)
        {
    //        Logger::error << "Невернное значение >> " << exception << std::endl;
        }
        catch(const Field field)
        {
    //        Logger::error << "Неверный параметр поля >> " + std::to_string(field) << std::endl;
        }

        return {};
    }

    void Employee::ChangeStatusRole() noexcept
    {
        _fieldStatus[FIELD_ROLE] = ST_OVERWRITEDATA;
    //    Logger::info << "Перезапись должности >> " << _role << std::endl;
    }

    void Employee::ChangeStatusSurname() noexcept
    {
        _fieldStatus[FIELD_SURNAME] = ST_OVERWRITEDATA;
    //    Logger::info << "Перезапись фамилии >> " << _surname << std::endl;
    }

    void Employee::ChangeStatusName() noexcept
    {
        _fieldStatus[FIELD_NAME] = ST_OVERWRITEDATA;
    //    Logger::info << "Перезапись имени >> " << _name << std::endl;
    }

    void Employee::ChangeStatusPatronymic() noexcept
    {
        _fieldStatus[FIELD_PATRONYMIC] = ST_OVERWRITEDATA;
    //    Logger::info << "Перезапись отчества >> " << _patronymic << std::endl;
    }

    void Employee::ChangeStatusSex() noexcept
    {
        _fieldStatus[FIELD_SEX] = ST_OVERWRITEDATA;
    //    Logger::info << "Перезапись пола >> " << _sex << std::endl;
    }

    void Employee::ChangeStatusDateOfBirth() noexcept
    {
        _fieldStatus[FIELD_DATE_OF_BIRTH] = ST_OVERWRITEDATA;
    //    Logger::info << "Перезапись даты рождения >> " << _dateOfBirth << std::endl;
    }

    void Employee::ChangeStatusPassport(const bool canOverwrite) noexcept
    {
        if (canOverwrite)
        {
            _fieldStatus[FIELD_PASSPORT] = ST_OVERWRITEDATA;
    //        Logger::info << "Перезапись паспорта >> " << _passport << std::endl;
        }
        else
        {
            uint64_t passport = _passport;
            _passport = rand(); // Перезапись на случайное число
            _fieldStatus[FIELD_PASSPORT] = ST_DUBLICATE;
    //        Logger::warning << "Повторяющийся паспорт >> " << passport << " изменен на >> " << _passport << std::endl;
        }
    }

    void Employee::ChangeStatusPhone(const bool canOverwrite) noexcept
    {
        if (canOverwrite)
        {
            _fieldStatus[FIELD_PHONE] = ST_OVERWRITEDATA;
    //        Logger::info << "Перезапись телефона >> " << _phone << std::endl;
        }
        else
        {
            uint64_t phone = _phone;
            _phone = rand(); // Перезапись на случайное число
            _fieldStatus[FIELD_PHONE] = ST_DUBLICATE;
    //        Logger::warning << "Повторяющийся телефон >> " << phone << " изменен на >> " << _phone << std::endl;
        }
    }

    void Employee::ChangeStatusEmail(const bool canOverwrite) noexcept
    {
        if (canOverwrite)
        {
            _fieldStatus[FIELD_EMAIL] = ST_OVERWRITEDATA;
    //        Logger::info << "Перезапись почты >> " << _email << std::endl;
        }
        else
        {
            _fieldStatus[FIELD_EMAIL] = ST_DUBLICATE;
    //        Logger::warning << "Повторяющаяся почта >> " << _email << std::endl;
        }
    }

    void Employee::ChangeStatusDateOfHiring() noexcept
    {
        _fieldStatus[FIELD_DATE_OF_HIRING] = ST_OVERWRITEDATA;
    //    Logger::info << "Перезапись даты принятия на работу >> " << _dateOfHiring << std::endl;
    }

    void Employee::ChangeStatusWorkingHours() noexcept
    {
        _fieldStatus[FIELD_WORKING_HOURS] = ST_OVERWRITEDATA;
    //    Logger::info << "Перезапись часов работы >> " << _workingHours << std::endl;
    }

    void Employee::ChangeStatusSalary() noexcept
    {
        _fieldStatus[FIELD_SALARY] = ST_OVERWRITEDATA;
    //    Logger::info << "Перезапись зарплаты >> " << _salary << std::endl;
    }

    void Employee::ChangeStatusPassword() noexcept
    {
        _fieldStatus[FIELD_PASSWORD] = ST_OVERWRITEDATA;
    //    Logger::info << "Перезапись пароля >> " << _password << std::endl;
    }

    /*
        QByteArray input = QByteArray::fromBase64( QString("eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzUxMiJ9").toUtf8() );
        qDebug() << input.size() << "IN:>" << input.toStdString().c_str();
        QJsonDocument jdocHeader = QJsonDocument::fromJson( input ) ;

        QJsonObject jobjHeader = jdocHeader.object();
        qDebug() << jobjHeader;

        QByteArray qsHeader64 =  jdocHeader.toJson(QJsonDocument::JsonFormat::Compact);//.toBase64() ;
        qDebug() << qsHeader64.size() << "OUT:>" << QString(qsHeader64);

        qDebug() << qsHeader64.toBase64();
        qDebug() << "------------does order matter-------";
        qDebug() << QByteArray("123456789").toBase64();
        qDebug() << QByteArray("987654321").toBase64();
     */
}
