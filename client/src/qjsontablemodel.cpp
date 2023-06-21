#include "qjsontablemodel.h"
#include "client.h"
#include "utils.h"

#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>


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

    const auto fieldNames = Client::Employee::getFieldNames();
    _headers.append({{fieldNames.front().first, fieldNames.front().second}, false});
    for (decltype(fieldNames.size()) i = 1, I = fieldNames.size(); i < I; ++i)
    {
        const auto& [fieldNameEng, fieldNameRus] = fieldNames[i];

        auto it_permissions = iPermissions.find(fieldNameEng);
        if (it_permissions != iPermissions.end())
        {
            _headers.append({{ fieldNameEng, fieldNameRus }, it_permissions->toString() == "write"});
        }
    }

    return true;
}

void QJsonTableModel::submitAll()
{
    if (!_recordsCreateCache.empty())
    {
        sendCreateRequest(QJsonDocument(QJsonObject{{_name, _recordsCreateCache}}).toJson());

        while(!_recordsCreateCache.empty())
            _recordsCreateCache.pop_back();
    }
    else
        qInfo() << "Пустые данные для создания!";

    if (!_recordsDeleteCache.empty())
    {
        sendDeleteRequest(QJsonDocument(QJsonObject{{_name, _recordsDeleteCache}}).toJson());

        while(!_recordsDeleteCache.empty())
             _recordsDeleteCache.pop_back();
    }
    else
        qInfo() << "Пустые данные для удаления!";

    if (!_recordsUpdateCache.empty())
    {
        sendUpdateRequest(QJsonDocument(QJsonObject{{_name, _recordsUpdateCache}}).toJson());

        while(!_recordsUpdateCache.empty())
             _recordsUpdateCache.pop_back();
    }
    else
        qInfo() << "Пустые данные для обновления!";
}

bool QJsonTableModel::checkField(int row, int column, const QString &value) const
{
    if (column < 0)
        return false;

    const QString& field = _headers[column].first.first;
    QString message = value;

    if (auto tableView = static_cast<QWidget*>(parent()))
    {
        if (Client::Employee::checkField(field, message))
        {
            if (checkFieldOnDuplicate(row, column, message))
            {
                return true;
            }
            else
            {
                QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), message, QMessageBox::NoButton, tableView);
                QTimer::singleShot(1000, &warning, &QMessageBox::close);
                warning.exec();
                qDebug() << "Ошибка: " << message;
            }
        }
        else
        {
            QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), message, QMessageBox::NoButton, tableView);
            QTimer::singleShot(1000, &warning, &QMessageBox::close);
            warning.exec();
            qDebug() << "Ошибка: " << message;
        }
    }

    return false;
}

bool QJsonTableModel::deleteRow(int row)
{
    QJsonObject jsonObject = getJsonObject(row);
    const qint64 id = jsonObject["id"].toInteger();

    QJsonObject record;
    record.insert("id", id);

    if (_strategy == OnFieldChange)
    {
        sendUpdateRequest(QJsonDocument(QJsonObject{{_name, record}}).toJson());
        _array.removeAt(row); // TODO: Сделать проверку на успешное удаление
    }
    else if (_strategy == OnManualSubmit)
    {
        bool found = false;
        for (decltype(_recordsDeleteCache.size()) i = 0, I = _recordsDeleteCache.size(); i < I; ++i)
        {
            if (_recordsDeleteCache.at(i).isObject())
            {
                const QJsonObject object = _recordsDeleteCache.at(i).toObject();
                if (object.contains("id"))
                {
                    if (object.value("id") == id)
                    {
                        return true;
                    }
                }
            }
        }

        if (!found)
            _recordsDeleteCache.push_back(record);
    }

    return true;
}

bool QJsonTableModel::createEmail(int row)
{
    if (auto tableView = static_cast<QWidget*>(parent()))
    {
        if (_array.at(row).isObject())
        {
            QJsonObject rowObject = _array.at(row).toObject();
            if (rowObject.contains("id") &&
                rowObject.contains("surname") &&
                rowObject.contains("name") &&
                rowObject.contains("patronymic") &&
                rowObject.contains("email"))
            {
                const qint64 id = rowObject.take(Client::Employee::id()).toInteger();
                const QString surname = rowObject.take(Client::Employee::surname()).toString();
                const QString name = rowObject.take(Client::Employee::name()).toString();
                const QString patronymic = rowObject.take(Client::Employee::patronymic()).toString();
                const QString email = rowObject.take(Client::Employee::email()).toString();

                QString newEmail = Utils::CreateEmail(QVector<QString>{surname, name, patronymic});
                const int column = std::distance(_headers.constBegin(), std::find_if(_headers.constBegin(), _headers.constEnd(), [&](const auto& header)
                {
                    return header.first.first == Client::Employee::email();
                }));

                while (true)
                {
                    if (column > 0 && checkFieldOnDuplicate(row, column, newEmail))
                    {
                        if (newEmail != email)
                            updateRecord(id, Client::Employee::email(), newEmail);

                        break;
                    }
                    else
                    {
                        while (true)
                        {
                            bool ok;
                            QString message = QInputDialog::getText(tableView,
                                                             tr("Предупреждение"),
                                                             tr("Данная почта уже существует, измените почту:"),
                                                             QLineEdit::Normal, newEmail, &ok);
                            qDebug() << "Данная почта уже существует: " << newEmail;

                            if (ok)
                            {
                                if (Client::Employee::checkField(Client::Employee::email(), message))
                                {
                                    break;
                                }
                                else
                                {
                                    QMessageBox warning(QMessageBox::Icon::Warning, tr("Предупреждение"), message, QMessageBox::NoButton, tableView);
                                    QTimer::singleShot(1000, &warning, &QMessageBox::close);
                                    warning.exec();
                                    qDebug() << "Ошибка: " << message;
                                }
                            }
                            else
                            {
                                return false;
                            }
                        }
                    }
                }

                return true;
            }
        }
    }

    return false;
}

bool QJsonTableModel::checkFieldOnDuplicate(int row, int column, QString &iValue) const
{
    const QString& field = _headers[column].first.first;
    const QString& fieldName = _headers[column].first.second;

    if (field == "passport" || field == "phone" || field == "email")
    {
        qsizetype size = _array.count();
        for (decltype(size) i = 0; i < size; ++i)
        {
            if (i != row)
            {
                if (_array.at(i).isObject())
                {
                    QJsonObject row = _array.at(i).toObject();
                    if (row.contains(field))
                    {
                        auto column = row.take(field);
                        if (column.isString())
                        {
                            auto value = column.toString();
                            if (value == iValue)
                            {
                                iValue = "поле >> " + fieldName + value + " - повторяется";
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool QJsonTableModel::checkRowOnDeleted(int row) const
{
    QJsonObject jsonObject = getJsonObject(row);
    const qint64 id = jsonObject["id"].toInt();

    for (const auto& recordDelete : _recordsDeleteCache)
    {
        if (recordDelete.isObject())
        {
            if (recordDelete.toObject().contains(Client::Employee::id()))
            {
                const QJsonValue recordId = recordDelete.toObject().take(Client::Employee::id());
                if (recordId.isDouble())
                {
                    if (recordId.toInt() == id)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void QJsonTableModel::updateRecord(int index, const QString &columnName, const QString &value)
{
    QJsonObject record;
    record.insert("id", index);
    record.insert("column", columnName);
    record.insert("value", value);

    if (_strategy == OnFieldChange)
    {
        sendUpdateRequest(QJsonDocument(QJsonObject{{_name, record}}).toJson());
    }
    else if (_strategy == OnManualSubmit)
    {
        bool found = false;
        for (decltype(_recordsUpdateCache.size()) i = 0, I = _recordsUpdateCache.size(); i < I; ++i)
        {
            if (_recordsUpdateCache.at(i).isObject())
            {
                const QJsonObject object = _recordsUpdateCache.at(i).toObject();
                if (object.contains("id") && object.contains("column") && object.contains("value"))
                {
                    if (object.value("id") == index && object.value("column") == columnName)
                    {
                        found = true;
                        if (object.value("value") != value)
                        {
                            _recordsUpdateCache.replace(i, record);
                            break;
                        }
                    }
                }
            }
        }

        if (!found)
            _recordsUpdateCache.push_back(record);
    }
}

JsonTableModel *QJsonTableModel::relationModel(int column) const
{
    QString header = headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
    QSet<QString> uniqueData;
    for (int i = 0, I = rowCount({}); i < I; ++i)
    {
        QString text = data(index(i, column), Qt::DisplayRole).toString();
        if (!uniqueData.contains(text))
            uniqueData.insert(text);
     }

    return new JsonTableModel(header, {uniqueData.begin(), uniqueData.end()});
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

int QJsonTableModel::rowCount(const QModelIndex &) const
{
    return _array.size();
}

int QJsonTableModel::columnCount(const QModelIndex &) const
{
    return _headers.size();
}

void QJsonTableModel::setJsonObject(const QModelIndex &index, const QJsonObject &iJsonObject)
{
    _array[index.row()] = iJsonObject;
}

QJsonObject QJsonTableModel::getJsonObject(int row) const
{
    return _array[row].toObject();
}

bool QJsonTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QString message = value.toString();
    const QString& field = _headers[index.column()].first.first;

    if (index.isValid() && !message.isEmpty() && role == Qt::EditRole)
    {
        if (checkField(index.row(), index.column(), message))
        {
            QJsonObject jsonObject = getJsonObject(index.row());
            jsonObject[field] = value.toJsonValue();
            setJsonObject(index, jsonObject);
            updateRecord(jsonObject["id"].toInt(), field, message);
            emit dataChanged(index, index);
    //        emit dataChanged(createIndex(index.row(), index.column()), createIndex(index.row(), index.column()));
            return true;
        }
    }

    return false;
}

QVariant QJsonTableModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case Qt::BackgroundRole:
        {
            if (checkRowOnDeleted(index.row()))
                return QColor(Qt::red);
        }
        case Qt::EditRole:
        case Qt::DisplayRole:
        {
            const QJsonObject obj = getJsonObject(index.row());
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
    {
        if (checkRowOnDeleted(index.row()))
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        else
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return {};
}

bool QJsonTableModel::sortColumn(const QJsonValue &first, const QJsonValue &second, int column, Qt::SortOrder order) const
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

bool QJsonTableModel::isSortColumn(int column) const
{
    return std::is_sorted(std::begin(_array), std::end(_array), [&](const auto &lhs, const auto &rhs) { return sortColumn(lhs, rhs, column); });
}
