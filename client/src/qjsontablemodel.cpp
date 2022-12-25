#include "qjsontablemodel.h"
#include "client.h"

#include <QJsonDocument>
#include <QJsonObject>


inline void swap(QJsonValueRef first, QJsonValueRef second)
{
    QJsonValue temp(first);
    first = QJsonValue(second);
    second = temp;
}

QJsonTableModel::QJsonTableModel(const QJsonDocument &json, QObject *parent) :
    QAbstractTableModel(parent)
{
    setJson(json);
}

bool QJsonTableModel::setJson(const QJsonDocument &json)
{
    return setJson(json.array());
}

bool QJsonTableModel::setJson( const QJsonArray &array)
{
    if (array.size() == 0)
        return false;

    _array = array;
    _headers.append(Client::Employee::ID());
    _headers.append(Client::Employee::Role());
    _headers.append(Client::Employee::Surname());
    _headers.append(Client::Employee::Name());
    _headers.append(Client::Employee::Patronymic());
    _headers.append(Client::Employee::Sex());
    _headers.append(Client::Employee::DateOfBirth());
    _headers.append(Client::Employee::Passport());
    _headers.append(Client::Employee::Phone());
    _headers.append(Client::Employee::Email());
    _headers.append(Client::Employee::DateOfHiring());
    _headers.append(Client::Employee::WorkingHours());
    _headers.append(Client::Employee::Salary());
    _headers.append(Client::Employee::Password());
    return true;
}

QVariant QJsonTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    switch (orientation)
    {
        case Qt::Horizontal:
            return _headers.value(section);
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
        const QString& key = _headers[index.column()];
        QJsonObject jsonObject = getJsonObject(index);
        jsonObject[key] = value.toJsonValue();
        setJsonObject(index, jsonObject);
        emit dataChanged(index, index);
//        emit dataChanged(createIndex(index.row(), index.column()), createIndex(index.row(), index.column()));
        return true;
    }
    return false;
}

QVariant QJsonTableModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            const QJsonObject obj = getJsonObject(index);
            const QString& key = _headers[index.column()];
            if (obj.contains(key))
            {
                QJsonValue value = obj[key];

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
    if (index.column() > 0)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool QJsonTableModel::sortColumn(const QJsonValue &first, const QJsonValue &second, int column, Qt::SortOrder order)
{
    auto value_1 = first.toObject()[_headers[column]];
    auto value_2 = second.toObject()[_headers[column]];
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
