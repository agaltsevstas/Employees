#ifndef QJSONTABLEMODEL_H
#define QJSONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>
#include <QPair>


class QJsonTableModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    QJsonTableModel(const QString& iName, const QJsonDocument &iDatabase, const QJsonDocument &iPermissions, QObject *parent = nullptr);
    QJsonTableModel(const QString& iName, const QJsonDocument &iDatabase, QObject *parent = nullptr);

    enum EditStrategy {OnFieldChange, OnRowChange, OnManualSubmit};

    void setEditStrategy(EditStrategy iStrategy);
    EditStrategy egetEitStrategy() const { return _strategy; }

    bool setDatabase(const QJsonDocument &iDatabase);
    bool setDatabase(const QJsonArray &iDatabase);

    bool setPermissions(const QJsonDocument &iPermissions);
    bool setPermissions(const QJsonObject &iPermissions);

    void submitAll();

    bool createUser();
    bool deleteUser();

Q_SIGNALS:
    void sendRequest(const QByteArray &iRequest);

private:
    void setJsonObject(const QModelIndex &index, const QJsonObject &iJsonObject);
    QJsonObject getJsonObject(const QModelIndex &index) const;

    bool isSortColumn(int column);
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
    QString _name;
    EditStrategy _strategy = OnFieldChange;
    QList<QPair<QPair<QString, QString>, bool>> _headers;
    QJsonArray _array;
    QJsonArray _recordsCache;
    friend class Delegate;
};

#endif // QJSONTABLEMODEL_H
