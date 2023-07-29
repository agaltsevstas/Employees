#ifndef DATABASE_H
#define DATABASE_H

#include "server.h"

#include <QWidget>
#include <QSqlDatabase>
#include <QJsonArray>

class QSqlTableModel;

namespace Server
{
    class DataBase : public QWidget
    {
        Q_OBJECT
    public:

        explicit DataBase(QWidget *parent = nullptr);
        ~DataBase();
        bool connect(); // Подключение к базе данных

        bool checkFieldOnDuplicate(const QByteArray &iColumn, const QVariant &iValue) const;
        bool authentication(const QByteArray &iUserName, const QByteArray &iPassword, QString &oID, QString &oRole, QByteArray& oData) const;
        bool getPeronalData(const qint64 &iID, const QByteArray &iRole, const QByteArray &iUserName, QByteArray& oData) const;
        bool sendRequest(const QByteArray &iRequest) const;
        bool sendRequest(const QByteArray &iRequest, QByteArray &oData, const QByteArray &iTable = {}) const;
        bool insertRecord(const QMap<QString, QVariant> &iData) const; // Добавление записи в таблицу
        bool deleteRecord(const qint64 &iID) const; // Удаление записи из таблицы по id
        bool updateRecord(const qint64 &iID, const QByteArray &iColumn, const QVariant &iValue) const; // Обновление записи из таблицы по id
        const QSqlTableModel* createTableModel();

    private:
        /// Внутренние методы для работы с базой данных
        bool open();           // Открытие базы данных
        bool createDataBase(); // Создание базы данных
        bool createTable();    // Создание базы таблицы в базе данных

    private:
        QJsonArray _records;
        QSqlDatabase _db;
        Employee _employee;
    };
}

#endif // DATABASE_H
