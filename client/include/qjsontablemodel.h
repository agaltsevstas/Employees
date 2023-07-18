#ifndef QJSONTABLEMODEL_H
#define QJSONTABLEMODEL_H

#include "QtGui/qcolor.h"
#include <QAbstractTableModel>
#include <QJsonArray>
#include <QPair>


namespace Client
{
    class TableView;
}

class JsonTableModel;

class QJsonTableModel final : public QAbstractTableModel
{
    Q_OBJECT
    typedef std::function<void(const bool, const QString&)> HandleResponse;

public:
    QJsonTableModel(const QString& iName, const QJsonDocument &iDatabase, const QJsonDocument &iPermissions, QObject *parent = nullptr);
    QJsonTableModel(const QString& iName, const QJsonDocument &iDatabase, QObject *parent = nullptr);

    enum EditStrategy {OnFieldChange, OnRowChange, OnManualSubmit};

    void setEditStrategy(EditStrategy iStrategy) noexcept;
    inline EditStrategy egetEitStrategy() const noexcept { return _strategy; }

    bool setDatabase(const QJsonDocument &iDatabase) noexcept;
    bool setDatabase(const QJsonArray &iDatabase) noexcept;

    bool setPermissions(const QJsonDocument &iPermissions);
    bool setPermissions(const QJsonObject &iPermissions);

    void submitAll();
    bool checkField(int row, int column, const QString &ivalue) const;
    bool checkField(const QModelIndex &index, const QString &value) const;
    void addRow(const QJsonObject &iUser);
    void deleteRow(int row);
    void restoreRow(int row);
    bool canDeleteRow(int row);
    [[nodiscard]] QList<int> valueSearch(const QString &iValue) const noexcept;
    [[nodiscard]] QAbstractItemModel *relationModel(int column) const;
    [[nodiscard]] inline qsizetype rowsCount() const noexcept { return _array.size(); }
    [[nodiscard]] inline qsizetype columnsCount() const noexcept { return _headers.size(); }

Q_SIGNALS:
    void sendCreateRequest(const QByteArray &iRequest, const HandleResponse &handleResponse = Q_NULLPTR);
    void sendDeleteRequest(const QByteArray &iRequest, const HandleResponse &handleResponse = Q_NULLPTR);
    void sendUpdateRequest(const QByteArray &iRequest, const HandleResponse &handleResponse = Q_NULLPTR);

private:
    void setJsonObject(const QModelIndex &index, const QJsonObject &iJsonObject);
    [[nodiscard]] QJsonObject getJsonObject(int row) const;
    bool isSortColumn(int column) const;
    bool sortColumn(const QJsonValue &first, const QJsonValue &second, int column, Qt::SortOrder order = Qt::SortOrder::AscendingOrder) const;
    void updateRecord(int index, const QString &columnName, const QString &value);
    bool createEmail(int row);
    bool checkFieldOnDuplicate(int row, int column, QString &iValue) const;
    bool checkRowOnDeleted(int row) const;

private:
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const noexcept override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const noexcept override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

private:
    QString _name;
    EditStrategy _strategy = OnFieldChange;
    QList<QPair<QPair<QString, QString>, bool>> _headers;
    QJsonArray _array;
    QJsonArray _recordsCreatedCache, _recordsDeletedCache, _recordsUpdatedCache;

    friend class Delegate;
};

#endif // QJSONTABLEMODEL_H
