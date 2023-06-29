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

class JsonTableModel : public QAbstractTableModel
{
public: JsonTableModel(const QString &iHeader, const QVector<QString>& iData) :
       _header(iHeader),
       _data(iData)
    {

    }

private:
    inline QVariant headerData(int, Qt::Orientation, int) const noexcept override
    {
        return _header;
    }

    inline int columnCount(const QModelIndex &) const noexcept override
    {
        return 1;
    }

    inline int rowCount(const QModelIndex &) const noexcept override
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

    void setEditStrategy(EditStrategy iStrategy) noexcept;
    inline EditStrategy egetEitStrategy() const noexcept { return _strategy; }

    bool setDatabase(const QJsonDocument &iDatabase) noexcept;
    bool setDatabase(const QJsonArray &iDatabase) noexcept;

    bool setPermissions(const QJsonDocument &iPermissions);
    bool setPermissions(const QJsonObject &iPermissions);

    void submitAll();
    bool checkField(int row, int column, const QString &ivalue) const;
    inline void addRow(const QJsonObject &iUser) noexcept { _array.push_back(iUser); };
    bool deleteRow(int row);
    void restoreRow(int row);
    bool canDeleteRow(int row);
    QList<int> valueSearch(const QString &iValue) const noexcept;
    JsonTableModel *relationModel(int column) const;
    inline qsizetype size() const noexcept { return _array.size(); }

Q_SIGNALS:
    void sendCreateRequest(const QByteArray &iRequest);
    void sendDeleteRequest(const QByteArray &iRequest);
    void sendUpdateRequest(const QByteArray &iRequest);

private:
    void setJsonObject(const QModelIndex &index, const QJsonObject &iJsonObject);
    QJsonObject getJsonObject(int row) const;
    bool isSortColumn(int column) const;
    bool sortColumn(const QJsonValue &first, const QJsonValue &second, int column, Qt::SortOrder order = Qt::SortOrder::AscendingOrder) const;
    void updateRecord(int index, const QString &columnName, const QString &value);
    bool createEmail(int row);
    bool checkFieldOnDuplicate(int row, int column, QString &iValue) const;
    bool checkRowOnDeleted(int row) const;

private:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const noexcept override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const noexcept override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
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
