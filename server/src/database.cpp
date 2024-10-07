#include "database.h"
#include "exception.h"
#include "server.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlTableModel>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <ranges>


namespace Server
{
    namespace
    {
        constexpr auto DATABASE_POSTGRES = "QPSQL"; // Тип базы данных
        constexpr auto DATABASE_HOSTNAME = "127.0.0.1"; // Хост
        constexpr auto DATABASE_PORT = 5432; // Порт
        constexpr auto DATABASE_NAME = "employees"; // Название базы данных
    }

    DataBase::DataBase(QWidget *parent) : QWidget(parent)
    {

    }

    DataBase::~DataBase()
    {
        qInfo() << "Закрытие БД";
        _db->close();
    }

    void DataBase::connect()
    {
        _db.reset(new QSqlDatabase(QSqlDatabase::addDatabase(DATABASE_POSTGRES)));
        _db->setHostName(DATABASE_HOSTNAME);
        _db->setPort(DATABASE_PORT);
        _db->setUserName("agaltsevstas");
        _db->setPassword("");

        try
        {
            if (!this->open())
                throw Error();
        }
        catch (...)
        {
            std::rethrow_exception(std::current_exception());
        }

        qInfo() << "Успешное подключение к БД";
    }

    bool DataBase::checkFieldOnDuplicate(const QByteArray& iColumn, const QVariant& iValue) const
    {
        if (iColumn == Employee::passport() ||
            iColumn == Employee::phone() ||
            iColumn == Employee::email())
        {
            QSqlQuery query(*_db);
            query.prepare("SELECT * FROM employee WHERE " + iColumn + " = :value;");
            query.bindValue(":value", iValue.toString());

            if (!query.exec())
            {
                qCritical() << "Ошибка: " << query.lastError().text();
                return false;
            }

            return query.size() == 0;
        }

        return true;
    }

    bool DataBase::authentication(const QByteArray& iUserName, const QByteArray& iPassword, QString& oID, QString& oRole, QByteArray& oData) const
    {
        QString userName = iUserName;
        QString password = QString(QCryptographicHash::hash(iPassword, QCryptographicHash::Md5).toHex());
        qsizetype index = userName.indexOf('@');
        if (index > -1)
            userName.remove(index, userName.size()); // Получение логина от почты

        QSqlQuery query(*_db);
        query.prepare("SELECT "
                      "employee.id, "
                      "role.code, "
                      "role.name as role, "
                      "employee.surname, "
                      "employee.name, "
                      "employee.patronymic, "
                      "employee.sex, "
                      "employee.date_of_birth, "
                      "employee.passport, "
                      "employee.phone, "
                      "employee.email, "
                      "employee.date_of_hiring, "
                      "employee.working_hours, "
                      "employee.salary, "
                      "employee.password "
                      "FROM employee LEFT JOIN role ON employee.role_id = role.id "
                      "WHERE employee.email = :userName AND employee.password = :password;");
        query.bindValue(":userName", userName + "@tradingcompany.ru");
        query.bindValue(":password", password);
        if (!query.exec())
        {
            qCritical() << "Ошибка: " << query.lastError().text();
            return false;
        }

        QJsonObject record;
        if (query.next())
        {
            for (const auto i : std::views::iota(0, query.record().count()))
            {
                if (query.record().fieldName(i) == "code")
                {
                    oRole = query.value(i).toString();
                    continue;
                }
                else if (query.record().fieldName(i) == Employee::id())
                {
                    oID = query.value(i).toString();
                    /// Добавил вывод ID
                    // continue;
                }

                record.insert(query.record().fieldName(i), QJsonValue::fromVariant(query.value(i)));
            }
        }

        oData = QJsonDocument(QJsonObject{{Employee::employeeTable(), std::move(record)}}).toJson();
        return true;
    }

    bool DataBase::authentication(const QString& iID, QByteArray& oData) const
    {
        QSqlQuery query(*_db);
        query.prepare("SELECT "
                      "employee.id, "
                      "role.code, "
                      "role.name as role, "
                      "employee.surname, "
                      "employee.name, "
                      "employee.patronymic, "
                      "employee.sex, "
                      "employee.date_of_birth, "
                      "employee.passport, "
                      "employee.phone, "
                      "employee.email, "
                      "employee.date_of_hiring, "
                      "employee.working_hours, "
                      "employee.salary, "
                      "employee.password "
                      "FROM employee LEFT JOIN role ON employee.role_id = role.id "
                      "WHERE employee.id = :ID;");
        query.bindValue(":ID", iID);
        if (!query.exec())
        {
            qCritical() << "Ошибка: " << query.lastError().text();
            return false;
        }

        QJsonObject record;
        if (query.next())
        {
            for (const auto i : std::views::iota(0, query.record().count()))
            {
                if (query.record().fieldName(i) == "code")
                {
                    continue;
                }
                else if (query.record().fieldName(i) == Employee::id())
                {
                    /// Добавил вывод ID
                    // continue;
                }

                record.insert(query.record().fieldName(i), QJsonValue::fromVariant(query.value(i)));
            }
        }

        oData = QJsonDocument(QJsonObject{{Employee::employeeTable(), std::move(record)}}).toJson();
        return true;
    }

    bool DataBase::getPeronalData(const qint64& iID, const QByteArray& iRole, const QByteArray& iUserName, QByteArray& oData) const
    {
        QString userName = QString::fromUtf8(iUserName);
        QString role = QString::fromUtf8(iRole);
        qsizetype index = userName.indexOf('@');
        if (index > -1)
            userName.remove(index, userName.size()); // Получение логина от почты

        QSqlQuery query(*_db);
        query.prepare("SELECT "
                      "employee.id, "
                      "role.name as role, "
                      "employee.surname, "
                      "employee.name, "
                      "employee.patronymic, "
                      "employee.sex, "
                      "employee.date_of_birth, "
                      "employee.passport, "
                      "employee.phone, "
                      "employee.email, "
                      "employee.date_of_hiring, "
                      "employee.working_hours, "
                      "employee.salary, "
                      "employee.password "
                      "FROM employee LEFT JOIN role ON employee.role_id = role.id "
                      "WHERE employee.id = :id AND role.code = :role AND employee.email = :userName;");
        query.bindValue(":id", iID);
        query.bindValue(":role", role);
        query.bindValue(":userName", userName + "@tradingcompany.ru");
        if (!query.exec())
        {
            qCritical() << "Ошибка: " << query.lastError().text();
            return false;
        }

        QJsonObject record;
        if (query.next())
        {
            for (const auto i : std::views::iota(0, query.record().count()))
                record.insert(query.record().fieldName(i), QJsonValue::fromVariant(query.value(i)));
        }

        oData = QJsonDocument(QJsonObject{{Employee::employeeTable(), record}}).toJson();
        return true;
    }

    bool DataBase::sendRequest(const QByteArray& iRequest) const
    {
        const QString request = QString::fromUtf8(iRequest);
        QSqlQuery query(*_db);
        if (!query.exec(request))
        {
            qCritical() << "Ошибка: " << query.lastError().text();
            return false;
        }

        return true;
    }

    bool DataBase::sendRequest(const QByteArray& iRequest, QByteArray& oData, const QByteArray& iTable) const
    {
        const QString request = QString::fromUtf8(iRequest);
        QSqlQuery query(*_db);
        if (!query.exec(request))
        {
            qWarning() << "Ошибка: " << query.lastError().text() << "запроса";
            return false;
        }

        oData.clear();
        if (query.size() > 1)
        {
            QJsonArray recordsArray;
            while (query.next())
            {
                QJsonObject recordObject;
                for (const auto i : std::views::iota(0, query.record().count()))
                    recordObject.insert(query.record().fieldName(i), QJsonValue::fromVariant(query.value(i)));

                recordsArray.push_back(std::move(recordObject));
            }

            if (!recordsArray.empty())
            {
                if (!iTable.isEmpty())
                {
                    oData = QJsonDocument(QJsonObject{{iTable, std::move(recordsArray)}}).toJson();
                }
                else
                {
                    oData = QJsonDocument(std::move(recordsArray)).toJson();
                }
            }
        }
        else if (query.size() == 1)
        {
            QJsonObject recordObject;
            while (query.next())
            {
                for (const auto i : std::views::iota(0, query.record().count()))
                {
                    recordObject.insert(query.record().fieldName(i), QJsonValue::fromVariant(query.value(i)));
                }
            }

            if (!recordObject.empty())
            {
                if (!iTable.isEmpty())
                {
                    oData = QJsonDocument(QJsonObject{{iTable, std::move(recordObject)}}).toJson();
                }
                else
                {
                    oData = QJsonDocument(std::move(recordObject)).toJson();
                }
            }
        }

        return !oData.isEmpty();
    }

    const QSqlTableModel* DataBase::createTableModel()
    {
        QSqlTableModel* table = new QSqlTableModel(this, *_db);
        table->setTable("employee");
        table->select();
        return table;
    }

    bool DataBase::open()
    {
        _db->setDatabaseName(DATABASE_NAME);
        if (!_db->open())
        {
            qWarning() << "База данных не существует" << _db->lastError().text();
            QMessageBox::StandardButton reply = QMessageBox::question(this, "База данных не сущуествует", "Создать?", QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes)
                return this->createDataBase();
            else
                return false;
        }

        qInfo() << "Выполнено подключение к базе данных" << _db->databaseName();
        return true;
    }

    bool DataBase::createDataBase()
    {
        _db->setDatabaseName("postgres");
        try
        {
            if (!_db->open())
                throw OpenDBError(_db->databaseName());

            qInfo() << "Попытка создать базу данных" << DATABASE_NAME;
            QSqlQuery query(*_db);
            if (!query.exec("create database " + QString(DATABASE_NAME) + " TEMPLATE=template0 ENCODING 'UTF-8' LC_COLLATE 'ru_RU.UTF-8' LC_CTYPE 'ru_RU.UTF-8'"))
                throw CreateDBError(query.lastError().text());

            qInfo() << "База данных:" << DATABASE_NAME <<  "успешно создалась";
            QMessageBox::information(this,  "Успешно!", "База данных " + QString(DATABASE_NAME) + " успешно создалась");
            return this->createTable();
        }
        catch (const OpenDBError& iException)
        {
            qCritical() << "Не выполнено подключение к базе данных " << iException.what();
            std::exception_ptr exception = std::make_exception_ptr(iException);
            std::rethrow_exception(exception);
        }
        catch (const OpenTableError& iException)
        {
            qCritical() << "Не выполнено подключение к таблице " << iException.what();
            std::exception_ptr exception = std::make_exception_ptr(iException);
            std::rethrow_exception(exception);
        }
        catch (const CreateTableError& iException)
        {
            qCritical() << "Ошибка: " << iException.what() << ", таблица не создалась";
            std::exception_ptr exception = std::make_exception_ptr(iException);
            std::rethrow_exception(exception);
        }
        catch (const CreateDBError& iException)
        {
            qCritical() << "Ошибка: " << iException.what() << ", база данных не создалась";
            QMessageBox::critical(this,  "Ошибка!", "База данных " + QString(DATABASE_NAME) + " не создалась");
            std::exception_ptr exception = std::make_exception_ptr(iException);
            std::rethrow_exception(exception);
        }
        catch (...)
        {
            return false;
        }

        return true;
    }

    bool DataBase::createTable()
    {
        QSqlQuery query(*_db);
        if (!query.exec("CREATE TABLE IF NOT EXISTS public.employee"
                        "(id integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 CACHE 1 ),"
                        "role_id integer NOT NULL,"
                        "surname text COLLATE pg_catalog.\"default\" NOT NULL,"
                        "name text COLLATE pg_catalog.\"default\" NOT NULL,"
                        "patronymic text COLLATE pg_catalog.\"default\" NOT NULL,"
                        "sex character varying(3) COLLATE pg_catalog.\"default\" NOT NULL,"
                        "date_of_birth date NOT NULL,"
                        "passport bigint NOT NULL,"
                        "phone bigint NOT NULL,"
                        "email text COLLATE pg_catalog.\"default\" NOT NULL,"
                        "date_of_hiring date NOT NULL,"
                        "working_hours text COLLATE pg_catalog.\"default\" NOT NULL,"
                        "salary numeric(10,2) NOT NULL,"
                        "password character varying(128) COLLATE pg_catalog.\"default\" NOT NULL,"
                        "CONSTRAINT employee_pkey PRIMARY KEY (id),"
                        "CONSTRAINT role_pkey FOREIGN KEY (role_id)"
                        "REFERENCES public.role (id) MATCH SIMPLE"
                        "ON UPDATE NO ACTION"
                        "ON DELETE NO ACTION)"))
        {
            throw CreateTableError(query.lastError().text());
        }

        if (!open())
            throw OpenTableError(Employee::employeeTable());

        qInfo() << "Таблица успешно создалась";
        return true;
    }

    bool DataBase::insertRecord(const QHash<QString, QVariant>& iData) const
    {
        if (iData.size() != 14)
        {
            qCritical() << "Недостаточно данных для выполнения запроса";
            return false;
        }

        QSqlQuery query(*_db);
        query.prepare("INSERT INTO public.employee ("
                      "id, "
                      "role_id, "
                      "surname, "
                      "name, "
                      "patronymic, "
                      "sex, "
                      "date_of_birth, "
                      "passport, "
                      "phone, "
                      "email, "
                      "date_of_hiring, "
                      "working_hours, "
                      "salary, "
                      "password) "
                      "OVERRIDING SYSTEM VALUE VALUES ("
                      ":id, "
                      "(SELECT role.id FROM role WHERE role.name = :role_name), "
                      ":surname, "
                      ":name, "
                      ":patronymic, "
                      ":sex, "
                      ":date_of_birth, "
                      ":passport, "
                      ":phone, "
                      ":email, "
                      ":date_of_hiring, "
                      ":working_hours, "
                      ":salary, "
                      ":password);");

        query.bindValue(":id", iData[Employee::id()].toULongLong());
        query.bindValue(":role_name", iData[Employee::role()].toString());
        query.bindValue(":surname", iData[Employee::surname()].toString());
        query.bindValue(":name", iData[Employee::name()].toString());
        query.bindValue(":patronymic", iData[Employee::patronymic()].toString());
        query.bindValue(":sex", iData[Employee::sex()].toString());
        query.bindValue(":date_of_birth", iData[Employee::dateOfBirth()].toString());
        query.bindValue(":passport", iData[Employee::passport()].toULongLong());
        query.bindValue(":phone", iData[Employee::phone()].toULongLong());
        query.bindValue(":email", iData[Employee::email()].toString());
        query.bindValue(":date_of_hiring", iData[Employee::dateOfHiring()].toString());
        query.bindValue(":working_hours", iData[Employee::workingHours()].toString());
        query.bindValue(":salary", iData[Employee::salary()].toDouble()); // numeric как-то по особому обрабатывает
        query.bindValue(":password", iData[Employee::password()].toString());
        if (!query.exec())
        {
            qCritical() << "Ошибка: " << query.lastError().text();
            return false;
        }

        return true;
    }

    bool DataBase::deleteRecord(const qint64& iID) const
    {
        QSqlQuery query(*_db);

        query.prepare("DELETE FROM employee WHERE id = :ID;");
        query.bindValue(":ID", iID);
        if (!query.exec())
        {
            qCritical() << "Ошибка: " << query.lastError().text();
            return false;
        }

        return true;
    }

    bool DataBase::updateRecord(const qint64& iID, const QByteArray& iColumn, const QVariant &iValue) const
    {
        QSqlQuery query(*_db);
        if (iColumn == Employee::role())
        {
            query.prepare("UPDATE employee SET role_id = (select role.id FROM role WHERE role.name = :value) "
                          "FROM role WHERE employee.role_id = role.id "
                          "AND employee.id = :ID");
        }
        else
        {
            query.prepare("UPDATE employee SET " + iColumn + " = :value WHERE id = :ID;");
        }
        query.bindValue(":value", iValue.toString());
        query.bindValue(":ID", iID);
        if (!query.exec())
        {
            qCritical() << "Ошибка: " << query.lastError().text();
            return false;
        }

        return true;
    }
}
