#ifndef TABLE_P_H
#define TABLE_P_H

#include <QWidget>
#include <QJsonArray>
#include <QHash>


namespace Client
{
    class TablePrivate final : public QWidget
    {
        Q_OBJECT
        using HandleField = std::function<bool(const QString&)>;
        using HandleResponse = std::function<void(const bool, const QVariant&)>;

    public:
        TablePrivate(const QString& iName, const QJsonDocument& iData, const QJsonDocument& iPersonalPermissions, const QJsonDocument& iPermissions, QWidget* parent = nullptr);

        explicit TablePrivate(const QString& iName, QWidget* parent = nullptr);
        ~TablePrivate();

        enum EditStrategy
        {
            OnFieldChange,
            OnManualSubmit
        };

        /*!
         * \brief Установить стратегию для отправки данных на сервер
         * \param iStrategy - OnFieldChange - данные отправляются автоматически, OnManualSubmit - данные отправляются вручную
         */
        void setEditStrategy(EditStrategy iStrategy);

        /*!
         * \brief Получить стратегию для отправки данныз на север
         * \return OnFieldChange - данные отправляются автоматически, OnManualSubmit - данные отправляются вручную
         */
        [[nodiscard("getEditStrategy")]] inline constexpr EditStrategy getEditStrategy() const noexcept { return _strategy; }

        /*!
         * \brief Отправка изменений на сервер
         */
        void submitAll();

        /*!
         * \brief Откатить все изменения
         */
        void revertAll();

        /*!
         * \brief Проверка на какие-либо изменения
         * \return true - есть изменения, false - нет изменений
         */
        bool checkChanges() const noexcept;

    Q_SIGNALS:

        /*!
         * \brief Отправка запроса с изменениями на сервер
         * \param iRequest - Запрос с данными
         * \param handleResponse - Ответ на запрос
         */
        void sendRequest(const QByteArray& iRequest, const HandleResponse& handleResponse = Q_NULLPTR);

        /*!
         * \brief Отправка слова для поиска
         * \param iValue - Значение слова
         */
        void sendValueSearch(const QString& iValue);

        /*!
         * \brief Сообщение об очистке поискового слова
         */
        void sendClearSearch();

        /*!
         * \brief Выход из личного кабинета
         */
        void logout();

    public Q_SLOTS:

        /*!
         * \brief Отправка пользовательских данных
         * \param iFieldName - Имя поля
         * \param handleField - Ответ на запрос
         */
        void sendUserData(const QString& iFieldName, const HandleField& handleField = Q_NULLPTR);

    private Q_SLOTS:

        /*!
         * \brief Откатить данные с сервера
         */
        void onResetDataClicked();

        /*!
         * \brief Нажатие на поисковое слово
         */
        void onSearchClicked();

    private Q_SLOTS:
        void createEmail();

    private Q_SLOTS:
        void update(const QString& iValue);

    private:
        bool _isRoleChanged = false;
        QString _name;
        EditStrategy _strategy = OnFieldChange;
        QHash<QString, QString> _dataCache;
        QJsonArray _recordsCache;
    };
}

#endif // TABLE_P_H

