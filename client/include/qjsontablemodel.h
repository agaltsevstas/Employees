#ifndef QJSONTABLEMODEL_H
#define QJSONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>


class QJsonTableModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    QJsonTableModel(const QJsonDocument &json, QObject *parent = nullptr);

    bool setJson(const QJsonDocument &json);
    bool setJson(const QJsonArray &array);

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
    QStringList _headers;
    QJsonArray _array;
};

#endif // QJSONTABLEMODEL_H
