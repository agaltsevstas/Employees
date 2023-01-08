#ifndef QJSONTABLEMODEL_H
#define QJSONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>
#include <QPair>


class QJsonTableModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    QJsonTableModel(const QJsonDocument &iDatabase, const QJsonDocument &iPermissions, QObject *parent = nullptr);
    QJsonTableModel(const QJsonDocument &iDatabase, QObject *parent = nullptr);

    enum EditStrategy {OnFieldChange, OnRowChange, OnManualSubmit};

    void setEditStrategy(EditStrategy iStrategy) { _strategy = iStrategy; }
    EditStrategy editStrategy() const { return _strategy; }

    bool setDatabase(const QJsonDocument &iDatabase);
    bool setDatabase(const QJsonArray &iDatabase);

    bool setPermissions(const QJsonDocument &iPermissions);
    bool setPermissions(const QJsonObject &iPermissions);

    void setJsonObject(const QModelIndex &index, const QJsonObject &iJsonObject);
    QJsonObject getJsonObject(const QModelIndex &index) const;

    bool isSortColumn(int column);

private:
    bool sortColumn(const QJsonValue &first, const QJsonValue &second, int column, Qt::SortOrder order = Qt::SortOrder::AscendingOrder);

private:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

private:
    EditStrategy _strategy = EditStrategy::OnFieldChange;
    QList<QPair<QPair<QString, QString>, bool>> _headers;
    QJsonArray _array;
};

#endif // QJSONTABLEMODEL_H
