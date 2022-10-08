#ifndef QJSONTABLEMODEL_H
#define QJSONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>

class QJsonTableModel : public QAbstractTableModel
{
public:
    QJsonTableModel(const QJsonDocument &json, QObject *parent = nullptr);

    bool setJson(const QJsonDocument &json);
    bool setJson(const QJsonArray &array);

    virtual QJsonObject getJsonObject(const QModelIndex &index) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    QStringList meaders;
    QStringList _headers;
    QJsonArray _array;
};

#endif // QJSONTABLEMODEL_H
