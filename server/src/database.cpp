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

#define DATABASE_POSTGRES "QPSQL"         // Тип базы данных
#define DATABASE_HOSTNAME "127.0.0.1"     // Хост
#define DATABASE_PORT      5432           // Порт
#define DATABASE_NAME     "employees"     // Название базы данных

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

    bool DataBase::authentication(const QString &iUserName, const QString &iPassword, QByteArray &oData)
    {
        QString userName = iUserName;
        qsizetype index = userName.indexOf('@');
        if (index > -1)
            userName.remove(index, userName.size()); // Получение логина от почты

        const QString str = "select * from " + QString::fromStdString(Employee::TableName()) + " where " +
                                               QString::fromStdString(Employee::Email()) + " = '" + userName + "@tradingcompany.ru' and " +
                                               QString::fromStdString(Employee::Password()) + " = '" + iPassword +"';";

        QSqlQuery query(_db);
        if (!query.exec(str))
        {
            qWarning() << "Ошибка: " << query.lastError().text() << "аутентификации";
            return false;
        }

        QJsonDocument json;
        QJsonArray recordsArray;
        QJsonObject recordObject;
        while (query.next())
        {
            query >> _employee;
            auto size = query.record().count();
            for (decltype(size) i = 0; i < size; ++i)
            {
                recordObject.insert(query.record().fieldName(i), QJsonValue::fromVariant(query.value(i)));
            }
            recordsArray.push_back(recordObject);
        }
        json.setArray(recordsArray);
        oData = json.toJson();
        return true;

//        try
//        {
//            for (const auto &object: _vectorObjects)
//            {
//                std::string emailCheck, loginCheck;
//                emailCheck = loginCheck = object->_email;
//                loginCheck.erase(loginCheck.find('@'), loginCheck.size()); // Получение логина от почты
//                if (login == emailCheck || login == loginCheck)
//                {
//                    isLoginFound = true;
//                    break;
//                }
//            }

//            for (auto object: _vectorObjects)
//            {
//                if (isLoginFound && password == object->_password)
//                {
//                    LOGIN(object);
//                    CheckParameters(object.get(), true);
//                    object->Functional();
//                    LOGOUT(object);

//                    AccountLogin(); // Рекурсия
//                }
//            }
//        }

        return false;
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
        const QString str = "create database " DATABASE_NAME " TEMPLATE=template0 ENCODING 'UTF-8' LC_COLLATE 'ru_RU.UTF-8' LC_CTYPE 'ru_RU.UTF-8'";
        QSqlQuery query(_db);
        if (!query.exec(str))
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
        QSqlQuery query;
        const QString table = "create table if not exists employee(id serial primary key, "
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
                              "password varchar(128) NOT NULL)";

        if (!query.exec(table))
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

        if (inserIntoTable(data))
            return true;
        else
            return false;
    }

    /* Метод для удаления записи из таблицы
     * */
    bool DataBase::removeRecord(const int id)
    {
        // Удаление строки из базы данных будет производитсья с помощью SQL-запроса
        QSqlQuery query;

        // Удаление производим по id записи, который передается в качестве аргумента функции
//        query.prepare("DELETE FROM " TABLE " WHERE id= :ID ;");
//        query.bindValue(":ID", id);

//        // Выполняем удаление
//        if (!query.exec())
//        {
//            qDebug() << "error delete row " << TABLE;
//            qDebug() << query.lastError().text();
//            return false;
//        }
//        else
//        {
//            return true;
//        }
        return false;
    }
}
