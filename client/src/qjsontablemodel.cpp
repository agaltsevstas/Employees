#include "qjsontablemodel.h"
#include "client.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QColor>


inline void swap(QJsonValueRef first, QJsonValueRef second)
{
    QJsonValue temp(first);
    first = QJsonValue(second);
    second = temp;
}

QJsonTableModel::QJsonTableModel(const QString& iName, const QJsonDocument &iDatabase, const QJsonDocument &iPermissions, QObject *parent) :
    QAbstractTableModel(parent), _name(iName)
{
    setDatabase(iDatabase);
    setPermissions(iPermissions);
}

QJsonTableModel::QJsonTableModel(const QString& iName, const QJsonDocument &iDatabase, QObject *parent) :
    QAbstractTableModel(parent), _name(iName)
{
    setDatabase(iDatabase);
}

void QJsonTableModel::setEditStrategy(EditStrategy iStrategy)
{
    if (iStrategy != _strategy)
    {
        _strategy = iStrategy;

        if (_strategy == OnFieldChange)
            submitAll();
    }
}

bool QJsonTableModel::setDatabase(const QJsonDocument &iDatabase)
{
    return setDatabase(iDatabase.array());
}

bool QJsonTableModel::setDatabase(const QJsonArray &iDatabase)
{
    if (iDatabase.isEmpty())
        return false;

    _array = iDatabase;
    return true;
}

bool QJsonTableModel::setPermissions(const QJsonDocument &iPermissions)
{
    return setPermissions(iPermissions.object());
}

bool QJsonTableModel::setPermissions(const QJsonObject &iPermissions)
{
    if (iPermissions.isEmpty())
        return false;

    const auto fields = Client::Employee::getFileds();
    _headers.append({{fields.front().first, fields.front().second}, false});
    for (decltype(fields.size()) i = 1, I = fields.size(); i < I; ++i)
    {
        const auto& [field, name] = fields[i];

        auto it_permissions = iPermissions.find(field);
        if (it_permissions != iPermissions.end())
        {
            _headers.append({{field, name }, it_permissions->toString() == "write"});
        }
    }

    return true;
}

void QJsonTableModel::submitAll()
{
    if (!_recordsCache.empty())
    {
        sendRequest(QJsonDocument(QJsonObject{{_name, _recordsCache}}).toJson());

        while(_recordsCache.count())
             _recordsCache.pop_back();
    }
    else
        qInfo() << "Пустые данные!";
}

bool QJsonTableModel::createUser()
{
    return true;
}

bool QJsonTableModel::deleteUser()
{
    return true;
}

QVariant QJsonTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    switch (orientation)
    {
        case Qt::Horizontal:
            return _headers.value(section).first.second;
        case Qt::Vertical:
            //return section + 1;
            return {};
        default:
            return {};
    }
}

int QJsonTableModel::rowCount(const QModelIndex &parent) const
{
    return _array.size();
}

int QJsonTableModel::columnCount(const QModelIndex &parent) const
{
    return _headers.size();
}

void QJsonTableModel::setJsonObject(const QModelIndex &index, const QJsonObject &iJsonObject)
{
    _array[index.row()] = iJsonObject;
}

QJsonObject QJsonTableModel::getJsonObject(const QModelIndex &index) const
{
    return _array[index.row()].toObject();
}

bool QJsonTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && !value.toString().isEmpty() && role == Qt::EditRole)
    {
        const QString& key = _headers[index.column()].first.first;
        QJsonObject jsonObject = getJsonObject(index);
        jsonObject[key] = value.toJsonValue();
        setJsonObject(index, jsonObject);

        QJsonObject record;
        record.insert("id", jsonObject["id"].toInteger());
        record.insert("column", key);
        record.insert("value", value.toString());
        if (_strategy == OnFieldChange)
        {
            sendRequest(QJsonDocument(QJsonObject{{_name, record}}).toJson());
        }
        else if (_strategy == OnManualSubmit)
        {
            bool found = false;
            for (decltype(_recordsCache.size()) i = 0, I = _recordsCache.size(); i < I; ++i)
            {
                if (_recordsCache.at(i).isObject())
                {
                    const QJsonObject object = _recordsCache.at(i).toObject();
                    if (object.contains("column") && object.contains("value"))
                    {
                        if (object.value("column") == key)
                        {
                            found = true;
                            if (object.value("value") != value.toString())
                            {
                                _recordsCache.replace(i, record);
                                break;
                            }
                        }
                    }
                }
            }

            if (!found)
                _recordsCache.push_back(record);
        }

        emit dataChanged(index, index);
//        emit dataChanged(createIndex(index.row(), index.column()), createIndex(index.row(), index.column()));
        return true;
    }
    return false;
}

QVariant QJsonTableModel::data(const QModelIndex &index, int role) const
{
//    if(role == Qt::BackgroundRole)
//    {
//            return QColor(Qt::red);
//    }
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
            const QJsonObject obj = getJsonObject(index);
            const auto& [field, name] = _headers[index.column()].first;
            if (obj.contains(field))
            {
                QJsonValue value = obj[field];

                if (value.isString())
                {
                    return value.toString();
                }
                else if (value.isDouble())
                {
                    return QString::number(value.toDouble());
                }
                else
                {
                    Q_ASSERT(false);
                    return {};
                }
            }
        }
    }

    return {};
}

Qt::ItemFlags QJsonTableModel::flags(const QModelIndex &index) const
{
    if (_headers[index.column()].second)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool QJsonTableModel::sortColumn(const QJsonValue &first, const QJsonValue &second, int column, Qt::SortOrder order)
{
    auto value_1 = first.toObject()[_headers[column].first.first];
    auto value_2 = second.toObject()[_headers[column].first.first];
    if (value_1.isString())
    {
        if (order == Qt::SortOrder::AscendingOrder)
            return value_1.toString() < value_2.toString();
        else
            return value_1.toString() > value_2.toString();
    }
    else if (value_1.isDouble())
    {
        if (order == Qt::SortOrder::AscendingOrder)
            return value_1.toDouble() < value_2.toDouble();
        else
            return value_1.toDouble() > value_2.toDouble();
    }

    return false;
}

void QJsonTableModel::sort(int column, Qt::SortOrder order)
{
    qDebug() << "Сортировка";
    std::sort(_array.begin(), _array.end(), [&](const auto &first, const auto &second)
    {
        return sortColumn(first, second, column, order);
    });

    emit layoutChanged();
}

bool QJsonTableModel::isSortColumn(int column)
{
    return std::is_sorted(std::begin(_array), std::end(_array), [&](const auto &lhs, const auto &rhs) { return sortColumn(lhs, rhs, column); });
}
