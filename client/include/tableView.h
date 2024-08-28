#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>


class QJsonTableModel;

namespace Client
{
    class TableView final : public QTableView
    {
        Q_OBJECT
        Q_PROPERTY(bool sortingEnabled READ isSortingEnabled WRITE setSortingEnabled)
        using HandleField = std::function<bool(const QString&)>;
        using HandleResponse = std::function<void(const bool, const QVariant&)>;

    public:
        explicit TableView(QWidget* parent = nullptr);

        enum EditStrategy
        {
            OnFieldChange,
            OnManualSubmit
        };

        /*!
         * \brief Установить стратегию для отправки данных на сервер
         * \param iStrategy - OnFieldChange - данные отправляются автоматически, OnManualSubmit - данные отправляются вручную
         */
        void setEditStrategy(EditStrategy iStrategy) noexcept;

        /*!
         * \brief Получить стратегию для отправки данныз на север
         * \return OnFieldChange - данные отправляются автоматически, OnManualSubmit - данные отправляются вручную
         */
        [[nodiscard("getEditStrategy")]] EditStrategy getEditStrategy() const noexcept;

        /*!
         * \brief Установить БД
         * \param iName - Имя модели
         * \param iDatabase - БД
         * \param iPermissions - Права
         */
        void setModel(const QString& iName, const QJsonDocument& iDatabase, const QJsonDocument& iPermissions);

        /*!
         * \brief Установить модель данных
         * \param iName - Имя модели
         * \param iDatabase - БД
         */
        void setModel(const QString& iName, const QJsonDocument& iDatabase);

        /*!
         * \brief Получить БД
         * \return БД
         */
        [[nodiscard("getModel")]] const QAbstractItemModel* getModel() const noexcept;

        /*!
         * \brief Отправка изменений на сервер
         */
        void submitAll();

        /*!
         * \brief Откатить все изменения
         */
        void revertAll();

        /*!
         * \brief Добавить нового пользователя
         * \return true - Пользователь успешно добавлен, иначе false
         */
        bool addUser();

        /*!
         * \brief Удаление пользователя
         * \return true - Пользователь успешно удален, иначе false
         */
        bool deleteUser();

        /*!
         * \brief Восстановление пользователя
         */
        void restoreUser();

        /*!
         * \brief Проверка на удаление пользователя
         * \return true - можно удалить, иначе false
         */
        [[nodiscard("canDeleteUser")]] std::optional<bool> canDeleteUser();

        /*!
         * \brief Проверка на какие-либо изменения
         * \return true - есть изменения, false - нет изменений
         */
        bool checkChanges() const noexcept;

    Q_SIGNALS:

        /*!
         * \brief Отправка запроса на сервер на создание данных
         * \param iRequest - Запрос с данными
         * \param handleResponse - ответ на запрос
         */
        void sendCreateData(const QByteArray& iData, const HandleResponse& handleResponse = Q_NULLPTR);

        /*!
         * \brief Отправка запроса на сервер на удаление данных
         * \param iRequest - Запрос с данными
         * \param handleResponse - Ответ на запрос
         */
        void sendDeleteData(const QByteArray& iData, const HandleResponse& handleResponse = Q_NULLPTR);

        /*!
         * \brief Отправка запроса на сервер на обновление данных
         * \param iRequest - Запрос с данными
         * \param handleResponse - Ответ на запрос
         */
        void sendUpdateData(const QByteArray& iData, const HandleResponse& handleResponse = Q_NULLPTR);

        /*!
         * \brief Получение пользовательских данных
         * \param iFieldName - Имя поля
         * \param handleField - Ответ на запрос
         */
        void getUserData(const QString& iFieldName, const HandleField& handleField);

    private:
        void setModel(QAbstractItemModel* model) override;

    public Q_SLOTS:
        void valueSearchChanged(const QString& iValue);
        void clearSearchChanged();

    private:
        QList<int> _hiddenIndices;
        QJsonTableModel* _model = nullptr;
    };
}

#endif // TABLEVIEW_H
