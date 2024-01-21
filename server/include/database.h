#ifndef DATABASE_H
#define DATABASE_H

#include <QWidget>
#include <QSqlDatabase>


class QSqlDatabase;
class QSqlTableModel;

namespace Server
{
    class DataBase : public QWidget
    {
        Q_OBJECT
        Q_DISABLE_COPY(DataBase);
    public:

        explicit DataBase(QWidget *parent = nullptr);
        ~DataBase();

        /*!
         * \brief Подключение к БД
         * \return true - успех, иначе false
         */
        bool connect();

        /*!
         * \brief Проверка значение на дубликат
         * \param iColumn - Столбец
         * \param iValue - Значение
         * \return true - проверка пройдена, иначе false
         */
        bool checkFieldOnDuplicate(const QByteArray& iColumn, const QVariant& iValue) const;

        /*!
         * \brief Аутентификация
         * \param iUserName - Имя пользователя
         * \param iPassword - Пароль
         * \param oID - Получение id пользователя
         * \param oRole - Получение роли пользователя
         * \param oData - Получение данных пользователя
         * \return true - успех, иначе false
         */
        bool authentication(const QByteArray& iUserName, const QByteArray& iPassword, QString& oID, QString& oRole, QByteArray& oData) const;

        /*!
         * \brief Аутентификация
         * \param iID - id пользователя
         * \param oData - Получение данных пользователя
         * \return true - успех, иначе false
         */
        bool authentication(const QString& iID, QByteArray& oData) const;

        /*!
         * \brief Получение персональных данных
         * \param iID пользователя
         * \param iRole - Роль пользователя
         * \param iUserName - Имя пользователя
         * \param oData - Данные пользователя
         * \return true - успех, иначе false
         */
        bool getPeronalData(const qint64& iID, const QByteArray& iRole, const QByteArray& iUserName, QByteArray& oData) const;

        /*!
         * \brief Отправка запроса на сервер
         * \param iRequest - Запрос
         * \return true - успех, иначе false
         */
        bool sendRequest(const QByteArray& iRequest) const;

        /*!
         * \brief Отправка запроса на сервер
         * \param iRequest - Запрос
         * \param oData - Получение данных
         * \param iTable - Имя таблицы
         * \return true - успех, иначе false
         */
        bool sendRequest(const QByteArray& iRequest, QByteArray& oData, const QByteArray& iTable = {}) const;

        /*!
         * \brief Добавление записи в таблицу
         * \param iData - Новая запись
         * \return true - успех, иначе false
         */
        bool insertRecord(const QHash<QString, QVariant>& iData) const;

        /*!
         * \brief Удаление записи из таблицы по id пользователя
         * \param iID пользователя
         * \return true - успех, иначе false
         */
        bool deleteRecord(const qint64& iID) const;

        /*!
         * \brief Обновление записи из таблицы по id пользователя
         * \param iID пользователя
         * \param iColumn - Столбец
         * \param iValue - Значение
         * \return true - успех, иначе false
         */
        bool updateRecord(const qint64& iID, const QByteArray& iColumn, const QVariant& iValue) const;

        /*!
         * \brief Создание таблицы в БД
         * \return Созданная таблица
         */
        [[nodiscard("createTableModel")]] const QSqlTableModel* createTableModel();

    private:
        bool open();
        bool createDataBase();
        bool createTable();

    private:
        QScopedPointer<QSqlDatabase> _db;
    };
}

#endif // DATABASE_H
