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

        explicit DataBase(QWidget *parent = 0);
        ~DataBase();
        bool connectToDataBase(); // Подключение к базе данных
        bool authentication(const QByteArray &iUserName, const QByteArray &iPassword, QString &oID, QString &oRole, QByteArray& oData);
        bool getPeronalData(const qint64 &iID, const QString &iRole, const QString &iUserName, QByteArray& oData);
        bool sendRequest(const QByteArray &iRequest);
        bool sendRequest(const QByteArray &iRequest, QByteArray &oData, const QByteArray &iTable = {});
        bool inserIntoTable(const QVariantList &data); // Добавление записей в таблицу
        bool inserIntoTable(const QString &fname, const QString &sname, const QString &nik); // Добавление записей в таблицу
        bool removeRecord(const qint64 &iID); // Удаление записи из таблицы по id
        QSqlTableModel* createTableModel();

    private:
        /// Внутренние методы для работы с базой данных
        bool open();           // Открытие базы данных
        bool restore();        // Восстановление базы данных
        void close();          // Закрытие базы данных
        bool createDataBase(); // Создание базы данных
        bool createTable();    // Создание базы таблицы в базе данных

    private:
        QJsonArray _records;
        QSqlDatabase _db; // Объект базы данных
        Employee _employee;
    };
}

#endif // DATABASE_H
