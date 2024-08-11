#include "server.h"
#include "utils.h"

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

    bool Employee::checkField(const QString& iField, QString& iValue)
    {
        Field field = checkParameters.value(iField);
        return _checkField(iValue, field).status == Status::ST_OK;
    }

    const Employee::Type Employee::_checkField(QString& iValue, const Field iField)
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
                        qWarning() << "Пустой ID >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректный ID >> " + iValue + ", " + helpFields()[id()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорректный ID >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "ID изменен на >> " << iValue;
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
                        qWarning() << "Пустая должность >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректная должность >> " + iValue + ", " + helpFields()[role()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорректная должность >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "Должность изменена на >> " << iValue;
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
                        qWarning() << "Пустая фамилия >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректная фамилия >> " + iValue + ", " + helpFields()[surname()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорректная фамилия >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "Фамилия изменена на >> " << iValue;
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
                        qWarning() << "Пустое имя >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорретное имя >> " + iValue + ", " + helpFields()[name()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорретное имя >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "Имя изменено на >> " << iValue;
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
                        qWarning() << "Пустое отчество >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорретное отчество >> " + iValue + ", " + helpFields()[patronymic()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорретное отчество >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "Отчество изменено на >> " << iValue;
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
                        qWarning() << "Пустой пол >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректный пол >> " + iValue + ", " + helpFields()[sex()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорректный пол >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "Пол изменен на >> " << iValue;
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
                        qWarning() << "Пустая дата рождения >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректная дата рождения >> " + iValue + ", " + helpFields()[dateOfBirth()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорректная дата рождения >> " << iValue;
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
                            qWarning() << "Неудовлетворительный возраст >> " << "Лет: " << year << "Месяцев: " << month << "Дней: " << day;
                        }
                        else
                        {
                            type.status = ST_OK;
                            qInfo() << "Дата рождения изменена на >> " << iValue;
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
                        qWarning() << "Пустой паспорт >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректный паспорт >> " + iValue + ", " + helpFields()[passport()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорректный паспорт >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "Паспорт изменен на >> " << iValue;
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
                        qWarning() << "Пустой телефон >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректный телефон >> " + iValue + ", " + helpFields()[phone()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорректный телефон >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "Телефон изменен на >> " << iValue;
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
                        qWarning() << "Пустая почта >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректная почта >> " + iValue + ", " + helpFields()[email()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорректная почта >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "Почта изменена на >> " << iValue;
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
                        qWarning() << "Пустая дата принятия на работу >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректная дата принятия на работу >> " + iValue + ", " + helpFields()[dateOfHiring()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорректная дата принятия на работу >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "Дата принятия на работу изменена на >> " << iValue;
                    }
                    type.stringValue = iValue;
                    return type;
                }

                case FIELD_WORKING_HOURS :
                {
                    QRegularExpression regular("((((([Пп]онедельник|[Вв]торник|[Сс]реда|[Чч]етверг|[Пп]ятница|[Сс]уббота|[Вв]оскресенье),*)+)|"
                                               "([Пп]онедельник|[Вв]торник|[Сс]реда|[Чч]етверг|[Пп]ятница|[Сс]уббота|[Вв]оскресенье)-"
                                               "([Пп]онедельник|[Вв]торник|[Сс]реда|[Чч]етверг|[Пп]ятница|[Сс]уббота|[Вв]оскресенье))="
                                               "([0-1]?[0-9]|[2][0-3]):([0-5][0-9])-([0-1]?[0-9]|[2][0-3]):([0-5][0-9]);*)+");
                    if (iValue.isEmpty())
                    {
                        iValue = "Пустые часы работы >> " + iValue + ", " + helpFields()[workingHours()];
                        type.status = ST_EMPTY;
                        qWarning() << "Пустые часы работы >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректные часы работы >> " + iValue + ", " + helpFields()[workingHours()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорректные часы работы >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "Часы работы изменены на >> " << iValue;
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
                        qWarning() << "Пустая зарплата >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректная зарплата >> " + iValue + ", " + helpFields()[salary()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорректная зарплата >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "Зарплата изменена на >> " << iValue;
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
                        qWarning() << "Пустой пароль >> " << iValue;
                    }
                    else if (!regular.match(iValue).hasMatch())
                    {
                        iValue = "Некорректный пароль >> " + iValue + ", " + helpFields()[password()];
                        type.status = ST_WRONGDATA;
                        qWarning() << "Некорректный пароль >> " << iValue;
                    }
                    else
                    {
                        type.status = ST_OK;
                        qInfo() << "Пароль изменен на >> " << iValue;
                    }
                    type.stringValue = iValue;
                    return type;
                }

                default:
                    throw iField;
            }
        }
        catch (const QString& exception)
        {
            qCritical() << "Невернное значение >> " << exception;
        }
        catch (const Field field)
        {
            qCritical() << "Неверный параметр поля >> " + QString::number(field);
        }

        return {};
    }
}
