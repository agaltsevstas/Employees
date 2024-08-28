#ifndef QJSONTABLEMODEL_H
#define QJSONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>


namespace Client
{
    class TableView;
}

class JsonTableModel;

class QJsonTableModel final : public QAbstractTableModel
{
    Q_OBJECT
    using HandleResponse = std::function<void(const bool, const QVariant&)>;

public:
    QJsonTableModel(const QString& iName, const QJsonDocument& iDatabase, const QJsonDocument& iPermissions, QObject* parent = nullptr);
    QJsonTableModel(const QString& iName, const QJsonDocument& iDatabase, QObject* parent = nullptr);

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
    [[nodiscard("getEditStrategy")]] inline EditStrategy getEditStrategy() const noexcept { return _strategy; }

    /*!
     * \brief Отправка изменений на сервер
     */
    void submitAll();

    /*!
     * \brief Откатить все изменения
     */
    void revertAll();

    /*!
     * \brief Проверить поле на дубликат/валидность
     * \param row - Строка
     * \param column - Столбец
     * \param iValue - Значение
     * \return true - проверка прошла, иначе нет
     */
    bool checkField(int row, int column, const QString& iValue) const;

    /*!
     * \brief Проверить поле на дубликат/валидность
     * \param index - Индекс, который ключает строку и столбец
     * \param iValue - Значение
     * \return true - проверка прошла, иначе нет
     */
    bool checkField(const QModelIndex& index, const QString& iValue) const;

    /*!
     * \brief Добавить новую строку
     * \param iEmployee - Пользователь
     */
    void addRow(const QJsonObject& iEmployee);

    /*!
     * \brief Удаление строки по индексу
     * \param row - Индекс строки
     */
    void deleteRow(int row);

    /*!
     * \brief Восстановить строку по индекс
     * \param row - Индекс строки
     */
    void restoreRow(int row);

    /*!
     * \brief Проверка на удаление строки
     * \param row - Индекс строки
     * \return true - можно удалить, иначе false
     */
    bool canDeleteRow(int row);

    /*!
     * \brief Проверка на какие-либо изменения
     * \return true - есть изменения, false - нет изменений
     */
    bool checkChanges() const noexcept;

    /*!
     * \brief Поиск по значению
     * \param iValue - Значение
     * \return Возврат всех индексов строк, где присутствует искомое значение
     */
    [[nodiscard("search")]] QList<int> search(const QString& iValue) const noexcept;

    /*!
     * \brief Кастомная модель для роли/пола
     * \param column - номер столбца (роль/пол)
     * \return Кастомная модель
     */
    [[nodiscard("relationModel")]] QAbstractItemModel* relationModel(int column) const;

    /*!
     * \brief Число столбцов
     * \param parent - Индекс
     * \return Число столбцов
     */
    [[nodiscard("rowCount")]] int rowCount(const QModelIndex& parent = QModelIndex()) const noexcept override;

Q_SIGNALS:

    /*!
     * \brief Отправка запроса на сервер на создание данных
     * \param iRequest - Запрос с данными
     * \param handleResponse - ответ на запрос
     */
    void sendCreateRequest(const QByteArray& iRequest, const HandleResponse& handleResponse = Q_NULLPTR);

    /*!
     * \brief Отправка запроса на сервер на удаление данных
     * \param iRequest - Запрос с данными
     * \param handleResponse - Ответ на запрос
     */
    void sendDeleteRequest(const QByteArray& iRequest, const HandleResponse& handleResponse = Q_NULLPTR);

    /*!
     * \brief Отправка запроса на сервер на обновление данных
     * \param iRequest - Запрос с данными
     * \param handleResponse - Ответ на запрос
     */
    void sendUpdateRequest(const QByteArray& iRequest, const HandleResponse& handleResponse = Q_NULLPTR);

private:
    bool setDatabase(const QJsonDocument& iDatabase) noexcept;
    bool setDatabase(const QJsonArray& iDatabase) noexcept;
    bool setPermissions(const QJsonDocument& iPermissions);
    bool setPermissions(const QJsonObject& iPermissions);
    void setJsonObject(int row, const QJsonObject& iJsonObject);
    [[nodiscard("getJsonObject")]] QJsonObject getJsonObject(int row) const;
    bool isSortColumn(int column) const;
    bool sortColumn(const QJsonValue& first, const QJsonValue& second, int column, Qt::SortOrder order = Qt::SortOrder::AscendingOrder) const;
    void updateRecord(int row, const QString &iColumnName, const QString &iValue);
    bool createEmail(int row);
    bool checkFieldOnDuplicate(int row, int column, QString& iValue) const;
    bool checkRowOnDeleted(int row) const;
    bool checkRowOnCreated(int row) const;
    bool checkFieldOnUpdated(const QModelIndex& index) const;

private:
    [[nodiscard("headerData")]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const noexcept override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    [[nodiscard("data")]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard("flags")]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

private:
    QString _name;
    EditStrategy _strategy = OnFieldChange;
    QVector<QPair<QPair<QString, QString>, bool>> _headers;
    QJsonArray _array;
    QJsonArray _recordsCache, _recordsCreatedCache, _recordsDeletedCache, _recordsUpdatedCache;

    friend class Delegate;
};

#endif // QJSONTABLEMODEL_H
