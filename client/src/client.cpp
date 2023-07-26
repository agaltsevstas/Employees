#include "client.h"
#include "utils.h"

#include <regex>
#include <QSqlError>
#include <QSqlRecord>
#include <QRegularExpression>

namespace Client
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
}
