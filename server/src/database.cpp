#include "database.h"
#include "utils.h"

#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlTableModel>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCryptographicHash>

#define DATABASE_POSTGRES "QPSQL"         // Тип базы данных
#define DATABASE_HOSTNAME "127.0.0.1"     // Хост
#define DATABASE_PORT      5432           // Порт
#define DATABASE_NAME     "employees"     // Название базы данных

/// TODO: сделать методы update, get, where...

namespace Server
{
    DataBase::DataBase(QWidget *parent) : QWidget(parent)
    {

    }

    DataBase::~DataBase()
    {
        close();
    }

    void DataBase::close()
    {
        _db.close();
    }

    bool DataBase::connectToDataBase()
    {
        _db = QSqlDatabase::addDatabase("QPSQL");
        _db.setHostName(DATABASE_HOSTNAME);
        _db.setPort(DATABASE_PORT);
        _db.setUserName("agaltsevstas");
        _db.setPassword("");

        if (this->open())
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool DataBase::authentication(const QByteArray &iUserName, const QByteArray &iPassword, QString &oID, QString &oRole, QByteArray& oData)
    {
        QString userName = iUserName;
        QString password = QString(QCryptographicHash::hash(iPassword, QCryptographicHash::Md5).toHex());
        qsizetype index = userName.indexOf('@');
        if (index > -1)
            userName.remove(index, userName.size()); // Получение логина от почты

        QSqlQuery query(_db);
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
            qWarning() << "Ошибка: " << query.lastError().text() << "аутентификации";
            return false;
        }

        QJsonObject record;
        if (query.next())
        {
//            query >> _employee;

            auto size = query.record().count();
            for (decltype(size) i = 0; i < size; ++i)
            {
                if (query.record().fieldName(i) == "code")
                {
                    oRole = query.value(i).toString();
                    continue;
                }
                else if (query.record().fieldName(i) == Employee::id())
                {
                    oID = query.value(i).toString();
//                    continue;
                }

                record.insert(query.record().fieldName(i), QJsonValue::fromVariant(query.value(i)));
            }
        }

        oData = QJsonDocument(QJsonObject{{Employee::employeeTable(), record}}).toJson();
        qInfo() << oData;
        return true;
    }

    bool DataBase::getPeronalData(const qint64 &iID, const QString &iRole, const QString &iUserName, QByteArray& oData)
    {
        QString userName = iUserName;
        qsizetype index = userName.indexOf('@');
        if (index > -1)
            userName.remove(index, userName.size()); // Получение логина от почты

        QSqlQuery query(_db);
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
        query.bindValue(":role", iRole);
        query.bindValue(":userName", userName + "@tradingcompany.ru");
        if (!query.exec())
        {
            qWarning() << "Ошибка: " << query.lastError().text() << "аутентификации";
            return false;
        }

        QJsonObject record;

        if (query.next())
        {
//            query >> _employee;

            auto size = query.record().count();
            for (decltype(size) i = 0; i < size; ++i)
            {
                record.insert(query.record().fieldName(i), QJsonValue::fromVariant(query.value(i)));
            }
        }

        oData = QJsonDocument(QJsonObject{{Employee::employeeTable(), record}}).toJson();
        return true;
    }

    bool DataBase::sendRequest(const QByteArray &iRequest)
    {
        const QString request = QString::fromUtf8(iRequest);
        QSqlQuery query(_db);
        if (!query.exec(request))
        {
            qWarning() << "Ошибка: " << query.lastError().text() << "запроса";
            return false;
        }

        return true;
    }

    bool DataBase::sendRequest(const QByteArray &iRequest, QByteArray &oData, const QByteArray &iTable)
    {
        const QString request = QString::fromUtf8(iRequest);
        QSqlQuery query(_db);
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
                for (decltype(query.record().count()) i = 0, I = query.record().count(); i < I; ++i)
                {
                    recordObject.insert(query.record().fieldName(i), QJsonValue::fromVariant(query.value(i)));
                }

                recordsArray.push_back(recordObject);
            }

            if (!recordsArray.empty())
            {
                if (!iTable.isEmpty())
                {
                    oData = QJsonDocument(QJsonObject{{iTable, recordsArray}}).toJson();
                }
                else
                {
                    oData = QJsonDocument(recordsArray).toJson();
                }
            }
        }
        else if (query.size() == 1)
        {
            QJsonObject recordObject;
            while (query.next())
            {
                for (decltype(query.record().count()) i = 0, I = query.record().count(); i < I; ++i)
                {
                    recordObject.insert(query.record().fieldName(i), QJsonValue::fromVariant(query.value(i)));
                }
            }

            if (!recordObject.empty())
            {
                if (!iTable.isEmpty())
                {
                    oData = QJsonDocument(QJsonObject{{iTable, recordObject}}).toJson();
                }
                else
                {
                    oData = QJsonDocument(recordObject).toJson();
                }
            }
        }

        return !oData.isEmpty();
    }

    QSqlTableModel* DataBase::createTableModel()
    {
        QSqlTableModel* table = new QSqlTableModel(this, _db);
        table->setTable("employee");
        table->select();
        return table;
    }

    bool DataBase::open()
    {
        _db.setDatabaseName(DATABASE_NAME);
        if (!_db.open())
        {
            qWarning() << "База данных не существует" << _db.lastError().text();
            QMessageBox::StandardButton reply = QMessageBox::question(this, "База данных не сущуествует", "Создать?", QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes)
            {
                return this->createDataBase();
            }
            else
            {
                qInfo() << "Выход из программы";
                QApplication::quit();
            }
        }

        qInfo() << "Выполнено подключение к базе данных" << _db.databaseName();
        return true;
    }

    bool DataBase::createDataBase()
    {
        _db.setDatabaseName("postgres");
        if (!_db.open())
        {
            qInfo() << "Не выполнено подключение к базе данных" << _db.databaseName();
            return false;
        }

        qInfo() << "Попытка создать базу данных" << DATABASE_NAME;
        QSqlQuery query(_db);
        if (!query.exec("create database " DATABASE_NAME " TEMPLATE=template0 ENCODING 'UTF-8' LC_COLLATE 'ru_RU.UTF-8' LC_CTYPE 'ru_RU.UTF-8'"))
        {
            qCritical() << "Ошибка: " << query.lastError().text() << ", база данных не создалась";
            QMessageBox::critical(this,  "Ошибка!", "База данных " + QString(DATABASE_NAME) + " не создалась");
            return false;
        }

        qInfo() << "База данных:" << DATABASE_NAME <<  "успешно создалась";
        QMessageBox::information(this,  "Успешно!", "База данных " + QString(DATABASE_NAME) + " успешно создалась");
        return this->createTable();
    }

    bool DataBase::createTable()
    {
        QSqlQuery query(_db);
        if (!query.exec("CREATE TABLE IF NOT EXISTS employee(id serial primary key, "
                        "position text NOT NULL, "
                        "surname text NOT NULL, "
                        "name text NOT NULL, "
                        "patronymic text NOT NULL, "
                        "sex varchar(3) NOT NULL, "
                        "dateOfBirth date NOT NULL, "
                        "passport bigint NOT NULL, "
                        "phone bigint NOT NULL, "
                        "email text NOT NULL, "
                        "dateOfHiring date NOT NULL, "
                        "workingHours text NOT NULL, "
                        "salary money NOT NULL, "
                        "password varchar(128) NOT NULL);"))
        {
            qCritical() << "Ошибка: " << query.lastError().text() << ", таблица не создалась";
            return false;
        }

        qInfo() << "Таблица успешно создалась";
        return this->open();
    }

    bool DataBase::inserIntoTable(const QVariantList &data)
    {
        /* Запрос SQL формируется из QVariantList,
         * в который передаются данные для вставки в таблицу.
         * */
        QSqlQuery query;
        QSqlDatabase::database().commit();
        /* В начале SQL запрос формируется с ключами,
         * которые потом связываются методом bindValue
         * для подстановки данных из QVariantList
         * */
//        query.prepare("INSERT INTO " TABLE " ( " TABLE_FNAME ", "
//                                                 TABLE_SNAME ", "
//                                                 TABLE_NIK " ) "
//                      "VALUES (:FName, :SName, :Nik)");
//        query.bindValue(":FName",       data[0].toString());
//        query.bindValue(":SName",       data[1].toString());
//        query.bindValue(":Nik",         data[2].toString());

        // После чего выполняется запросом методом exec()
//        if (!query.exec())
//        {
//            qDebug() << "error insert into " << TABLE;
//            qDebug() << query.lastError().text();
//            return false;
//        }
//        else
//        {
//            return true;
//        }
        return false;
    }

    bool DataBase::inserIntoTable(const QString &fname, const QString &sname, const QString &nik)
    {
        QVariantList data;
        data.append(fname);
        data.append(sname);
        data.append(nik);

        return inserIntoTable(data);
    }

    bool DataBase::removeRecord(const qint64 &iID)
    {
        QSqlQuery query(_db);

        query.prepare("DELETE FROM employee WHERE id = :id;");
        query.bindValue(":ID", iID);

        if (!query.exec())
        {
            qDebug() << query.lastError().text();
            return false;
        }

        return true;
    }
}
