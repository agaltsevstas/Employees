#include "qjsontablemodel.h"
#include "client.h"

#include <QJsonDocument>
#include <QJsonObject>

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
    beginResetModel();
    if (array.size() == 0)
        return false;

    _array = array;
    _headers.append(Client::Employee::ID());
    _headers.append(Client::Employee::Position());
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
    endResetModel();
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

QJsonObject QJsonTableModel::getJsonObject(const QModelIndex &index) const
{
    return _array[index.row()].toObject();
}

QVariant QJsonTableModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case Qt::DisplayRole:
        {
            QJsonObject obj = getJsonObject(index);
            const QString& key = _headers[index.column()];
            if (obj.contains(key))
            {
                QJsonValue v = obj[key];

                if (v.isString())
                {
                    return v.toString();
                }
                else if (v.isDouble())
                {
                    return QString::number(v.toDouble());
                }
                else
                {
                    Q_ASSERT(false);
                    return {};
                }
            }
            else
            {
                return {};
            }
        }
        case Qt::ToolTipRole:
            return {};
        default:
            return {};
    }
}
