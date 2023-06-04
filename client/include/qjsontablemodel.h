#ifndef QJSONTABLEMODEL_H
#define QJSONTABLEMODEL_H

#include "QtGui/qcolor.h"
#include <QAbstractTableModel>
#include <QJsonArray>
#include <QPair>


//class JsonTableModel;

class JsonTableModel : public QAbstractTableModel
{
public: JsonTableModel(const QString &iHeader, const QVector<QString>& iData) :
       _header(iHeader),
       _data(iData)
    {

    }
private:

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        return _header;
    }

    int columnCount(const QModelIndex &parent) const override
    {
        return 1;
    }

    int rowCount(const QModelIndex &parent) const override
    {
        return _data.size();
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        switch (role)
        {
            case Qt::FontRole:
//        case Qt::BackgroundRole:
//        case Qt::ForegroundRole:
            return QColor(Qt::red);
            [[fallthrough]];
            case Qt::EditRole:
            case Qt::DisplayRole:
            {
                return _data[index.row()];
            }
        }

        return {};
    }

private:
    QString _header;
    QVector<QString> _data;
};

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

    JsonTableModel *relationModel(int column) const;

Q_SIGNALS:
    void sendRequest(const QByteArray &iRequest);

private:
    void setJsonObject(const QModelIndex &index, const QJsonObject &iJsonObject);
    QJsonObject getJsonObject(const QModelIndex &index) const;

    bool isSortColumn(int column) const;
    bool sortColumn(const QJsonValue &first, const QJsonValue &second, int column, Qt::SortOrder order = Qt::SortOrder::AscendingOrder) const;

    bool createEmail(int row);
    bool checkDublicate(const QModelIndex &index, QString &iValue)/* const*/;
    bool createRecord(int index, const QString &columnName, const QString &value);

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
