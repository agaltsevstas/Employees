#ifndef DATABASE_H
#define DATABASE_H

#include "employee.h"

#include <QWidget>
#include <QSqlDatabase>

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
        bool authentication(const QString &iUserName, const QString &iPassword, QByteArray &oData);
        QSqlTableModel* createTableModel();

    private:
        /// Внутренние методы для работы с базой данных
        bool open();           // Открытие базы данных
        bool restore();        // Восстановление базы данных
        void close();          // Закрытие базы данных
        bool createDataBase(); // Создание базы данных
        bool createTable();    // Создание базы таблицы в базе данных

    public slots:
        bool inserIntoTable(const QVariantList &data); // Добавление записей в таблицу
        bool inserIntoTable(const QString &fname, const QString &sname, const QString &nik); // Добавление записей в таблицу
        bool removeRecord(const int id); // Удаление записи из таблицы по её id

    private:
        QSqlDatabase _db; // Объект базы данных
        Employee _employee;
        QString _role;
    };
}

#endif // DATABASE_H
