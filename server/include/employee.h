#ifndef Employee_h
#define Employee_h

#pragma once

#include <iomanip>
#include <functional>
#include <map>
#include <string>

#include <QSqlQuery>

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

/*!
 * @brief Абстрактный класс
 */
class Employee
{
public:

    /// Уровень доступа
    enum Access
    {
        /// Блокировка
        LOCK = 0,

        /// Чтение
        READ,

        /// Запись
        WRITE
    };

    /// Карта полей с их доступом
    std::map<Field, Access> _fieldAccess
    {
        {FIELD_ID,             WRITE},
        {FIELD_ROLE,           WRITE},
        {FIELD_SURNAME,        WRITE},
        {FIELD_NAME,           WRITE},
        {FIELD_PATRONYMIC,     WRITE},
        {FIELD_SEX,            WRITE},
        {FIELD_DATE_OF_BIRTH,  WRITE},
        {FIELD_PASSPORT,       WRITE},
        {FIELD_PHONE,          WRITE},
        {FIELD_EMAIL,          WRITE},
        {FIELD_DATE_OF_HIRING, WRITE},
        {FIELD_WORKING_HOURS,  WRITE},
        {FIELD_SALARY,         WRITE},
        {FIELD_PASSWORD,       WRITE},
    };

    static std::string TableName() { return "employee"; }
    static std::string PermissionTable()    { return "permission"; }
    static std::string PersonalDataPermissionTable()    { return "personal_data_permission"; }
    static std::string DatabasePermissionTable()    { return "database_permission"; }
    static std::string ID()           { return "id"; }
    static std::string Role()         { return "role"; }
    static std::string Surname()      { return "surname"; }
    static std::string Name()         { return "name"; }
    static std::string Patronymic()   { return "patronymic"; }
    static std::string Sex()          { return "sex"; }
    static std::string DateOfBirth()  { return "date_of_birth"; }
    static std::string Passport()     { return "passport"; }
    static std::string Phone()        { return "phone"; }
    static std::string Email()        { return "email"; }
    static std::string DateOfHiring() { return "date_of_hiring"; }
    static std::string WorkingHours() { return "working_hours"; }
    static std::string Salary()       { return "salary"; }
    static std::string Password()     { return "password"; }

    /// Карта доступа
    const std::map<std::string, std::map<Field, Access>> accessControl =
    {
        {"Бухгалтер", _fieldAccess},
        {"Водитель", _fieldAccess},
        {"Главный_бухгалтер", _fieldAccess},
        {"Главный_юрист-консультант", _fieldAccess},
        {"Грузчик", _fieldAccess},
        {"Директор", _fieldAccess},
        {"Логист", _fieldAccess},
        {"Менеджер_по_закупкам", _fieldAccess},
        {"Менеджер_по_продажам", _fieldAccess},
        {"Кассир", _fieldAccess},
        {"Начальник_отдела_закупок", _fieldAccess},
        {"Начальник_склада", _fieldAccess},
        {"Менеджер_по_персоналу", _fieldAccess},
        {"Юрист", _fieldAccess}
    };

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
        std::string stringValue;
    };
    
    /// Карта параметров и оберток инициализаций полей
    const std::map<std::string, std::function<void(Employee&, const std::string&)>> _setParameters =
    {
        {ID(),           nullptr},
        {Role(),         &Employee::SetRole},
        {Surname(),      &Employee::SetSurname},
        {Name(),         &Employee::SetName},
        {Patronymic(),   &Employee::SetPatronymic},
        {Sex(),          &Employee::SetSex},
        {DateOfBirth(),  &Employee::SetDateOfBirth},
        {Passport(),     &Employee::SetPassport},
        {Phone(),        &Employee::SetPhone},
        {Email(),        &Employee::SetEmail},
        {DateOfHiring(), &Employee::SetDateOfHiring},
        {WorkingHours(), &Employee::SetWorkingHours},
        {Salary(),       &Employee::SetSalary},
        {Password(),     &Employee::SetPassword}
    };
    
    /// Карта полей с их статусом
    std::map<Field, Status> _fieldStatus
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
    const Employee& operator = (const Employee &object);

    /*!
     * @brief Перегрузка оператора >> (ввода).
     * Запись данных в поля объекта
     * @param iLine - строка
     * @param object - объект
     */
    friend bool operator >> (const QSqlQuery &iLine, Employee &object);

    /*!
     * @brief Перегрузка оператора << (вывода).
     * Вывод полей объекта
     * @param ioOut - поток вывода
     * @param object - объект
     * @return Поток вывода
     */
    friend std::ostream& operator << (std::ostream &ioOut, const Employee &object);

    /*!
     * @brief Перегрузка оператора == (сравнения).
     * Сравнение двух объектов на равенство
     * @param first - первый объект
     * @param second - второй объект
     * @return Логическое значение
     */
    friend bool operator == (const Employee &first, const Employee &second);

    /*!
     * @brief Получение значений полей
     * @return Значение поля
     */
    std::string GetRole() const;
    std::string GetSurname() const;
    std::string GetName() const;
    std::string GetPatronymic() const;
    
protected:
    /*!
     * @brief Получение значений полей
     * @return Значение поля
     */
    uint32_t    GetId() const;
    std::string GetSex() const;
    std::string GetDateOfBirth() const;
    uint64_t    GetPassport() const;
    uint64_t    GetPhone() const;
    std::string GetEmail() const;
    std::string GetDateOfHiring() const;
    std::string GetWorkingHours() const;
    uint32_t    GetSalary() const;
    std::string GetPassword() const;

private:
    uint32_t    _id = 0;       /// ID
    std::string _role;         /// Должность
    std::string _surname;      /// Фамилия
    std::string _name;         /// Имя
    std::string _patronymic;   /// Отчество
    std::string _sex;          /// Пол
    std::string _dateOfBirth;  /// Дата рождения
    uint64_t    _passport = 0; /// Паспорт
    uint64_t    _phone = 0;    /// Телефон
    std::string _email;        /// Почта
    std::string _dateOfHiring; /// Дата принятия на работу
    std::string _workingHours; /// Часы работы
    uint32_t    _salary = 0;   /// Зарплата
    std::string _password;     /// Пароль
    
    /*!
     * @brief Инициализация полей
     * @param Значение поля
     */
    void SetId(const std::string &iID);
    void SetRole(const std::string &iRole);
    void SetSurname(const std::string &iSurname);
    void SetName(const std::string &iName);
    void SetPatronymic(const std::string &iPatronymic);
    void SetSex(const std::string &iSex);
    void SetDateOfBirth(const std::string &iDateOfBirth);
    void SetPhone(const std::string &iPhone);
    void SetEmail(const std::string &iEmail);
    void SetDateOfHiring(const std::string &iDateOfHiring);
    void SetWorkingHours(const std::string &iWorkingHours);
    void SetPassport(const std::string &iPassport);
    void SetSalary(const std::string &iSalary);
    void SetPassword(const std::string &iPassword);

    /*!
     * @brief Проверка полей на повреждение/перезапись данных
     * @param iWarning - Предупреждение о невалидности данных поля
     */
    void CheckId(const std::string &iWarning = {}); /// Не используется!
    void CheckRole(const std::string &iWarning = {});
    void CheckSurname(const std::string &iWarning = {});
    void CheckName(const std::string &iWarning = {});
    void CheckPatronymic(const std::string &iWarning = {});
    void CheckSex(const std::string &iWarning = {});
    void CheckDateOfBirth(const std::string &iWarning = {});
    void CheckPassport(const std::string &iWarning = {});
    void CheckPhone(const std::string &iWarning = {});
    void CheckEmail(const std::string &iWarning = {});
    void CheckDateOfHiring(const std::string &iWarning = {});
    void CheckWorkingHours(const std::string &iWarning = {});
    void CheckSalary(const std::string &iWarning = {});
    void CheckPassword(const std::string &iWarning = {});

    /*!
     * @brief Изменение статуса полей на перезапись/дублирование данных
     */
    void ChangeStatusRole();
    void ChangeStatusSurname();
    void ChangeStatusName();
    void ChangeStatusPatronymic();
    void ChangeStatusSex();
    void ChangeStatusDateOfBirth();
    /// @param canOverwrite - true-перезапись/false-дублирование
    void ChangeStatusPassport(const bool canOverwrite = false);
    /// @param canOverwrite - true-перезапись/false-дублирование
    void ChangeStatusPhone(const bool canOverwrite = false);
    /// @param canOverwrite - true-перезапись/false-дублирование
    void ChangeStatusEmail(const bool canOverwrite);
    void ChangeStatusDateOfHiring();
    void ChangeStatusWorkingHours();
    void ChangeStatusSalary();
    void ChangeStatusPassword();

    /*!
     * @brief Рекурсия, которая вызывается в случае неверного введения данных.
     * Валидация ввода данных
     * @param iField - Номер поля
     * @param setParameter - Инициализация одного из полей
     * @param iMessage - Сообщение, которое подказывает в каком формате вводить данные
     */
    void Recursion(const Field iField,
                   std::function<void(Employee&, std::string&)> setParameter,
                   const std::string &iMessage);
    /*!
     * @brief Получение определенного типа данных (uint32_t/uint64_t/string)
     * @param iValue - Значение поля
     * @param iField - Номер поля
     * @return Значение поля с определенным типом (uint32_t/uint64_t/string)
     */
    template<typename T> T Get(const std::string &iValue, const Field iField);

    /// Пустая структура для возврата из метода в случае неудачи (затычка)
    const Type empty = Type();

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
    const Type CheckField(std::string iValue, const Field iField);

    /*!
     * @TODO: Установление премии
     * @param premium - Премия
     */
    void SetPremium(int premium);

    /*!
     * @TODO: Установление штрафа
     * @param fine - Штраф
     */
    void SetFine(int fine);

    /*!
     * @TODO: Получение премии
     * @return Премия
     */
    uint GetPremium() const;
    /*!
     * @TODO: Получение штрафа
     * @return Штраф
     */
    uint GetFine() const;
};

#endif // Employee_h
